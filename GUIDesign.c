#include <userint.h>
#include "GUIDesign.h"
#include "Adwin.h"
//#include <ansi_c.h>
//#include "AnalogSettings.h"
//#include "AnalogSettings.c" 
//#include <cvirte.h>		
//#include <userint.h>
#include "main.h"
//#include "GUIDesign.h"
extern int ChangedVals;
extern int panelHandle,panelHandle2,panelHandle3,panelHandle4;
extern int currentx,currenty,currentpage;
extern int pic_static,pic_off,pic_change,pic_don;
extern int ischecked[10],isdimmed;
extern int DigTableValues[17][17][10];
extern double TimeArray[17][10];
extern struct AnalogTableValues{
	int		fcn;		//fcn is an integer refering to a function to use.
						// 0-step, 1-linear, 2- exp, 3- 'S' curve
	double 	fval;		//the final value
	double	tscale;		//the timescale to approach final value
	} AnalogTable[17][17][10];

extern int menuHandle;
extern double EventPeriod;
extern int processnum;
//Clipboard for copy/paste functions
double TimeClip;
struct AnalogTableClip{
	int		fcn;		//fcn is an integer refering to a function to use.
						// 0-step, 1-linear, 2- exp, 3- 'S' curve
	double 	fval;		//the final value
	double	tscale;		//the timescale to approach final value
	} AnalogClip[17];
int DigClip[17];


//******************************************************************
int CVICALLBACK QuitCallback (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
		{
		case EVENT_COMMIT:
			QuitUserInterface(1);
			break;
		}
	return 0;
}

//******************************************************************

void CVICALLBACK ANALOGSET_CALLBACK (int menuBar, int menuItem, void *callbackData,
		int panel)
{
	DisplayPanel(panelHandle2);
}
//******************************************************************

void CVICALLBACK DIGITALSET_CALLBACK (int menuBar, int menuItem, void *callbackData,
		int panel)
{
	DisplayPanel(panelHandle3);
}
//******************************************************************

void CVICALLBACK MENU_CALLBACK (int menuBar, int menuItem, void *callbackData,
		int panel)
{
	switch(menuItem)
	{
		case MENU_FILE_SAVESET:
			SaveSettings();
			break;
		
		case MENU_FILE_LOADSET:
			LoadSettings();
			break;
	
	}	
}

//******************************************************************
void LoadSettings(void)
{
	int status;
	//If .ini exists, then open the file dialog.  Else just open the file dialog.  Add a method to load 
	//last used settings on program startup.
	FILE *fpini;
	char fname[100]="",c,fsavename[500]="";
	int i=0,j=0,k=0,inisize=0;
	//Check if .ini file exists.  Load it if it does.
	if(!(fpini=fopen("gui.ini","r"))==NULL)		
	{											
		while (fscanf(fpini,"%c",&c) != -1) fname[inisize++] = c;  //Read the contained name
	}
	fclose(fpini);
	
	status=FileSelectPopup ("", "*.pan", "", "Load Settings", VAL_LOAD_BUTTON, 0, 0, 1, 1,fsavename );
	if(!(status==0))
	{
		RecallPanelState(PANEL, fsavename, 1);
		LoadArrays(fsavename,strlen( fsavename));
	}
	else
	{
		MessagePopup ("File Error", "No file was selected");
	}
	DrawNewTable(0);
}
//*******************************************************************************************
void LoadLastSettings(int check)
{
	//If .ini exists, then open the file dialog.  Else just open the file dialog.  Add a method to load 
	//last used settings on program startup.
	FILE *fpini;
	char fname[100]="",c,fsavename[500]="",loadname[100]="";
	int i=0,j=0,k=0,inisize=0;
	//Check if .ini file exists.  Load it if it does.
	if(!(fpini=fopen("gui.ini","r"))==NULL)		
	{											
		while (fscanf(fpini,"%c",&c) != -1) fname[inisize++] = c;  //Read the contained name
	}
	fclose(fpini);
	
	c=fname[inisize-1];
	strncpy(loadname,fname,inisize-2);
	if((check==0)||(c==49))			  // 49 is the ascii code for "1"
	{
		RecallPanelState(PANEL, loadname, 1);
		LoadArrays(fname,strlen( loadname));
	}
	DrawNewTable(0);
}
//*********************************************************************************************

