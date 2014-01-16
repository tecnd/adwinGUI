/********************************************************************
 * AD9910_Sweep_Waystation.c
 * By Justin Winkler
 *
 * This program stores bytes necessary for programming the AD9910
 * to perform frequency sweeps.
 *
 * COMMENTS NEED UPDATING FOR NEW VERSION OF CODE
 *
 * Bytes are received from an Ethernet connection and placed in
 * an array.  When triggered, the stored bytes are sent from the
 * Rabbit to the AD9854, programming the DDS to perform
 * sweeps between two frequencies with a desired delta frequency.
 *
 * This program will usually simply listen for a TCP connection.
 * Once a connection is established, the Rabbit can be sent bytes which indicate
 * particular operations for it to perform.  As of this version, there are 4
 * operations that can be performed.
 * The first is to receive data from the Ethernet connection
 * that will be used to program a sweep in the DDS, storing that
 * data in an array.  The second operation is to clear out the array of
 * data for programming the DDS (this would be used if the end user had made
 * a typo and sent incorrect data).  The third operation is to master reset the
 * DDS and program in some default settings to the DDS.  The fourth operation
 * stops the Rabbit from listening for TCP connections, and instead the Rabbit
 * starts listening for a trigger.  When triggered, the Rabbit programs the DDS
 * to perform the stored sweeps, in the order that the Rabbit was sent.  While
 * waiting for a trigger, the Rabbit cannot establish a TCP connection, and
 * this will continue until the Rabbit runs out of stored sweeps to perform.
 *
 * Once the appropriate indicator byte is sent to this program to
 * store bytes for a sweep, this program expects a set number of
 * bytes sent to it from an Ethernet connection, and it expects to receive
 * the information in a particular order.
 * The first byte of data is just a number indicating the direction
 * of the sweep: 1 (or non-zero, really) to indicate a  sweep from
 * a higher frequency to a lower frequency, or 0 to indicate a
 * sweep from low to high.  The next 6 bytes should be the first
 * frequency tuning word (FTW 1), which should be the lower of the
 * two frequencies.  The 6 bytes after that  should be the second
 * frequency tuning word (FTW 2), which is the higher frequency.
 * The final 6 bytes should be the delta frequency word.
 * Naturally, this read in setup leaves the burden of calculating
 * tuning words to the program sending bytes to the Rabbit.
 *
 * As of this version, the DDS can be programmed to perform
 * sweeps in any direction.  The ramp rate that the DDS
 * is set to is kept static, but sweeps can be performed over
 * varying amounts of time by adjusting the delta frequency
 * sent to this program.  Tweaks have been made from version
 * to version in the attempt to decrease the amount of time
 * it takes the Rabbit to setup a sweep after being triggered.
 ********************************************************************/

/********************************************************************
 * Pin Configuration
 * List of connections between the Rabbit and the DDS
 *
 * Port E
 *
 * PE0 -> IO_RESET
 * PE1 -> CSB (keep at logic low for serial data transfer)
 * PE3 -> SCLK
 * PE4 -> SDO (planned, not currently connected,
 					probably won't be used)
 * PE5 -> SDIO
 * PE6 -> DRHOLD
 * PE7 -> DR_CTL
 *
 * Port F
 *
 * PF0 -> P_0
 * PF1 -> P_1
 * PF2 -> P_2
 * PF3 -> DR_OVR (planned, not currently connected)
 * PF4 -> OSK
 * PF5 -> IO_UPDATE
 * PF6 -> EXT_PWR_DWN
 * PF7 -> RESET
 *
 * Port G
 *
 * PG0 -> External Trigger
 ********************************************************************/

#class auto

/****************************************
 * TCP Connection Macros and Constants
 ****************************************/

#define TCPCONFIG 1	// Macro used to setup the Rabbit for TCP communication

// More TCP macros
#define MY_IP_ADDRESS "10.10.6.101"	// IP Address used by Rabbit
#define MY_NETMASK "255.255.255.0" 	// Netmask used by Rabbit
#define MY_GATEWAY "10.10.6.19"		// Gateway used by Rabbit

// TCP relevant constants
#define PORT 1111 // Port to be used by the Rabbit when listening for a connection

/*********************************************
 * AD9910 Register Layout Constants
 *
 * List and definition of the serial
 * addresses of each register and the
 * number of bytes to be sent to each
 * register.  The default register
 * settings are also defined.
 *
 * I'll leave notes with some general
 * information about certain registers.
 *
 * Check the AD9910 manual for more
 * information about the registers.  Rev. A
 * of the manual includes a register map
 * and bit descriptions starting on page 49.
 *
 * Not all of these definitions are used,
 * but I've gone ahead and defined all of
 * them in case they prove useful later.
 *********************************************/

/*********************************************
 * General Control Register Information
 *
 * There are 3 control registers, each
 * 4 bytes long.  These registers determine
 * which features of the AD9910 are in use,
 * each bit or group of bits often serving
 * as a way of selecting specific features.
 * Full details can found in the manual,
 * Rev. A, starting on page 54.  I'll write
 * a summary of the bits that control
 * features of particular interest.  If
 * I haven't mentioned a control bit, then
 * the default is what we want set for
 * that bit at all times, at least for the
 * purposes of this sweep program.
**********************************************/

/*********************************************
 * Control Function Register 1 Information
 * Default settings for CFR1 are
 * <31:24> -> 0x00
 * <23:16> -> 0x00
 * <15:8>  -> 0x00
 * <7:0>   -> 0x00
 *
 * CFR1<23> determines whether OSK is
 * externally controlled.  I am not yet
 * sure whether we will be using OSK,
 * but this could be potentially useful
 * (remember, PE3 connects to the OSK
 * pin on the evaluation board).
 * Default setting is 0, which sets
 * the OSK pin as inoperative.
 *
 * CFR1<16> determines whether the DDS
 * outputs a cosine of sine wave (the
 * default is 0 for cosine).  This could
 * potentially be a handy way of creating
 * a 90 degree phase offset.  Needs
 * investigation.
 *
 * CFR1<15:10> all affect ways that
 * accumulators or timers reset.  Setting
 * one of these bits high usually causes
 * a particular register to reset on an
 * I/O_UPDATE or PROFILE<2:0> change.
 * I suspect most of these can be kept
 * at 0 (default) for normal operation,
 * but activating one or two of
 * these may prove convenient.
 * Testing is needed.
 *
 * CFR1<9:8> are OSK enable and auto OSK
 * control bits (0 is disabled).
 * I still need to see if we'll be
 * using OSK.
 *
 * CFR1<7:3> activates or disables clock
 * signals to certain parts of the DDS,
 * allowing unused parts to be powered
 * down to reduce current drawn by the
 * DDS.  This is a feature that we will
 * probably want to utilize, eventually.
 *********************************************/

