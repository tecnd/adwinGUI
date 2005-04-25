#include <userint.h>
#include "Scan.h"
#include "Scan2.h"
int CVICALLBACK CALLBACK_SCANOK (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
		{
		case EVENT_COMMIT:
			ReadScanValues();
			HidePanel(panelHandle7);
			break;
		}
	return 0;
}


void InitializeScanPanel(void)
{
 SetCtrlVal (panelHandle7,SCANPANEL_NUM_COLUMN,currentx);
 
 SetCtrlVal (panelHandle7,SCANPANEL_NUM_ROW,currenty);
 SetCtrlVal (panelHandle7,SCANPANEL_NUM_PAGE,currentpage);
 SetCtrlVal (panelHandle7,SCANPANEL_NUM_CHANNEL,AChName[currenty].chnum);
 SetCtrlVal (panelHandle7,SCANPANEL_NUM_DURATION,TimeArray[currenty][currentpage]);
 
 SetCtrlVal (panelHandle7,SCANPANEL_RING_MODE,AnalogScan.Scan_Mode); 
 SetCtrlVal (panelHandle7, SCANPANEL_NUM_SCANSTART, AnalogScan.Start_Of_Scan);
 SetCtrlVal (panelHandle7, SCANPANEL_NUM_SCANEND, AnalogScan.End_Of_Scan);
 SetCtrlVal (panelHandle7, SCANPANEL_NUM_SCANSTEP, AnalogScan.Scan_Step_Size);
 SetCtrlVal (panelHandle7, SCANPANEL_NUM_ITERATIONS, AnalogScan.Iterations_Per_Step);
 
 DisplayPanel(panelHandle7);
}

void ReadScanValues(void)
{
	AnalogScan.Row=currenty;
	AnalogScan.Column=currentx;
	AnalogScan.Page=currentpage;
	GetCtrlVal (panelHandle7, SCANPANEL_NUM_SCANSTART, &AnalogScan.Start_Of_Scan);
 	GetCtrlVal (panelHandle7, SCANPANEL_NUM_SCANEND, &AnalogScan.End_Of_Scan);
 	GetCtrlVal (panelHandle7, SCANPANEL_NUM_SCANSTEP, &AnalogScan.Scan_Step_Size);
 	GetCtrlVal (panelHandle7, SCANPANEL_NUM_ITERATIONS, &AnalogScan.Iterations_Per_Step);
	AnalogScan.Current_Step=0;
	AnalogScan.Current_Scan_Value=AnalogScan.Start_Of_Scan;
}
