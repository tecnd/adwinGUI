/************************************************************************
File Name: ddstranslator.c
-------
Date Modified: December 3, 2004 (Ian Leroux)
-------
Description: Translation of DDS (AD9854 Direct Digital Synthesizer from 
Analog Devices) settings to ADWIN sequencer commands

Although some comments might be phrased as though this code directly
controls the DDS, it really only compiles commands that will later
be sent to the ADWIN.

This file exports three functions:

create_dds_cmd_sequence takes an array of DDSOPTIONS structs describing
settings such as frequencies, sweeps, signal amplitudes etc. and
translates it to a set of 1-byte commands to be executed by the ADWIN
in order to program the DDS for the desired behaviour.

get_dds_cmd is used by the main program in GUIDesign.c to query these
commands and integrate them into the master ADWIN control array, which is
then downloaded to the sequencer and executed.

free_dds_cmd_sequence deletes a compiled sequence of commands from memory


Commands sent to the ADWIN are of two forms
0b000000bb, where bb is a 2-bit number:
	When the ADWIN executes this command it sends the two bits over the
	serial link to the DDS
Bigger numbers: represent special actions to be taken directly by the
	ADWIN

All data transfers to the DDS are in network order:
Most significant byte first, and most significant bit first within each byte

Modified: Mar 10, 2005 (Stefan Myrskog)
Fixed a bug where the ramp rate is twice the desired value.

*************************************************************************/

#include "ddstranslator.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

/******************************************************************************
Command Sequence Manipulation Tools
******************************************************************************/

/* for each 1-byte command there is an associated index, which corresponds to
the time at which the command should be executed, measured in multiples of the
ADWIN event period
*/
typedef struct dds_cmd_sequence_struct {
	/* the two arrays are always at the same length, cmd_array_size */
	unsigned char*	cmds;	/* array of compiled cmds */
	unsigned long*	times;	/* event index for the cmds */
	unsigned long	array_size;
	unsigned long	num_written; /* portion of array currently filled */
} dds_cmd_sequence_struct;



/* heap-allocate a fresh dds_cmd_sequence_struct, initialised to empty */
static dds_cmds_ptr new_dds_cmd_sequence () {
	dds_cmds_ptr newstruct;
	
	newstruct = malloc(sizeof(struct dds_cmd_sequence_struct));
	newstruct->cmds = NULL;
	newstruct->times = NULL;
	newstruct->array_size = 0;
	newstruct->num_written = 0;
	return newstruct;
}

void free_dds_cmd_sequence (dds_cmds_ptr dead_cmds) {
	if (dead_cmds != NULL) {
		if (dead_cmds->cmds != NULL) {
			free(dead_cmds->cmds);
		}
		if (dead_cmds->times != NULL) {
			free(dead_cmds->times);
		}
		/* don't bother resetting the struct fields since it's going to get
		freed immediately */
		free(dead_cmds);
	}
}

/* Fetch the ADWIN command value for the specified index in the sequence,
or return -1 if none is available

Need to return int to allow for the -1 (search failed flag)

Use binary search, but be careful never to decrement below 0, even in the
case where the search fails at the zeroth index. Decrementing an unsigned #
causes wraparound, a bad array index, and then general protection fault.

This function is publicly accessible, so needs to check for NULL pointers
*/
int get_dds_cmd (dds_cmds_ptr cmd_sequence, unsigned long event_index)
{
	unsigned long lbound, ubound, mid;
	unsigned long* times;
	
	if(cmd_sequence == NULL) return -1;
	
	times = cmd_sequence->times;
	lbound = 0;
	ubound = cmd_sequence->num_written;
	/* if array is empty, ubound==0 and search is skipped */
	
	/* search space is lbound <= i < ubound, as in usual for-loop convention */
	while(lbound < ubound)
	{
		/* lbound <= mid < ubound because of integer divide, so it's within 
			search space and a safe array index
		*/
		mid = (lbound + ubound) / 2;
		
		if(times[mid] == event_index)
			return (int) cmd_sequence->cmds[mid];
		else if(times[mid] < event_index)
			lbound = mid + 1;
		else
		    ubound = mid;
		/* N.B.: the search space is narrowed at each pass, since mid is always
		*  excluded, either by incrementing the (inclusive) lower bound past it
		*  or by reducing the (exclusive) upper bound to it.
		*  This guarantees search convergence.
		*  Also, we never decrement anything below the (known-good) lbound,
		*  so we can't wraparound by decrementing 0
		*/
	}
	
	/* lbound==ubound so search space is empty */
	return -1;
}

