/************************************************************************
File Name: ddstranslator.c
-------
Description: Translation of DDS (AD9854 Direct Digital Synthesizer from
Analog Devices) settings to ADWIN sequencer commands

Although some comments might be phrased as though this code directly
controls the DDS, it really only compiles commands that will later
be sent to the ADWIN.

Modified: Mar 10, 2005 (Stefan Myrskog)
Fixed a bug where the ramp rate is twice the desired value.
Apr 21, 2005 (Ian Leroux)
Recommented, renamed to account for the existence of multiple dds's, dropped
some superfluous configuration of the control register, made sure an update
clock always strobes the UCLK line etc ...
*************************************************************************/

#include "ddstranslator.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

/******************************************************************************
Command Sequence Manipulation Tools
******************************************************************************/

typedef struct dds_cmd_sequence_struct
{
	/* the two arrays are always at the same length, array_size */
	unsigned char *cmds;	   /* to be executed at "time" entry w/ same index */
	unsigned long *times;	   /* measured in ADWIN event periods */
	unsigned long array_size;  /* allocated size of cmds and times */
	unsigned long num_written; /* portion of array currently filled */
} dds_cmd_sequence_struct;

static dds_cmds_ptr new_dds_cmd_sequence()
{
	dds_cmds_ptr newstruct;

	newstruct = malloc(sizeof(dds_cmd_sequence_struct));
	newstruct->cmds = NULL;
	newstruct->times = NULL;
	newstruct->array_size = 0;
	newstruct->num_written = 0;
	return newstruct;
}

void free_dds_cmd_sequence(dds_cmds_ptr dead_cmds)
{
	if (dead_cmds != NULL)
	{
		if (dead_cmds->cmds != NULL)
		{
			free(dead_cmds->cmds);
		}
		if (dead_cmds->times != NULL)
		{
			free(dead_cmds->times);
		}
		free(dead_cmds);
	}
}

