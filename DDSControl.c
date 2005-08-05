

#include "DDSControl.h"
#include "DDSControl2.h"
/*
Code attached to DDSControl.uir


*/

int CVICALLBACK CMD_OK_CALLBACK (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	
	float temp_start_freq,temp_end_freq,temp_current;
	int temp_is_stop;
	
	switch (event)
		{
		case EVENT_COMMIT:
			ChangedVals=1;
			//update values in the array
			GetCtrlVal(panel, PANEL_NUM_FREQ1, &temp_start_freq);
			GetCtrlVal(panel, PANEL_NUM_FREQ2, &temp_end_freq);  
			GetCtrlVal(panel, PANEL_NUM_CURRENT, &temp_current);
			GetCtrlVal(panel, PANEL_CMD_ONOFF, &temp_is_stop);
			
			
			switch(Active_DDS_Panel)
			{
				case 1:
					ddstable[currentx][currentpage].start_frequency = temp_start_freq;
					ddstable[currentx][currentpage].end_frequency = temp_end_freq;
					ddstable[currentx][currentpage].amplitude = temp_current;
					if (temp_is_stop==0)
					{
						ddstable[currentx][currentpage].is_stop = FALSE;
					}
					else
					{
						ddstable[currentx][currentpage].is_stop = TRUE;
					}
					break;

				case 2:
					dds2table[currentx][currentpage].start_frequency = temp_start_freq;
					dds2table[currentx][currentpage].end_frequency = temp_end_freq;
					dds2table[currentx][currentpage].amplitude = temp_current;
					if (temp_is_stop==0)
					{
						dds2table[currentx][currentpage].is_stop = FALSE;
					}
					else
					{
						dds2table[currentx][currentpage].is_stop = TRUE;
					}
					break;

				case 3:
					dds3table[currentx][currentpage].start_frequency = temp_start_freq;
					dds3table[currentx][currentpage].end_frequency = temp_end_freq;
					dds3table[currentx][currentpage].amplitude = temp_current;
					if (temp_is_stop==0)
					{
						dds3table[currentx][currentpage].is_stop = FALSE;
					}
					else
					{
						dds3table[currentx][currentpage].is_stop = TRUE;
					}
					break;
			 } // end switch statement
			
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
void SetDDSControlPanel(int Active_DDS)
{
	switch (Active_DDS)
	{
		case 1:    // Rb evaporation DDS
			SetCtrlVal(panelHandle6,PANEL_NUM_CURRENT, ddstable[currentx][currentpage].amplitude);
			SetCtrlVal(panelHandle6,PANEL_NUM_FREQ1,ddstable[currentx][currentpage].start_frequency);
			SetCtrlVal (panelHandle6,PANEL_NUM_FREQ2, ddstable[currentx][currentpage].end_frequency);
		
			if (ddstable[currentx][currentpage].is_stop==TRUE)
			{
				SetCtrlVal(panelHandle6,PANEL_CMD_ONOFF,1);
			}
			else
			{
				SetCtrlVal(panelHandle6,PANEL_CMD_ONOFF,0);
			}
			SetCtrlVal (panelHandle6, PANEL_TXT_DDS_MODE, "DDS: Rb Evaporation");

			break;
			
		case 2:	// K40 Evaporation DDS
			SetCtrlVal(panelHandle6,PANEL_NUM_CURRENT, dds2table[currentx][currentpage].amplitude);
			SetCtrlVal(panelHandle6,PANEL_NUM_FREQ1,dds2table[currentx][currentpage].start_frequency);
			SetCtrlVal (panelHandle6,PANEL_NUM_FREQ2, dds2table[currentx][currentpage].end_frequency);
		
			if (dds2table[currentx][currentpage].is_stop==TRUE)
			{
				SetCtrlVal(panelHandle6,PANEL_CMD_ONOFF,1);
			}
			else
			{
				SetCtrlVal(panelHandle6,PANEL_CMD_ONOFF,0);
			}
			SetCtrlVal (panelHandle6, PANEL_TXT_DDS_MODE, "DDS: K40 Evaporation");

			break;
			
		case 3:	// K40 HyperFine Splitting DDS
			SetCtrlVal(panelHandle6,PANEL_NUM_CURRENT, dds3table[currentx][currentpage].amplitude);
			SetCtrlVal(panelHandle6,PANEL_NUM_FREQ1,dds3table[currentx][currentpage].start_frequency);
			SetCtrlVal (panelHandle6,PANEL_NUM_FREQ2, dds3table[currentx][currentpage].end_frequency);
		
			if (dds3table[currentx][currentpage].is_stop==TRUE)
			{
				SetCtrlVal(panelHandle6,PANEL_CMD_ONOFF,1);
			}
			else
			{
				SetCtrlVal(panelHandle6,PANEL_CMD_ONOFF,0);
			}
			SetCtrlVal (panelHandle6, PANEL_TXT_DDS_MODE, "DDS: K40 Hyper Fine Splitting");

			break;
		
	}// end of switch statement	
		
}