/* 
Ensure the cmd_array can contain _at least_ min_size
commands. Doubles the array size every time it grows to give amortized
constant-time performance.

Never pass in a null pointer (the outside world doesn't have access to this
function, so we can guarantee this just for the code in this file).
*/
static void check_cmd_array_size (dds_cmds_ptr cmd_sequence,
									unsigned long min_size) {
	unsigned long new_size;
	
	/* fast check for common case, skips rest of function */
	if(min_size > cmd_sequence->array_size) {
		/* if we need to grow, grow to larger of min_size or 2* current size */
		new_size = 2 * cmd_sequence->array_size;
		new_size = min_size > new_size ? min_size : new_size;
		
		/* realloc to new size */
		cmd_sequence->cmds = realloc(cmd_sequence->cmds,
											new_size*sizeof(unsigned char));
		cmd_sequence->times = realloc(cmd_sequence->times,
											new_size*sizeof(unsigned long));
		cmd_sequence->array_size = new_size;
	}
}

/*
Add a single instruction byte to the command sequence, taking care of resizing

cmd_sequence: (pointer to) sequence to which the instruction should be added

cmd_time: the time (measured in update periods) at which the instruction
is to be sent, with the special value NEXT_AVAILABLE meaning "as soon as
possible after the previous command"

cmd_byte: the byte to be interpreted by the ADWIN

Return value: the next available command time

start_index needs to be greater than previous indices so that the index
sequence is monotonically increasing (needed for lookup to work properly). 
Should always be ok as long as we construct the commands in chronological order

Never pass in a NULL cmd_sequence (locally-maintained invariant)
*/
#define NEXT_AVAILABLE ULONG_MAX
static unsigned long append_byte (dds_cmds_ptr cmd_seq,
									unsigned long cmd_time,
									unsigned char cmd_byte)
{
	unsigned long next_avail;
	
	if (cmd_seq->num_written) { /* non-empty sequence */
		/* one event period past the last-specified time */
		next_avail = cmd_seq->times[cmd_seq->num_written - 1] + 1;
	} else { /* empty sequence, start at 0 */
		next_avail = 0;
	}
		
	if (cmd_time == NEXT_AVAILABLE) {
		cmd_time = next_avail;
	} else { /* specific time requested */
		if(cmd_time < next_avail) {
			printf("WARNING: tried to send a DDS command before the previous "
					"one had completed.\n"
					"Deferring to next available time slot\n");
			cmd_time = next_avail;
		} /* else specific request was ok, leave it */ 
	}
	
	
	check_cmd_array_size(cmd_seq, ++(cmd_seq->num_written));
	
	/* we've already incremented num_written, so num_written-1 is the
	index to fill in
	*/
	cmd_seq->times[cmd_seq->num_written - 1] = cmd_time;
	cmd_seq->cmds[cmd_seq->num_written - 1] = cmd_byte;
	return cmd_time + 1;
}

/* Append the sequence of commands needed to send instr_byte over the
serial connection to the DDS, starting at "time" (in event periods).

Return value: the next available command time

cmd_seq is never NULL (locally maintained invariant)
*/
static unsigned long append_serial_byte (dds_cmds_ptr cmd_seq,
										unsigned long time,
										unsigned char instr_byte)
{
	/* send byte 2 bits at a time, starting with the MSB. */
	append_byte(cmd_seq, time, (instr_byte>>6) & 0x03);
	append_byte(cmd_seq, NEXT_AVAILABLE, (instr_byte>>4) & 0x03);
	append_byte(cmd_seq, NEXT_AVAILABLE, (instr_byte>>2) & 0x03);
	return append_byte(cmd_seq, NEXT_AVAILABLE, instr_byte & 0x03);
}

/******************************************************************************
Wrappers for translating DDS commands

Programming a register on the DDS involves sending 1 instruction byte giving
the address of the register to be modified, followed by the desired contents
of the register

In the instruction byte, the most significant bit (first to be transmitted),
indicates whether this is a read or write (always a write for us), and the 4
least significant bits are the address of the register to update.

Instruction Byte Breakdown
Bit
MSB  D6  D5  D4  D3  D2  D1  D0
R/W  X    X   X  A3  A2  A1  A0
Bit 7 controls whether read/write 0-> write    1-> read 
A3-A0 define the register to access
******************************************************************************/

/* special command codes for ADWIN */
#define IO_RESET		0x04
#define UPDATE_CLOCK	0x05
#define MASTER_RESET	0x06

/* serial addresses of the various registers */
#define ADDR_PHASE_ADJUST_1		0x00
#define ADDR_PHASE_ADJUST_2		0x01

/* low end of frequency ramp */
#define ADDR_FREQ_TUNING_WORD_1	0x02
/* high end of frequency ramp */
#define ADDR_FREQ_TUNING_WORD_2	0x03
/* frequency ramp step size */
#define ADDR_DELTA_FREQ_WORD	0x04

#define ADDR_UPDATE_CLOCK		0x05

/* frequency ramp step time */
#define ADDR_RAMPRATE_CLOCK		0x06
/* assorted control functions, see below */
#define ADDR_CONTROL_REGISTER	0x07
/* output amplitude */
#define ADDR_OSK_MULTIPLIER		0x08
/* output amplitude ramping rate */
#define ADDR_OSK_RAMPRATE		0x0A

