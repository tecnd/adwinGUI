
/************************************************************************
File Name: ddstranslator.h
-------
Author: David McKay, Ultra Cold Atoms Lab
-------
Date Created: July 22, 2004
-------
Date Modified: July 26, 2004
-------
Description: Header file for parsing DDS  commands to the ADWIN 
-------
*************************************************************************/


/************************************************************************
Header File Protection
*************************************************************************/
#ifndef DDSTRANS_H
#define DDSTRANS_H
 

/************************************************************************
Includes
*************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "vars.h"

/************************************************************************
Macro Definitions
*************************************************************************/

#define NUM_CHANNELS 3
#define TIME_INTERVAL 0.000005
#define STEP_GRANULARITY 0.0001 /* 0.01 % */ 
#define AMPLIFICATION_CONST 234 
#define INDUCTANCE_CONST 0.0000001
#define CAPACITANCE_CONST 0.00001
#define IMAX	5 /*10 mA*/
#undef DEBUG_MODE

/*address instruction bytes*/
#define ADDR_FREQ1	0x02
#define ADDR_FREQ2	0x03
#define ADDR_DELTAFREQ	0x04
#define ADDR_RAMPRATE	0x06
#define ADDR_CTRLREG	0x07
#define ADDR_CURRENT	0x08


#define IORESET		4
#define UPDATECLK	5
#define MASTERRESET	6

/************************************************************************
Structure/Typedef Declarations
*************************************************************************/


typedef struct adwin_cmd_struct {
	long cmd_time;
	unsigned int cmd_value;
} ADWIN_CMD;
		
/************************************************************************
Function Declarations
*************************************************************************/

void createCommandArray(DDSOPTIONS** dds_settings, unsigned long num_settings, unsigned long num_pages, unsigned int PLL_multiplier, float external_clock);
int getValue(unsigned long valuetime); 
unsigned long parse_cmd_byte(unsigned int instr_byte, unsigned long cur_cmd_time, BOOL split_byte);
void destroyCommandArray(void);
unsigned long write_to_register(unsigned long cmd_time, unsigned int addr_byte, unsigned int num_bytes, unsigned int *data_bytes);
void update_frequency(DDSOPTIONS *cur_dds_options);
unsigned long write_frequency_word(float frequency_value, unsigned int addr_byte, unsigned long cmd_time);
unsigned long set_ramp_rate(unsigned long cmd_time,  float delta_time);
unsigned long set_current_value(unsigned long cmd_time, float target_current, float cur_frequency);
int searchArray(unsigned long start_index, unsigned long end_index, unsigned long valuetime);

#endif