// CFR1 - Control Function Register 1
// Serial Address: 0x00
// Bytes Expected: 4
#define CFR1_ADDRESS 0x00
#define CFR1_BYTES_EXPECTED 4

/**************************************************
 * Default settings for CFR2 are
 * <31:24> -> 0x00
 * <23:16> -> 0x40	(SYNC_CLK enabled)
 * <15:8>  -> 0x08	(PDCLK enabled)
 * <7:0>   -> 0x20	(Sync timing validation
 * 			disabled)
 *
 * CFR2<25> controls whether the DROVER pin
 * is in use.  Set this to 1 and the DROVER
 * pin will go high after a sweep completes.
 * This could be used to trigger sweeps
 * immediately after previous sweeps have
 * ended.  This has potential use for testing,
 * at the very least.  May also see later use
 * to help time experiments, though I don't
 * know the details about how timing will work
 * just yet.
 *
 * CFR2<21:20> effectively determines the type
 * of ramp that the DRG will perform.  If these
 * two bits are set to 00, then the DDS signal
 * control parameter that will be ramped is
 * frequency.  Likewise, 01 corresponds to phase
 * and 1x (10 or 11) corresponds to amplitude.
 * Initially, we'll just want to ramp frequency,
 * but setting up phase and amplitude ramps
 * are good features to add at some point.
 * By the way, note that this control
 * scheme means that only one of these
 * control parameters may be ramped at a time
 * (the DDS cannot simulaneously perform
 * a frequency sweep and an amplitude sweep).
 *
 * CFR2<19> is the digital ramp enable bit.
 * Set this to 1 at almost all times (only
 * disable if switching to single tone mode
 * proves necessary).
 *
 * CFR2<11> controls whether the PDCLK pin is
 * enabled or not (0 = disabled, 1 = enabled).
 * PDCLK is meant to serve as a data clock
 * for the parallel data port, which we will not
 * be using.  PDCLK is by default enabled and
 * may as well be turned off, so set this bit
 * to 0.
 *
 * CFR2<7> is the matched latency enable bit,
 * which determines whether or not simultaneous
 * changes to amplitude, phase, and frequency
 * arrive at the output in the order listed or
 * simultaneously.  Default is that the changes
 * arrive at the output in the order listed.
 * Turning on matched latency enable might
 * prove useful.
 ****************************************************/

// CFR2 - Control Function Register 2
// Serial Address: 0x01
// Bytes Expected: 4
#define CFR2_ADDRESS 0x01
#define CFR2_BYTES_EXPECTED 4

/********************************************************
 * Default settings for CFR3 are
 * <31:24> -> 0x1F	(VCO SEL set to 111
 * 			meaning PLL is bypassed)
 * <23:16> -> 0x3F	(I_cp set to 111 meaning
 * 			the charge pump current
 * 			is set to 387 micro Amps)
 * <15:8>  -> 0x40 	(REFCLK input divider is
 * 			selected and operating normally)
 * <7:0>   -> 0x00
 *
 * CFR3<26:24> is the VCO SEL bit.  Appears to affect
 * the frequency range the PLL can achieve.  Will
 * probably need to be set to 101 (VCO5) to achieve
 * the desired 1 GHz sysclock frequency, although the
 * manual suggests that this varies between devices.
 * Testing of the PLL should be done very carefully.
 *
 * CFR3<21:19> selects the charge pump current in
 * the REFCLK PLL.  I am not at all sure if we want
 * to use this or not.
 *
 * CFR3<8> is the PLL enable bit.  Default is 0 to bypass
 * the REFCLK PLL.  This will need to be enabled to
 * reach the desired frequencies, probably.
 *
 * CFR3<7:1> are called N and the 7-bit number
 * is the divide modulus of the REFCLK PLL feedback
 * divider.  I assume this sets the amount that
 * the system frequency is multiplied by
 * but I should double check this with
 * Professor Aubin.
 ********************************************************/

// CFR3 - Control Function Register 3
// Serial Address: 0x02
// Bytes Expected: 4
#define CFR3_ADDRESS 0x02
#define CFR3_BYTES_EXPECTED 4

/********************************************************
 * Auxiliary DAC Control Register Information
 *
 * Default Value is 0x7F (127 in decimal)
 *
 * Although there are 4 bytes assigned to the Auxiliary
 * DAC Control register, 3 bytes are left open.
 * The 8-bits that are used control the "full-scale
 * output current of the main DAC."  In other words,
 * this register can alter the output current according
 * to an equation in the manual (page 23 of Rev. A).
 * May be useful in later phases, but not for
 * early testing.
 *
 * Actually, this should be set up so that
 * the full scale output is low, to prevent
 * the DDS outputing voltages outside of
 * its voltage compliance range.
 *********************************************************/

// Auxillary DAC Control
// Serial Address: 0x03
// Bytes Expected: 4
#define AUX_DAC_CONTROL_ADDRESS 0x03
#define AUX_DAC_CONTROL_BYTES_EXPECTED 4

/************************************************************
 * I/O Update Rate Register informationn
 *
 * Default value is 0xFFFFFFFF.
 *
 * This register controls the rate of I/O updates if
 * I/O updates are being internally generated.  Since
 * I/O updates being externally controlled (CFR2<23> = 0)
 * for this version of code, this register will not be used.
 ************************************************************/

// I/O Update Rate
// Serial Address: 0x04
// Bytes Expected: 4
#define IO_UPDATE_RATE_ADDRESS 0x04
#define IO_UPDATE_RATE_BYTES_EXPECTED 4