#define ADDR_CONTROL_DAC		0x0B

/* Control Register bit assignments */
/*31:29	Unused */
#define CR_BIT_COMPARATOR_PD 28 /* Comparator power down (active high) */
/* 27 Must always be written low! (otherwise DDS stops working) */
#define CR_BIT_CONTROL_DAC_PD 26 /* Control DAC power down (active high) */
#define CR_BIT_FULL_DAC_PD 25 /* Full DAC power down affects cosine DAC, 
								control DAC and reference */
#define CR_BIT_DIGITAL_PD 24 /* Digital section power down */
/* 23 Reserved, write 0 */
#define CR_BIT_PLL_RANGE 22 /* PLL range bit, affects the VCO gain ? */
#define CR_BIT_PLL_BYPASS 21 /* Use REFCLK directly without PLL multiplier */
#define CR_BIT_PLL_MULT4 20  /* 20:16 PLL multiplier factor */
#define CR_BIT_PLL_MULT3 19  /* must be in range [4,20] */
#define CR_BIT_PLL_MULT2 18
#define CR_BIT_PLL_MULT1 17
#define CR_BIT_PLL_MULT0 16
#define CR_BIT_CLR_ACC1 15 /* Clear accumulator 1 when written high */
#define CR_BIT_CLR_ACCS 14 /* While high, locks accumulators 1 and 2 to zero */
#define CR_BIT_TRIANGLE 13 /* Triangle bit: automatically ramp frequency back
							and forth between the frequency tuning words */

/* 12 Don't care */
#define CR_BIT_MODE2 11 /* 11:9	operating mode, see below */
#define CR_BIT_MODE1 10 
#define CR_BIT_MODE0 9
#define CR_BIT_INTERNAL_UPDATE 8 /*internal update active.
								We want this low so that _we_ generate the
								update clock signal */
/* 7 Reserved, write to 0 */
#define CR_BIT_INV_SINC_BYPASS 6 /*	inverse sinc filter bypass */
#define CR_BIT_OSK_ENABLE 5 /* OSK enable bit. Enable amplitude modulation */
#define CR_BIT_INTERNAL_OSK 4 /* internal OSK override. Leave low so that our
								amplitude settings are respected */
/* 3:2 Reserved, write 0 */
#define CR_BIT_LSB_FIRST 1 /* Serial port LSB first bit.
							Leave to low (MSB first) */
#define CR_BIT_SDO_ACTIVE 0 /* Serial port SDO active.
							Leave low as we never read from the DDS. */

/* operating modes */
#define MODE_SINGLE_TONE 0
#define MODE_FSK 1
#define MODE_RAMPED_FSK 2
#define MODE_CHIRP 3
#define MODE_BPSK 4

/* Shadow registers:
As the command sequence gets built up, these keep track of the state that
the DDS will be in at each point in the program.
Whenever any of these values gets changed, we set a flag,
and a single update_dds routine takes care of programming all the
flagged registers and sending the UPDATE_CLOCK command.

This way we can accumulate incremental changes (eg to the control register),
set the flag, and when time comes to make use of those changes only the final
version of the register is programmed (this can't hurt us because the DDS only
pays attention to values in the i/o buffers when the UPDATE_CLOCK is sent).

These arrays hold the most significant byte in the highest-numbered array
index, for ease in doing bit-addressing arithmetic.
*/
typedef struct dds_shadow_struct {
	unsigned char	phase_adjust_reg_1[2];
	BOOL			phase_adjust_reg_1_dirty;
	unsigned char	phase_adjust_reg_2[2];
	BOOL			phase_adjust_reg_2_dirty;
	unsigned char	freq_tuning_word_1[6];
	BOOL			freq_tuning_word_1_dirty;
	unsigned char	freq_tuning_word_2[6];
	BOOL			freq_tuning_word_2_dirty;
	unsigned char	delta_freq_word[6];
	BOOL			delta_freq_word_dirty;
	unsigned char	update_clock[4];
	BOOL			update_clock_dirty;
	unsigned char	ramp_rate_clock[3];
	BOOL			ramp_rate_clock_dirty;
	unsigned char 	control_reg[4];
	BOOL			control_reg_dirty;
	unsigned char	osk_multiplier[2];
	BOOL			osk_multiplier_dirty;
	unsigned char	osk_ramp_rate[1];
	BOOL			osk_ramp_rate_dirty;
	unsigned char	control_dac[2];
	BOOL			control_dac_dirty;
} dds_shadow_struct;

/* Append an i/o reset command to cmd_seq, to be executed at "time" (measured in
event periods.
Note that this has no effect on the internal state of the DDS.

Return value: the next available command time

cmd_seq is never NULL (locally maintained invariant)
*/
static unsigned long io_reset (dds_cmds_ptr cmd_seq,
								unsigned long time)
{
	return append_byte(cmd_seq, time, IO_RESET);
}

