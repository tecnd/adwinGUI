#include <userint.h>
#include "GUIDesign.h"


/************************************************************************
File Name: ddstranslator.c
-------
Author: David McKay, Ultra Cold Atoms Lab
-------
Date Created: July 22, 2004
-------
Date Modified: July 26, 2004
-------
Description: To take dds options and translate them into binary values
to send to the ADWIN
-------
*************************************************************************/


/************************************************************************
Header File Include 
*************************************************************************/

#include "ddstranslator.h"

/************************************************************************
Private Global Variables 
*************************************************************************/

ADWIN_CMD	*adwin_commands = NULL;
long 		num_commands = 0;		
unsigned int	control_register_bytes[4] = {0x20,0x01,0x64,0x10}; /*the control register bytes...must be consistent or else an option may change*/
float		sysclk;


/************************************************************************
Function Definitions 
*************************************************************************/


/************************************************************************
Author: David McKay
-------
Date Created: July 22, 2004
-------
Date Modified: July 22, 2004
-------
Description: Creates a 3D matrix of values to send to the ADWIN at specified
times in order to change properties on the DDS as specified by the user 
-------
Return Value: void
-------
Parameter1: 
DDSOPTIONS* dds_settings
An array of dds settings  (ie they specify desired values of the DDS
properties at certain times)
-------
Parameter 2: 
long num_settings
The number of settings to enumerator through
-------
Parameter 3: 
unsigned int PLL_multiplier
A control register setting that indicates the system clock speed
-------
Parameter 4: 
float external_clock
Value of the external oscillator
*************************************************************************/

void createCommandArray(DDSOPTIONS** dds_settings, unsigned long num_settings, unsigned long num_pages, unsigned int PLL_multiplier, float external_clock)
{

	long i,j;
	long cmdtime;
	
	if (adwin_commands != NULL)
	{
		void destroyCommandArray(); /*destroy the old array first (if it exists)*/
	}

	#ifdef DEBUG_MODE
		printf("In create command array\n\n");
	#endif				
	
	/*Set up the control register bytes*/

	/*Set to External Update Clock*/
	control_register_bytes[1] &= 0xFE; /*toggle bit 0 low*/

	/*Set PLL*/
	control_register_bytes[2] &= 0xDF; /*toggle bit 5 low*/
	
	/*set sys clock = refclk * PLL Multiplier*/
	sysclk = external_clock * PLL_multiplier;	

	/*Set PLL Multiplier*/
	control_register_bytes[2] |= 0x1F; /*toggle bits 0-4 high*/
	PLL_multiplier |= 0xE0; /*toggle bits 5-7 high*/
	control_register_bytes[2] &= PLL_multiplier; /*set bits 0-4 in control register = to bits 0-4 in PLL_multiplier*/
	
	/*Set OSK EN high and OSK INT low*/
	control_register_bytes[0] &= 0xEF;
	control_register_bytes[0] |= 0x20;

	
	for (j=0;j<num_pages;j++)
	{
		for (i=0;i<num_settings;i++)
		{
		
			//set the start time based on the start time
			//and delta time of the previous element
			if ((i+j)!=0)
			{
				if (i==0)
				{
					dds_settings[i][j].start_time = dds_settings[num_settings-1][j-1].start_time + dds_settings[num_settings-1][j-1].delta_time;
				}
				else
				{
					dds_settings[i][j].start_time = dds_settings[i-1][j].start_time + dds_settings[i-1][j].delta_time;
				}
			}
			else
			{
				dds_settings[0][0].start_time = 0;
			}
		
			if (dds_settings[i][j].is_update==TRUE)
			{
				/*The DDS needs to be update, interpret the update parameters
				 * and translate into bit commands*/
		
				if (dds_settings[i][j].is_stop==TRUE)
				{
					/*stop the DDS*/
					/*toggle bits 0-2 high in control register byte 4*/
					control_register_bytes[3] |= 0x07;
					cmdtime = write_to_register(dds_settings[i][j].start_time/TIME_INTERVAL, ADDR_CTRLREG, 4, control_register_bytes);
					
					/*io clock*/
					write_to_register(cmdtime, UPDATECLK, 0, NULL);
				}
				else /*update the frequency*/
				{
				
					update_frequency(&dds_settings[i][j]);

				}
			}
		}
	}
	
	return;
	
}