/*************************************************************
 * Frequency Tuning Word (FTW) Register Information
 *
 * This is a 32-bit register that controls the output
 * frequency of the DDS.
 *
 * From the manual, the equation that describes the
 * relationship between the FTW and the output frequency
 * is
 *
 * f_out = (FTW*sysclock_frequency) / (2^32)
 *
 * The manual suggests this register is only
 * used during RAM mode, so this register will probably
 * not be used.
 *************************************************************/

// FTW - Frequency Tuning Word
// Serial Address: 0x07
// Bytes Expected: 4
#define FTW_ADDRESS 0x07
#define FTW_BYTES_EXPECTED 4

/*************************************************************
 * Phase Offset Word (POW) Register Information
 *
 * This is a 16-bit register that controls the phase
 * of the output signal.
 *
 * From the manual, the phase offset generated by the
 * DDS is
 *
 * Phase Offset (in Radians) = (2*pi*POW) / (2^16)
 *
 * The manual suggests this register is only
 * used during RAM mode, so this register will probably
 * not be used.
 *************************************************************/

// POW - Phase Offset Word
// Serial Address: 0x08
// Bytes Expected: 2
#define POW_ADDRESS 0x08
#define POW_BYTES_EXPECTED 2

/*************************************************************
 * Amplitude Scale Factor (ASF) Register Information
 *
 * This is a 32-bit register that controls the amplitude
 * of the output signal.
 *
 * The register has 3 parts.
 *
 * ASF<31:16> control the ramp rate of the amplitude,
 * which is used if Output Shift Keying (OSK) is enabled
 * and being automatically controlled.
 *
 * ASF<15:2> is the 14 bit amplitude scale factor.
 * The relevant equation is
 *
 * Amplitude Scale = ASF / (2^14)
 *
 * Amplitude scale being the amplitude as a fraction of the
 * full scale amplitude.  This amplitude scale factor is the
 * most important part of this register.
 *
 * ASF<1:0> controls the amplitude step size and like the
 * ramp rate control is only used if OSK is enabled and
 * being automatically controlled.
 *
 * The manual suggests this register is only
 * used for RAM mode, so this register will probably
 * not be used.
 *************************************************************/

// ASF - Amplitude Scale Factor
// Serial Address: 0x09
// Bytes Expected: 4
#define ASF_ADDRESS 0x09
#define ASF_BYTES_EXPECTED 4

/*************************************************************
 * Multichip Sync Register Information
 *
 * This is a register that controls various functions
 * relating to synchronizing multiple devices.  If I
 * understand the manual, the sunchronization capabilities
 * of the AD9910 are meant for when the parallel ports are
 * in use so I do not believe that this register is useful.
 *************************************************************/

// Multichip Sync
// Serial Address: 0x0A
// Bytes Expected: 4
#define MULTICHIP_SYNC_ADDRESS 0x0A
#define MULTICHIP_SYNC_BYTES_EXPECTED 4

/*************************************************************
 * Digital Ramp Limit Register Information
 *
 * This 64-bit register sets the upper and lower limits of a
 * digital ramp.
 *
 * Bits <63:32> describe the value of the upper limit
 * of the ramp.
 *
 * Bits <31:0> describe the value of the lower limit of
 * the ramp.
 *
 * Presumably the values set to this register
 * should be the FTW, POW, or ASF that corresponds to the
 * desired frequency, phase offset, or amplitude scale limits.
 * Naturally, choosing whether to send a FTW, POW or ASF
 * depends on the type of ramp the DDS will perform as
 * determined by CFR2<21:20>.
 *************************************************************/

// Digital Ramp Limit
// Serial Address: 0x0B
// Bytes Expected: 8
#define DR_LIMIT_ADDRESS 0x0B
#define DR_LIMIT_BYTES_EXPECTED 8

/*************************************************************
 * Digital Ramp Step Size Register Information
 *
 * This 64-bit register sets the increment and decrement
 * step size of a ramp.
 *
 * Bits <63:32> describe the value of the decrement step size.
 *
 * Bits <31:0> describe the value of the increment step size.
 *
 * During a ramp, after a set amount of time has passed,
 * the output parameter is increased or decreased by a set
 * amount.  This register describes the amount the output
 * parameter will be decreased during a downward sweep
 * and increased during an upwards sweep.
 *
 * The step sizes can be calculated using the following
 * equations from the manual:
 *
 * Frequency Step = (M * sysclock_frequency) / (2^32)
 *
 * Phase Step (in radians) = (pi * M) / (2^31)
 *
 * Amplitude Step = M * I_FS / 2^32
 *
 * Where M is the magnitude of the decrement/increment
 * and I_FS is the full-scale output current of the DAC
 * (presumably as controlled by the Auxiliary DAC Control
 * Register).
 *************************************************************/

// Digital Ramp Step
// Serial Address: 0x0C
// Bytes Expected: 8
#define DR_STEP_ADDRESS 0x0C
#define DR_STEP_BYTES_EXPECTED 8

/*************************************************************
 * Digital Ramp Rate Register Information
 *
 * This 32-bit register sets the time interval between
 * increment and decrement values.
 *
 * Bits <31:16> describe the negative slope value that
 * defines the time interval between decrement values.
 *
 * The negative slope step interval is found
 * using the following equation:
 *
 * -dt = 4N / sysclock_frequency
 *
 *  where N is the 16-bit value stored in the ramp rate
 *  register and -dt is the negative slope step interval.
 *
 * Bits <15:0> describe the positive slope value that
 * defines the time interval between increment values.
 *
 * The positive slope step interval is found
 * using the following equation:
 *
 * +dt = 4P / sysclock_frequency
 *
 * where P is the 16-bit value stored in the ramp rate
 * register and +dt is the positive slope step interval.
 *************************************************************/

// Digital Ramp Rate
// Serial Address: 0x0D
// Bytes Expected: 4
#define DR_RATE_ADDRESS 0x0D
#define DR_RATE_BYTES_EXPECTED 4

