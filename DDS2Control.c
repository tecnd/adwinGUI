

#include "DDS2Control.h"
#include "DDS2Control2.h"

/*
Code attached to DDSControl.uir


*/

int CVICALLBACK CMD2_OK_CALLBACK (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	
	float temp_var;
	int temp_int;
	
	switch (event)
		{
		case EVENT_COMMIT:
			ChangedVals=1;
			//update values in the array
			GetCtrlVal(panel, DDS2PANEL_NUM2_FREQ1, &temp_var);
			ddstable[currentx][currentpage].start_frequency = temp_var;
			
			GetCtrlVal(panel, DDS2PANEL_NUM2_FREQ2, &temp_var);
			ddstable[currentx][currentpage].end_frequency = temp_var;
			
			GetCtrlVal(panel, DDS2PANEL_NUM2_CURRENT, &temp_var);
			ddstable[currentx][currentpage].amplitude = temp_var;
			
			GetCtrlVal(panel, DDS2PANEL_CMD2_ONOFF, &temp_int);
			if (temp_int==0)
			{
				dds2table[currentx][currentpage].is_stop = FALSE;
			}
			else
			{
				dds2table[currentx][currentpage].is_stop = TRUE;
			}
			
			HidePanel(panel);
			DrawNewTable(isdimmed);
			break;
		}
	return 0;
}

int CVICALLBACK CMD2_CANCEL_CALLBACK (int panel, int control, int event,
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

int CVICALLBACK CMD2_ONOFF_CALLBACK (int panel, int control, int event,
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
void SetDDSC2ontrolPanel(void)
{
	SetCtrlVal(panelHandle6,DDS2PANEL_NUM2_CURRENT, dds2table[currentx][currentpage].amplitude);
	SetCtrlVal(panelHandle6,DDS2PANEL_NUM2_FREQ1,dds2table[currentx][currentpage].start_frequency);
	SetCtrlVal (panelHandle6,DDS2PANEL_NUM2_FREQ2, dds2table[currentx][currentpage].end_frequency);
	
	if (dds2table[currentx][currentpage].is_stop==TRUE)
	{
		SetCtrlVal(panelHandle8,DDS2PANEL_CMD2_ONOFF,1);
	}
	else
	{
		SetCtrlVal(panelHandle8,DDS2PANEL_CMD2_ONOFF,0);
	}
	
}
