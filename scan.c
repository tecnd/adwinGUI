#include <userint.h>
#include "Scan.h"
#include "Scan2.h"
#include "GuiDesign.h"
/*
Displays information on the SCAN panel, and reads the information.  
This code doesn't act on the scan information, that is done in GUIDesign.c::UpdateScanValue
Scan has 4 modes of operation:
Analog, Time, DDS (single cell) and DDS(floor)

*/

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
 	SetCtrlVal (panelHandle7,SCANPANEL_NUM_COLUMN,PScan.Column);
 	SetCtrlVal (panelHandle7,SCANPANEL_NUM_ROW,PScan.Row);
	SetCtrlVal (panelHandle7,SCANPANEL_NUM_PAGE,PScan.Page);
 	
 	SetCtrlVal (panelHandle7,SCANPANEL_NUM_CHANNEL,AChName[PScan.Row].chnum);
 	SetCtrlVal (panelHandle7,SCANPANEL_NUM_DURATION,TimeArray[PScan.Column][PScan.Page]);
 
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
	
	SetCtrlVal (panelHandle7, SCANPANEL_NUM_DDSFLOORSTART, PScan.DDSFloor.Floor_Start);
	SetCtrlVal (panelHandle7, SCANPANEL_NUM_DDSFLOOREND,   PScan.DDSFloor.Floor_End);
	SetCtrlVal (panelHandle7, SCANPANEL_NUM_DDSFLOORSTEP,  PScan.DDSFloor.Floor_Step);
	SetCtrlVal (panelHandle7, SCANPANEL_NUM_DDSFLOORITER,  PScan.DDSFloor.Iterations_Per_Step);
	
	
	DisplayPanel(panelHandle7);
}


//********************************************************************************************************
// last update:  
// May 12,2005:  read in values for time and dds, set mode (ANALOG, TIME or DDS)
//               set initial values
void ReadScanValues(void)
{
//	PScan.Row=currenty;
//	PScan.Column=currentx;
//	PScan.Page=currentpage;
	
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
	
	GetCtrlVal (panelHandle7, SCANPANEL_NUM_DDSFLOORSTART, &PScan.DDSFloor.Floor_Start);
	GetCtrlVal (panelHandle7, SCANPANEL_NUM_DDSFLOOREND,   &PScan.DDSFloor.Floor_End);
	GetCtrlVal (panelHandle7, SCANPANEL_NUM_DDSFLOORSTEP,  &PScan.DDSFloor.Floor_Step);
	GetCtrlVal (panelHandle7, SCANPANEL_NUM_DDSFLOORITER,  &PScan.DDSFloor.Iterations_Per_Step);
	
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
			PScan.ScanMode=1;// set to Time scan
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
			PScan.ScanMode=2;// set to DDS scan
			HidePanel(panelHandle7);
			HidePanel(panelHandle4);
			break;
		}
	return 0;
}


//********************************************************************************************************
// Added Mar09, 2006
int CVICALLBACK CALLBACK_DDSFLOORSCANOK (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
		{
		case EVENT_COMMIT:

			ReadScanValues();
			PScan.ScanMode=3;// set to scan the DDS Floor
			HidePanel(panelHandle7);
			HidePanel(panelHandle4);
			break;
		}
	return 0;
}
//********************************************************************************************************

int CVICALLBACK CMD_GETSCANVALS_CALLBACK (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	int cx=0,cy=0,cz=0;
	double dtemp;
	switch (event)
		{
		case EVENT_COMMIT:
			
			PScan.Column=currentx;
			PScan.Row=currenty;
			PScan.Page=currentpage;
			cx=currentx;cy=currenty;cz=currentpage;	
			PScan.Analog.Analog_Mode		=AnalogTable[cx][cy][cz].fcn;
			PScan.Analog.Start_Of_Scan		=AnalogTable[cx][cy][cz].fval;
			PScan.Analog.End_Of_Scan		=AnalogTable[cx][cy][cz].fval;
			PScan.Analog.Scan_Step_Size		=0.1;
			PScan.Analog.Iterations_Per_Step=1;
			
			PScan.Time.Start_Of_Scan		=TimeArray[cx][cz];
			PScan.Time.End_Of_Scan			=TimeArray[cx][cz];
			PScan.Time.Scan_Step_Size		=0.1;
			PScan.Time.Iterations_Per_Step	=1;
		
			PScan.DDS.Base_Freq				=ddstable[cx][cz].start_frequency;
			PScan.DDS.Start_Of_Scan			=ddstable[cx][cz].end_frequency;
			PScan.DDS.End_Of_Scan			=ddstable[cx][cz].end_frequency;
			PScan.DDS.Iterations_Per_Step	=1;
			PScan.DDS.Current				=ddstable[cx][cz].amplitude;
			
			GetCtrlVal (panelHandle, PANEL_NUM_DDS_OFFSET, &dtemp);
			PScan.DDSFloor.Floor_Start		=dtemp;
			PScan.DDSFloor.Floor_End		=dtemp;
			PScan.DDSFloor.Floor_Step		=0.1;
			PScan.DDSFloor.Iterations_Per_Step=1;
			
			
			InitializeScanPanel();
			DisplayPanel(panelHandle7);
			break;
		}
	return 0;
}

//********************************************************************************************************
int CVICALLBACK CHECK_USE_LIST_CALLBACK (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
		{
		case EVENT_COMMIT:

			break;
		}
	return 0;
}