void SaveSettings(void)
{
	// Save settings:  First look for file .ini  This will be a simple 1 line file staating the name of the last file
	//saved.  Load this up and use as the starting name in the file dialog.
	FILE *fpini;
	char fname[100]="",c,fsavename[500]="";
	int i=0,j=0,k=0,inisize=0,status,loadonboot=0;
	//Check if .ini file exists.  Load it if it does.
	if(!(fpini=fopen("gui.ini","r"))==NULL)		// if "proto.ini" exists, then read it  Just contains a filename.  
	{												//If not, do nothing
		while (fscanf(fpini,"%c",&c) != -1) fname[inisize++] = c;  //Read the contained name
	}
 	fclose(fpini);
 	
	status=FileSelectPopup ("", "*.pan", "", "Save Settings", VAL_SAVE_BUTTON, 0, 0, 1, 1,fsavename);
	GetMenuBarAttribute (menuHandle, MENU_FILE_BOOTLOAD, ATTR_CHECKED, &loadonboot);
	if(!(status==0))
	{
		SavePanelState(PANEL, fsavename, 1);
		if(!(fpini=fopen("gui.ini","w"))==NULL)
		{
			fprintf(fpini,fsavename);
			fprintf(fpini,"\n%d",loadonboot);
		}
		fclose(fpini);	
		SaveArrays(fsavename, strlen(fsavename));
	}
	else
	{
		MessagePopup ("File Error", "No file was selected");
	}
}
//*********************************************************************
int CVICALLBACK ANALOGTABLE_CALLBACK (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	int cx=0,cy=0,pixleft=0,pixtop=0;
	Point cpoint={0,0};
	char buff[80]="";
	int leftbuttondown,rightbuttondown,keymod;
	
	ChangedVals=1;
	switch (event)
		{
		case EVENT_LEFT_DOUBLE_CLICK:
			cpoint.x=0;cpoint.y=0;
			GetActiveTableCell(panelHandle, PANEL_ANALOGTABLE, &cpoint);
			//now set the indicators in the control panel title..ie units
			currentx=cpoint.x;
			currenty=cpoint.y;
			currentpage=GetPage();
			sprintf(buff,"x:%d   y:%d    z:%d",currentx,currenty,currentpage);
			SetPanelAttribute (panelHandle4, ATTR_TITLE, buff);
			//Read the mouse position and open window there.
			GetGlobalMouseState (&panelHandle, &pixleft, &pixtop, &leftbuttondown,
								 &rightbuttondown, &keymod);
			SetPanelAttribute (panelHandle4, ATTR_LEFT, pixleft);
			SetPanelAttribute (panelHandle4, ATTR_TOP, pixtop);
			SetControlPanel();
			DisplayPanel(panelHandle4);
			
			break;
		}
	return 0;
}
//********************************************************************************************
int GetPage(void)
{
 return currentpage;
 //Scan and find out what page we are on....
}
//********************************************************************************************
void DrawNewTable(int isdimmed)
//if isdimmed=0  Draw everything
//if isdimmed=1  Dim out appropriate columns....
// Make isdimmed a global I guess...
{
	int i,j,k,cfcn,picmode,page,cmode,pic;
 	double vlast=0,vnow=0;
 	int dimset=0,nozerofound;
 	Point pval={0,0};
 	SetCtrlAttribute (panelHandle, PANEL_ANALOGTABLE, ATTR_VISIBLE, 0);
	SetCtrlAttribute (panelHandle, PANEL_DIGTABLE, ATTR_VISIBLE, 0);	
 	SetCtrlAttribute (panelHandle, PANEL_TIMETABLE, ATTR_VISIBLE, 0);
	page=GetPage();
	
 	CheckActivePages();
 	if(ischecked[page]==0)
 	{
 		SetCtrlAttribute (panelHandle, PANEL_ANALOGTABLE, ATTR_DIMMED, 1);
 		SetCtrlAttribute (panelHandle, PANEL_DIGTABLE, ATTR_DIMMED, 1);
		SetCtrlAttribute (panelHandle, PANEL_TIMETABLE, ATTR_DIMMED, 1); 		
 	}
	else
	{
		SetCtrlAttribute (panelHandle, PANEL_ANALOGTABLE, ATTR_DIMMED, 0);
		SetCtrlAttribute (panelHandle, PANEL_DIGTABLE, ATTR_DIMMED, 0);
		SetCtrlAttribute (panelHandle, PANEL_TIMETABLE, ATTR_DIMMED, 0);
	}
 	picmode=0;
	for(i=1;i<=14;i++)
	{
		SetTableCellAttribute (panelHandle, PANEL_TIMETABLE, MakePoint(i,1),ATTR_CELL_DIMMED, 0);
		for(j=1;j<=16;j++)
		{
			SetTableCellVal (panelHandle, PANEL_ANALOGTABLE, MakePoint(i,j), 0);
			SetTableCellAttribute (panelHandle, PANEL_ANALOGTABLE, MakePoint(i,j),ATTR_CELL_DIMMED, 0);
			SetTableCellAttribute (panelHandle, PANEL_DIGTABLE, MakePoint(i,j),ATTR_CELL_DIMMED, 0);
			//check what's happening...care about off, static, changing
			// step.  Vfinal=0 and last Vfinal=0  -> off
			// step	  Vfinal=Vconst and last Vconst=Vconst
			// changing mode>0			
			cmode=AnalogTable[i][j][page].fcn;
			vlast=AnalogTable[i-1][j][page].fval;
			vnow=AnalogTable[i][j][page].fval;			

			SetTableCellAttribute (panelHandle, PANEL_ANALOGTABLE,MakePoint(i,j), ATTR_TEXT_BGCOLOR,VAL_GRAY);
			if(cmode==1)
			{
				if(vnow==0)
				{
					SetTableCellAttribute (panelHandle, PANEL_ANALOGTABLE,MakePoint(i,j), ATTR_TEXT_BGCOLOR,VAL_GRAY);
				}
				else
				{
					SetTableCellAttribute (panelHandle, PANEL_ANALOGTABLE,MakePoint(i,j), ATTR_TEXT_BGCOLOR,VAL_RED);
				}
			}
			if(cmode==2)
			{
				SetTableCellAttribute (panelHandle, PANEL_ANALOGTABLE,MakePoint(i,j), ATTR_TEXT_BGCOLOR,VAL_GREEN);
			}
			if(cmode==3)
			{
				SetTableCellAttribute (panelHandle, PANEL_ANALOGTABLE,MakePoint(i,j), ATTR_TEXT_BGCOLOR,VAL_BLUE);
			}
			if(cmode==4)
			{
				SetTableCellAttribute (panelHandle, PANEL_ANALOGTABLE,MakePoint(i,j), ATTR_TEXT_BGCOLOR,VAL_MAGENTA);
			}
			if(vnow==vlast)
			{
				SetTableCellAttribute (panelHandle, PANEL_ANALOGTABLE,MakePoint(i,j), ATTR_TEXT_BGCOLOR,VAL_GRAY);
			}
			if(vnow==0&&cmode==1)
			{
				SetTableCellAttribute (panelHandle, PANEL_ANALOGTABLE,MakePoint(i,j), ATTR_TEXT_BGCOLOR,VAL_GRAY);
			}
			if(((vnow==vlast)&&(!(vlast==0)))||((cmode=0)&&(!(vlast==0))))
			{
				SetTableCellAttribute (panelHandle, PANEL_ANALOGTABLE,MakePoint(i,j), ATTR_TEXT_BGCOLOR,MakeColor(255,128,128));
			}
			
			if(DigTableValues[i][j][page]==1)
			{
		
			SetTableCellAttribute (panelHandle, PANEL_DIGTABLE,MakePoint(i,j), ATTR_TEXT_BGCOLOR,VAL_RED);
			
			//	SetTableCellAttrib
			//	SetTableCellVal (panelHandle, PANEL_DIGTABLE, MakePoint(i,j), pic_don);
			}
			else
			{
				SetTableCellVal (panelHandle, PANEL_DIGTABLE, MakePoint(i,j), 0);
				SetTableCellAttribute (panelHandle, PANEL_DIGTABLE,MakePoint(i,j), ATTR_TEXT_BGCOLOR,VAL_GRAY);
			}
			//Done digital drawing.
		}
		// update the times row
		SetTableCellVal(panelHandle,PANEL_TIMETABLE,MakePoint(i,1),TimeArray[i][page]);
	}
	//now check if we need to dim out any columns(of timetable,AnalogTable and DigTable
	if(isdimmed==1)
	{
		nozerofound=1;
		for(i=1;i<=14;i++)
		{
			dimset=0;
			if((nozerofound==0)||(TimeArray[i][page]==0))
			{
				nozerofound=0;
				dimset=1;
			}
			else if((nozerofound==1)&&(TimeArray[i][page]<0))
			{
				dimset=1;
			}
			SetTableCellAttribute(panelHandle,PANEL_TIMETABLE,MakePoint(i,1),ATTR_CELL_DIMMED,dimset);	
			for(j=1;j<=16;j++)
			{
				SetTableCellAttribute (panelHandle, PANEL_ANALOGTABLE, MakePoint(i,j),ATTR_CELL_DIMMED, dimset);
				SetTableCellAttribute (panelHandle, PANEL_DIGTABLE, MakePoint(i,j),ATTR_CELL_DIMMED, dimset);			
			}
		}
	}
	SetCtrlAttribute (panelHandle, PANEL_ANALOGTABLE, ATTR_VISIBLE, 1);
	SetCtrlAttribute (panelHandle, PANEL_DIGTABLE, ATTR_VISIBLE, 1);
	SetCtrlAttribute (panelHandle, PANEL_TIMETABLE, ATTR_VISIBLE, 1);
}

