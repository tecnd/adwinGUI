/*  LaserSettings.c 
	Author: David Burns, July 2006 */


#include <ansi_c.h>
#include <cvirte.h>		
#include <userint.h>
#include "LaserSettings.h"
#include "LaserSettings2.h"
#include "vars.h"
#include "GUIDesign.h"
#include "DigitalSettings2.h"

/*************************************************************************************************************************/
int CVICALLBACK LaserSelect (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
/*  Callback Function for when a given laser is selected from the pulldown menu.
	Loads Laser Settings into GUI table */
{
	int laserNum;
	
	switch (event)
		{
		case EVENT_COMMIT:
			GetCtrlVal (panel, control, &laserNum);
			FillLaserTable(laserNum);
			break;
		}
	return 0;
}
/*************************************************************************************************************************/
int CVICALLBACK ExitLaserSettings (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
		{
		case EVENT_COMMIT:
			HidePanel (panelHandle10);
			break;
		}
	return 0;
}
/*************************************************************************************************************************/
int CVICALLBACK LaserSettingsTable (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
/* Callback function for the Settings Table. Updates changes to the corresponding virtual parameters */
{
	int laserNum;
	
	switch (event)
		{
		case EVENT_VAL_CHANGED:
			GetCtrlVal (panel,PANEL_LASER_RING, &laserNum);
			switch(eventData1)
			{
				case 1:
					GetTableCellVal (panelHandle10, PANEL_LASER_SET_TABLE,MakePoint(2,1),LaserProperties[laserNum].Name);
					SetLaserLabels();
					break;
				case 2:
					GetTableCellVal (panelHandle10, PANEL_LASER_SET_TABLE,MakePoint(2,2),LaserProperties[laserNum].IP);
					break;
				case 3:
					GetTableCellVal (panelHandle10, PANEL_LASER_SET_TABLE,MakePoint(2,3),&LaserProperties[laserNum].Port);
					break;
				case 5:
					GetTableCellVal (panelHandle10, PANEL_LASER_SET_TABLE,MakePoint(2,5),&LaserProperties[laserNum].DDS_Clock);
					break;
				case 6:
					GetTableCellVal (panelHandle10, PANEL_LASER_SET_TABLE,MakePoint(2,6),&LaserProperties[laserNum].ICPREF);
					FillLaserTable(laserNum);
					break;
			}
			break;
	}
	return 0;
}
/*************************************************************************************************************************/
int CVICALLBACK SetICPFD (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
/* Callback function for selecting the frequency detect current multiplier, sets the value in he table and data array */
{
	int laserNum;
	
	switch (event)
		{
		case EVENT_COMMIT:
			GetCtrlVal (panel,PANEL_LASER_RING, &laserNum);
			GetCtrlVal (panel, control, &LaserProperties[laserNum].ICP_FD_Mult); 
			SetTableCellVal (panel, PANEL_LASER_SET_TABLE,MakePoint(2,7),LaserProperties[laserNum].ICP_FD_Mult*LaserProperties[laserNum].ICPREF);
			break;
		}
	return 0;
}
/*************************************************************************************************************************/
int CVICALLBACK SetICPWL (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
/* Callback function for selecting the wide loop current multiplier, sets the value in he table and data array */
{
	int laserNum; 
	switch (event)
		{
		case EVENT_COMMIT:
			GetCtrlVal (panel,PANEL_LASER_RING, &laserNum);
			GetCtrlVal (panel, control, &LaserProperties[laserNum].ICP_WL_Mult); 
			SetTableCellVal (panel, PANEL_LASER_SET_TABLE,MakePoint(2,8),LaserProperties[laserNum].ICP_WL_Mult*LaserProperties[laserNum].ICPREF);
			break;
		}
	return 0;
}
/*************************************************************************************************************************/
int CVICALLBACK setICPFL (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
/* Callback function for selecting the closed loop current multiplier, sets the value in he table and data array */
{
	int laserNum; 
	switch (event)
		{
		case EVENT_COMMIT:
			GetCtrlVal (panel,PANEL_LASER_RING, &laserNum);
			GetCtrlVal (panel, control, &LaserProperties[laserNum].ICP_FL_Mult); 
			SetTableCellVal (panel, PANEL_LASER_SET_TABLE,MakePoint(2,9),LaserProperties[laserNum].ICP_FL_Mult*LaserProperties[laserNum].ICPREF);
			break;
		}
	return 0;
}
/*************************************************************************************************************************/
int CVICALLBACK LASER_TOGGLE_CALLBACK (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
/* Callback funtion for the Laser on/off button. */
{
	int laserNum;
	
	switch (event)
		{
		case EVENT_COMMIT:
			GetCtrlVal (panel,PANEL_LASER_RING, &laserNum);
			GetCtrlVal (panel, control, &LaserProperties[laserNum].Active);
			SetDigitalChannels();
			SetLaserLabels();
			break;
		}
	return 0;
}
/*************************************************************************************************************************/
void SetLaserLabels(void)
/*  Copies the LASER_NAMES into the appropriate label locations on the panel. Red bkgnd applied to active lasers, white 
	for inactive lasers. */
{
	int i;
	for(i=0;i<NUMBERLASERS;i++)
	{
		SetTableCellVal (panelHandle, PANEL_TBL_ANAMES, MakePoint(1,i+NUMBERANALOGCHANNELS+NUMBERDDS+1), LaserProperties[i].Name);
		
		if (LaserProperties[i].Active ==1)
			SetTableCellAttribute (panelHandle, PANEL_TBL_ANAMES,MakePoint(1,i+NUMBERANALOGCHANNELS+NUMBERDDS+1), ATTR_TEXT_BGCOLOR,VAL_RED); 
		else
			SetTableCellAttribute (panelHandle, PANEL_TBL_ANAMES,MakePoint(1,i+NUMBERANALOGCHANNELS+NUMBERDDS+1), ATTR_TEXT_BGCOLOR,VAL_WHITE);
			
		ReplaceListItem (panelHandle10,PANEL_LASER_RING,i,LaserProperties[i].Name,i);
	}
}
/*************************************************************************************************************************/
void FillLaserTable(int laserNum)
/*  Loads the laser table (for display) with the stored virtual parameters for the given laser */
{
	
	SetTableCellVal (panelHandle10, PANEL_LASER_SET_TABLE,MakePoint(2,1),LaserProperties[laserNum].Name);
	SetTableCellVal (panelHandle10, PANEL_LASER_SET_TABLE,MakePoint(2,2),LaserProperties[laserNum].IP);
	SetTableCellVal (panelHandle10, PANEL_LASER_SET_TABLE,MakePoint(2,3),LaserProperties[laserNum].Port);
	SetTableCellVal (panelHandle10, PANEL_LASER_SET_TABLE,MakePoint(2,4),LaserProperties[laserNum].DigitalChannel);
	SetTableCellVal (panelHandle10, PANEL_LASER_SET_TABLE,MakePoint(2,5),LaserProperties[laserNum].DDS_Clock);
	SetTableCellVal (panelHandle10, PANEL_LASER_SET_TABLE,MakePoint(2,6),LaserProperties[laserNum].ICPREF);
	SetTableCellVal (panelHandle10, PANEL_LASER_SET_TABLE,MakePoint(2,7),LaserProperties[laserNum].ICP_FD_Mult*LaserProperties[laserNum].ICPREF);
	SetTableCellVal (panelHandle10, PANEL_LASER_SET_TABLE,MakePoint(2,8),LaserProperties[laserNum].ICP_WL_Mult*LaserProperties[laserNum].ICPREF);
	SetTableCellVal (panelHandle10, PANEL_LASER_SET_TABLE,MakePoint(2,9),LaserProperties[laserNum].ICP_FL_Mult*LaserProperties[laserNum].ICPREF);
	
	SetCtrlVal (panelHandle10, PANEL_LASER_TOGGLE, LaserProperties[laserNum].Active);
	SetCtrlVal (panelHandle10, PANEL_ICPFD_RING, LaserProperties[laserNum].ICP_FD_Mult); 
	SetCtrlVal (panelHandle10, PANEL_ICPWL_RING, LaserProperties[laserNum].ICP_WL_Mult); 
	SetCtrlVal (panelHandle10, PANEL_ICPFL_RING, LaserProperties[laserNum].ICP_FL_Mult); 
	
}
/*************************************************************************************************************************/
void LaserSettingsInit(void)
/*  Sets the LASER_CHANNEL array with values defined in LaserSettings2.h
	Fills table with laser0 data and sets ring menu to laser0 
	Called at program startup */
	
{
	int i;
	
	LaserProperties[0].DigitalChannel=LASCHAN0;
	LaserProperties[1].DigitalChannel=LASCHAN1;
	LaserProperties[2].DigitalChannel=LASCHAN2;
	LaserProperties[3].DigitalChannel=LASCHAN3;
	
	
	for (i=0;i<NUMBERLASERS;i++)
	{
		SetTableCellAttribute (panelHandle, PANEL_TBL_ANAMES, MakePoint(2,i+NUMBERANALOGCHANNELS+NUMBERDDS+1), ATTR_CELL_TYPE, VAL_CELL_STRING);
		SetTableCellVal (panelHandle, PANEL_TBL_ANAMES, MakePoint(2,i+NUMBERANALOGCHANNELS+NUMBERDDS+1),"MHz");
	
	}
	
	SetLaserLabels();
	FillLaserTable(0);
	SetCtrlVal (panelHandle10,PANEL_LASER_RING, 0);
}