/************************************************************************
Author: David McKay
-------
Date Created: July 22, 2004
-------
Date Modified: July 22, 2004
-------
Description: Accesses the value to be updated at time t
-------
Return Value: If a value needs to be updated at time t, then that
value is returned, otherwise the output is negative.
-------
Parameter1: 
int valuetime
The time for which to return the update value. The time is represented as units
of 5 micro seconds
*************************************************************************/

int getValue (unsigned long valuetime)
{
	
	unsigned long i;

	if (adwin_commands == NULL)
	{
		return -1;
	}
	
	
	return searchArray(0, num_commands-1, valuetime);
	
}

/************************************************************************
Author: David McKay
-------
Date Created: August 24, 2004
-------
Date Modified: August 24, 2004 
-------
Description: Uses binary search to search the adwin commands array
-------
*************************************************************************/
int searchArray(unsigned long start_index, unsigned long end_index, unsigned long valuetime)
{

	unsigned long mid_index;
	
	if (end_index < start_index)
	{
		return -1;
	}
	
	mid_index = (end_index - start_index)/2 + start_index;


	if (adwin_commands[mid_index].cmd_time < valuetime)
	{
		return searchArray(mid_index + 1, end_index, valuetime);
	}
	else if (adwin_commands[mid_index].cmd_time > valuetime)
	{
		return searchArray(start_index, mid_index -1, valuetime);
	}
	else
	{
		return adwin_commands[mid_index].cmd_value;
	}

}

/************************************************************************
Author: David McKay
-------
Date Created: July 26, 2004
-------
Date Modified: July 26, 2004
-------
Description: Deallocates the memory for the command array
-------
*************************************************************************/

void destroyCommandArray()
{

	if (adwin_commands == NULL)
	{
		return; /*already deallocated*/
	}

	
	free(adwin_commands);

	adwin_commands = NULL;
	

	return;
	
}

/************************************************************************
Author: David McKay
-------
Date Created: July 26, 2004
-------
Date Modified: July 26, 2004
-------
Description: Takes a byte, parses it into 2 bit segmetents (MSB) and adds it to the
ADWIN commands array
-------
Return Value: long, represents the time at which to add the next command
-------
Parameter1: 
int instr_byte
The byte to be parsed
-------
Parameter 2: 
long cur_cmd_time
The time corresponding to the start of the command
-------
Parameter 3: 
BOOL split_byte
If true, then split into 2 bit segments, if false, then send whole byte
-------
************************************************************************/
unsigned long parse_cmd_byte(unsigned int instr_byte, unsigned long cur_cmd_time, BOOL split_byte)
{
	
	int i, num_segments;
	unsigned int temp_values[4];
	
	if (split_byte==TRUE)
	{
	
		/*get bit 7,6*/
		temp_values[0] = instr_byte & 0xC0;
		temp_values[0]  >>= 6;

		/*get bits 5,4*/
        	temp_values[1] = instr_byte & 0x30;
		temp_values[1] >>= 4;
	
		/*get bits 3,2*/
		temp_values[2] = instr_byte & 0x0C;
		temp_values[2] >>= 2;
	
		/*get bits 1,0*/
		temp_values[3] = instr_byte & 0x03;

		num_segments = 4;
		
	}
	else
	{
		temp_values[0] = instr_byte;
		num_segments = 1;
	}
	
	/*realloc num_segments new places in the commands array*/
	adwin_commands = (ADWIN_CMD *) realloc(adwin_commands, (num_commands + num_segments) * sizeof(ADWIN_CMD));
	num_commands += num_segments;
	
	
	for (i=0;i<num_segments;i++)
	{
		
		adwin_commands[num_commands - num_segments + i].cmd_time = cur_cmd_time + i;
		adwin_commands[num_commands - num_segments + i].cmd_value = temp_values[i];
	}

	return (cur_cmd_time + num_segments);
}