//*************************************************************************************
int CVICALLBACK TOGGLE1_CALLBACK (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
		{
		case EVENT_COMMIT:
			SetCtrlVal (panelHandle, PANEL_TB_SHOWPHASE1, 1);
			SetCtrlVal (panelHandle, PANEL_TB_SHOWPHASE2, 0);
			SetCtrlVal (panelHandle, PANEL_TB_SHOWPHASE3, 0);
			SetCtrlVal (panelHandle, PANEL_TB_SHOWPHASE4, 0);
			SetCtrlVal (panelHandle, PANEL_TB_SHOWPHASE5, 0);
			SetCtrlVal (panelHandle, PANEL_TB_SHOWPHASE6, 0);
			SetCtrlVal (panelHandle, PANEL_TB_SHOWPHASE7, 0);
			currentpage=1;
			DrawNewTable(isdimmed);
			break;
		}
	return 0;
}

int CVICALLBACK TOGGLE2_CALLBACK (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
		{
		case EVENT_COMMIT:
			SetCtrlVal (panelHandle, PANEL_TB_SHOWPHASE1, 0);
			SetCtrlVal (panelHandle, PANEL_TB_SHOWPHASE2, 1);
			SetCtrlVal (panelHandle, PANEL_TB_SHOWPHASE3, 0);
			SetCtrlVal (panelHandle, PANEL_TB_SHOWPHASE4, 0);
			SetCtrlVal (panelHandle, PANEL_TB_SHOWPHASE5, 0);
			SetCtrlVal (panelHandle, PANEL_TB_SHOWPHASE6, 0);
			SetCtrlVal (panelHandle, PANEL_TB_SHOWPHASE7, 0);
			currentpage=2;
			DrawNewTable(isdimmed);
			break;
		}
	return 0;
}

