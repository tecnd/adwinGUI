

#include "DDSControl.h"
#include "DDSControl2.h"

/*
Code attached to DDSControl.uir

This section of the the program is used to control the Direct Digital Synthesizer
AD9854 (Analog Devices)

Every time we change a parameter of the DDS we need to send it an instruction byte
and the appropriate number of data bytes.


Instruction Byte Breakdown
Bit
MSB  D6  D5  D4  D3  D2  D1  D0
R/W  X    X   X  A3  A2  A1  A0
Bit 7 controls whether read/write 0-> write    1-> read 
A3-A0 define the register to access


Registers we use
2	Frequency Tuning Word 1			6 Bytes
3	Frequency Tuning Word 2			6 Bytes
4	Delta frequency Word			6 Bytes
5	Update Clock					4 bytes
6	Ramp Rate Clock					3 Bytes
7	Control Register				4 Bytes----see below
B   QDac							2 Bytes

Control Register Breakdown
bit 8		INT/EXT Update Clk  when set low, uses IO pin as an input
bit 9-11 	operating mode		0- single tone  1- FSK
								2- Ramped FSK   3- Chirp Mode    4- BPSK 
bit 16-20 	PLL multipler (range 4-20)							
bit	24		Digital power down -  effectivly shuts down the synthesis section
bit 25		Full power down
bit 26  	QDAC powerdown
bit 28		Comparator power down  (connects to           )
*/

int CVICALLBACK CMD_OK_CALLBACK (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	
	float temp_var;
	int temp_int;
	
	switch (event)
		{
		case EVENT_COMMIT:

			//update values in the array
			GetCtrlVal(panel, PANEL_NUM_FREQ1, &temp_var);
			ddstable[currentx][currentpage].frequency1 = temp_var;
			
			GetCtrlVal(panel, PANEL_NUM_FREQ2, &temp_var);
			ddstable[currentx][currentpage].frequency2 = temp_var;
			
			GetCtrlVal(panel, PANEL_NUM_CURRENT, &temp_var);
			ddstable[currentx][currentpage].current = temp_var;
			
			GetCtrlVal(panel, PANEL_CMD_ONOFF, &temp_int);
			if (temp_int==0)
			{
				ddstable[currentx][currentpage].is_stop = FALSE;
			}
			else
			{
				ddstable[currentx][currentpage].is_stop = TRUE;
			}
			
			HidePanel(panel);
			DrawNewTable(isdimmed);
			break;
		}
	return 0;
}

int CVICALLBACK CMD_CANCEL_CALLBACK (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
		{
		case EVENT_COMMIT:

			HidePanel(panel);
			
			break;
		}
	return 0;
}

int CVICALLBACK CMD_ONOFF_CALLBACK (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	
	int ctrl_value;
	
	switch (event)
		{
		case EVENT_COMMIT:

			GetCtrlAttribute(panel,control,ATTR_CTRL_VAL, &ctrl_value);

			if (ctrl_value==0)
			{
				SetCtrlAttribute(panel, control, ATTR_LABEL_TEXT, "DDS ON"); 
			}
			else
			{
				SetCtrlAttribute(panel, control, ATTR_LABEL_TEXT, "DDS OFF");
			}
			
			break;
		}
	return 0;
}

//***************************************************************************
void SetDDSControlPanel(void)
{
	SetCtrlVal(panelHandle6,PANEL_NUM_CURRENT, ddstable[currentx][currentpage].current);
	SetCtrlVal(panelHandle6,PANEL_NUM_FREQ1,ddstable[currentx][currentpage].frequency1);
	SetCtrlVal (panelHandle6,PANEL_NUM_FREQ2, ddstable[currentx][currentpage].frequency2);
	
	if (ddstable[currentx][currentpage].is_stop==TRUE)
	{
		SetCtrlVal(panelHandle6,PANEL_CMD_ONOFF,1);
	}
	else
	{
		SetCtrlVal(panelHandle6,PANEL_CMD_ONOFF,0);
	}
	
}