/* Append a master reset command to cmd_seq to be executed at time (measured in
event periods). Update the shadow to represent the state of the DDS after the
reset.

Return value: the next available command time

cmd_seq and shadow are never NULL (locally maintained invariants)
*/
static unsigned long master_reset (dds_cmds_ptr cmd_seq,
									unsigned long time,
									dds_shadow_struct* shadow)
{
	shadow->phase_adjust_reg_1[0] = 0x00;
	shadow->phase_adjust_reg_1[1] = 0x00;
	shadow->phase_adjust_reg_1_dirty = FALSE;
	shadow->phase_adjust_reg_2[0] = 0x00;
	shadow->phase_adjust_reg_2[1] = 0x00;
	shadow->phase_adjust_reg_2_dirty = FALSE;
	shadow->freq_tuning_word_1[0] = 0x00;
	shadow->freq_tuning_word_1[1] = 0x00;
	shadow->freq_tuning_word_1[2] = 0x00;
	shadow->freq_tuning_word_1[3] = 0x00;
	shadow->freq_tuning_word_1[4] = 0x00;
	shadow->freq_tuning_word_1[5] = 0x00;
	shadow->freq_tuning_word_1_dirty = FALSE;
	shadow->freq_tuning_word_2[0] = 0x00;
	shadow->freq_tuning_word_2[1] = 0x00;
	shadow->freq_tuning_word_2[2] = 0x00;
	shadow->freq_tuning_word_2[3] = 0x00;
	shadow->freq_tuning_word_2[4] = 0x00;
	shadow->freq_tuning_word_2[5] = 0x00;
	shadow->freq_tuning_word_2_dirty = FALSE;
	shadow->delta_freq_word[0] = 0x00;
	shadow->delta_freq_word[1] = 0x00;
	shadow->delta_freq_word[2] = 0x00;
	shadow->delta_freq_word[3] = 0x00;
	shadow->delta_freq_word[4] = 0x00;
	shadow->delta_freq_word[5] = 0x00;
	shadow->delta_freq_word_dirty = FALSE;
	shadow->update_clock[0] = 0x40;
	shadow->update_clock[1] = 0x00;
	shadow->update_clock[2] = 0x00;
	shadow->update_clock[3] = 0x00;
	shadow->update_clock_dirty = FALSE;
	shadow->ramp_rate_clock[0] = 0x00;
	shadow->ramp_rate_clock[1] = 0x00;
	shadow->ramp_rate_clock[2] = 0x00;
	shadow->ramp_rate_clock_dirty = FALSE;
	shadow->control_reg[0] = 0x20;
	shadow->control_reg[1] = 0x01;
	shadow->control_reg[2] = 0x64;
	shadow->control_reg[3] = 0x10;
	shadow->control_reg_dirty = FALSE;
	shadow->osk_multiplier[0] = 0x00;
	shadow->osk_multiplier[1] = 0x00;
	shadow->osk_multiplier_dirty = FALSE;
	shadow->osk_ramp_rate[0] = 0x80;
	shadow->osk_ramp_rate_dirty = FALSE;
	shadow->control_dac[0] = 0x00;
	shadow->control_dac[1] = 0x00;
	shadow->control_dac_dirty = FALSE;
	
	return append_byte(cmd_seq, time, MASTER_RESET);
}

