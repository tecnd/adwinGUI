

#include "DDSControl.h"
#include "DDSControl2.h"

/*
Code attached to DDSControl.uir


*/

int CVICALLBACK CMD_OK_CALLBACK (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	
	float temp_var;
	int temp_int;
	
	switch (event)
		{
		case EVENT_COMMIT:
			ChangedVals=1;
			//update values in the array
			GetCtrlVal(panel, PANEL_NUM_FREQ1, &temp_var);
			ddstable[currentx][currentpage].start_frequency = temp_var;
			
			GetCtrlVal(panel, PANEL_NUM_FREQ2, &temp_var);
			ddstable[currentx][currentpage].end_frequency = temp_var;
			
			GetCtrlVal(panel, PANEL_NUM_CURRENT, &temp_var);
			ddstable[currentx][currentpage].amplitude = temp_var;
			
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
	
}