/********************************************************
 * Profile Register Information
 *
 * The single tone and RAM profiles
 * share serial addresses.  The manual notes that
 * RAM profiles are in effect when CFR1<31> = 1
 * and the single tone profiles are in effect when
 * CFR1<31> = 0, CFR2<19> = 0, and CFR2<4> = 0.
 * See AD9910 manual, Rev. A, page 60 for details.
 *
 * My understanding is that the profiles can be
 * easily switched between using the P0 - P2 pins
 * and allow a user of the DDS to preprogram
 * waveforms using the single tone or RAM mode.
 *
 * According to the manual, when a data parameter
 * (frequency, phase, or amplitude) is selected
 * by the DRG to be ramped, the other two parameters
 * are supplied by the registers of the active profile.
 * This means the profiles must be utilized.
 *
 * A profile is 64-bits.  The bit descriptions are
 * as follows:
 *
 * <63:62> are open.
 *
 * <61:48> designate the amplitude scale factor (ASF).
 *
 * As stated above,
 *
 * Amplitude Scale = ASF / (2^14)
 *
 * Amplitude scale being the amplitude as a fraction of the
 * full scale amplitude.
 *
 * <47:32> designate the phase offset word (POW).
 *
 * As stated above, the phase offset generated by the
 * DDS is
 *
 * Phase Offset (in Radians) = (2*pi*POW) / (2^16)
 *
 * <31:0> designate the frequency tuning word (FTW).
 *
 * As stated above, the equation that describes the
 * relationship between the FTW and the output frequency
 * is
 *
 * f_out = (FTW*sysclock_frequency) / (2^32)
 *********************************************************/

// Single Tone Profile 0
// Serial Address: 0x0E
// Bytes Expected: 8
#define ST_PROFILE_0_ADDRESS 0x0E
#define ST_PROFILE_0_BYTES_EXPECTED 8

// RAM Profile 0
// Serial Address: 0x0E
// Bytes Expected: 8
#define RAM_PROFILE_0_ADDRESS 0x0E
#define RAN_PROFILE_0_BYTES_EXPECTED 8

// Single Tone Profile 1
// Serial Address: 0x0F
// Bytes Expected: 8
#define ST_PROFILE_1_ADDRESS 0x0F
#define ST_PROFILE_1_BYTES_EXPECTED 8

// RAM Profile 1
// Serial Address: 0x0F
// Bytes Expected: 8
#define RAM_PROFILE_1_ADDRESS 0x0F
#define RAN_PROFILE_1_BYTES_EXPECTED 8

// Single Tone Profile 2
// Serial Address: 0x10
// Bytes Expected: 8
#define ST_PROFILE_2_ADDRESS 0x10
#define ST_PROFILE_2_BYTES_EXPECTED 8

// RAM Profile 2
// Serial Address: 0x10
// Bytes Expected: 8
#define RAM_PROFILE_2_ADDRESS 0x10
#define RAN_PROFILE_2_BYTES_EXPECTED 8

// Single Tone Profile 3
// Serial Address: 0x11
// Bytes Expected: 8
#define ST_PROFILE_3_ADDRESS 0x11
#define ST_PROFILE_3_BYTES_EXPECTED 8

// RAM Profile 3
// Serial Address: 0x11
// Bytes Expected: 8
#define RAM_PROFILE_3_ADDRESS 0x11
#define RAN_PROFILE_3_BYTES_EXPECTED 8

// Single Tone Profile 4
// Serial Address: 0x12
// Bytes Expected: 8
#define ST_PROFILE_4_ADDRESS 0x12
#define ST_PROFILE_4_BYTES_EXPECTED 8

// RAM Profile 4
// Serial Address: 0x12
// Bytes Expected: 8
#define RAM_PROFILE_4_ADDRESS 0x12
#define RAN_PROFILE_4_BYTES_EXPECTED 8

// Single Tone Profile 5
// Serial Address: 0x13
// Bytes Expected: 8
#define ST_PROFILE_5_ADDRESS 0x13
#define ST_PROFILE_5_BYTES_EXPECTED 8

// RAM Profile 5
// Serial Address: 0x13
// Bytes Expected: 8
#define RAM_PROFILE_5_ADDRESS 0x13
#define RAN_PROFILE_5_BYTES_EXPECTED 8

// Single Tone Profile 6
// Serial Address: 0x14
// Bytes Expected: 8
#define ST_PROFILE_6_ADDRESS 0x14
#define ST_PROFILE_6_BYTES_EXPECTED 8

// RAM Profile 6
// Serial Address: 0x14
// Bytes Expected: 8
#define RAM_PROFILE_6_ADDRESS 0x14
#define RAN_PROFILE_6_BYTES_EXPECTED 8

// Single Tone Profile 7
// Serial Address: 0x15
// Bytes Expected: 8
#define ST_PROFILE_7_ADDRESS 0x15
#define ST_PROFILE_7_BYTES_EXPECTED 8

// RAM Profile 7
// Serial Address: 0x15
// Bytes Expected: 8
#define RAM_PROFILE_7_ADDRESS 0x15
#define RAN_PROFILE_7_BYTES_EXPECTED 8

/******************************************************
 * Miscellaneous macros and definitions
 ******************************************************/

#memmap xmem
#use dcrtcp.lib

// Row and column length of the 2-D command list.
// I've simply opted to use a static array for storing
// data in the Rabbit, putting a limit on how many sweeps
// can be queued in the Rabbit.
#define CL_COLUMN_LEN 20	// Number of events that can be triggered
#define CL_ROW_LEN 15		// Number of characters stored per event

// Some command list formatting numbers
// These are meant for navigating the command list with a for loop.

// This version doesn't use these much.  They aren't, strictly speaking, necessary,
// though do make for good style when using them to navigate the
// command list with a for loop.  To widdle down the time needed for
// programming a sweep, the for loops were removed in favor of
// hardcoding the array indices accessed, a technique which is slightly faster.
#define DIR_INDEX 0	// Indicates index of command list where the direction should be stored
#define FTW_1_START 1   // Indicates index of command list where the 6 bytes of FTW 1 start
#define FTW_2_START 5	// Indicates index of command list where the 6 bytes of FTW 2 start
#define DFW_START 9    // Indicates index of command list where the 6 bytes of delta frequency word start

// When receiving data from the Ethernet connection, if these bytes are received
// they indicate an operation to be performed by the Rabbit.
#define COMMAND_RECEIVE 0x00	// Indicates that the next CL_ROW_LEN worth of bytes from the connection
                                // are to be stored for programming frequency sweeps