int CVICALLBACK TOGGLE3_CALLBACK (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
		{
		case EVENT_COMMIT:
			SetCtrlVal (panelHandle, PANEL_TB_SHOWPHASE1, 0);
			SetCtrlVal (panelHandle, PANEL_TB_SHOWPHASE2, 0);
			SetCtrlVal (panelHandle, PANEL_TB_SHOWPHASE3, 1);
			SetCtrlVal (panelHandle, PANEL_TB_SHOWPHASE4, 0);
			SetCtrlVal (panelHandle, PANEL_TB_SHOWPHASE5, 0);
			SetCtrlVal (panelHandle, PANEL_TB_SHOWPHASE6, 0);
			SetCtrlVal (panelHandle, PANEL_TB_SHOWPHASE7, 0);
			currentpage=3;
			DrawNewTable(isdimmed);
			break;
		}
	return 0;
}

int CVICALLBACK TOGGLE4_CALLBACK (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
		{
		case EVENT_COMMIT:
			SetCtrlVal (panelHandle, PANEL_TB_SHOWPHASE1, 0);
			SetCtrlVal (panelHandle, PANEL_TB_SHOWPHASE2, 0);
			SetCtrlVal (panelHandle, PANEL_TB_SHOWPHASE3, 0);
			SetCtrlVal (panelHandle, PANEL_TB_SHOWPHASE4, 1);
			SetCtrlVal (panelHandle, PANEL_TB_SHOWPHASE5, 0);
			SetCtrlVal (panelHandle, PANEL_TB_SHOWPHASE6, 0);
			SetCtrlVal (panelHandle, PANEL_TB_SHOWPHASE7, 0);
			currentpage=4;
			DrawNewTable(isdimmed);			
			break;
		}
	return 0;
}

int CVICALLBACK TOGGLE5_CALLBACK (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
		{
		case EVENT_COMMIT:
			SetCtrlVal (panelHandle, PANEL_TB_SHOWPHASE1, 0);
			SetCtrlVal (panelHandle, PANEL_TB_SHOWPHASE2, 0);
			SetCtrlVal (panelHandle, PANEL_TB_SHOWPHASE3, 0);
			SetCtrlVal (panelHandle, PANEL_TB_SHOWPHASE4, 0);
			SetCtrlVal (panelHandle, PANEL_TB_SHOWPHASE5, 1);
			SetCtrlVal (panelHandle, PANEL_TB_SHOWPHASE6, 0);
			SetCtrlVal (panelHandle, PANEL_TB_SHOWPHASE7, 0);
			currentpage=5;
			DrawNewTable(isdimmed);			
			break;
		}
	return 0;
}

int CVICALLBACK TOGGLE6_CALLBACK (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
		{
		case EVENT_COMMIT:
			SetCtrlVal (panelHandle, PANEL_TB_SHOWPHASE1, 0);
			SetCtrlVal (panelHandle, PANEL_TB_SHOWPHASE2, 0);
			SetCtrlVal (panelHandle, PANEL_TB_SHOWPHASE3, 0);
			SetCtrlVal (panelHandle, PANEL_TB_SHOWPHASE4, 0);
			SetCtrlVal (panelHandle, PANEL_TB_SHOWPHASE5, 0);
			SetCtrlVal (panelHandle, PANEL_TB_SHOWPHASE6, 1);
			SetCtrlVal (panelHandle, PANEL_TB_SHOWPHASE7, 0);
			currentpage=6;
			DrawNewTable(isdimmed);			
			break;
		}
	return 0;
}

int CVICALLBACK TOGGLE7_CALLBACK (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
		{
		case EVENT_COMMIT:
			SetCtrlVal (panelHandle, PANEL_TB_SHOWPHASE1, 0);
			SetCtrlVal (panelHandle, PANEL_TB_SHOWPHASE2, 0);
			SetCtrlVal (panelHandle, PANEL_TB_SHOWPHASE3, 0);
			SetCtrlVal (panelHandle, PANEL_TB_SHOWPHASE4, 0);
			SetCtrlVal (panelHandle, PANEL_TB_SHOWPHASE5, 0);
			SetCtrlVal (panelHandle, PANEL_TB_SHOWPHASE6, 0);
			SetCtrlVal (panelHandle, PANEL_TB_SHOWPHASE7, 1);
			currentpage=7;
			DrawNewTable(isdimmed);
			break;
		}
	return 0;
}

void CheckActivePages(void)
{
  	int bool;
  	GetCtrlVal (panelHandle, PANEL_CHECKBOX, &bool);
  	ischecked[1]=bool;
  	GetCtrlVal (panelHandle, PANEL_CHECKBOX_2, &bool);
  	ischecked[2]=bool;
 	GetCtrlVal (panelHandle, PANEL_CHECKBOX_3, &bool);
  	ischecked[3]=bool;
  	GetCtrlVal (panelHandle, PANEL_CHECKBOX_4, &bool);
  	ischecked[4]=bool;
  	GetCtrlVal (panelHandle, PANEL_CHECKBOX_5, &bool);
  	ischecked[5]=bool;
  	GetCtrlVal (panelHandle, PANEL_CHECKBOX_6, &bool);
  	ischecked[6]=bool;
  	GetCtrlVal (panelHandle, PANEL_CHECKBOX_7, &bool);
	ischecked[7]=bool;  	
}
//***************************************************************************************************

