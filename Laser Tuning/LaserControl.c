/*  LaserControl.c 
	Author: David Burns, July 2006 */

#include <ansi_c.h>
#include <cvirte.h>		
#include <userint.h>
#include "LaserControl.h"
#include "LaserControl2.h"
#include "GUIDesign2.h"
#include "vars.h"


int CVICALLBACK CancelLasConCALLBACK (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
/* Cancel Button Callback */
{
	switch (event)
		{
		case EVENT_COMMIT:
			HidePanel(panel);
			break;
		}
	return 0;
}

int CVICALLBACK SetLasConCALLBACK (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
/*	Callback for the done button. Updates LaserTable with values entered */
{
	double freq;
	switch (event)
		{
		case EVENT_COMMIT:
			GetCtrlVal (panel,PANEL_LAS_CONTROL_MODE, &LaserTable[currenty-(NUMBERANALOGCHANNELS+NUMBERDDS+1)][currentx][currentpage].fcn);
			GetCtrlVal (panel,PANEL_LAS_TARGET_FREQ, &freq); 
			
			if(fabs(freq)>MAX_ROSA_FREQ)
			{
				SetCtrlVal(panel,PANEL_LAS_TARGET_FREQ,freq/fabs(freq)*MAX_ROSA_FREQ);	
			}
			else if(fabs(freq)<MIN_ROSA_FREQ)
			{
				SetCtrlVal(panel,PANEL_LAS_TARGET_FREQ,freq/fabs(freq)*MIN_ROSA_FREQ);
			}
			else
			{
				LaserTable[currenty-(NUMBERANALOGCHANNELS+NUMBERDDS+1)][currentx][currentpage].fval=freq;
				DrawNewTable(1);
				HidePanel(panel);
			}
		break;
		}
	return 0;
}

void SetLaserControlPanel(int laserNum)
/*  Called before LaserControl.uir is displayed. Sets the laser name and particular freq/mode values corresponding to the 
	cell which was clicked */
{
	SetCtrlVal (panelHandle11,PANEL_LAS_CONTROL_MODE, LaserTable[currenty-(NUMBERANALOGCHANNELS+NUMBERDDS+1)][currentx][currentpage].fcn);
	
	if(LaserTable[currenty-(NUMBERANALOGCHANNELS+NUMBERDDS+1)][currentx][currentpage].fcn!=0)
		SetCtrlVal (panelHandle11,PANEL_LAS_TARGET_FREQ, LaserTable[currenty-(NUMBERANALOGCHANNELS+NUMBERDDS+1)][currentx][currentpage].fval); 
	else
		SetCtrlVal (panelHandle11,PANEL_LAS_TARGET_FREQ, findLastVal(currenty,currentx, currentpage)); 
	
	SetCtrlVal (panelHandle11,PANEL_LAS_NAME,LaserProperties[laserNum].Name);
	SetCtrlVal (panelHandle11,PANEL_LAS_LAST_VAL,findLastVal(currenty,currentx, currentpage)); 
}