#define COMMAND_CLEAR 0x01	// Empties the command list of any stored data.
#define COMMAND_SETUP_DDS 0x02	// Master restarts the DDS and programs some initial settings for the DDS
#define COMMAND_FINALIZE 0xff   // Switch from listening for a connection to listening for a trigger.

char commandlist[CL_COLUMN_LEN][CL_ROW_LEN]; 	// Where all bytes used to program a sweep will be stored
int CLused;					// Value of how many sweeps have been stored
int CLwrInd;				// When writing to the command list, indicates which column to write to
int CLrdInd;				// When reading from the command list, indicates which colum to read from

tcp_Socket commandsock;		// Socket used for connections


/**********************************************
 * sendByte
 *
 * Sends a single byte to the DDS.
 *
 * Return value: None
 *
 * Parameters:
 *	oneByte - A character that has the value
 *	of the byte to be sent to the DDS.
 *
 * Function Details:
 * The unsigned character oneByte is read using
 * bitwise operations and used to set the SDIO
 * pin to each bit value of oneByte (the pin
 * is set high or low).  The rising edge of
 * the SCLK pin causes the DDS to register the
 * bit value currently set for SDIO, and doing
 * this 8 times sends a byte.
 *
 * Default operation of the AD9910 reads bytes
 * MSB-first(big endian) and this function is
 * written assuming the AD9910 is setup to
 * read MSB-first.
 **********************************************/
void sendByte(unsigned char oneByte)
{
   int c;

   // I've written this routine partially using assembler, for the sake of speed.
   // WARNING: This routine does not update the shadow registers for port E or F.

   // For whatever reason, the following assembler routine did not work after mounting
   // the DDS and the Rabbit to the box.  The C version of this function worked fine,
   // so that will be used for now.

   // Regardless, I'm not very familiar with assembler, so the assembler version needs to be
   // improved anyways.

/*
	for(c = 7; c >= 0; c--)	// Read through all 8 bits of oneByte, starting at the most significant byte
   {
   	if((oneByte >> c) & 01) // If the c-th bit of oneByte is 1, set SDIO to 1...
      {
	   	#asm
        		set	5,a						; set bit 0 only (SDIO)
				ioi	ld (PEDR),a			; write data to port f
		#endasm
      }
      else							// ...otherwise set SDIO to 0
      {
      	#asm
         	res	5,a						; clear bit 0 only (SDIO)
				ioi	ld (PEDR),a				; write data to port f
         #endasm
      }
      // Toggle SCLK so the DDS registers the value of SDIO
      #asm
      set	3,a						; set bit 6 only (SCLK)
		ioi	ld (PEDR),a				; write data to port e

      res	3,a						; clear bit 6 only (SCLK)
		ioi	ld (PEDR),a				; write data to port e
      #endasm
   }
*/

   // The below code peforms the same operation, but uses pure
   // C (slower), rather than assembler (faster)

   // Note that the print commands should only ever be used for debugging,
   // as they slow down the following routine even more than usual

   //Print Hex Value of Sent Byte
//   printf("Sending Byte, Hex Value: %x\n", oneByte);



   for(c = 7; c >= 0; c--)
   {
   	// SDIO is connected to PE5
      BitWrPortI(PEDR, &PEDRShadow, (oneByte >> c) & 01, 5);	// Input to SDIO

      // Print Binary Value of Sent Byte
//      printf("%d", (oneByte >> c) & 01);

      // SCLK is connected to PE3
      BitWrPortI(PEDR, &PEDRShadow, 1, 3);						// Toggle SCLK
      BitWrPortI(PEDR, &PEDRShadow, 0, 3);
   }


//   printf("\n");

}

/*************************************************************
 * boardSetup
 *
 * Sets up the value and operation of the Rabbit's pin outs.
 *
 * Return value: None
 *
 * Parameters: None
 *
 * Function Details: This is a function that is called once,
 * at the beginning of the program.  It's simply used
 * for initial pin settings.
 *
 * As of this version, only Ports E, F, and potentially G are
 * used, so those are the only ports that need to be configured.
 * All pins except pin 3 of port F and pin 0 of port G are set
 * to output, and all pins are set low initially
 * (not that this effects the input pins PF3 and PG0).
 *************************************************************/
void boardSetup()
{

   // Configure Port E
   WrPortI(PECR, &PECRShadow, 0x00);       	// clear all bits to pclk/2
   WrPortI(PEFR, &PEFRShadow, 0x00);       	// clear all bits to normal function
   WrPortI(PEDDR, &PEDDRShadow, 0xFF);    	// set all bits to output
   WrPortI(PEDR, &PEDRShadow, 0x00);       	// set all bits output low

   // Configure Port F
   WrPortI(PFCR, &PFCRShadow, 0x00);		// clear all bits for pclk/2
   WrPortI(PFFR, &PFFRShadow, 0x00);		// clear all bits for normal function
   WrPortI(PFDCR, &PFDCRShadow, 0x00);		// set all bits to totem-pole output
   WrPortI(PFDDR, &PFDDRShadow, 0xFB);		// set all bits but 3 to output
   						// set bit 3 to input
   WrPortI(PFDR, &PFDRShadow, 0x00);		// set all bits low

   // Configure Port G
   WrPortI(PGCR, &PGCRShadow, 0x00);		// clear all bits for pclk/2
   WrPortI(PGFR, &PGFRShadow, 0x00);		// clear all bits for normal function
   WrPortI(PGDCR, &PGDCRShadow, 0x00);		// set all bits to totem-pole output
   WrPortI(PGDDR, &PGDDRShadow, 0xFE);		// set all bits but 0 to output
						// set bit 0 to input
   WrPortI(PGDR, &PGDRShadow, 0x00);		// set all bits low

//   printf("Board Setup\n");
}

/*************************************************************
 * ioReset
 *
 * Toggles the IO RESET pin on the DDS.
 *
 * Return value: None
 *
 * Parameters: None
 *
 * Function Details: A simple helper function.  This
 * toggles PE0 high then low.  PE0 is connected to the
 * IO_RESET pin of the DDS.  Calling this function
 * thus restarts the input cycle.
 *************************************************************/