int CVICALLBACK DIGTABLE_CALLBACK (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	int status=0,i_pict=0,page,digval;
	Point pval={0,0};
	
	ChangedVals=1;
	page=GetPage();
	switch (event)
		{
		case EVENT_LEFT_DOUBLE_CLICK:
			GetActiveTableCell(PANEL,PANEL_DIGTABLE,&pval);
			//GetTableCellVal(PANEL,PANEL_DIGTABLE,pval,&i_pict);
			digval=DigTableValues[pval.x][pval.y][page];			
			if(digval==0)
			{
			//	SetTableCellVal(PANEL,PANEL_DIGTABLE,pval,pic_don);
				SetTableCellAttribute (panelHandle, PANEL_DIGTABLE,pval, ATTR_TEXT_BGCOLOR,VAL_RED);
				DigTableValues[pval.x][pval.y][page]=1;
			}
			else
			{
				SetTableCellAttribute (panelHandle, PANEL_DIGTABLE,pval, ATTR_TEXT_BGCOLOR,VAL_GRAY);
				//SetTableCellVal(PANEL,PANEL_DIGTABLE,pval,0);
				DigTableValues[pval.x][pval.y][page]=0;
			}
			break;
		}
	return 0;
}
//***************************************************************************************************
int CVICALLBACK TIMETABLE_CALLBACK (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	double dval,oldtime,ratio,tscaleold;
	Point pval={0,0};
	int page,i,j;
	
	ChangedVals=1;
	switch (event)
		{
		case EVENT_VAL_CHANGED:
		//EVENT_VAL_CHANGED only seems to pick up the last changed values, or values changed using the 
		//controls (i.e. increment arrows).  Similar problems with other events...reading all times(for the page)
		// at a change event seems to work.
			page=GetPage();
			
			for(i=1;i<=14;i++)
			{
				oldtime=TimeArray[i][page];
				GetTableCellVal(PANEL,PANEL_TIMETABLE,MakePoint(i,1),&dval);
				TimeArray[i][page]=dval;
				//now rescale the time scale for waveform fcn. Go over all 16 analog channels
				ratio=dval/oldtime;
				if(oldtime==0) {ratio=1;}
				
				for(j=1;j<=16;j++)
				{
					tscaleold=AnalogTable[i][j][page].tscale;
					AnalogTable[i][j][page].tscale=tscaleold*ratio;
				}
			}
			break;
		case EVENT_LEFT_DOUBLE_CLICK:
			isdimmed=0;
			DrawNewTable(isdimmed);
			break;
		}
	return 0;
}
//***************************************************************************************************

int CVICALLBACK CMD_RUN_CALLBACK (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	int repeat=0;
	switch (event)
		{
		case EVENT_COMMIT:
			GetCtrlVal(panelHandle,PANEL_TOGGLEREPEAT,&repeat);
			if(repeat==1)
			{
			SetCtrlAttribute (panelHandle, PANEL_TIMER, ATTR_ENABLED, 1);
			//activate timer
			}
			else
			{
			SetCtrlAttribute (panelHandle, PANEL_TIMER, ATTR_ENABLED, 0);
			//deactivate timer
			}
			RunOnce();
			break;
		}
		
	return 0;
}



void CVICALLBACK MENU_DEBUG_CALLBACK (int menuBar, int menuItem, void *callbackData,
		int panel)
{
	int status;
	GetCtrlAttribute (panelHandle, PANEL_DEBUG, ATTR_VISIBLE, &status);
	if(status==0){status=1;}
	else{status=0;}
	SetCtrlAttribute (panelHandle, PANEL_DEBUG, ATTR_VISIBLE, status);
}

void CVICALLBACK TITLE1_CALLBACK (int menuBar, int menuItem, void *callbackData,
		int panel)
{
	char buff[80];
	int status;
	status=PromptPopup ("Enter control button label", "Enter a new label for Phase 1 control button",buff, sizeof buff-2);
	if(status==0)
	{
		status = SetCtrlAttribute (panelHandle, PANEL_TB_SHOWPHASE1,ATTR_OFF_TEXT, buff);
		status = SetCtrlAttribute (panelHandle, PANEL_TB_SHOWPHASE1,ATTR_ON_TEXT, buff);
	}
}

void CVICALLBACK TITLE2_CALLBACK (int menuBar, int menuItem, void *callbackData,
		int panel)
{
	char buff[80];
	int status;
	status=PromptPopup ("Enter control button label", "Enter a new label for Phase 2 control button",buff, sizeof buff-2);
	if(status==0)
	{
		status = SetCtrlAttribute (panelHandle, PANEL_TB_SHOWPHASE2,ATTR_OFF_TEXT, buff);
		status = SetCtrlAttribute (panelHandle, PANEL_TB_SHOWPHASE2,ATTR_ON_TEXT, buff);
	}
}

void CVICALLBACK TITLE3_CALLBACK (int menuBar, int menuItem, void *callbackData,
		int panel)
{
	char buff[80];
	int status;
	status=PromptPopup ("Enter control button label", "Enter a new label for Phase 3 control button",buff, sizeof buff-2);
	if(status==0)
	{
		status = SetCtrlAttribute (panelHandle, PANEL_TB_SHOWPHASE3,ATTR_OFF_TEXT, buff);
		status = SetCtrlAttribute (panelHandle, PANEL_TB_SHOWPHASE3,ATTR_ON_TEXT, buff);
	}
}