/* Append to cmd_seq instructions to update all modified registers in the 
"shadow" bank, followed by an UPDATE_CLOCK command to start using the modified
values. The instructions will be scheduled to start at "time" (measured in 
event periods)

Return value: the next available command time

cmd_seq and shadow are never NULL (locally maintained invariants)
*/
static unsigned long update_dds (dds_cmds_ptr cmd_seq,
								unsigned long time,
								dds_shadow_struct* shadow)
{
	BOOL any_changes = FALSE;
	/* for each dirty register in shadow, send out the address & data, MSB first
	Have to manually keep track of time here because we don't know which
	register, if any, will be first to be updated
	*/
	if (shadow->phase_adjust_reg_1_dirty) {
		time = append_serial_byte(cmd_seq, time, ADDR_PHASE_ADJUST_1);
		time = append_serial_byte(cmd_seq, time, shadow->phase_adjust_reg_1[1]);
		time = append_serial_byte(cmd_seq, time, shadow->phase_adjust_reg_1[0]);
		shadow->phase_adjust_reg_1_dirty = FALSE;
		any_changes = TRUE;
	}
	if (shadow->phase_adjust_reg_2_dirty) {
		time = append_serial_byte(cmd_seq, time, ADDR_PHASE_ADJUST_2);
		time = append_serial_byte(cmd_seq, time, shadow->phase_adjust_reg_2[1]);
		time = append_serial_byte(cmd_seq, time, shadow->phase_adjust_reg_2[0]);
		shadow->phase_adjust_reg_2_dirty = FALSE;
		any_changes = TRUE;
	}
	if (shadow->freq_tuning_word_1_dirty) {
		time = append_serial_byte(cmd_seq, time, ADDR_FREQ_TUNING_WORD_1);
		time = append_serial_byte(cmd_seq, time, shadow->freq_tuning_word_1[5]);
		time = append_serial_byte(cmd_seq, time, shadow->freq_tuning_word_1[4]);
		time = append_serial_byte(cmd_seq, time, shadow->freq_tuning_word_1[3]);
		time = append_serial_byte(cmd_seq, time, shadow->freq_tuning_word_1[2]);
		time = append_serial_byte(cmd_seq, time, shadow->freq_tuning_word_1[1]);
		time = append_serial_byte(cmd_seq, time, shadow->freq_tuning_word_1[0]);
		shadow->freq_tuning_word_1_dirty = FALSE;
		any_changes = TRUE;
	}
	if (shadow->freq_tuning_word_2_dirty) {
		time = append_serial_byte(cmd_seq, time, ADDR_FREQ_TUNING_WORD_2);
		time = append_serial_byte(cmd_seq, time, shadow->freq_tuning_word_2[5]);
		time = append_serial_byte(cmd_seq, time, shadow->freq_tuning_word_2[4]);
		time = append_serial_byte(cmd_seq, time, shadow->freq_tuning_word_2[3]);
		time = append_serial_byte(cmd_seq, time, shadow->freq_tuning_word_2[2]);
		time = append_serial_byte(cmd_seq, time, shadow->freq_tuning_word_2[1]);
		time = append_serial_byte(cmd_seq, time, shadow->freq_tuning_word_2[0]);
		shadow->freq_tuning_word_2_dirty = FALSE;
		any_changes = TRUE;
	}
	if (shadow->delta_freq_word_dirty) {
		time = append_serial_byte(cmd_seq, time, ADDR_DELTA_FREQ_WORD);
		time = append_serial_byte(cmd_seq, time, shadow->delta_freq_word[5]);
		time = append_serial_byte(cmd_seq, time, shadow->delta_freq_word[4]);
		time = append_serial_byte(cmd_seq, time, shadow->delta_freq_word[3]);
		time = append_serial_byte(cmd_seq, time, shadow->delta_freq_word[2]);
		time = append_serial_byte(cmd_seq, time, shadow->delta_freq_word[1]);
		time = append_serial_byte(cmd_seq, time, shadow->delta_freq_word[0]);
		shadow->delta_freq_word_dirty = FALSE;
		any_changes = TRUE;
	}
	if (shadow->update_clock_dirty) {
		time = append_serial_byte(cmd_seq, time, ADDR_UPDATE_CLOCK);
		time = append_serial_byte(cmd_seq, time, shadow->update_clock[3]);
		time = append_serial_byte(cmd_seq, time, shadow->update_clock[2]);
		time = append_serial_byte(cmd_seq, time, shadow->update_clock[1]);
		time = append_serial_byte(cmd_seq, time, shadow->update_clock[0]);
		shadow->update_clock_dirty = FALSE;
		any_changes = TRUE;
	}
	if (shadow->ramp_rate_clock_dirty) {
		time = append_serial_byte(cmd_seq, time, ADDR_RAMPRATE_CLOCK);
		time = append_serial_byte(cmd_seq, time, shadow->ramp_rate_clock[2]);
		time = append_serial_byte(cmd_seq, time, shadow->ramp_rate_clock[1]);
		time = append_serial_byte(cmd_seq, time, shadow->ramp_rate_clock[0]);
		shadow->ramp_rate_clock_dirty = FALSE;
		any_changes = TRUE;
	}
	if (shadow->control_reg_dirty) {
		time = append_serial_byte(cmd_seq, time, ADDR_CONTROL_REGISTER);
		time = append_serial_byte(cmd_seq, time, shadow->control_reg[3]);
		time = append_serial_byte(cmd_seq, time, shadow->control_reg[2]);
		time = append_serial_byte(cmd_seq, time, shadow->control_reg[1]);
		time = append_serial_byte(cmd_seq, time, shadow->control_reg[0]);
		shadow->control_reg_dirty = FALSE;
		any_changes = TRUE;
	}
	if (shadow->osk_multiplier_dirty) {
		time = append_serial_byte(cmd_seq, time, ADDR_OSK_MULTIPLIER);
		time = append_serial_byte(cmd_seq, time, shadow->osk_multiplier[1]);
		time = append_serial_byte(cmd_seq, time, shadow->osk_multiplier[0]);
		shadow->osk_multiplier_dirty = FALSE;
		any_changes = TRUE;
	}
	if (shadow->osk_ramp_rate_dirty) {
		time = append_serial_byte(cmd_seq, time, ADDR_OSK_RAMPRATE);
		time = append_serial_byte(cmd_seq, time, shadow->osk_ramp_rate[0]);
		shadow->osk_ramp_rate_dirty = FALSE;
		any_changes = TRUE;
	}
	if (shadow->control_dac_dirty) {
		time = append_serial_byte(cmd_seq, time, ADDR_CONTROL_DAC);
		time = append_serial_byte(cmd_seq, time, shadow->control_dac[1]);
		time = append_serial_byte(cmd_seq, time, shadow->control_dac[0]);
		shadow->control_dac_dirty = FALSE;
		any_changes = TRUE;
	}
	if (any_changes) {
		return append_byte(cmd_seq, time, UPDATE_CLOCK);
	} else {
		return time;
	}
}