/************************************************************************
Author: David McKay
-------
Date Created: July 26, 2004
-------
Date Modified: July 26, 2004
-------
Description: Write out the command sequence to update a register on
the DDS
-------
Return Value: long, the next available command time after writing all
the commands to update the register
-------
Parameter 1: 
long cmd_time 
The time at which to start writing these instructions
-------
Parameter 2: 
int addr_byte
The address instruction
-------
Parameter 3: 
int num_bytes
The number of data bytes for the address being written to 
-------
Parameter 4: 
int data_bytes[6]
The data bytes to be written
************************************************************************/
unsigned long write_to_register(unsigned long cmd_time, unsigned int addr_byte, unsigned int num_bytes, unsigned int *data_bytes)
{

	int i;
	
	if (num_bytes==0)
	{
		/*don't parse*/
		cmd_time = parse_cmd_byte(addr_byte, cmd_time, FALSE);
		return cmd_time;
	}
	
	cmd_time = parse_cmd_byte(addr_byte, cmd_time, TRUE);
		
	for (i=num_bytes-1;i>=0;i--)
	{
		cmd_time = parse_cmd_byte(data_bytes[i], cmd_time, TRUE);
	}
			
	return cmd_time;

}

/************************************************************************
Author: David McKay
-------
Date Created: July 26, 2004
-------
Date Modified: July 26, 2004
-------
Description: Write out the commands required to update the frequency
as specified
-------
Parameter 1: 
DDSSOPTIONS cur_dds_options
The settings which specify how to update the frequency
************************************************************************/
void update_frequency(DDSOPTIONS *cur_dds_options)
{

	unsigned long cmd_time, cmd_time2;
	float temp_frequency, temp_frequency2;
long i;
	
	cmd_time = (long) (cur_dds_options->start_time / TIME_INTERVAL);
	
	if (cur_dds_options->new_settings==TRUE)
	{

		/*Steps for starting up the DDS
		 * 1. Master Reset
		 * 2. Write Default Configuration Parameters (note that we start in single tone mode, also set the FSK to the starting frequency here)
		 * 3. Clear the accumulator
		 * 4. Set current value for frequency 1
		 * 5. Write Frequency 1 and 2
		 * 6. Write Ramp Rate and Delta Frequency
		 * 7. Update the registers
		 * 8. Change to ramped FSK mode and toggle the FSK bit */
		
		/**** 1 ****/
		/*send a master reset*/
		cmd_time = write_to_register(cmd_time, MASTERRESET, 0, NULL);

		/**** 2 ****/
		/*update the control register, set with default values*/
		/*set to single tone mode*/
		control_register_bytes[1] &= 0xF1;	   //bits 1,2,3 low

		/*turn on*/
		control_register_bytes[3] |= 0x04;		//power down the QDAC (not needed)
		control_register_bytes[3] &= 0xFC; 	   //bits 1,2 low
		
		/*if frequency 1 < frequency 2 then FSK is low (comparator PD), if frequency1 > frequency2 then FSK is high (comparator on)*/
		if (cur_dds_options->frequency1 < cur_dds_options->frequency2)
		{
			control_register_bytes[3] |= 0x10; //bit 4 high
		}
		else
		{
			control_register_bytes[3] &= 0xEF;  //bit 4 low
		}	
		
	     /*set accumulator bit low*/
		control_register_bytes[1] &= 0x7F;		//bit 7 low
		
		cmd_time = write_to_register(cmd_time, ADDR_CTRLREG, 4, control_register_bytes);
	
		/*io clock*/
		cmd_time = write_to_register(cmd_time, UPDATECLK, 0, NULL);

		/**** 3 ****/
		/*toggle the clear accumulator bit, to reset the accumulator  (this is for ramping between frequencies) */
		control_register_bytes[1] |= 0x80; /*toggle high*/

		cmd_time = write_to_register(cmd_time, ADDR_CTRLREG, 4, control_register_bytes);
	
		/*io clock*/
		cmd_time = write_to_register(cmd_time, UPDATECLK, 0, NULL);
	
		control_register_bytes[1] &= 0x7F; /*toggle low*/

		cmd_time = write_to_register(cmd_time, ADDR_CTRLREG, 4, control_register_bytes); 	
	
		/*io clock*/
		cmd_time = write_to_register(cmd_time, UPDATECLK, 0, NULL);
	
		/*reset the io line*/
		cmd_time = write_to_register(cmd_time, IORESET, 0, NULL);

		/**** 4 ****/
		cmd_time = set_current_value(cmd_time, cur_dds_options->current, cur_dds_options->frequency1);
		
		/**** 5 ****/
		/*set the frequency 1 and 2 values, note that the lowest frequency must go in F1*/
       		if (cur_dds_options->frequency1<cur_dds_options->frequency2)
       		{
				temp_frequency = cur_dds_options->frequency1;
				temp_frequency2 = cur_dds_options->frequency2;		
       		}
       		else
       		{
				temp_frequency = cur_dds_options->frequency2;
				temp_frequency2 = cur_dds_options->frequency1;
       		}

       		/*write F1 and F2*/
       		cmd_time = write_frequency_word(temp_frequency, ADDR_FREQ1, cmd_time);
       		cmd_time = write_frequency_word(temp_frequency2, ADDR_FREQ2, cmd_time);
      
		/**** 6 ****/
		/*Set the ramping characteristics (ie linear, exponential)*/
	
		/*set ramp rate*/
       	cmd_time = set_ramp_rate(cmd_time, cur_dds_options->delta_time); 
       
        if (cur_dds_options->transition_type == LINEAR)
		{
			/*set delta frequency, constant because it's linear*/
			cmd_time = write_frequency_word((temp_frequency2-temp_frequency)*STEP_GRANULARITY, ADDR_DELTAFREQ, cmd_time);

		}
	
		/**** 7 ****/
		/*io clock...update settings from buffers*/
		cmd_time = write_to_register(cmd_time, UPDATECLK, 0, NULL);	
		
		/**** 8 ****/
		
		/*Set to ramped FSK mode and flip the FSK bit...must do separately*/
		
		/*set ramped fsk*/
		control_register_bytes[1] |= 0x04;
		cmd_time = write_to_register(cmd_time, ADDR_CTRLREG, 4, control_register_bytes);
		cmd_time = write_to_register(cmd_time, UPDATECLK, 0, NULL);
		
		/*flip FSK*/
		control_register_bytes[3] ^= 0x10; /*flips bit 4*/
		cmd_time = write_to_register(cmd_time, ADDR_CTRLREG, 4, control_register_bytes); 
		cmd_time = write_to_register(cmd_time, UPDATECLK, 0, NULL);
		

		

	}
	else
	{

		/*The new settings parameter equals FALSE. Therefore, it is assumed that the DDS is already running
		 * and that all we want to do is to change the frequency 2 value (frequency 1 = frequency 2 or the previous setting)
		 * and update the delta clock and frequency*/
	      
		/*Steps for Updating
		 * 1. Change Frequency 2
		 * 2. Change the Ramp Rate and Rate Clock
		 * 3. Change current value */
		

		/*io reset*/
		cmd_time = write_to_register(cmd_time, IORESET, 0, NULL);
		
		/**** 1/2  *****/
		if (cur_dds_options->frequency2 < cur_dds_options->frequency1)
		{
			cmd_time = write_frequency_word(cur_dds_options->frequency2, ADDR_FREQ1, cmd_time);
			cmd_time = write_frequency_word((cur_dds_options->frequency1-cur_dds_options->frequency2)*STEP_GRANULARITY, ADDR_DELTAFREQ, cmd_time);
		}
		else
		{
			cmd_time = write_frequency_word(cur_dds_options->frequency2, ADDR_FREQ2, cmd_time);
			cmd_time = write_frequency_word((cur_dds_options->frequency2 - cur_dds_options->frequency1)*STEP_GRANULARITY, ADDR_DELTAFREQ, cmd_time); 
					       	
		}

		/*set ramp rate*/
		cmd_time = set_ramp_rate(cmd_time, cur_dds_options->delta_time);

		/**** 3 ****/
		cmd_time = set_current_value(cmd_time, cur_dds_options->current, cur_dds_options->frequency1);
		
		/*io clock*/
		cmd_time = write_to_register(cmd_time, UPDATECLK, 0, NULL);
		
	}
	
	
	
	return;
	
}