void CVICALLBACK TITLE4_CALLBACK (int menuBar, int menuItem, void *callbackData,
		int panel)
{
	char buff[80];
	int status;
	status=PromptPopup ("Enter control button label", "Enter a new label for Phase 4 control button",buff, sizeof buff-2);
	if(status==0)
	{
		status = SetCtrlAttribute (panelHandle, PANEL_TB_SHOWPHASE4,ATTR_OFF_TEXT, buff);
		status = SetCtrlAttribute (panelHandle, PANEL_TB_SHOWPHASE4,ATTR_ON_TEXT, buff);
	}
}

void CVICALLBACK TITLE5_CALLBACK (int menuBar, int menuItem, void *callbackData,
		int panel)
{
	char buff[80];
	int status;
	status=PromptPopup ("Enter control button label", "Enter a new label for Phase 1 control button",buff, sizeof buff-2);
	if(status==0)
	{
		status = SetCtrlAttribute (panelHandle, PANEL_TB_SHOWPHASE5,ATTR_OFF_TEXT, buff);
		status = SetCtrlAttribute (panelHandle, PANEL_TB_SHOWPHASE5,ATTR_ON_TEXT, buff);
	}
}

void CVICALLBACK TITLE6_CALLBACK (int menuBar, int menuItem, void *callbackData,
		int panel)
{
	char buff[80];
	int status;
	status=PromptPopup ("Enter control button label", "Enter a new label for Phase 6 control button",buff, sizeof buff-2);
	if(status==0)
	{
		status = SetCtrlAttribute (panelHandle, PANEL_TB_SHOWPHASE6,ATTR_OFF_TEXT, buff);
		status = SetCtrlAttribute (panelHandle, PANEL_TB_SHOWPHASE6,ATTR_ON_TEXT, buff);
	}
}

void CVICALLBACK TITLE7_CALLBACK (int menuBar, int menuItem, void *callbackData,
		int panel)
{
	char buff[80];
	int status;
	status=PromptPopup ("Enter control button label", "Enter a new label for Phase 7 control button",buff, sizeof buff-2);
	if(status==0)
	{
		status = SetCtrlAttribute (panelHandle, PANEL_TB_SHOWPHASE7,ATTR_OFF_TEXT, buff);
		status = SetCtrlAttribute (panelHandle, PANEL_TB_SHOWPHASE7,ATTR_ON_TEXT, buff);
	}
}



void CVICALLBACK SETGD5_CALLBACK (int menuBar, int menuItem, void *callbackData,
		int panel)
{
	SetMenuBarAttribute (menuHandle, MENU_UPDATEPERIOD_SETGD5, ATTR_CHECKED, 1);
	SetMenuBarAttribute (menuHandle, MENU_UPDATEPERIOD_SETGD10, ATTR_CHECKED, 0);
	SetMenuBarAttribute (menuHandle, MENU_UPDATEPERIOD_SETGD100, ATTR_CHECKED, 0);
	SetMenuBarAttribute (menuHandle, MENU_UPDATEPERIOD_SETGD1000, ATTR_CHECKED, 0);
	EventPeriod=0.005;

}

void CVICALLBACK SETGD10_CALLBACK (int menuBar, int menuItem, void *callbackData,
		int panel)
{
    SetMenuBarAttribute (menuHandle, MENU_UPDATEPERIOD_SETGD5, ATTR_CHECKED, 0);
	SetMenuBarAttribute (menuHandle, MENU_UPDATEPERIOD_SETGD10, ATTR_CHECKED, 1);
	SetMenuBarAttribute (menuHandle, MENU_UPDATEPERIOD_SETGD100, ATTR_CHECKED, 0);
	SetMenuBarAttribute (menuHandle, MENU_UPDATEPERIOD_SETGD1000, ATTR_CHECKED, 0);
	EventPeriod=0.010;
}

void CVICALLBACK SETGD100_CALLBACK (int menuBar, int menuItem, void *callbackData,
		int panel)
{
SetMenuBarAttribute (menuHandle, MENU_UPDATEPERIOD_SETGD5, ATTR_CHECKED, 0);
	SetMenuBarAttribute (menuHandle, MENU_UPDATEPERIOD_SETGD10, ATTR_CHECKED, 0);
	SetMenuBarAttribute (menuHandle, MENU_UPDATEPERIOD_SETGD100, ATTR_CHECKED, 1);
	SetMenuBarAttribute (menuHandle, MENU_UPDATEPERIOD_SETGD1000, ATTR_CHECKED, 0);
	EventPeriod=0.100;
}

void CVICALLBACK SETGD1000_CALLBACK (int menuBar, int menuItem, void *callbackData,
		int panel)
{
	SetMenuBarAttribute (menuHandle, MENU_UPDATEPERIOD_SETGD5, ATTR_CHECKED, 0);
	SetMenuBarAttribute (menuHandle, MENU_UPDATEPERIOD_SETGD10, ATTR_CHECKED, 0);
	SetMenuBarAttribute (menuHandle, MENU_UPDATEPERIOD_SETGD100, ATTR_CHECKED, 0);
	SetMenuBarAttribute (menuHandle, MENU_UPDATEPERIOD_SETGD1000, ATTR_CHECKED, 1);
	EventPeriod=1.00;
}