/* if necessary, modifies the control register in shadow so that
control_register[bit] has the same truth value as val

shadow is never NULL
*/
static void set_control_register_bit (dds_shadow_struct* shadow,
										int bit,
										int val)
{
	if (val) { /* want bit set */
		if (!((shadow->control_reg[bit/8] >> (bit % 8)) & 0x01)) {
			shadow->control_reg[bit/8] |= 0x01 << (bit %8);
			shadow->control_reg_dirty = TRUE;
		}
	} else { /* want bit clear */
		if (((shadow->control_reg[bit/8] >> (bit % 8)) & 0x01)) {
			shadow->control_reg[bit/8] &= ~(0x01 << (bit % 8));
			shadow->control_reg_dirty = TRUE;
		}
	}
}

static void set_PLL_mult (dds_shadow_struct* shadow, unsigned int pll_mult)
{
	/* make sure multiplier value is in range */
	pll_mult = pll_mult > 20 ? 20 : pll_mult;
	pll_mult = pll_mult < 4 ? 4 : pll_mult;
	
	/* set_control_register_bit takes care of the dirty flag, modifies
	only if necessary */
	set_control_register_bit(shadow, CR_BIT_PLL_MULT4, (pll_mult>>4) & 0x01);
	set_control_register_bit(shadow, CR_BIT_PLL_MULT3, (pll_mult>>3) & 0x01);
	set_control_register_bit(shadow, CR_BIT_PLL_MULT2, (pll_mult>>2) & 0x01);
	set_control_register_bit(shadow, CR_BIT_PLL_MULT1, (pll_mult>>1) & 0x01);
	set_control_register_bit(shadow, CR_BIT_PLL_MULT0, pll_mult & 0x01);
}

static void set_operating_mode (dds_shadow_struct* shadow, unsigned int mode)
{
	/* make sure mode value is in range */
	mode = mode > 4 ? 4 : mode;
	
	/* set_control_register_bit takes care of the dirty flag, modifies
	only if necessary */
	set_control_register_bit(shadow, CR_BIT_MODE2, (mode>>2) & 0x01);
	set_control_register_bit(shadow, CR_BIT_MODE1, (mode>>1) & 0x01);
	set_control_register_bit(shadow, CR_BIT_MODE0, mode & 0x01);
}

/* Modify the shadow settings to give DDS output "amplitude" (where
MAX_AMPLITUDE corresponds to full output power).

shadow is never NULL (locally maintained invariant)
*/
static void set_amplitude (dds_shadow_struct* shadow, float amplitude)
{
	unsigned long osk_mult_setting;

	/* need manual OSK control for this to work */
	set_control_register_bit(shadow, CR_BIT_OSK_ENABLE, TRUE);
	set_control_register_bit(shadow, CR_BIT_INTERNAL_OSK, FALSE);

	if (shadow->osk_ramp_rate[0] >= 3) {
		/* The Output shaped keying function is really designed to ramp signals
		on and off (to reduce high-frequency switching noise in communications
		applications).
		We "trick" it into giving us output amplitude control by setting the
		ramp rate to a value less than 3. This stalls the ramp rate counter so that
		instead of ramping up the osk multiplier to full power it simply holds it
		constant.
		This way, the osk multiplier acts as a fixed output amplitude setting
		*/
		shadow->osk_ramp_rate[0] = 0;
		shadow->osk_ramp_rate_dirty = TRUE;
	}
	
	/* The osk multiplier value is scaled out of 4095 (the 12 lower bits of the
	register */
	osk_mult_setting = (4095*amplitude)/MAX_AMPLITUDE;
	/* make sure value is in bounds */
	osk_mult_setting = osk_mult_setting > 4095 ? 4095 : osk_mult_setting;

	if (shadow->osk_multiplier[1] != (unsigned char) (osk_mult_setting >> 8)
			|| shadow->osk_multiplier[0] != (unsigned char) osk_mult_setting) {
		/* setting has changed */
		shadow->osk_multiplier[1] = (unsigned char) (osk_mult_setting >> 8);
		shadow->osk_multiplier[0] = (unsigned char) osk_mult_setting;
		shadow->osk_multiplier_dirty = TRUE;
	}
}