/************************************************************************
Author: David McKay
-------
Date Created: July 26, 2004
-------
Date Modified: July 26, 2004
-------
Description: Takes the desired frequency (in MHz) and converts it to the 
required 48 bit value specified by the the DDS
-------
Return Value: long, the new command time
-------
Parameter 1:
float frequency_value
The desired frequency value (in MHz) 
-------
Parameter 2: 
int addr_byte
the register to write the frequency value to
-------
Parameter 3:
The current command time
-------
************************************************************************/
unsigned long write_frequency_word(float frequency_value, unsigned int addr_byte, unsigned long cmd_time)
{

	unsigned int data_bytes[6];
	double frequency_word;
	
	int i;


	
	/*NOTE: The 48 bit value which specifies the frequency is give by the following formula
	* (desired frequency * 2^N)/SYSCLK */ 

	frequency_value = frequency_value / sysclk;

	frequency_word = frequency_value * pow(2,48);

	#ifdef DEBUG_MODE
		printf("%e %f  %f  \n", frequency_word, sysclk, frequency_value);
	#endif
	
	for (i=5; i>=0; i--)
	{
		data_bytes[i] = (int) (frequency_word/pow(2,8*i));
		frequency_word -= data_bytes[i]*pow(2,8*i);		
	}

	cmd_time = write_to_register(cmd_time, addr_byte, 6, data_bytes);

	return cmd_time;
	
}