//*********************************************************************************************************
void RunOnce (void)
//here we need to first scan through all the active pages...
//		dim out unused columns
//		Build the meta-tables. (2D instead of 3D)
//		Build the update list
// 		
{
	//could/should change these following defs and use malloc instead.
	double MetaTimeArray[500];
	int MetaDigitalArray[17][500];
//	struct AnVals{
//		int		fcn;		//fcn is an integer refering to a function to use.
//						// 0-step, 1-linear, 2- exp, 3- 'S' curve
//		double 	fval;		//the final value
//		double	tscale;		//the timescale to approach final value
//		} MetaAnalogArray[16][500];
	struct AnVals MetaAnalogArray[17][500];
	int i,j,k,mindex,nozerofound,tsize;
	
	isdimmed=1;
	
			//Lets build the times list first...so we know how long it will be.
			//check each page...find used columns and dim out unused....(with 0 or negative values)
			SetCtrlAttribute(panelHandle,PANEL_ANALOGTABLE,ATTR_TABLE_MODE,VAL_COLUMN); 
			mindex=0;
			for(k=1;k<=7;k++)
			{
				nozerofound=1;
				if(ischecked[k]==1)
				{
					for(i=1;i<14;i++)
					{
						if((nozerofound==1)&&(TimeArray[i][k]>0))
						{
							mindex++;
							MetaTimeArray[mindex]=TimeArray[i][k];
							for(j=1;j<=16;j++)
							{
								MetaAnalogArray[j][mindex].fcn=AnalogTable[i][j][k].fcn;
								MetaAnalogArray[j][mindex].fval=AnalogTable[i][j][k].fval;
								MetaAnalogArray[j][mindex].tscale=AnalogTable[i][j][k].tscale;
								MetaDigitalArray[j][mindex]=DigTableValues[i][j][k];
							}
						}
						else if (TimeArray[i][k]==0) 
						{
							nozerofound=0;
						}
					}
				}
			}
			isdimmed=1;
			DrawNewTable(isdimmed);
			tsize=mindex;
			BuildUpdateList(MetaTimeArray,MetaAnalogArray,MetaDigitalArray,tsize);
			
			//BuildUpdateList(MetaTimeArray,MetaDigitalArray);

			//Send to ADwin
}
//*********************************************************************************************************

int CVICALLBACK CMDSTOP_CALLBACK (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
		{
		case EVENT_COMMIT:
			SetCtrlAttribute (panelHandle, PANEL_TIMER, ATTR_ENABLED, 0);
			SetCtrlVal(panelHandle,PANEL_TOGGLEREPEAT,0);
			
			break;
		}
	return 0;
}

int CVICALLBACK TIMER_CALLBACK (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
		{
		case EVENT_TIMER_TICK:
			SetCtrlAttribute (panelHandle, PANEL_TIMER, ATTR_ENABLED, 0);
			//disable timer and re-enable in the runonce (or update list) loop, if the repeat butn is pressed.
			//reset the timer too and set a timer time of 50ms?
			RunOnce();
		 
			break;
		}
	return 0;
}

void CVICALLBACK BOOTADWIN_CALLBACK (int menuBar, int menuItem, void *callbackData,
		int panel)
{

	Boot("C:\\ADWIN\\ADWIN10.BTL",0);
	processnum=Load_Process("TransferData.TA1");
}

void CVICALLBACK BOOTLOAD_CALLBACK (int menuBar, int menuItem, void *callbackData,
		int panel)
{
	  int ischecked=0;
	  GetMenuBarAttribute (menuHandle, MENU_FILE_BOOTLOAD, ATTR_CHECKED,&ischecked);
	  SetMenuBarAttribute (menuHandle, MENU_FILE_BOOTLOAD, ATTR_CHECKED,abs(ischecked-1));

}

//**********************************************************************************************
void CVICALLBACK CLEARPANEL_CALLBACK (int menuBar, int menuItem, void *callbackData,
		int panel)
{
	// add code to clear all the analog and digital information.....
	int i=0,j=0,k=0,status=0;
	
	status=ConfirmPopup("Clear Panel","Do you really want to clear the panel?") ;
	if(status==1)
	{
		ChangedVals=1;
		for(i=1;i<=14;i++)
		{
			for(j=1;j<=16;j++)
			{
				for(k=1;k<=7;k++)
				{
					AnalogTable[i][j][k].fcn=0;		
					AnalogTable[i][j][k].fval=0;
					AnalogTable[i][j][k].tscale=0;
					DigTableValues[i][j][k]=0;
					TimeArray[i][k]=0;
				}
			}
		}
		DrawNewTable(0);
	}
}
//**********************************************************************************************

void CVICALLBACK INSERTCOLUMN_CALLBACK (int menuBar, int menuItem, void *callbackData,
		int panel)
{
	char buff[20]="",buff2[100]="";
	int page,status,i=0,j=0;
	Point cpoint={0,0};
	page=GetPage();
	GetActiveTableCell (panelHandle, PANEL_TIMETABLE, &cpoint);
	sprintf(buff,"%d",cpoint.x);
	strcat(buff2,"Really insert column at ");
	strcat(buff2,buff);
	status=ConfirmPopup("insert Array",buff2);
	if(status==1)
	{
		ChangedVals=1;
//		InsertTableColumns (panelHandle, PANEL_TIMETABLE, cpoint.x, 1,0);
//		InsertTableColumns (panelHandle, PANEL_ANALOGTABLE, cpoint.x,1, 2);
//		InsertTableColumns (panelHandle, PANEL_DIGTABLE, cpoint.x, 1,2);
		ShiftColumn(cpoint.x,1);
		DrawNewTable(0);
		// add code to insert vals from tables too....
	}
}