/* Modify the DDS shadow registers to set the ramp rate clock for a frequency
sweep that takes delta_time seconds.

sysclk is the DDS system clock frequency in MHz (refclk * PLL multiplier)

shadow is never NULL
*/
static void set_frequency_ramp_rate(dds_shadow_struct* shadow,
									float delta_time,
									float sysclk)
{

	unsigned long ramp_rate; 
	unsigned char new_ramprate_reg[3];
	int i;
	
	/* output frequency steps every (ramp_rate + 1) * 2 system clock periods 
	so we want:
	delta_time * STEP_GRANULARITY = step time = (ramp_rate + 1) * 2 / sysclk
	
	note the unit conversion from MHz to Hz for sysclk
	*/
	ramp_rate = (sysclk * 1e6) * delta_time * STEP_GRANULARITY / 1 - 1;
	/* casts truncate high bits */
	new_ramprate_reg[2] = (unsigned char) (ramp_rate >> 16);
	new_ramprate_reg[1] = (unsigned char) (ramp_rate >> 8);
   	new_ramprate_reg[0] = (unsigned char) ramp_rate;
   	
	for (i = 0; i < 3; ++i) {
		if (shadow->ramp_rate_clock[i] != new_ramprate_reg[i]) {
			shadow->ramp_rate_clock[i] = new_ramprate_reg[i];
			shadow->ramp_rate_clock_dirty = TRUE;
		}
	}
}

/* Take the desired frequency (in MHz) and set the 6-byte freq_reg to the
corresponding value.

sysclk is the DDS system clock frequency, also in MHz

Return TRUE if freq_reg had to be modified, FALSE otherwise

freq_reg is never NULL
*/
static BOOL convert_freq(unsigned char* freq_reg,
						float frequency,
						float sysclk)
{
	int i;
	double shift_factor;
	double frequency_ratio;
	unsigned char new_byte_val;
	BOOL was_modified = FALSE;
	
	/* The frequency is specified as a fixed-point 48-bit fraction according to
	the following formula (freq * 2^48)/SYSCLK, where 2^48 is the phase
	accumulator resolution
	
	the most significant byte is (freq/SYSCLCK * 2^8) (mod 256),
	the second is freq/SYSCLCK * 2^16 (mod 256),
	and so forth
	*/

	frequency_ratio = frequency / sysclk;
	
	for (i = 5, shift_factor = 256; i >= 0; --i, shift_factor *= 256) {
		/* cast to unsigned char takes care of modulus and truncation */
		new_byte_val = frequency_ratio * shift_factor;
		if (freq_reg[i] != new_byte_val) {
			freq_reg[i] = new_byte_val;
			was_modified = TRUE;
		}
	}
	return was_modified;
}