/************************************************************************
Author: David McKay
-------
Date Created: July 28, 2004
-------
Date Modified: July 28, 2004
-------
Description: Sets the ramp rate based on the delta time
-------
Return Value: long, the new command time
-------
Parameter 1:
The current command time
-------
Parameter 2: 
unsigned long delta_time
The length of time the ramp up/down should take
-------
************************************************************************/
unsigned long set_ramp_rate(unsigned long cmd_time, float delta_time)
{

	unsigned long ramp_rate; 
	unsigned int data_bytes[3];
	
	ramp_rate = sysclk*pow(10,6)*delta_time*STEP_GRANULARITY - 1;
	data_bytes[2] = (ramp_rate & 0xFF0000)>>16;
	data_bytes[1] = (ramp_rate & 0x00FF00)>>8;
   	data_bytes[0] = (ramp_rate & 0x0000FF);

	cmd_time = write_to_register(cmd_time, ADDR_RAMPRATE, 3, data_bytes);

	return cmd_time;
	
}

/************************************************************************
Author: David McKay
-------
Date Created: July 28, 2004
-------
Date Modified: July 28, 2004
-------
Description: Sets the current output based on the frequency 
-------
Return Value: long, the new command time
-------
Parameter 1:
The current command time
-------
Parameter 2: 
float target_current
Target current through the inductor
-------
Parameter 3: 
float cur_frequency
The current frequency value (current out is a function of frequency)
-------
************************************************************************/

unsigned long set_current_value(unsigned long cmd_time, float target_current, float cur_frequency)
{

	unsigned int data_bytes[2];
	unsigned long current_value;

	current_value = 4095*target_current/(IMAX);
			 
	if (current_value > 4095)
	{
		current_value = 4095; /*max possible value for this parameter*/
	}

	data_bytes[1] = (current_value & 0x0F00) >> 8;
	data_bytes[0] = (current_value & 0x00FF);

	cmd_time = write_to_register(cmd_time, ADDR_CURRENT, 2, data_bytes);

	return cmd_time;	
	
}
  