void ioReset()
{    //changed port number to usewith the FPGA****JH
   BitWrPortI(PEDR, &PEDRShadow, 1, 7);
   BitWrPortI(PEDR, &PEDRShadow, 0, 7);

//   printf("IO Reset Toggled\n");
}

/*************************************************************
 * masterReset
 *
 * Toggles the MRESET pin on the DDS.
 *
 * Return value: None
 *
 * Parameters: None
 *
 * Function Details: A simple helper function.  This
 * toggles PF7 high then low.  PF7 is connected to the
 * RESET pin of the DDS (the master reset control).
 * Calling this function thus restarts the DDS,
 * clearing all memory elements and resetting all registers
 * to their default values.
 *************************************************************/

void masterReset()
{
   BitWrPortI(PFDR, &PFDRShadow, 1, 7);
   BitWrPortI(PFDR, &PFDRShadow, 0, 7);

//   printf("Master Reset Toggled\n");
}

/*************************************************************
 * toggleUpdateClock
 *
 * Toggles the I/O UD pin on the DDS.
 *
 * Return value: None
 *
 * Parameters: None
 *
 * Function Details: A simple helper function.  This
 * toggles PF5 high then low.  PF5 is connected to the
 * IO_UPDATE pin of the DDS.  This function thus controls
 * the update clock when the update clock is set to
 * be externally generated (which currently is the case for
 * this program).  A rising edge of the update clock
 * transfers all buffered binary input to the
 * registers of the DDS.
 *************************************************************/

void toggleUpdateClock()
{
   BitWrPortI(PFDR, &PFDRShadow, 1, 5);
   BitWrPortI(PFDR, &PFDRShadow, 0, 5);

//   printf("Update Clock Toggled\n");
}

/*************************************************************
 * DDSinitialSetup
 *
 * MAJOR REWRITE NEEDED FOR NEW VERSION
 *
 * Restarts the DDS and sets the control register and
 * various other registers to desired starting values.
 *
 * Return value: None
 *
 * Parameters: None
 *
 * Function Details: This function is called whenever the user
 * wishes to reset the DDS.  The function resets the DDS and
 * then programs the control register, FTW 1 register, and
 * ramp rate to useful initial settings.
 *
 * This function turns off the AND gate providing the
 * reference clock to the DDS.  This is done because
 * when the DDS is reset, it starts generating the
 * update clock internally.  Thus, the reference clock
 * needs to be disconnected to set up the Rabbit to
 * provide update clock as an external source.
 * When the reference clock is disconnected, the update clock
 * doesn't tick, preventing data transfers from being
 * prematurely sent to the registers, which would cause
 * data to be only partially transferred.
 * When the bytes to set the control register are sent to the DDS,
 * the bytes are all buffered while the DDS is unclocked.
 * Only once the reference clock is reconnected are these
 * buffered bytes then transferred to the register,
 * which then puts the control of the update clock
 * in the hands of the Rabbit.
 *************************************************************/

void DDSinitialSetup()
{
   masterReset();
	ioReset();
  /******* turning off message sent by rabbit to initialize for FPGA
   //printf("Setting intial control register...\n");

   // Leaving ramp rate variable, to be determined by used

   //	printf("Setting hardcoded ramp rate...\n");
	// Hardcoded ramp rate
   // Ramp rate is set to 1 for both directions
//	sendByte(DR_RATE_ADDRESS);
//	sendByte(0x00);
//	sendByte(0x0A);
//	sendByte(0x00);
//	sendByte(0x0A);

   // Clear contents of ramp limit registers
   sendByte(DR_LIMIT_ADDRESS);
   sendByte(0x00);
   sendByte(0x00);
   sendByte(0x00);
   sendByte(0x00);
   sendByte(0x00);
   sendByte(0x00);
   sendByte(0x00);
   sendByte(0x00);

	sendByte(CFR1_ADDRESS);	// Address of CFR1   */
	sendByte(0x00);	// AD9910 default
	sendByte(0x00);	// AD9910 default
  /* 	sendByte(0x40);	// Autoclear digital ramp accumulator
	sendByte(0x00);	// AD9910 default

//	sendByte(CFR2_ADDRESS);	// Address of CFR2
//	sendByte(0x00);	// AD9910 default
//	sendByte(0x40);	// AD9910 default
//	sendByte(0x00);	// Disable PDCLK
//	sendByte(0x20);	// AD9910 default

	sendByte(CFR2_ADDRESS);	// Address of CFR2
	sendByte(0x00);	// AD9910 default
	sendByte(0x48);	// Enable digital ramp
							// Set to ramp frequency
	sendByte(0x00);	// Disable PDCLK
	sendByte(0x20);	// AD9910 default

	// Setting for control register 3 without
   // using multiplier
/*
	sendByte(CFR3_ADDRESS);	// Address of CFR3
	sendByte(0x1F);   // AD9910 default
	sendByte(0x3F);	// AD9910 default
	sendByte(0xC0);	// Bypass Input Divider
	sendByte(0x00);	// AD9910 default
*/


	// Setting for control register 3
   // using multiplier
 /****conintuint turn off init sdio
	sendByte(0x02);	// Address of CFR3
	sendByte(0x35);   // DRV0 set so that REFCLK_OUT outputs a high current
   						// Use VCO5 for PLL output range of 820-1150 MHz
	sendByte(0x3F);	// AD9910 default
	sendByte(0xC1);	// Enable PLL, bypass input divider
	sendByte(0xC8);	// Set N to 100


   // Set the full scale output current
   // through this register.
	// I'm setting this as low as possible.
   sendByte(AUX_DAC_CONTROL_ADDRESS);
   sendByte(0x00);
	sendByte(0x00);
	sendByte(0x00);
	sendByte(0x00);

	toggleUpdateClock();	// Transfer all of these commands to the registers of
   							// the DDS **(Update

	BitWrPortI(PEDR, &PEDRShadow, 1, 0);     //Sets DR_CTL     */
}