/* sets up the two frequency tuning words, the delta frequency word and the
ramp rate clock register
*/
static void set_frequency_registers(dds_shadow_struct* shadow,
									float start_frequency,
									float end_frequency,
									float ramp_time,
									float sysclk)
{
	float lowfreq, highfreq, freqstep;

	
	if (start_frequency < end_frequency) {
		lowfreq = start_frequency;
		highfreq = end_frequency;
	} else {
		lowfreq = end_frequency;
		highfreq = start_frequency;
	}
	freqstep = (highfreq - lowfreq) * STEP_GRANULARITY;
	
	/* only set the dirty flags if something was modified */	
	set_frequency_ramp_rate(shadow, ramp_time, sysclk);
	shadow->freq_tuning_word_1_dirty =
		convert_freq(shadow->freq_tuning_word_1, lowfreq, sysclk);
	shadow->freq_tuning_word_2_dirty =
		convert_freq(shadow->freq_tuning_word_2, highfreq, sysclk);
	shadow->delta_freq_word_dirty = 
		convert_freq(shadow->delta_freq_word, freqstep, sysclk);
}

	
/* Take a high-level specification dds_options of the behaviour of the
DDS over a time interval dds_options.delta_time and append to cmd_sequence
(with associated "shadow" bank) ADWIN instructions necessary to execute
this behaviour, starting at "time" (measured in event periods)

Return the next available command time
*/
static unsigned long execute_dds_settings(dds_cmds_ptr cmd_seq,
											unsigned long time,
											dds_shadow_struct* shadow,
											ddsoptions_struct dds_options,
											float sysclk)
{
	if (dds_options.is_stop) {
		/* set 0.0 amplitude and power down unused sections of the DDS
		Note that update_dds won't send anything if the DDS is already
		stopped, so we aren't wasting instructions
		*/
		set_amplitude(shadow, 0.0);
		set_control_register_bit(shadow, CR_BIT_CONTROL_DAC_PD, TRUE);
		set_control_register_bit(shadow, CR_BIT_FULL_DAC_PD, TRUE);
		set_control_register_bit(shadow, CR_BIT_DIGITAL_PD, TRUE);
		return update_dds(cmd_seq, time, shadow);
	} else { /* not stopped */
		set_operating_mode(shadow, MODE_RAMPED_FSK);
		set_frequency_registers(shadow,
							dds_options.start_frequency,
							dds_options.end_frequency,
							dds_options.delta_time,
							sysclk);
		set_amplitude(shadow, dds_options.amplitude);
		/* power up necessary sections of the DDS */
		set_control_register_bit(shadow, CR_BIT_FULL_DAC_PD, FALSE);
		set_control_register_bit(shadow, CR_BIT_DIGITAL_PD, FALSE);
		set_control_register_bit(shadow, CR_BIT_INV_SINC_BYPASS, FALSE);
		
		/* reset the phase and frequency accumulators */
		set_control_register_bit(shadow, CR_BIT_CLR_ACCS, TRUE);
		
		/* The comparator output is hard-wired on the DDS board
		to the FSK/BPSK/HOLD input pin, which controls the sweep.
		
		For a descending sweep, we want to start at the high frequency end (F2)
		and go to the low-frequency end (F1),
		so we need the FSK input to start high (while we set up) and flip low
		(to start the ramp)
		
		N.B, the input value of the FSK input pin is the opposite of the logic
		value of the "comparator power down" bit

		Vice-versa for an ascending sweep
		*/
		if (dds_options.start_frequency < dds_options.end_frequency) {
			set_control_register_bit(shadow, CR_BIT_COMPARATOR_PD, TRUE);
			update_dds(cmd_seq, time, shadow);
			
			/* unlock the accumulators and toggle the FSK input bit to start
			the ramp to the higher frequency */
			set_control_register_bit(shadow, CR_BIT_CLR_ACCS, FALSE);
			set_control_register_bit(shadow, CR_BIT_COMPARATOR_PD, FALSE);
			return update_dds(cmd_seq, NEXT_AVAILABLE, shadow);
		} else { /* descending sweep */
			set_control_register_bit(shadow, CR_BIT_COMPARATOR_PD, FALSE);
			update_dds(cmd_seq, time, shadow);
			
			/* unlock the accumulators and toggle the FSK input bit to start
			the ramp to the lower frequency */
			set_control_register_bit(shadow, CR_BIT_CLR_ACCS, FALSE);
			set_control_register_bit(shadow, CR_BIT_COMPARATOR_PD, TRUE);
			return update_dds(cmd_seq, NEXT_AVAILABLE, shadow);
		}
	}
}
		


/* Create and return a sequence of 1-byte commands to send to the ADWIN at
specified times in order to have the DDS execute the high-level behaviour
specified in dds_settings

The size of the dds_settings array is given by num_settings

PLL_multiplier is the desired DDS clock multiplier (the DDS's system clock
will run at PLL_multiplier * refclk)

refclk is the frequency (in MHz) of the DDS's external oscillator

adwin_event_period is the time (in seconds) between adwin update events
*/
dds_cmds_ptr create_dds_cmd_sequence(ddsoptions_struct* dds_settings,
									unsigned long num_settings,
									unsigned int PLL_multiplier,
									float refclk,
									float adwin_event_period)
{
	dds_cmds_ptr cmd_sequence = NULL;
	dds_shadow_struct shadow;
	
	long i;
	double idealtime;
	float sysclk;

	if (dds_settings == NULL || num_settings < 1) {
		return NULL;
	}

	sysclk = refclk * PLL_multiplier;

	cmd_sequence = new_dds_cmd_sequence();


	/* start from a known DDS state */ 
	master_reset(cmd_sequence, 0, &shadow);
	
	/* changes from default settings: */
	set_PLL_mult(&shadow, PLL_multiplier);
	set_control_register_bit(&shadow, CR_BIT_PLL_BYPASS, FALSE);
	/* we want to control the update clock */
	set_control_register_bit(&shadow, CR_BIT_INTERNAL_UPDATE, FALSE);
 	
	idealtime = 0.0;
	
	/* Normally settings get scheduled at idealtime, corresponding to the
	settings on the front panel.  If this is impossible (because previous
	commands haven't completed), then append_byte will generate a warning.
	However, we _know_ that the first batch of settings will be delayed until
	after the initial reset and configuration, so we handle it specially here
	to avoid the error message.
	*/
	execute_dds_settings(cmd_sequence, NEXT_AVAILABLE, &shadow,
									dds_settings[1], sysclk);
	
	for (i=2; i<=num_settings; i++) {
		//set the start time based on the start time
		//and delta time of the previous element
		idealtime += dds_settings[i-1].delta_time;
		
		execute_dds_settings(cmd_sequence, idealtime / adwin_event_period,
								&shadow, dds_settings[i], sysclk);
	}
	
	return cmd_sequence;
}

