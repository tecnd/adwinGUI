//Things to do:
//		in analog control	If timescale too long then prompt for overwrite of following cell
//		overall				-	Verify that channel values aren't used twice..
//							-	Let choose negative channel to turn off.
//							-	Rewrite Analog table to be textual..use tect or BG color to indicate mode
//							-	Use proportionality constants
//	
//		fix bug where the 16th row of the analog (and probably digital) panel arent being updated.
//      guess of what needs to be fixed:  Drawpanel, make update list
//		load, save arrays ?  Analog/digital setup


/*
Caveats:
If making any changes to the program, be aware of several inconsistencies.
Adwin software is base 1 for arrays
C (LabWindows) is base 0 for arrays,
however some controls for LabWindows are base 1   : Tables
and some are base 0								  : Listboxes



//Update History
Apr 15:  Run button flashes with ADwin is operating.
Apr 29:  Fixed Digital line 16.  Loop counted to 15, should have been 16
May 04:  Adding code to delete/insert columns.
May 06:  Added Copy/Paste fcns for column.  Doesn't work 100% yet.... test that channels 16 work for dig. and analog.
May 10:  Fixed a bug where the arrays weren't properly initialized, causing strange values to be written to the adwin
		 Added flashing led's to notify when the ADwin is doing something
May 13:  Fixed 16th line of the panel, now is actually output.  Bug was result of inconsistency with arrays.
		 i.e. Base 0 vs base 1.  
		 - fixed by increasing the internal array size to 17 and starting from 1. (dont read element 0)		 
May 18:  Improved performance.  Added a variable (ChangedVals) that is 0 when no changes have been made since
 		the last run.  If ChangedVals==0, then don't recreate/resend the arrays, shortens delay between runs.
June24:	Add support for more analog output cards.  Change the #define NUMBEROFANALOGCHANNELS xx to reflect the number of channels.
		NOTE:  You need to change this in every .c file. 
		Still need to update the code to use a different channel for digital.  (currently using 17, which will be overwritten
		if using more than 16 analog channels.
July26: Begin adding code to control the DDS (AD9854 from Analog Devices)
	    Use an extra line on the analog table (17 or 25) as the DDS control interface

*/

#define ALLOC_GLOBALS  
#include "main.h"
#include "Adwin.h"
#include <time.h>
#define VAR_DECLS 1
#include "vars.h"
//#include <userint.h>
//#include <stdio.h>
//#include  <Windows.h>
 
//**************INITIALIZE GLOBALS***********************************************************************

extern int currentx=0,currenty=0,currentpage=0;
extern int ischecked[10]={0},isdimmed=0;
extern int ChangedVals=1;
extern struct AnalogTableValues AnalogTable[17][NUMBERANALOGCHANNELS+1][10]={1,0,1};
	// the structure is the values/elements contained at each point in the 
	// analog panel.  The array aval, is set up as [x][y][page]
	
extern int DigTableValues[17][NUMBERDIGITALCHANNELS+1][10]={0};
extern int ChMap[NUMBERANALOGCHANNELS+1]={0};	// The channel mapping (for analog). i.e. if we program line 1 as channel 
				// 12, the ChMap[12]=1

extern double TimeArray[17][10]={0};
extern struct AnalogChannelProperties AChName[NUMBERANALOGCHANNELS+1]={0,"","",0,1};		

extern double EventPeriod=DefaultEventPeriod;

extern struct DDSClock DDSFreq = {15.036, 8, 120.288}; 