main() {
	char bytein;	// Used for reading bytes from the Ethernet connection
	int status;		// Integer used for TCP functions
   int listen;		// Used to indicate whether the Rabbit is listening for an Ethernet connection or not
   int c, r;		// Used for for loops

	listen = 1;		// Start off listening for an Ethernet connection

   boardSetup();

   // Clear the command list to all zeros
   for(c = 0; c < CL_COLUMN_LEN; c++)
   {
   	for(r = 0; r < CL_ROW_LEN; r++)
      {
      	commandlist[c][r] = 0;
      }
   }

  	CLused = 0;		// The command list is considered empty
   CLwrInd = 0;	// Start writing at index zero
   CLrdInd = 0;   // Start reading from index zero

	DDSinitialSetup();

   sock_init();

   while(1)
   {
   	while(listen)
      {
         // Functions used for listening for an Ethernet connection

         printf("Listening for a connection\n");
		 	tcp_listen(&commandsock,PORT,0,0,NULL,0);
		 	sock_wait_established(&commandsock,0,NULL,&status);

		 	printf("Receiving incoming connection\n");
			sock_mode(&commandsock,TCP_MODE_ASCII);

	   	while(tcp_tick(&commandsock))
			{
   			sock_wait_input(&commandsock,0,NULL,&status);	// Wait for input

				bytein = sock_getc(&commandsock);	// Get a character from the socket

            printf("Received byte: %x\n", bytein);

        		// Process bytein
	   	   switch(bytein)
		      {
		      	case COMMAND_RECEIVE:			// Fills current row with bytes sent through Ethernet
		         	if(CLused < CL_COLUMN_LEN)	// Only read in bytes if the command list is not full
		            {
                  	printf("Filling Command List Row\n");

                     // Get CL_ROW_LEN characters from the socket
		               for(r = 0; r < CL_ROW_LEN; r++)
		               {
					   		sock_wait_input(&commandsock,0,NULL,&status);		// Wait for input
                        commandlist[CLwrInd][r] = sock_getc(&commandsock);	// Get a character from the socket
                        printf("Received byte: %x\n", commandlist[CLwrInd][r]);
	   	            }

                     CLused++;	// Indicate that another row of the socket has been filled

	                  // Have command list write index indicator circle through command list
	                  if(CLwrInd == CL_COLUMN_LEN - 1)
	                  {
	                  	CLwrInd = 0;
	                  }
	                  else
	                  {
	                  	CLwrInd++;
	                  }
	   	         }
	               else
	               {
	               	printf("Command List is Full\n");

                     // Get the next CL_ROW_LEN characters from the socket,
                     // but do nothing with them.
                     // This prevents the switch statement from processing
                     // bytes meant to be stored in the command list.
                     for(r = 0; r < CL_ROW_LEN; r++)
                     {
                     	sock_wait_input(&commandsock, 0, NULL, &status);
                        sock_getc(&commandsock);
                     }
	               }
	               break;

	            case COMMAND_CLEAR:		// Clear command list
               	printf("Clearing Command List\n");

                  // Clears the command list and all command list
                  // related variables to zero
	            	for(c = 0; c < CL_COLUMN_LEN; c++)
	   				{
	   					for(r = 0; r < CL_ROW_LEN; r++)
	      				{
	      					commandlist[c][r] = 0;
	      				}
	   				}

	  					CLused = 0;
	   				CLwrInd = 0;
	   				CLrdInd = 0;
	               break;
               case COMMAND_SETUP_DDS:	// Effectively restart the DDS and program a few initial settings
               	DDSinitialSetup();
               	break;
               case COMMAND_FINALIZE:	// Switch the DDS over to listening for a digital trigger,
               								// rather than an Ethernet connection
               	listen = 0;
	            default: 	// Do nothing for unspecified cases
	            	break;
            }
			}

			sock_err:
		  		switch(status)		// Handle errors
	   		{
		      	case 1: /* foreign host closed */
						printf("User closed session\n");
				  		break;
		      	case -1: /* time-out */
						printf("\nConnection timed out\n");
						break;
				}
   	}
      while(!listen)
      {
      	costate
         {
            //printf("Current logic value of Port G, pin 0: %d\n", BitRdPortI(PGDR, 0));
          //  if (!BitRdPortI(PGDR, 0))      // Wait for trigger (read PG0)
          //		abort;
	       //	waitfor(DelayMs(1));          // Waits 1 milliseconds to see if the trigger is released.
            										 // This sort of wait will be removed when triggered by the ADWIN,
                                           // but I'm currently triggering this manually for development.
   	  	 	//if (BitRdPortI(PGDR, 0))
      	 	//{

            // My attempt at getting the Rabbit to trigger on a rising edge.
            // The idea is to stall the program in while loops
            // until the trigger line goes from low to high.
            while(BitRdPortI(PGDR, 0));
				while(!BitRdPortI(PGDR, 0));

         		//printf("Trigger received...\n");
         		if(CLused >= 1)
            	{
                  /*
                  // Print out command list row to be sent, for debugging purposes
						for(c = 0; c < 19; c++)
						{
							if(c == 0)
							{
								if(commandlist[CLrdInd][c])
								{
									printf("Sweep from high to low");
								}
								else
								{
									printf("Sweep from low to high");
								}
							}
							else
							{
								if(c == FTW_1_START)
								{
									printf("\nFTW 1\n");
								}
								if(c == FTW_2_START)
								{
									printf("\nFTW 2\n");
								}
								if(c == DFW_START)
								{
									printf("\nDFW\n");
								}
								printf("%x ", commandlist[CLrdInd][c]);
							}
						}
						printf("\n\n");
                  */

                  if(commandlist[CLrdInd][0])
                  {
                  	// Handle sweep from high to low
                       BitWrPortI(PEDR, &PEDRShadow, 1, 1);

                     // Send step size to DDS
	                  // Will set decrement size
	                  // as designated by index 9 through 12
                     // Increment size will be
                     // as large as possible
	                  sendByte(DR_STEP_ADDRESS);		// Address for step size
	                  // Set decrement step size
	                  sendByte(commandlist[CLrdInd][9]);
	                  sendByte(commandlist[CLrdInd][10]);
	                  sendByte(commandlist[CLrdInd][11]);
	                  sendByte(commandlist[CLrdInd][12]);
	                  // Set increment step size
                     // Set to largest possible step to send DDS to upper bound
	               	sendByte(0xFF);
	                  sendByte(0xFF);
	                  sendByte(0xFF);
	                  sendByte(0xFF);

                     // Set ramp rate
                    	sendByte(DR_RATE_ADDRESS);
                     // Set ramp negative slope size
							sendByte(commandlist[CLrdInd][13]);
							sendByte(commandlist[CLrdInd][14]);
                     // Set ramp positive slope size
                     // Set to smallest possible time step to
                     // send DDS to upper bound as quickly as possible
							sendByte(0x00);
							sendByte(0x01);


                     //****THIS SECTION WAS CHANGED TO TEST A A SOLUTION**/
                     /*
	                  // Set both ramp limits to starting frequency
							sendByte(DR_LIMIT_ADDRESS);	// Address for ramp limits
	                  // Set upper limit to higher frequency
						   sendByte(commandlist[CLrdInd][1]);
						   sendByte(commandlist[CLrdInd][2]);
						   sendByte(commandlist[CLrdInd][3]);
						   sendByte(commandlist[CLrdInd][4]);
							// Set lower limit to higher frequency
						   sendByte(commandlist[CLrdInd][1]);
						   sendByte(commandlist[CLrdInd][2]);
						   sendByte(commandlist[CLrdInd][3]);
						   sendByte(commandlist[CLrdInd][4]);


  	                 	toggleUpdateClock();  */

                  	// Set both ramp limits to appropriate limit frequencies
							sendByte(DR_LIMIT_ADDRESS);	// Address for ramp limits
	                  // Set upper limit to higher frequency
						   sendByte(commandlist[CLrdInd][1]);
						   sendByte(commandlist[CLrdInd][2]);
						   sendByte(commandlist[CLrdInd][3]);
						   sendByte(commandlist[CLrdInd][4]);
							// Set lower limit to lower frequency
						   sendByte(commandlist[CLrdInd][5]);
						   sendByte(commandlist[CLrdInd][6]);
						   sendByte(commandlist[CLrdInd][7]);
						   sendByte(commandlist[CLrdInd][8]);

		               toggleUpdateClock();

   	               // Set DRCTL to start high
	                  BitWrPortI(PEDR, &PEDRShadow, 1, 0);

							// Set DRCTL down to begin sweep from high to low
							BitWrPortI(PEDR, &PEDRShadow, 0, 0);
                  }
                  else
                  {
                  	// Handle sweep from low to high
                     	 BitWrPortI(PEDR, &PEDRShadow, 0, 1);
                     // Send step size to DDS
	                  // Will set increment size
	                  // as designated by index 9 through 12
							// Decrement size will be
                     // as large as possible
	                  sendByte(DR_STEP_ADDRESS);		// Address for step size
	                  // Set decrement step size
                     // Set as big as possible to send DDS to lower bound
	                  sendByte(0xFF);
	                  sendByte(0xFF);
	                  sendByte(0xFF);
	                  sendByte(0xFF);
	                  // Set increment step size
	               	sendByte(commandlist[CLrdInd][9]);
	                  sendByte(commandlist[CLrdInd][10]);
	                  sendByte(commandlist[CLrdInd][11]);
	                  sendByte(commandlist[CLrdInd][12]);

                  	// Set ramp rate
                    	sendByte(DR_RATE_ADDRESS);
                     // Set ramp negative slope size
                     // Set to smallest possible time step to
                     // send DDS to lower bound as quickly as possible
							sendByte(0x00);
							sendByte(0x01);
                     // Set ramp positive slope size
                     sendByte(commandlist[CLrdInd][13]);
							sendByte(commandlist[CLrdInd][14]);

                     /*
	                  // Set both ramp limits to starting frequency
							sendByte(DR_LIMIT_ADDRESS);	// Address for ramp limits
	                  // Set upper limit to lower frequency
						   sendByte(commandlist[CLrdInd][5]);
						   sendByte(commandlist[CLrdInd][6]);
						   sendByte(commandlist[CLrdInd][7]);
						   sendByte(commandlist[CLrdInd][8]);
							// Set lower limit to lower frequency
						   sendByte(commandlist[CLrdInd][5]);
						   sendByte(commandlist[CLrdInd][6]);
						   sendByte(commandlist[CLrdInd][7]);
						   sendByte(commandlist[CLrdInd][8]);
                     */

	                 	//toggleUpdateClock();

                  	// Set both ramp limits to appropriate limit frequencies
							sendByte(DR_LIMIT_ADDRESS);	// Address for ramp limits
	                  // Set upper limit to higher frequency
						   sendByte(commandlist[CLrdInd][1]);
						   sendByte(commandlist[CLrdInd][2]);
						   sendByte(commandlist[CLrdInd][3]);
						   sendByte(commandlist[CLrdInd][4]);
							// Set lower limit to lower frequency
						   sendByte(commandlist[CLrdInd][5]);
						   sendByte(commandlist[CLrdInd][6]);
						   sendByte(commandlist[CLrdInd][7]);
						   sendByte(commandlist[CLrdInd][8]);

	                 	toggleUpdateClock();

							// Set DRCTL to start low
							BitWrPortI(PEDR, &PEDRShadow, 0, 0);

							// Set DRCTL high to begin sweep from low to high
							BitWrPortI(PEDR, &PEDRShadow, 1, 0);
                  }

	               // Have command list read index indicator circle through command list
   	         	if(CLrdInd == CL_COLUMN_LEN - 1)
      	         {
         	      	CLrdInd = 0;
            	   }
               	else
	               {
   	            	CLrdInd++;
      	         }

         	      // The row of command list that has just been read is not emptied
            	   // but is free to be written over.  Value of CLused prevents
               	// reusing the row until it has been written over.
	               CLused--;

                  // When the all queued sweeps have been programmed,
                  // switch back to listening for an Ethernet connection
                  if(CLused == 0)
                  {
                     printf("Command List is Empty\n");
	         	      listen = 1;
                  }

            	}
               // In case the Rabbit was switched to listening for a trigger
               // when the command list was empty, switch back to listen
               // for an Ethernet connection.
               else
               {
               		printf("Command List is Empty\n");
	         	      listen = 1;
               }
		      //}
   	   }
      }
	}
}