/************************************************************************
File Name: ddstranslator.h
-------
Original Author: David McKay, Ultra Cold Atoms Lab
-------
Date Created: July 22, 2004
-------
Date Modified: December 2, 2004 (Ian Leroux)
-------
Description: Header file for translation of high-level DDS settings to
ADWIN sequencer commands
-------
*************************************************************************/

#ifndef DDSTRANS_H
#define DDSTRANS_H
#include "vars.h"

/* frequency ramps are done in 10000 steps */
#define STEP_GRANULARITY 0.0001
/* DDS amplitude settings in the user interface are on a percent scale */
#define MAX_AMPLITUDE	100.0

typedef struct dds_cmd_sequence_struct * dds_cmds_ptr;

/* Compile high-level DDS settings into a sequence of ADWIN commands
dds_settings: array of pages of DDS settings to generate
num_settings: size of the each page in the array
PLL_multiplier: desired PLL multiplier setting for the DDS
external_clock: frequency of the DDS reference clock
	(note that the DDS's internal clock is PLL_multiplier * external_clock
adwin_event_period: time (in s) between adwin sequencer events

Return value: pointer to the compiled array of adwin commands
*/
dds_cmds_ptr create_dds_cmd_sequence(ddsoptions_struct* dds_settings,
									unsigned long num_settings,
									unsigned int PLL_multiplier,
									float external_clock,
									float adwin_event_period);

/* erase all stored (compiled) ADWIN commands and release associated memory 
After this call dead_cmds points to nothing, and should be reset to NULL
immediately unless something else is assigned to it
*/
void free_dds_cmd_sequence(dds_cmds_ptr dead_cmds);

/* 
Fetch the ADWIN command value for the specified index in the sequence,
or return -1 if none is available
*/
int get_dds_cmd(dds_cmds_ptr cmd_sequence, unsigned long event_index); 

#endif