/* Return the cmd at the specified index (time) or -1 if there is none.
Need to return (signed) int to allow for the -1 (search failed flag).
This function is publicly accessible, so needs to check for NULL pointers.
*/
int get_dds_cmd(dds_cmds_ptr cmd_sequence, unsigned long event_index)
{
	unsigned long lbound, ubound, mid;
	unsigned long *times;

	if (cmd_sequence == NULL)
		return -1;

	times = cmd_sequence->times;
	lbound = 0;
	ubound = cmd_sequence->num_written;
	/* if array is empty, ubound==0 and search is skipped */

	/* search space is lbound <= i < ubound, as in usual for-loop convention */
	while (lbound < ubound)
	{
		/* lbound <= mid < ubound because of integer divide, so it's within
			search space and a safe array index
		*/
		mid = (lbound + ubound) / 2;

		if (times[mid] == event_index)
			return (int)cmd_sequence->cmds[mid];
		else if (times[mid] < event_index)
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

/* Ensure the cmd_array can contain _at least_ min_size
commands. Doubles the array size every time it grows to give amortized
constant-time performance.

Never pass in a null pointer (locally-maintained invariant).
*/
static void check_cmd_array_size(dds_cmds_ptr cmd_sequence,
								 unsigned long min_size)
{
	unsigned long new_size;

	if (min_size > cmd_sequence->array_size)
	{
		/* if we need to grow, grow to larger of min_size or 2* current size */
		new_size = 2 * cmd_sequence->array_size;
		new_size = min_size > new_size ? min_size : new_size;

		/* realloc to new size */
		cmd_sequence->cmds = realloc(cmd_sequence->cmds,
									 new_size * sizeof(cmd_sequence->cmds[0]));
		cmd_sequence->times = realloc(cmd_sequence->times,
									  new_size * sizeof(cmd_sequence->times[0]));
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
static unsigned long append_byte(dds_cmds_ptr cmd_seq,
								 unsigned long cmd_time,
								 unsigned char cmd_byte)
{
	unsigned long next_avail;

	if (cmd_seq->num_written)
	{ /* non-empty sequence */
		/* one event period past the last-specified time */
		next_avail = cmd_seq->times[cmd_seq->num_written - 1] + 1;
	}
	else
	{ /* empty sequence, start at 0 */
		next_avail = 0;
	}

	if (cmd_time == NEXT_AVAILABLE)
	{
		cmd_time = next_avail;
	}
	else
	{ /* specific time requested */
		if (cmd_time < next_avail)
		{
			printf("%d\n", cmd_time);
			printf("%d\n", next_avail);
			printf("WARNING: tried to send a DDS command before the previous "
				   "one had completed.\n"
				   "Deferring to next available time slot\n"
				   "Tried to write to event %d ",
				   next_avail);
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
static unsigned long append_serial_byte(dds_cmds_ptr cmd_seq,
										unsigned long time,
										unsigned char instr_byte)
{
	/* send byte 2 bits at a time, starting with the MSB. */
	append_byte(cmd_seq, time, (instr_byte >> 6) & 0x03);
	append_byte(cmd_seq, NEXT_AVAILABLE, (instr_byte >> 4) & 0x03);
	append_byte(cmd_seq, NEXT_AVAILABLE, (instr_byte >> 2) & 0x03);
	return append_byte(cmd_seq, NEXT_AVAILABLE, instr_byte & 0x03);
}

/******************************************************************************
Wrappers for translating DDS commands

Commands sent to the ADWIN are of two forms
0b000000bb, where bb is a 2-bit number:
	When the ADWIN executes this command it sends the two bits over the
	serial link to the DDS
Bigger numbers: represent special actions to be taken directly by the
	ADWIN

All data transfers to the DDS are in network order:
Most significant byte first, and most significant bit first within each byte

Programming a register on the DDS involves sending 1 instruction byte giving
the address of the register to be modified, followed by the desired contents
of the register

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
#define IO_RESET 0x04
#define UPDATE_CLOCK 0x05
#define MASTER_RESET 0x06

/* serial addresses of the various registers */
#define AD9852_PHASE_ADJUST_1 0x00
#define AD9852_PHASE_ADJUST_2 0x01
#define AD9852_FREQ_TUNING_WORD_1 0x02 /* low end of frequency ramp */
#define AD9852_FREQ_TUNING_WORD_2 0x03 /* high end of frequency ramp */
#define AD9852_DELTA_FREQ_WORD 0x04	   /* frequency ramp step size */
#define AD9852_UPDATE_CLOCK 0x05
#define AD9852_RAMPRATE_CLOCK 0x06 /* frequency ramp step time */
#define AD9852_CONTROL_REGISTER 0x07
#define AD9852_OSK_MULTIPLIER 0x08 /* output amplitude */
#define AD9852_OSK_RAMPRATE 0x0A   /* output amplitude ramping rate */
#define AD9852_CONTROL_DAC 0x0B

#define AD9858_CFR 0x00	  /* Control Function Register */
#define AD9858_DFTW 0x01  /* Delta-Frequency Tuning Word */
#define AD9858_DFRRW 0x02 /* Delta-Frequency Ramp Rate Word */
#define AD9858_FTW0 0x03  /* Frequency Tuning Word 0 */
#define AD9858_POW0 0x04  /* Phase Offset Word 0 */
#define AD9858_FTW1 0x05  /* Frequency Tuning Word 1 */
#define AD9858_POW1 0x06  /* Phase Offset Word 1 */
#define AD9858_FTW2 0x07  /* Frequency Tuning Word 2 */
#define AD9858_POW2 0x08  /* Phase Offset Word 2 */
#define AD9858_FTW3 0x09  /* Frequency Tuning Word 3 */
#define AD9858_POW3 0x0A  /* Phase Offset Word 3 */
/* Serial address 0x0B is reserved on the AD9858 */

/* Control Register bit assignments */
/*31:29	Unused */
#define AD9852_CR_COMPARATOR_PD 28 /* Comparator power down (active high) */
/* 27 Must always be written low! (otherwise DDS stops working) */
#define AD9852_CR_CONTROL_DAC_PD 26 /* Control DAC power down (active high) */
#define AD9852_CR_FULL_DAC_PD 25	/* Full DAC power down affects cosine DAC, \
									   control DAC and reference */
#define AD9852_CR_DIGITAL_PD 24		/* Digital section power down */
/* 23 Reserved, write 0 */
#define AD9852_CR_PLL_RANGE 22	/* PLL range bit, affects the VCO gain ? */
#define AD9852_CR_PLL_BYPASS 21 /* Use REFCLK directly without PLL multiplier */
#define AD9852_CR_PLL_MULT4 20	/* 20:16 PLL multiplier factor */
#define AD9852_CR_PLL_MULT3 19	/* must be in range [4,20] */
#define AD9852_CR_PLL_MULT2 18
#define AD9852_CR_PLL_MULT1 17
#define AD9852_CR_PLL_MULT0 16
#define AD9852_CR_CLR_ACC1 15 /* Clear accumulator 1 when written high */
#define AD9852_CR_CLR_ACCS 14 /* While high, locks accs 1 and 2 to zero */
#define AD9852_CR_TRIANGLE 13 /* Triangle bit: automatically ramp frequency back \
								 and forth between the frequency tuning words */
/* 12 Don't care */
#define AD9852_CR_MODE2 11 /* 11:9	operating mode, see below */
#define AD9852_CR_MODE1 10
#define AD9852_CR_MODE0 9
#define AD9852_CR_INTERNAL_UPDATE 8 /*internal update active.                    \
									  We want this low so that _we_ generate the \
									  update clock signal */
/* 7 Reserved, write to 0 */
#define AD9852_CR_INV_SINC_BYPASS 6 /*	inverse sinc filter bypass */
#define AD9852_CR_OSK_ENABLE 5		/* OSK enable bit. Enable amplitude modulation */
#define AD9852_CR_INTERNAL_OSK 4	/* internal OSK override. Leave low so that our \
								   amplitude settings are respected */
/* 3:2 Reserved, write 0 */
#define AD9852_CR_LSB_FIRST 1  /* Serial port LSB first bit. \
							 Leave to low (MSB first) */
#define AD9852_CR_SDO_ACTIVE 0 /* Serial port SDO active. \
							Leave low as we never read from the DDS. */

/* operating modes */
#define MODE_SINGLE_TONE 0
#define MODE_FSK 1
#define MODE_RAMPED_FSK 2
#define MODE_CHIRP 3
#define MODE_BPSK 4

/* And now for the AD9858 ... */
/* Frequency-Detect Mode Charge Pump Current */
#define AD9858_CR_FREQDET_CPI1 31
#define AD9858_CR_FREQDET_CPI0 30
/* Final Closed-Loop Mode Charge Pump Output Current */
#define AD9858_CR_FINALCLOSED_CPI2 29
#define AD9858_CR_FINALCLOSED_CPI1 28
#define AD9858_CR_FINALCLOSED_CPI0 27
/* Wide Closed-Loop Charge Pump Output Current */
#define AD9858_CR_WIDECLOSED_CPI2 26
#define AD9858_CR_WIDECLOSED_CPI1 25
#define AD9858_CR_WIDECLOSED_CPI0 24
/* Auto-clear frequency accumulator */
#define AD9858_CR_AUTOCLEARFREQ 23
/* Auto-clear phase accumulator */
#define AD9858_CR_AUTOCLEARPHASE 22
/* Load delta-frequency timer */
#define AD9858_CR_LOADDELTAFREQ 21
/* Clear Frequency Accumulator */
#define AD9858_CR_CLEARFREQ 20
/* Clear Phase Accumulator */
#define AD9858_CR_CLEARPHASE 19
/* The auto-clear bits don't immediately reset or lock the accumulators, they
just load zero into the accumulators on every strobe of the Frequency Update
pin, which we don't use. The ordinary Clear Accumulator bits clear and lock the
respective counter until the bits are cleared */

/* bit 18 is not used in the AD9858's control register */
/* PLL Fast-Lock Enable */
#define AD9858_CR_FASTLOCK 17
/* PLL Fast-Lock considers tuning word */
#define AD9858_CR_FASTLOCK_TUNING 16
/* Frequency Sweep Enable */
#define AD9858_CR_FREQSWEEP 15
/* Sine/Cosine select bit */
#define AD9858_CR_SINCOS 14
/* Charge Pump current offset */
#define AD9858_CR_CPOFFSET 13
/* Phase Detector Ref. Input Freq. divider */
#define AD9858_CR_PDRIFD1 12
#define AD9858_CR_PDRIFD0 11
/* Charge Pump Polarity */
#define AD9858_CR_CPPOLAR 10
/* Phase Detect. Feedback Input Freq. divider */
#define AD9858_CR_PDFIFD1 9
#define AD9858_CR_PDFIFD0 8
/* bit 7 is not used in the AD9858's control register */
/* REFCLK divide-by-2 disable */
#define AD9858_CR_CLKDIV2DIS 6
/* SYNCLK disable */
#define AD9858_CR_SYNCLKDIS 5
/* Analog Mixer Power-down */
#define AD9858_CR_MIXERPD 4
/* Phase Detector Power-down */
#define AD9858_CR_PDPD 3
/* DDS and DAC Power-down */
#define AD9858_CR_DDSPD 2
/* SDIO Input Only (3-wire serial operation) */
#define AD9858_CR_SDIO 1
/* LSB-first transmission order */
#define AD9858_CR_LSBFIRST 0

/* Shadow registers:
As the command sequence gets built up, these keep track of the state that
the DDS will be in at each point in the program.
Whenever any of these values gets changed, we set a flag,
and a single update_ad9852 or update_ad9858 routine takes care of programming
all the flagged registers and sending the UPDATE_CLOCK command.

This way we can accumulate incremental changes (eg to the control register),
set the flag, and when time comes to make use of those changes only the final
version of the register is programmed (this can't hurt us because the DDS only
pays attention to values in the i/o buffers when the UPDATE_CLOCK is sent).

These arrays hold the most significant byte in the highest-numbered array
index, for ease in doing bit-addressing arithmetic.
*/
typedef struct ad9852_shadow_struct
{
	unsigned char phase_adjust_reg_1[2];
	BOOL phase_adjust_reg_1_dirty;
	unsigned char phase_adjust_reg_2[2];
	BOOL phase_adjust_reg_2_dirty;
	unsigned char freq_tuning_word_1[6];
	BOOL freq_tuning_word_1_dirty;
	unsigned char freq_tuning_word_2[6];
	BOOL freq_tuning_word_2_dirty;
	unsigned char delta_freq_word[6];
	BOOL delta_freq_word_dirty;
	unsigned char update_clock[4];
	BOOL update_clock_dirty;
	unsigned char ramp_rate_clock[3];
	BOOL ramp_rate_clock_dirty;
	unsigned char control_reg[4];
	BOOL control_reg_dirty;
	unsigned char osk_multiplier[2];
	BOOL osk_multiplier_dirty;
	unsigned char osk_ramp_rate[1];
	BOOL osk_ramp_rate_dirty;
	unsigned char control_dac[2];
	BOOL control_dac_dirty;
} ad9852_shadow_struct;

typedef struct ad9858_shadow_struct
{
	unsigned char control_function_reg[4];
	BOOL control_function_reg_dirty;
	unsigned char delta_freq_tuning_word[4];
	BOOL delta_freq_tuning_word_dirty;
	unsigned char delta_freq_ramp_rate_word[2];
	BOOL delta_freq_ramp_rate_word_dirty;
	unsigned char freq_tuning_word_0[4];
	BOOL freq_tuning_word_0_dirty;
	unsigned char phase_offset_word_0[2];
	BOOL phase_offset_word_0_dirty;
	unsigned char freq_tuning_word_1[4];
	BOOL freq_tuning_word_1_dirty;
	unsigned char phase_offset_word_1[2];
	BOOL phase_offset_word_1_dirty;
	unsigned char freq_tuning_word_2[4];
	BOOL freq_tuning_word_2_dirty;
	unsigned char phase_offset_word_2[2];
	BOOL phase_offset_word_2_dirty;
	unsigned char freq_tuning_word_3[4];
	BOOL freq_tuning_word_3_dirty;
	unsigned char phase_offset_word_3[2];
	BOOL phase_offset_word_3_dirty;
} ad9858_shadow_struct;

/* Append an i/o reset command to cmd_seq, to be executed at "time" (measured in
event periods. Note that this has no effect on the internal state of the DDS.

Return value: the next available command time

cmd_seq is never NULL (locally maintained invariant)

This function is never used. Remove?
*/
static unsigned long io_reset(dds_cmds_ptr cmd_seq,
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
static unsigned long ad9852_master_reset(dds_cmds_ptr cmd_seq,
										 unsigned long time,
										 ad9852_shadow_struct *shadow)
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

static unsigned long ad9858_master_reset(dds_cmds_ptr cmd_seq,
										 unsigned long time,
										 ad9858_shadow_struct *shadow)
{
	shadow->control_function_reg[3] = 0x00;
	shadow->control_function_reg[2] = 0x00;
	shadow->control_function_reg[1] = 0x00;
	shadow->control_function_reg[0] = 0x18;
	shadow->control_function_reg_dirty = FALSE;
	shadow->freq_tuning_word_0[3] = 0x00;
	shadow->freq_tuning_word_0[2] = 0x00;
	shadow->freq_tuning_word_0[1] = 0x00;
	shadow->freq_tuning_word_0[0] = 0x00;
	shadow->freq_tuning_word_0_dirty = FALSE;
	shadow->phase_offset_word_0[1] = 0x00;
	shadow->phase_offset_word_0[0] = 0x00;
	shadow->phase_offset_word_0_dirty = FALSE;

	/* the other registers do not have well-defined (or at least
	 * well-documented) default values.  We don't want to mark them all dirty,
	 * because that would force a writeout of all kinds of probably meaningless
	 * data. Instead, give them all impossible values, as the best available
	 * substitute for "undefined".  Any code that cares about these values will
	 * write sane numbers, set the dirty flag, and get real values downloaded to
	 * the DDS */
	shadow->delta_freq_tuning_word[3] = 0xFF;
	shadow->delta_freq_tuning_word[2] = 0xFF;
	shadow->delta_freq_tuning_word[1] = 0xFF;
	shadow->delta_freq_tuning_word[0] = 0xFF;
	shadow->delta_freq_tuning_word_dirty = FALSE;
	shadow->delta_freq_ramp_rate_word[1] = 0xFF;
	shadow->delta_freq_ramp_rate_word[0] = 0xFF;
	shadow->delta_freq_ramp_rate_word_dirty = FALSE;
	shadow->freq_tuning_word_1[3] = 0xFF;
	shadow->freq_tuning_word_1[2] = 0xFF;
	shadow->freq_tuning_word_1[1] = 0xFF;
	shadow->freq_tuning_word_1[0] = 0xFF;
	shadow->freq_tuning_word_1_dirty = FALSE;
	shadow->phase_offset_word_1[1] = 0xFF;
	shadow->phase_offset_word_1[0] = 0xFF;
	shadow->phase_offset_word_1_dirty = FALSE;
	shadow->freq_tuning_word_2[3] = 0xFF;
	shadow->freq_tuning_word_2[2] = 0xFF;
	shadow->freq_tuning_word_2[1] = 0xFF;
	shadow->freq_tuning_word_2[0] = 0xFF;
	shadow->freq_tuning_word_2_dirty = FALSE;
	shadow->phase_offset_word_2[1] = 0xFF;
	shadow->phase_offset_word_2[0] = 0xFF;
	shadow->phase_offset_word_2_dirty = FALSE;
	shadow->freq_tuning_word_3[3] = 0xFF;
	shadow->freq_tuning_word_3[2] = 0xFF;
	shadow->freq_tuning_word_3[1] = 0xFF;
	shadow->freq_tuning_word_3[0] = 0xFF;
	shadow->freq_tuning_word_3_dirty = FALSE;
	shadow->phase_offset_word_0[1] = 0xFF;
	shadow->phase_offset_word_3[0] = 0xFF;
	shadow->phase_offset_word_3_dirty = FALSE;

	return append_byte(cmd_seq, time, MASTER_RESET);
}
/* Append to cmd_seq instructions to update all modified registers in the
"shadow" bank, followed by an UPDATE_CLOCK command to start using the modified
values. The instructions will be scheduled to start at "time" (measured in
event periods)

Return value: the next available command time

cmd_seq and shadow are never NULL (locally maintained invariants)
*/
static unsigned long update_ad9852(dds_cmds_ptr cmd_seq,
								   unsigned long time,
								   ad9852_shadow_struct *shadow)
{
	/* for each dirty register in shadow, send out the address & data, MSB first
	Have to manually keep track of time here because we don't know which
	register, if any, will be first to be updated
	*/
	if (shadow->phase_adjust_reg_1_dirty)
	{
		time = append_serial_byte(cmd_seq, time, AD9852_PHASE_ADJUST_1);
		time = append_serial_byte(cmd_seq, time, shadow->phase_adjust_reg_1[1]);
		time = append_serial_byte(cmd_seq, time, shadow->phase_adjust_reg_1[0]);
		shadow->phase_adjust_reg_1_dirty = FALSE;
	}
	if (shadow->phase_adjust_reg_2_dirty)
	{
		time = append_serial_byte(cmd_seq, time, AD9852_PHASE_ADJUST_2);
		time = append_serial_byte(cmd_seq, time, shadow->phase_adjust_reg_2[1]);
		time = append_serial_byte(cmd_seq, time, shadow->phase_adjust_reg_2[0]);
		shadow->phase_adjust_reg_2_dirty = FALSE;
	}
	if (shadow->freq_tuning_word_1_dirty)
	{
		time = append_serial_byte(cmd_seq, time, AD9852_FREQ_TUNING_WORD_1);
		time = append_serial_byte(cmd_seq, time, shadow->freq_tuning_word_1[5]);
		time = append_serial_byte(cmd_seq, time, shadow->freq_tuning_word_1[4]);
		time = append_serial_byte(cmd_seq, time, shadow->freq_tuning_word_1[3]);
		time = append_serial_byte(cmd_seq, time, shadow->freq_tuning_word_1[2]);
		time = append_serial_byte(cmd_seq, time, shadow->freq_tuning_word_1[1]);
		time = append_serial_byte(cmd_seq, time, shadow->freq_tuning_word_1[0]);
		shadow->freq_tuning_word_1_dirty = FALSE;
	}
	if (shadow->freq_tuning_word_2_dirty)
	{
		time = append_serial_byte(cmd_seq, time, AD9852_FREQ_TUNING_WORD_2);
		time = append_serial_byte(cmd_seq, time, shadow->freq_tuning_word_2[5]);
		time = append_serial_byte(cmd_seq, time, shadow->freq_tuning_word_2[4]);
		time = append_serial_byte(cmd_seq, time, shadow->freq_tuning_word_2[3]);
		time = append_serial_byte(cmd_seq, time, shadow->freq_tuning_word_2[2]);
		time = append_serial_byte(cmd_seq, time, shadow->freq_tuning_word_2[1]);
		time = append_serial_byte(cmd_seq, time, shadow->freq_tuning_word_2[0]);
		shadow->freq_tuning_word_2_dirty = FALSE;
	}
	if (shadow->delta_freq_word_dirty)
	{
		time = append_serial_byte(cmd_seq, time, AD9852_DELTA_FREQ_WORD);
		time = append_serial_byte(cmd_seq, time, shadow->delta_freq_word[5]);
		time = append_serial_byte(cmd_seq, time, shadow->delta_freq_word[4]);
		time = append_serial_byte(cmd_seq, time, shadow->delta_freq_word[3]);
		time = append_serial_byte(cmd_seq, time, shadow->delta_freq_word[2]);
		time = append_serial_byte(cmd_seq, time, shadow->delta_freq_word[1]);
		time = append_serial_byte(cmd_seq, time, shadow->delta_freq_word[0]);
		shadow->delta_freq_word_dirty = FALSE;
	}
	if (shadow->update_clock_dirty)
	{
		time = append_serial_byte(cmd_seq, time, AD9852_UPDATE_CLOCK);
		time = append_serial_byte(cmd_seq, time, shadow->update_clock[3]);
		time = append_serial_byte(cmd_seq, time, shadow->update_clock[2]);
		time = append_serial_byte(cmd_seq, time, shadow->update_clock[1]);
		time = append_serial_byte(cmd_seq, time, shadow->update_clock[0]);
		shadow->update_clock_dirty = FALSE;
	}
	if (shadow->ramp_rate_clock_dirty)
	{
		time = append_serial_byte(cmd_seq, time, AD9852_RAMPRATE_CLOCK);
		time = append_serial_byte(cmd_seq, time, shadow->ramp_rate_clock[2]);
		time = append_serial_byte(cmd_seq, time, shadow->ramp_rate_clock[1]);
		time = append_serial_byte(cmd_seq, time, shadow->ramp_rate_clock[0]);
		shadow->ramp_rate_clock_dirty = FALSE;
	}
	if (shadow->control_reg_dirty)
	{
		time = append_serial_byte(cmd_seq, time, AD9852_CONTROL_REGISTER);
		time = append_serial_byte(cmd_seq, time, shadow->control_reg[3]);
		time = append_serial_byte(cmd_seq, time, shadow->control_reg[2]);
		time = append_serial_byte(cmd_seq, time, shadow->control_reg[1]);
		time = append_serial_byte(cmd_seq, time, shadow->control_reg[0]);
		shadow->control_reg_dirty = FALSE;
	}
	if (shadow->osk_multiplier_dirty)
	{
		time = append_serial_byte(cmd_seq, time, AD9852_OSK_MULTIPLIER);
		time = append_serial_byte(cmd_seq, time, shadow->osk_multiplier[1]);
		time = append_serial_byte(cmd_seq, time, shadow->osk_multiplier[0]);
		shadow->osk_multiplier_dirty = FALSE;
	}
	if (shadow->osk_ramp_rate_dirty)
	{
		time = append_serial_byte(cmd_seq, time, AD9852_OSK_RAMPRATE);
		time = append_serial_byte(cmd_seq, time, shadow->osk_ramp_rate[0]);
		shadow->osk_ramp_rate_dirty = FALSE;
	}
	if (shadow->control_dac_dirty)
	{
		time = append_serial_byte(cmd_seq, time, AD9852_CONTROL_DAC);
		time = append_serial_byte(cmd_seq, time, shadow->control_dac[1]);
		time = append_serial_byte(cmd_seq, time, shadow->control_dac[0]);
		shadow->control_dac_dirty = FALSE;
	}
	return append_byte(cmd_seq, time, UPDATE_CLOCK);
}

static unsigned long update_ad9858(dds_cmds_ptr cmd_seq,
								   unsigned long time,
								   ad9858_shadow_struct *shadow)
{
	/* for each dirty register in shadow, send out the address & data, MSB first
	Have to manually keep track of time here because we don't know which
	register, if any, will be first to be updated
	*/
	if (shadow->control_function_reg_dirty)
	{
		time = append_serial_byte(cmd_seq, time, AD9858_CFR);
		time = append_serial_byte(cmd_seq, time, shadow->control_function_reg[3]);
		time = append_serial_byte(cmd_seq, time, shadow->control_function_reg[2]);
		time = append_serial_byte(cmd_seq, time, shadow->control_function_reg[1]);
		time = append_serial_byte(cmd_seq, time, shadow->control_function_reg[0]);
		shadow->control_function_reg_dirty = FALSE;
	}
	if (shadow->delta_freq_tuning_word_dirty)
	{
		time = append_serial_byte(cmd_seq, time, AD9858_DFTW);
		time = append_serial_byte(cmd_seq, time, shadow->delta_freq_tuning_word[3]);
		time = append_serial_byte(cmd_seq, time, shadow->delta_freq_tuning_word[2]);
		time = append_serial_byte(cmd_seq, time, shadow->delta_freq_tuning_word[1]);
		time = append_serial_byte(cmd_seq, time, shadow->delta_freq_tuning_word[0]);
		shadow->delta_freq_tuning_word_dirty = FALSE;
	}
	if (shadow->delta_freq_ramp_rate_word_dirty)
	{
		time = append_serial_byte(cmd_seq, time, AD9858_DFRRW);
		time = append_serial_byte(cmd_seq, time, shadow->delta_freq_ramp_rate_word[1]);
		time = append_serial_byte(cmd_seq, time, shadow->delta_freq_ramp_rate_word[0]);
		shadow->delta_freq_ramp_rate_word_dirty = FALSE;
	}
	if (shadow->freq_tuning_word_0_dirty)
	{
		time = append_serial_byte(cmd_seq, time, AD9858_FTW0);
		time = append_serial_byte(cmd_seq, time, shadow->freq_tuning_word_0[3]);
		time = append_serial_byte(cmd_seq, time, shadow->freq_tuning_word_0[2]);
		time = append_serial_byte(cmd_seq, time, shadow->freq_tuning_word_0[1]);
		time = append_serial_byte(cmd_seq, time, shadow->freq_tuning_word_0[0]);
		shadow->freq_tuning_word_0_dirty = FALSE;
	}
	if (shadow->phase_offset_word_0_dirty)
	{
		time = append_serial_byte(cmd_seq, time, AD9858_POW0);
		time = append_serial_byte(cmd_seq, time, shadow->phase_offset_word_0[1]);
		time = append_serial_byte(cmd_seq, time, shadow->phase_offset_word_0[0]);
		shadow->phase_offset_word_0_dirty = FALSE;
	}
	if (shadow->freq_tuning_word_1_dirty)
	{
		time = append_serial_byte(cmd_seq, time, AD9858_FTW1);
		time = append_serial_byte(cmd_seq, time, shadow->freq_tuning_word_1[3]);
		time = append_serial_byte(cmd_seq, time, shadow->freq_tuning_word_1[2]);
		time = append_serial_byte(cmd_seq, time, shadow->freq_tuning_word_1[1]);
		time = append_serial_byte(cmd_seq, time, shadow->freq_tuning_word_1[0]);
		shadow->freq_tuning_word_1_dirty = FALSE;
	}
	if (shadow->phase_offset_word_1_dirty)
	{
		time = append_serial_byte(cmd_seq, time, AD9858_POW1);
		time = append_serial_byte(cmd_seq, time, shadow->phase_offset_word_1[1]);
		time = append_serial_byte(cmd_seq, time, shadow->phase_offset_word_1[0]);
		shadow->phase_offset_word_1_dirty = FALSE;
	}
	if (shadow->freq_tuning_word_2_dirty)
	{
		time = append_serial_byte(cmd_seq, time, AD9858_FTW2);
		time = append_serial_byte(cmd_seq, time, shadow->freq_tuning_word_2[3]);
		time = append_serial_byte(cmd_seq, time, shadow->freq_tuning_word_2[2]);
		time = append_serial_byte(cmd_seq, time, shadow->freq_tuning_word_2[1]);
		time = append_serial_byte(cmd_seq, time, shadow->freq_tuning_word_2[0]);
		shadow->freq_tuning_word_2_dirty = FALSE;
	}
	if (shadow->phase_offset_word_2_dirty)
	{
		time = append_serial_byte(cmd_seq, time, AD9858_POW2);
		time = append_serial_byte(cmd_seq, time, shadow->phase_offset_word_2[1]);
		time = append_serial_byte(cmd_seq, time, shadow->phase_offset_word_2[0]);
		shadow->phase_offset_word_2_dirty = FALSE;
	}
	if (shadow->freq_tuning_word_3_dirty)
	{
		time = append_serial_byte(cmd_seq, time, AD9858_FTW3);
		time = append_serial_byte(cmd_seq, time, shadow->freq_tuning_word_3[3]);
		time = append_serial_byte(cmd_seq, time, shadow->freq_tuning_word_3[2]);
		time = append_serial_byte(cmd_seq, time, shadow->freq_tuning_word_3[1]);
		time = append_serial_byte(cmd_seq, time, shadow->freq_tuning_word_3[0]);
		shadow->freq_tuning_word_3_dirty = FALSE;
	}
	if (shadow->phase_offset_word_3_dirty)
	{
		time = append_serial_byte(cmd_seq, time, AD9858_POW3);
		time = append_serial_byte(cmd_seq, time, shadow->phase_offset_word_3[1]);
		time = append_serial_byte(cmd_seq, time, shadow->phase_offset_word_3[0]);
		shadow->phase_offset_word_3_dirty = FALSE;
	}
	return append_byte(cmd_seq, time, UPDATE_CLOCK);
}

/* if necessary, modifies the control register in shadow so that
control_register[bit] has the same truth value as val

shadow is never NULL
*/
static void set_ad9852_cr_bit(ad9852_shadow_struct *shadow,
							  int bit,
							  int val)
{
	if (val)
	{ /* want bit set */
		if (!((shadow->control_reg[bit / 8] >> (bit % 8)) & 0x01))
		{
			shadow->control_reg[bit / 8] |= 0x01 << (bit % 8);
			shadow->control_reg_dirty = TRUE;
		}
	}
	else
	{ /* want bit clear */
		if (((shadow->control_reg[bit / 8] >> (bit % 8)) & 0x01))
		{
			shadow->control_reg[bit / 8] &= ~(0x01 << (bit % 8));
			shadow->control_reg_dirty = TRUE;
		}
	}
}

static void set_ad9858_cr_bit(ad9858_shadow_struct *shadow,
							  int bit,
							  int val)
{
	if (val)
	{ /* want bit set */
		if (!((shadow->control_function_reg[bit / 8] >> (bit % 8)) & 0x01))
		{
			shadow->control_function_reg[bit / 8] |= 0x01 << (bit % 8);
			shadow->control_function_reg_dirty = TRUE;
		}
	}
	else
	{ /* want bit clear */
		if (((shadow->control_function_reg[bit / 8] >> (bit % 8)) & 0x01))
		{
			shadow->control_function_reg[bit / 8] &= ~(0x01 << (bit % 8));
			shadow->control_function_reg_dirty = TRUE;
		}
	}
}

/* shadow is never NULL (locally maintained invariant) */
static void set_PLL_mult(ad9852_shadow_struct *shadow, unsigned int pll_mult)
{
	/* make sure multiplier value is in range */
	pll_mult = pll_mult > 20 ? 20 : pll_mult;
	pll_mult = pll_mult < 4 ? 4 : pll_mult;

	/* set_control_register_bit takes care of the dirty flag, modifies
	only if necessary */
	set_ad9852_cr_bit(shadow, AD9852_CR_PLL_MULT4, (pll_mult >> 4) & 0x01);
	set_ad9852_cr_bit(shadow, AD9852_CR_PLL_MULT3, (pll_mult >> 3) & 0x01);
	set_ad9852_cr_bit(shadow, AD9852_CR_PLL_MULT2, (pll_mult >> 2) & 0x01);
	set_ad9852_cr_bit(shadow, AD9852_CR_PLL_MULT1, (pll_mult >> 1) & 0x01);
	set_ad9852_cr_bit(shadow, AD9852_CR_PLL_MULT0, pll_mult & 0x01);
}

/* shadow is never NULL (locally maintained invariant) */
static void set_op_mode(ad9852_shadow_struct *shadow, unsigned int mode)
{
	/* make sure mode value is in range */
	mode = mode > 4 ? 4 : mode;

	/* set_ad9852_cr_bit takes care of the dirty flag, modifies
	only if necessary */
	set_ad9852_cr_bit(shadow, AD9852_CR_MODE2, (mode >> 2) & 0x01);
	set_ad9852_cr_bit(shadow, AD9852_CR_MODE1, (mode >> 1) & 0x01);
	set_ad9852_cr_bit(shadow, AD9852_CR_MODE0, mode & 0x01);
}

/* Modify the shadow settings to give DDS output "amplitude" (where
MAX_AMPLITUDE corresponds to full output power).

shadow is never NULL (locally maintained invariant)
*/
static void set_amplitude(ad9852_shadow_struct *shadow, double amplitude)
{
	unsigned long osk_mult_setting;

	/* need manual OSK control for this to work */
	set_ad9852_cr_bit(shadow, AD9852_CR_OSK_ENABLE, TRUE);
	set_ad9852_cr_bit(shadow, AD9852_CR_INTERNAL_OSK, FALSE);

	if (shadow->osk_ramp_rate[0] >= 3)
	{
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
	osk_mult_setting = (4095 * amplitude) / MAX_AMPLITUDE;
	/* make sure value is in bounds */
	osk_mult_setting = osk_mult_setting > 4095 ? 4095 : osk_mult_setting;

	if (shadow->osk_multiplier[1] != (unsigned char)(osk_mult_setting >> 8) || shadow->osk_multiplier[0] != (unsigned char)osk_mult_setting)
	{
		/* setting has changed */
		shadow->osk_multiplier[1] = (unsigned char)(osk_mult_setting >> 8);
		shadow->osk_multiplier[0] = (unsigned char)osk_mult_setting;
		shadow->osk_multiplier_dirty = TRUE;
	}
}

/* Modify the DDS shadow registers to set the ramp rate clock for a frequency
sweep that takes delta_time seconds.

sysclk is the DDS system clock frequency in MHz (refclk * PLL multiplier)

shadow is never NULL
*/
static void set_ad9852_ramp_rate(ad9852_shadow_struct *shadow,
								 double delta_time,
								 double sysclk)
{

	unsigned long ramp_rate;
	unsigned char new_ramprate_reg[3];
	int i;

	/* output frequency steps every (ramp_rate + 1) system clock periods
	so we want:
	delta_time * AD9852_GRANULARITY = step time = (ramp_rate + 1) / sysclk

	note the unit conversion from MHz to Hz for sysclk
	*/
	ramp_rate = (sysclk * 1e6) * delta_time * AD9852_GRANULARITY - 1;
	/* casts truncate high bits */
	new_ramprate_reg[2] = (unsigned char)(ramp_rate >> 16);
	new_ramprate_reg[1] = (unsigned char)(ramp_rate >> 8);
	new_ramprate_reg[0] = (unsigned char)ramp_rate;

	for (i = 0; i < 3; ++i)
	{
		if (shadow->ramp_rate_clock[i] != new_ramprate_reg[i])
		{
			shadow->ramp_rate_clock[i] = new_ramprate_reg[i];
			shadow->ramp_rate_clock_dirty = TRUE;
		}
	}
}

static void set_ad9858_ramp_rate(ad9858_shadow_struct *shadow,
								 double delta_time,
								 double sysclk)
{

	unsigned int ramp_rate;
	unsigned char new_ramprate_word[2];
	int i;

	/* output frequency steps every 8*DFRRW system clock periods
	so we want:
	delta_time * AD9858_GRANULARITY = step time = 8*DFRRW / sysclk

	note the unit conversion from MHz to Hz for sysclk
	*/
	ramp_rate = (sysclk * 1e6) * delta_time * AD9852_GRANULARITY / 8;
	/* casts truncate high bits */
	new_ramprate_word[1] = (unsigned char)(ramp_rate >> 8);
	new_ramprate_word[0] = (unsigned char)ramp_rate;

	for (i = 0; i < 2; ++i)
	{
		if (shadow->delta_freq_ramp_rate_word[i] != new_ramprate_word[i])
		{
			shadow->delta_freq_ramp_rate_word[i] = new_ramprate_word[i];
			shadow->delta_freq_ramp_rate_word_dirty = TRUE;
		}
	}
}

/* Take the desired frequency (in MHz) and set the freq_reg to the
corresponding value.

sysclk is the DDS system clock frequency, also in MHz

Return TRUE if freq_reg had to be modified, FALSE otherwise

freq_reg is never NULL
*/
static BOOL ad9852_convert_freq(unsigned char *freq_reg,
								double frequency,
								double sysclk)
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

	for (i = 5, shift_factor = 256; i >= 0; --i, shift_factor *= 256)
	{
		/* cast to unsigned char takes care of modulus and truncation */
		new_byte_val = frequency_ratio * shift_factor;
		if (freq_reg[i] != new_byte_val)
		{
			freq_reg[i] = new_byte_val;
			was_modified = TRUE;
		}
	}
	return was_modified;
}

static BOOL ad9858_convert_freq(unsigned char *freq_reg,
								double frequency,
								double sysclk)
{
	int i;
	long frequency_ratio;
	unsigned char new_byte_val;
	BOOL was_modified = FALSE;

	/* This case is much simpler: the frequency is a 32-bit fraction (out of
	 * sysclk), which is either signed (for the delta-frequency word)
	 * or unsigned (for the frequency tuning words).  Assume 2's complement and
	 * treat everything as signed, since it makes no sense to specify a
	 * frequency above Nyquist (one-half sysclk, corresponding to the maximum
	 * positive signed 32-bit number).
	*/

	frequency_ratio = frequency * 4294967296.0 / sysclk;

	for (i = 0; i < 4; ++i, frequency_ratio >>= 8)
	{
		/* cast to unsigned char takes care of modulus and truncation */
		new_byte_val = (unsigned char)frequency_ratio;
		if (freq_reg[i] != new_byte_val)
		{
			freq_reg[i] = new_byte_val;
			was_modified = TRUE;
		}
	}
	return was_modified;
}

/* sets up the two frequency tuning words, the delta frequency word and the
ramp rate clock register
*/
static void set_ad9852_freq_registers(ad9852_shadow_struct *shadow,
									  double start_frequency,
									  double end_frequency,
									  double ramp_time,
									  double sysclk)
{
	double lowfreq, highfreq, freqstep;

	if (start_frequency < end_frequency)
	{
		lowfreq = start_frequency;
		highfreq = end_frequency;
	}
	else
	{
		lowfreq = end_frequency;
		highfreq = start_frequency;
	}
	freqstep = (highfreq - lowfreq) * AD9852_GRANULARITY;

	/* only set the dirty flags if something was modified */
	set_ad9852_ramp_rate(shadow, ramp_time, sysclk);
	shadow->freq_tuning_word_1_dirty =
		ad9852_convert_freq(shadow->freq_tuning_word_1, lowfreq, sysclk);
	shadow->freq_tuning_word_2_dirty =
		ad9852_convert_freq(shadow->freq_tuning_word_2, highfreq, sysclk);
	shadow->delta_freq_word_dirty =
		ad9852_convert_freq(shadow->delta_freq_word, freqstep, sysclk);
}

static void set_ad9858_freq_registers(ad9858_shadow_struct *shadow,
									  double start_frequency,
									  double end_frequency,
									  double ramp_time,
									  double sysclk)
{
	double freqstep;

	freqstep = (end_frequency - start_frequency) * AD9858_GRANULARITY;

	set_ad9858_ramp_rate(shadow, ramp_time, sysclk);
	shadow->freq_tuning_word_0_dirty =
		ad9858_convert_freq(shadow->freq_tuning_word_0, start_frequency, sysclk);
	shadow->delta_freq_ramp_rate_word_dirty =
		ad9858_convert_freq(shadow->delta_freq_ramp_rate_word, freqstep, sysclk);
}

/* Take a high-level specification dds_options of the behaviour of the
DDS over a time interval dds_options.delta_time and append to cmd_sequence
(with associated "shadow" bank) ADWIN instructions necessary to execute
this behaviour, starting at "time" (measured in event periods)

Return the next available command time
*/
static unsigned long execute_ad9852_settings(dds_cmds_ptr cmd_seq,
											 unsigned long time,
											 ad9852_shadow_struct *shadow,
											 ddsoptions_struct dds_options,
											 double sysclk)
{
	if (dds_options.is_stop)
	{
		/* set 0.0 amplitude and power down unused sections of the DDS
		Note that update_dds won't send anything if the DDS is already
		stopped, so we aren't wasting instructions
		*/
		set_amplitude(shadow, 0.0);
		set_ad9852_cr_bit(shadow, AD9852_CR_FULL_DAC_PD, TRUE);
		set_ad9852_cr_bit(shadow, AD9852_CR_DIGITAL_PD, TRUE);
		update_ad9852(cmd_seq, time, shadow);
		return update_ad9852(cmd_seq, NEXT_AVAILABLE, shadow);
	}
	else
	{ /* not stopped */
		set_ad9852_freq_registers(shadow,
								  dds_options.start_frequency,
								  dds_options.end_frequency,
								  dds_options.delta_time,
								  sysclk);
		set_amplitude(shadow, dds_options.amplitude);
		/* power up necessary sections of the DDS */
		set_ad9852_cr_bit(shadow, AD9852_CR_FULL_DAC_PD, FALSE);
		set_ad9852_cr_bit(shadow, AD9852_CR_DIGITAL_PD, FALSE);

		/* reset the phase and frequency accumulators */
		set_ad9852_cr_bit(shadow, AD9852_CR_CLR_ACCS, TRUE);

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
		if (dds_options.start_frequency < dds_options.end_frequency)
		{
			set_ad9852_cr_bit(shadow, AD9852_CR_COMPARATOR_PD, TRUE);
			update_ad9852(cmd_seq, time, shadow);

			/* unlock the accumulators and toggle the FSK input bit to start
			the ramp to the higher frequency */
			set_ad9852_cr_bit(shadow, AD9852_CR_CLR_ACCS, FALSE);
			set_ad9852_cr_bit(shadow, AD9852_CR_COMPARATOR_PD, FALSE);
			return update_ad9852(cmd_seq, NEXT_AVAILABLE, shadow);
		}
		else
		{ /* descending sweep */
			set_ad9852_cr_bit(shadow, AD9852_CR_COMPARATOR_PD, FALSE);
			update_ad9852(cmd_seq, time, shadow);

			/* unlock the accumulators and toggle the FSK input bit to start
			the ramp to the lower frequency */
			set_ad9852_cr_bit(shadow, AD9852_CR_CLR_ACCS, FALSE);
			set_ad9852_cr_bit(shadow, AD9852_CR_COMPARATOR_PD, TRUE);
			return update_ad9852(cmd_seq, NEXT_AVAILABLE, shadow);
		}
	}
}

static unsigned long execute_ad9858_settings(dds_cmds_ptr cmd_seq,
											 unsigned long time,
											 ad9858_shadow_struct *shadow,
											 ddsoptions_struct dds_options,
											 double sysclk,
											 double freq_offset)
{
	if (dds_options.is_stop)
	{
		/* clear and lock phase accumulator to stop waveform */
		set_ad9858_cr_bit(shadow, AD9858_CR_CLEARPHASE, TRUE);
		/* power-down unused DDS sections */
		set_ad9858_cr_bit(shadow, AD9858_CR_MIXERPD, TRUE);
		set_ad9858_cr_bit(shadow, AD9858_CR_DDSPD, TRUE);
		return update_ad9858(cmd_seq, time, shadow);
	}
	else
	{ /* not stopped */
		/* power up DDS */
		set_ad9858_cr_bit(shadow, AD9858_CR_MIXERPD, FALSE);
		set_ad9858_cr_bit(shadow, AD9858_CR_DDSPD, FALSE);
		/* set ramp rate and frequency registers */
		set_ad9858_ramp_rate(shadow, dds_options.delta_time, sysclk);
		set_ad9858_freq_registers(shadow,
								  dds_options.start_frequency + freq_offset,
								  dds_options.end_frequency + freq_offset,
								  dds_options.delta_time,
								  sysclk);
		/* unlock phase accumulator and start waveform output */
		set_ad9858_cr_bit(shadow, AD9858_CR_CLEARPHASE, FALSE);
		/* reset frequency accumulator */
		set_ad9858_cr_bit(shadow, AD9858_CR_CLEARFREQ, TRUE);
		update_ad9858(cmd_seq, time, shadow);
		/* unlock frequency accumulator to start ramping */
		set_ad9858_cr_bit(shadow, AD9858_CR_CLEARFREQ, FALSE);
		return update_ad9858(cmd_seq, NEXT_AVAILABLE, shadow);
	}
}

/* Create and return a sequence of 1-byte commands to send to the ADWIN at
specified times in order to have the DDS execute the high-level behaviour
specified in dds_settings

The size of the dds_settings array is given by num_settings + 1 (counts from 1 instead of 0)

PLL_multiplier is the desired DDS clock multiplier (the DDS's system clock
will run at PLL_multiplier * refclk)

refclk is the frequency (in MHz) of the DDS's external oscillator

adwin_event_period is the time (in seconds) between adwin update events
*/

//Dave Burns' Additions must be tested still@@
dds_cmds_ptr create_ad9852_cmd_sequence(ddsoptions_struct *dds_settings,
										unsigned long num_settings,
										unsigned int PLL_multiplier,
										double refclk,
										double adwin_event_period)
{
	dds_cmds_ptr cmd_sequence = NULL;
	ad9852_shadow_struct shadow;

	//when set to 1 DDS is Powered Down (0 when on)
	int DDS_PD = 0;

	long i;
	double idealtime;
	double sysclk;

	if (dds_settings == NULL || num_settings < 1)
	{
		return NULL;
	}

	sysclk = refclk * PLL_multiplier;

	cmd_sequence = new_dds_cmd_sequence();

	/* start from a known DDS state */
	ad9852_master_reset(cmd_sequence, 0, &shadow);

	/* changes from default settings: */
	set_PLL_mult(&shadow, PLL_multiplier);
	set_ad9852_cr_bit(&shadow, AD9852_CR_PLL_BYPASS, FALSE);
	set_op_mode(&shadow, MODE_RAMPED_FSK);
	/* we want to control the update clock */
	set_ad9852_cr_bit(&shadow, AD9852_CR_INTERNAL_UPDATE, FALSE);
	//update_ad9852(cmd_sequence, NEXT_AVAILABLE, &shadow);

	idealtime = 0.0;

	/* Normally settings get scheduled at idealtime, corresponding to the
	settings on the front panel.  If this is impossible (because previous
	commands haven't completed), then append_byte will generate a warning.
	However, we _know_ that the first batch of settings will be delayed until
	after the initial reset and configuration, so we handle it specially here
	to avoid the error message.
	*/
	execute_ad9852_settings(cmd_sequence, NEXT_AVAILABLE, &shadow,
							dds_settings[1], sysclk);

	//Edited by Dave Burns -- attempt to eliminate redundant DDS updates
	for (i = 2; i <= num_settings; i++)
	{
		//set the start time based on the start time
		//and delta time of the previous element
		idealtime += dds_settings[i - 1].delta_time;

		//DDS is only updated if it is on or if it is about to be turned on/off
		if (dds_settings[i].is_stop == 0 || DDS_PD == 0)
		{
			if (dds_settings[i].is_stop == 1)
				DDS_PD = 1;

			//Unless all the following are true we update
			if (!((dds_settings[i].start_frequency == dds_settings[i].end_frequency) &&		//constant freq across interval
				  (dds_settings[i].start_frequency == dds_settings[i - 1].end_frequency) && //same as previous end freq
				  (dds_settings[i].amplitude == dds_settings[i - 1].amplitude) &&			//amplitude same as previous
				  (dds_settings[i - 1].is_stop == 0)))										//was previously on (ie not being turned on)
			{
				execute_ad9852_settings(cmd_sequence, idealtime / adwin_event_period,
										&shadow, dds_settings[i], sysclk);
			}
		}
		else if (dds_settings[i].is_stop == 1)
		{
			DDS_PD = 1;
		}
	}

	return cmd_sequence;
}

dds_cmds_ptr create_ad9858_cmd_sequence(ddsoptions_struct *dds_settings,
										unsigned long num_settings,
										double refclk,
										double adwin_event_period,
										double frequency_offset)
{
	dds_cmds_ptr cmd_sequence = NULL;
	ad9858_shadow_struct shadow;

	long i;
	double idealtime;

	if (dds_settings == NULL || num_settings < 1)
	{
		return NULL;
	}

	cmd_sequence = new_dds_cmd_sequence();

	/* start from a known DDS state */
	ad9858_master_reset(cmd_sequence, 0, &shadow);

	/* changes from default settings: */
	set_ad9858_cr_bit(&shadow, AD9858_CR_LOADDELTAFREQ, FALSE);
	set_ad9858_cr_bit(&shadow, AD9858_CR_FREQSWEEP, FALSE);
	set_ad9858_cr_bit(&shadow, AD9858_CR_CPPOLAR, FALSE);
	set_ad9858_cr_bit(&shadow, AD9858_CR_CLKDIV2DIS, TRUE); /*not dividing clock by 2*/
	set_ad9858_cr_bit(&shadow, AD9858_CR_SYNCLKDIS, TRUE);
	set_ad9858_cr_bit(&shadow, AD9858_CR_MIXERPD, TRUE);
	set_ad9858_cr_bit(&shadow, AD9858_CR_PDPD, FALSE); /*turn on phase detector*/

	/*turn on the charge pump*/
	set_ad9858_cr_bit(&shadow, AD9858_CR_FREQDET_CPI0, TRUE);
	set_ad9858_cr_bit(&shadow, AD9858_CR_FINALCLOSED_CPI2, TRUE);
	set_ad9858_cr_bit(&shadow, AD9858_CR_WIDECLOSED_CPI0, TRUE);

	idealtime = 0.0;

	/* Normally settings get scheduled at idealtime, corresponding to the
	settings on the front panel.  If this is impossible (because previous
	commands haven't completed), then append_byte will generate a warning.
	However, we _know_ that the first batch of settings will be delayed until
	after the initial reset and configuration, so we handle it specially here
	to avoid the error message.
	*/
	execute_ad9858_settings(cmd_sequence, NEXT_AVAILABLE, &shadow,
							dds_settings[1], refclk, frequency_offset);

	for (i = 2; i <= num_settings; i++)
	{
		//set the start time based on the start time
		//and delta time of the previous element
		idealtime += dds_settings[i - 1].delta_time;

		execute_ad9858_settings(cmd_sequence, idealtime / adwin_event_period,
								&shadow, dds_settings[i], refclk, frequency_offset);
	}
	return cmd_sequence;
}
