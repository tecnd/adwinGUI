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
			PScan.ScanMode=0;// set to Analog scan
			HidePanel(panelHandle7);
			HidePanel(panelHandle4);
			break;
		}
	return 0;
}

//********************************************************************************************************
// last update:  May 11, 2005 
// May12, 2005:  initialize for time and DDS scans

void InitializeScanPanel(void)
{
 	SetCtrlVal (panelHandle7,SCANPANEL_NUM_COLUMN,currentx);
 
 	SetCtrlVal (panelHandle7,SCANPANEL_NUM_ROW,currenty);
	SetCtrlVal (panelHandle7,SCANPANEL_NUM_PAGE,currentpage);
 	SetCtrlVal (panelHandle7,SCANPANEL_NUM_CHANNEL,AChName[currenty].chnum);
 	SetCtrlVal (panelHandle7,SCANPANEL_NUM_DURATION,TimeArray[currenty][currentpage]);
 
 	SetCtrlVal (panelHandle7, SCANPANEL_RING_MODE,  	PScan.Analog.Analog_Mode); 
 	SetCtrlVal (panelHandle7, SCANPANEL_NUM_SCANSTART, 	PScan.Analog.Start_Of_Scan);
 	SetCtrlVal (panelHandle7, SCANPANEL_NUM_SCANEND, 	PScan.Analog.End_Of_Scan);
	SetCtrlVal (panelHandle7, SCANPANEL_NUM_SCANSTEP, 	PScan.Analog.Scan_Step_Size);
 	SetCtrlVal (panelHandle7, SCANPANEL_NUM_ITERATIONS,	PScan.Analog.Iterations_Per_Step);
 
 	SetCtrlVal (panelHandle7, SCANPANEL_NUM_TIMESTART, 	PScan.Time.Start_Of_Scan);
 	SetCtrlVal (panelHandle7, SCANPANEL_NUM_TIMEEND, 	PScan.Time.End_Of_Scan);
 	SetCtrlVal (panelHandle7, SCANPANEL_NUM_TIMESTEP, 	PScan.Time.Scan_Step_Size);
 	SetCtrlVal (panelHandle7, SCANPANEL_NUM_TIMEITER,  	PScan.Time.Iterations_Per_Step);
 
 	SetCtrlVal (panelHandle7, SCANPANEL_NUM_DDSBASEFREQ,PScan.DDS.Base_Freq); 
 	SetCtrlVal (panelHandle7, SCANPANEL_NUM_DDSSTART,  	PScan.DDS.Start_Of_Scan);
 	SetCtrlVal (panelHandle7, SCANPANEL_NUM_DDSEND, 	PScan.DDS.End_Of_Scan);
 	SetCtrlVal (panelHandle7, SCANPANEL_NUM_DDSSTEP, 	PScan.DDS.Scan_Step_Size);
 	SetCtrlVal (panelHandle7, SCANPANEL_NUM_DDSITER,   	PScan.DDS.Iterations_Per_Step);
 	SetCtrlVal (panelHandle7, SCANPANEL_NUM_DDSCURRENT,	PScan.DDS.Current); 
	DisplayPanel(panelHandle7);
}


//********************************************************************************************************
// last update:  
// May 12,2005:  read in values for time and dds, set mode (ANALOG, TIME or DDS)
//               set initial values
void ReadScanValues(void)
{
	PScan.Row=currenty;
	PScan.Column=currentx;
	PScan.Page=currentpage;
	
	GetCtrlVal (panelHandle7, SCANPANEL_NUM_SCANSTART, &PScan.Analog.Start_Of_Scan);
 	GetCtrlVal (panelHandle7, SCANPANEL_NUM_SCANEND,   &PScan.Analog.End_Of_Scan);
 	GetCtrlVal (panelHandle7, SCANPANEL_NUM_SCANSTEP,  &PScan.Analog.Scan_Step_Size);
 	GetCtrlVal (panelHandle7, SCANPANEL_NUM_ITERATIONS,&PScan.Analog.Iterations_Per_Step);
	GetCtrlVal (panelHandle7, SCANPANEL_RING_MODE,     &PScan.Analog.Analog_Mode);	
	
	GetCtrlVal (panelHandle7, SCANPANEL_NUM_TIMESTART, &PScan.Time.Start_Of_Scan);
 	GetCtrlVal (panelHandle7, SCANPANEL_NUM_TIMEEND,   &PScan.Time.End_Of_Scan);
 	GetCtrlVal (panelHandle7, SCANPANEL_NUM_TIMESTEP,  &PScan.Time.Scan_Step_Size);
 	GetCtrlVal (panelHandle7, SCANPANEL_NUM_TIMEITER,  &PScan.Time.Iterations_Per_Step);
 	
 	GetCtrlVal (panelHandle7, SCANPANEL_NUM_DDSBASEFREQ, &PScan.DDS.Start_Of_Scan);
 	GetCtrlVal (panelHandle7, SCANPANEL_NUM_DDSSTART,    &PScan.DDS.Start_Of_Scan);
 	GetCtrlVal (panelHandle7, SCANPANEL_NUM_DDSEND,      &PScan.DDS.End_Of_Scan);
 	GetCtrlVal (panelHandle7, SCANPANEL_NUM_DDSSTEP,     &PScan.DDS.Scan_Step_Size);
 	GetCtrlVal (panelHandle7, SCANPANEL_NUM_DDSITER,     &PScan.DDS.Iterations_Per_Step);
	GetCtrlVal (panelHandle7, SCANPANEL_NUM_DDSBASEFREQ, &PScan.DDS.Base_Freq);
	GetCtrlVal (panelHandle7, SCANPANEL_NUM_DDSCURRENT,  &PScan.DDS.Current);
	ScanVal.Current_Step=0;
}


//********************************************************************************************************
// last update:  
int CVICALLBACK CALLBACK_SCAN_CANCEL (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
		{
		case EVENT_COMMIT:
			PScan.Scan_Active=FALSE;
			HidePanel(panelHandle7);
			HidePanel(panelHandle4);
			
			break;
		}
	return 0;
}



//********************************************************************************************************
// last update:
int CVICALLBACK CALLBACK_TIMESCANOK (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
		{
		case EVENT_COMMIT:

			ReadScanValues();
			PScan.ScanMode=1;// set to Analog scan
			HidePanel(panelHandle7);
			HidePanel(panelHandle4);
			break;
		}
	return 0;
}


//********************************************************************************************************
// last update:
int CVICALLBACK CALLBACK_DDSSCANOK (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
		{
		case EVENT_COMMIT:

			ReadScanValues();
			PScan.ScanMode=2;// set to Analog scan
			HidePanel(panelHandle7);
			HidePanel(panelHandle4);
			break;
		}
	return 0;
}