//************DONE GLOBALS******************************************************************
int main (int argc, char *argv[])
{
	int i,j,k;
	if (InitCVIRTE (0, argv, 0) == 0)
		return -1;	/* out of memory */
	if ((panelHandle = LoadPanel (0, "GUIDesign.uir", PANEL)) < 0)
		return -1;
	if ((panelHandle2 = LoadPanel (0, "AnalogSettings.uir", PANEL)) < 0)
		return -1;
	if ((panelHandle3 = LoadPanel (0, "DigitalSettings.uir", PANEL)) < 0)
		return -1;
	if ((panelHandle4 = LoadPanel (0, "AnalogControl.uir", PANEL)) < 0)
		return -1;
	if ((panelHandle5 = LoadPanel (0, "DDSSettings.uir", PANEL)) < 0)
		return -1;	
	if ((panelHandle6 = LoadPanel (0, "DDSControl.uir", PANEL)) < 0)
		return -1;
		
	//initialize the dds array
	for (i=0;i<17;i++)
	{
		ddstable[i] = (DDSOPTIONS *) malloc(sizeof(DDSOPTIONS) * 10);
	}
		
	//Load bitmaps
	SetCtrlAttribute (panelHandle, PANEL_DEBUG, ATTR_VISIBLE, 0);
	// Initialize arrays (to avoid undefined elements causing -99 to be written)
	for (j=0;j<=NUMBERANALOGCHANNELS;j++) {		 //ramp over # of analog chanels
		AChName[j].tfcn=1;
		AChName[j].tbias=0;
		AChName[j].resettozero=1;
		for(i=0;i<=16;i++){		// ramp over # of cells per page
			for(k=0;k<=9;k++){	// ramp over pages
				AnalogTable[i][j][k].fcn=1;
				AnalogTable[i][j][k].fval=0.0;
				AnalogTable[i][j][k].tscale=1;
				if (j<=16) {DigTableValues[i][j][k]=0;}
			}
		}
	}
	
	//initialize ddstable
	for (i=0;i<17;i++)
	{
		for (j=0;j<10;j++)
		{
			ddstable[i][j].frequency1 = 0;
			ddstable[i][j].frequency2 = 0;
			ddstable[i][j].current = 0;
			ddstable[i][j].transition_type = LINEAR;
			ddstable[i][j].is_update = FALSE;
			ddstable[i][j].delta_time = 0;
			ddstable[i][j].is_stop = FALSE;
			ddstable[i][j].start_time = 0;
			ddstable[i][j].new_settings = FALSE;
		}
	}
	
	// done initializing
	
	
	LoadLastSettings(1);
	
	SetCtrlVal (panelHandle, PANEL_TB_SHOWPHASE1, 1);
	SetCtrlVal (panelHandle, PANEL_TB_SHOWPHASE2, 0);
	SetCtrlVal (panelHandle, PANEL_TB_SHOWPHASE3, 0);
	SetCtrlVal (panelHandle, PANEL_TB_SHOWPHASE4, 0);
	SetCtrlVal (panelHandle, PANEL_TB_SHOWPHASE5, 0);
	SetCtrlVal (panelHandle, PANEL_TB_SHOWPHASE6, 0);
	SetCtrlVal (panelHandle, PANEL_TB_SHOWPHASE7, 0);

	EventPeriod=1.00;
	ClearListCtrl(panelHandle,PANEL_DEBUG);
	menuHandle=GetPanelMenuBar(panelHandle);
	SetMenuBarAttribute (menuHandle, MENU_UPDATEPERIOD_SETGD5, ATTR_CHECKED, 0);
	SetMenuBarAttribute (menuHandle, MENU_UPDATEPERIOD_SETGD10, ATTR_CHECKED, 0);
	SetMenuBarAttribute (menuHandle, MENU_UPDATEPERIOD_SETGD100, ATTR_CHECKED, 0);
	SetMenuBarAttribute (menuHandle, MENU_UPDATEPERIOD_SETGD1000, ATTR_CHECKED, 1);
	currentpage=1;
	
	// autochange the size of the analog table on main panel
//	DrawNewTable(0);
	Initialization();  
	DisplayPanel (panelHandle);
	RunUserInterface ();
	DiscardPanel (panelHandle);
	
	//clean up DDS Table
	for (i=0;i<17;i++)
	{
		free(ddstable[i]);
	}
	
	return 0;
}
//**********************************************************************************
void Initialization()
{
	int i=0,cellheight=0,fontsize=0;
	int j=0;
	char str_list_val[5];
	
	//Add in any extra rows (if the number of channels increases)
	InsertTableRows (panelHandle, PANEL_ANALOGTABLE, 16,
						 NUMBERANALOGCHANNELS-16, VAL_CELL_PICTURE);
	InsertTableRows (panelHandle, PANEL_DIGTABLE, 16,
						 NUMBERDIGITALCHANNELS-16, VAL_CELL_PICTURE);
	
	
	// Change Analog Settings window
	SetCtrlAttribute (panelHandle2, ANLGPANEL_NUM_ACH_LINE,ATTR_MAX_VALUE, NUMBERANALOGCHANNELS);
	SetCtrlAttribute (panelHandle2, ANLGPANEL_NUM_ACHANNEL,ATTR_MAX_VALUE, NUMBERANALOGCHANNELS);
	
	// change GUI
	
	SetCtrlAttribute (panelHandle, PANEL_ANALOGTABLE, ATTR_NUM_VISIBLE_ROWS, NUMBERANALOGCHANNELS+1);
	SetCtrlAttribute (panelHandle, PANEL_DIGTABLE, ATTR_NUM_VISIBLE_ROWS, NUMBERDIGITALCHANNELS);

	SetCtrlAttribute (panelHandle, PANEL_TBL_ANAMES, ATTR_VISIBLE_LINES, NUMBERANALOGCHANNELS+1);
	SetCtrlAttribute (panelHandle, PANEL_TBL_ANALOGUNITS, ATTR_VISIBLE_LINES, NUMBERANALOGCHANNELS+1); 
	SetCtrlAttribute (panelHandle, PANEL_TBL_DIGNAMES, ATTR_VISIBLE_LINES, NUMBERDIGITALCHANNELS);


	SetCtrlAttribute (panelHandle, PANEL_LABEL_1, ATTR_VISIBLE, 1);
	SetCtrlAttribute (panelHandle, PANEL_LABEL_2, ATTR_VISIBLE, 0);
	SetCtrlAttribute (panelHandle, PANEL_LABEL_3, ATTR_VISIBLE, 0);
	SetCtrlAttribute (panelHandle, PANEL_LABEL_4, ATTR_VISIBLE, 0);
	SetCtrlAttribute (panelHandle, PANEL_LABEL_5, ATTR_VISIBLE, 0);
	SetCtrlAttribute (panelHandle, PANEL_LABEL_6, ATTR_VISIBLE, 0);
	SetCtrlAttribute (panelHandle, PANEL_LABEL_7, ATTR_VISIBLE, 0);
	




	SetCtrlAttribute (panelHandle, PANEL_LABEL_1, ATTR_LEFT, 165);
	SetCtrlAttribute (panelHandle, PANEL_LABEL_1, ATTR_TOP, 88);
	SetCtrlAttribute (panelHandle, PANEL_LABEL_2, ATTR_LEFT, 165);
	SetCtrlAttribute (panelHandle, PANEL_LABEL_2, ATTR_TOP, 88);
	SetCtrlAttribute (panelHandle, PANEL_LABEL_3, ATTR_LEFT, 165);
	SetCtrlAttribute (panelHandle, PANEL_LABEL_3, ATTR_TOP, 88);
	SetCtrlAttribute (panelHandle, PANEL_LABEL_4, ATTR_LEFT, 165);
	SetCtrlAttribute (panelHandle, PANEL_LABEL_4, ATTR_TOP, 88);
	SetCtrlAttribute (panelHandle, PANEL_LABEL_5, ATTR_LEFT,165);
	SetCtrlAttribute (panelHandle, PANEL_LABEL_5, ATTR_TOP, 88);
	SetCtrlAttribute (panelHandle, PANEL_LABEL_6, ATTR_LEFT, 165);
	SetCtrlAttribute (panelHandle, PANEL_LABEL_6, ATTR_TOP, 88);
	SetCtrlAttribute (panelHandle, PANEL_LABEL_7, ATTR_LEFT, 165);
	SetCtrlAttribute (panelHandle, PANEL_LABEL_7, ATTR_TOP, 88);
	
	
	
	
	
	for (i=1;i<=NUMBERDIGITALCHANNELS;i++)
	{
		SetTableCellVal (panelHandle, PANEL_TBL_DIGNAMES, MakePoint(2,i), i);
	}

	SetAnalogChannels();
	for (i=1;i<=NUMBERANALOGCHANNELS;i++)
	{
		SetTableCellVal (panelHandle, PANEL_TBL_ANAMES, MakePoint(2,i), i);
	}
	   
	DrawNewTable(0);
	
	//set to display both analog and digital channels
	SetChannelDisplayed(1);
	
	//set to graphical display
	SetDisplayType(VAL_CELL_PICTURE);
	return;
	
}
//***************************************************************************************************
void ConvertIntToStr(int int_val, char *int_str)
{

	int i,j;
	
	for (i=j=floor(log10(int_val));i>=0;i--)
	{
		int_str[i] = (char) (((int) '0') + floor(((int) floor((int_val/pow(10,(j-i))))%10)));
	}
	
	int_str[j+1] = '\0';
	
	return;
	

}


