/************************************************************************
File Name: ddstranslator.h
-------
Description: Header file for translation of high-level DDS settings to
ADWIN sequencer commands
*************************************************************************/

#ifndef DDSTRANS_H
#define DDSTRANS_H
#include "vars.h"

/* AD9852 amplitude settings in the user interface are on a percent scale */
#define MAX_AMPLITUDE	100.0
/* Frequency step size as a multiple of the complete frequency ramp */
#define AD9852_GRANULARITY 0.0001
#define AD9858_GRANULARITY 0.0001

typedef struct dds_cmd_sequence_struct * dds_cmds_ptr;

/* erase all stored (compiled) ADWIN commands and release associated memory 
After this call dead_cmds points to nothing, and should be reset to NULL
immediately unless something else is assigned to it
*/
void free_dds_cmd_sequence(dds_cmds_ptr dead_cmds);

/* Fetch the ADWIN command value for the specified index in the sequence,
or return -1 if none is available
*/
int get_dds_cmd(dds_cmds_ptr cmd_sequence, unsigned long event_index);

/* Compile high-level DDS settings into a sequence of ADWIN commands
note that the DDS's internal clock is PLL_multiplier * external_clock
adwin_event_period is in seconds
*/
dds_cmds_ptr create_ad9852_cmd_sequence(ddsoptions_struct* dds_settings,
										unsigned long num_settings,
										unsigned int PLL_multiplier,
										double external_clock,
										double adwin_event_period);

dds_cmds_ptr create_ad9858_cmd_sequence(ddsoptions_struct* dds_settings,
										unsigned long num_settings,
										double refclk,
										double adwin_event_period,
										double frequency_offset);
#endif