//**********************************************************************************************

void CVICALLBACK DELETECOLUMN_CALLBACK (int menuBar, int menuItem, void *callbackData,
	int panel)
{
	char buff[20]="",buff2[100]="";
	int page,status,i=0,j=0;
	Point cpoint={0,0};
	page=GetPage();
	//PromptPopup ("Array Manipulation:Delete", "Delete which column?", buff, 3);
	GetActiveTableCell (panelHandle, PANEL_TIMETABLE, &cpoint);
	sprintf(buff,"%d",cpoint.x);

	strcat(buff2,"Really delete column ");
	strcat(buff2,buff);
	status=ConfirmPopup("Delete Array",buff2);
	if(status==1)
	{
		ChangedVals=1;
//		DeleteTableColumns (panelHandle, PANEL_TIMETABLE, cpoint.x, 1);
//		DeleteTableColumns (panelHandle, PANEL_ANALOGTABLE, cpoint.x, 1);
//		DeleteTableColumns (panelHandle, PANEL_DIGTABLE, cpoint.x, 1);
		// add code to delete vals from tables too....
		ShiftColumn(cpoint.x,-1);
		DrawNewTable(0);
	}
}
//**********************************************************************************************
void ShiftColumn(int col, int dir)
{
	int i,j,page,xstart,status;
	page=GetPage();
	
	if(dir==1){ xstart=15;}			// set direction of looping
	else {xstart=col;}
	
	//shift columns left or right depending on dir
	for(i=0;i<15-col;i++)
	{
		TimeArray[xstart-dir*i][page]=TimeArray[xstart-dir*i-dir][page];		
		for(j=1;j<=16;j++)
		{
			DigTableValues[xstart-dir*i][j][page]=DigTableValues[xstart-dir*i-dir][j][page];
			AnalogTable[xstart-dir*i][j][page].fcn=AnalogTable[xstart-dir*i-dir][j][page].fcn  ;
			AnalogTable[xstart-dir*i][j][page].fval=AnalogTable[xstart-dir*i-dir][j][page].fval ;
			AnalogTable[xstart-dir*i][j][page].tscale=AnalogTable[xstart-dir*i-dir][j][page].tscale;
		}
	}
	
	//if we inserted a column, then set all values to zero 
	// prompt and ask if we want to duplicate the selected column
	if(dir==1) 
	{
		TimeArray[col][page]=0;
		for(j=1;j<=16;j++)
		{
			DigTableValues[col][j][page]=0;
			AnalogTable[col][j][page].fcn=1;
			AnalogTable[col][j][page].fval=0;
			AnalogTable[col][j][page].tscale=1;
		}
		status=ConfirmPopup("Duplicate","Do you want to duplicate the selected column?");
		if (status==1)
		{
			for(j=1;j<=16;j++)
			{
				TimeArray[col][page]=TimeArray[col+1][page];
				DigTableValues[col][j][page]=DigTableValues[col+1][j][page];
				AnalogTable[col][j][page].fcn=AnalogTable[col+1][j][page].fcn;
				AnalogTable[col][j][page].fval=AnalogTable[col+1][j][page].fval;
				AnalogTable[col][j][page].tscale=AnalogTable[col+1][j][page].tscale;
			}
		}
	}   

}
//***************************************************************************************************************

void CVICALLBACK COPYCOLUMN_CALLBACK (int menuBar, int menuItem, void *callbackData,
		int panel)
{
	char buff[20]="",buff2[100]="";
	int page,status,i=0,j=0;
	Point cpoint={0,0};
	page=GetPage();
	
	//PromptPopup ("Array Manipulation:Delete", "Delete which column?", buff, 3);
	GetActiveTableCell (panelHandle, PANEL_TIMETABLE, &cpoint);
	sprintf(buff,"%d",cpoint.x);
	TimeClip=TimeArray[cpoint.x][page];
	for(j=1;j<=16;j++)
	{
		AnalogClip[j].fcn=AnalogTable[cpoint.x][j][page].fcn;	
		AnalogClip[j].fval=AnalogTable[cpoint.x][j][page].fval;
		AnalogClip[j].tscale=AnalogTable[cpoint.x][j][page].tscale;
		DigClip[j]=DigTableValues[cpoint.x][j][page];
	}
	DrawNewTable(0);

}

void CVICALLBACK PASTECOLUMN_CALLBACK (int menuBar, int menuItem, void *callbackData,
		int panel)
{
	char buff[20]="",buff2[100]="";
	int page,status,i=0,j=0;
	Point cpoint={0,0};
	
	ChangedVals=1;
	page=GetPage();
	GetActiveTableCell (panelHandle, PANEL_TIMETABLE, &cpoint);
	sprintf(buff,"%d",cpoint.x);
	TimeArray[cpoint.x][page]=TimeClip;
	for(j=1;j<=16;j++)
	{
		AnalogTable[cpoint.x][j][page].fcn=AnalogClip[j].fcn;	
		AnalogTable[cpoint.x][j][page].fval=AnalogClip[j].fval;
		AnalogTable[cpoint.x][j][page].tscale=AnalogClip[j].tscale;
		DigTableValues[cpoint.x][j][page]=DigClip[j];
	}
	DrawNewTable(0);



}
