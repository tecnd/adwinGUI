#include <userint.h>
#include "GUIDesign.h"
#include "GUIDesign2.h"


//Clipboard for copy/paste functions
double TimeClip;
struct AnalogTableClip{
	int		fcn;		//fcn is an integer refering to a function to use.
						// 0-step, 1-linear, 2- exp, 3- 'S' curve
	double 	fval;		//the final value
	double	tscale;		//the timescale to approach final value
	} AnalogClip[NUMBERANALOGCHANNELS+1];
int DigClip[NUMBERDIGITALCHANNELS+1];
ddsoptions_struct ddsclip;


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
	ChangedVals=TRUE; 
	DisplayPanel(panelHandle2);
}
//******************************************************************

void CVICALLBACK DIGITALSET_CALLBACK (int menuBar, int menuItem, void *callbackData,
		int panel)
{
	ChangedVals=TRUE; 
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
	static char defaultdir[200]="C:\\UserDate\\Data"; 
	int i=0,j=0,k=0,inisize=0;
	//Check if .ini file exists.  Load it if it does.
	if(!(fpini=fopen("gui.ini","r"))==NULL)		
	{											
		while (fscanf(fpini,"%c",&c) != -1) fname[inisize++] = c;  //Read the contained name
	}
	fclose(fpini);
	
	status=FileSelectPopup (defaultdir, "*.pan", "", "Load Settings", VAL_LOAD_BUTTON, 0, 0, 1, 1,fsavename );
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
	strcpy(defaultdir,"");

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
	static char defaultdir[200]="C:\\UserDate\\Data";
	int i=0,j=0,k=0,inisize=0,status,loadonboot=0;
	//Check if .ini file exists.  Load it if it does.
	if(!(fpini=fopen("gui.ini","r"))==NULL)		// if "proto.ini" exists, then read it  Just contains a filename.  
	{												//If not, do nothing
		while (fscanf(fpini,"%c",&c) != -1) fname[inisize++] = c;  //Read the contained name
	}
 	fclose(fpini);
 	
	status=FileSelectPopup (defaultdir, "*.pan", "", "Save Settings", VAL_SAVE_BUTTON, 0, 0, 1, 1,fsavename);
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
	strcpy(defaultdir,"");
}
//*********************************************************************
int CVICALLBACK ANALOGTABLE_CALLBACK (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	int cx=0,cy=0,pixleft=0,pixtop=0;
	Point cpoint={0,0};
	char buff[80]="";
	int leftbuttondown,rightbuttondown,keymod;
	int panel_to_display;
	
	ChangedVals=TRUE;
	switch (event)
		{
		case EVENT_LEFT_DOUBLE_CLICK:
			cpoint.x=0;cpoint.y=0;
			GetActiveTableCell(panelHandle, PANEL_ANALOGTABLE, &cpoint);
			//now set the indicators in the control panel title..ie units
			currentx=cpoint.x;
			currenty=cpoint.y;
			currentpage=GetPage();
			if(currenty==NUMBERANALOGCHANNELS+1) {
				SetDDSControlPanel();
				panel_to_display = panelHandle6;
			}
			else {
				
				SetControlPanel();      
				panel_to_display = panelHandle4;
				
			}
			
			sprintf(buff,"x:%d   y:%d    z:%d",currentx,currenty,currentpage);
			SetPanelAttribute (panel_to_display, ATTR_TITLE, buff);
			//Read the mouse position and open window there.
			GetGlobalMouseState (&panelHandle, &pixleft, &pixtop, &leftbuttondown,
								 &rightbuttondown, &keymod);
			SetPanelAttribute (panel_to_display, ATTR_LEFT, pixleft);
			SetPanelAttribute (panel_to_display, ATTR_TOP, pixtop);
			
			DisplayPanel(panel_to_display);  
			
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
	int analogtable_visible = 0;
	int digtable_visible = 0;
 	double vlast=0,vnow=0;
 	int dimset=0,nozerofound,val;
 
 	int ispicture=1,celltype=0; //celtype has 3 values.  0=Numerc, 1=String, 2=Picture  
 	int ColourTable[25]={VAL_BLACK,VAL_GRAY,VAL_GRAY,VAL_GRAY,0x00B0B0B0,0x00B0B0B0,0x00B0B0B0,VAL_GRAY,VAL_GRAY,VAL_GRAY,0x00B0B0B0,0x00B0B0B0,0x00B0B0B0,
 	VAL_GRAY,VAL_GRAY,VAL_GRAY,0x00B0B0B0,0x00B0B0B0,0x00B0B0B0,VAL_GRAY,VAL_GRAY,VAL_GRAY,0x00B0B0B0,0x00B0B0B0,0x00B0B0B0};
 	
 	Point pval={0,0};
 	GetCtrlAttribute (panelHandle, PANEL_ANALOGTABLE, ATTR_VISIBLE, &analogtable_visible);
	GetCtrlAttribute (panelHandle, PANEL_DIGTABLE, ATTR_VISIBLE, &digtable_visible);
 	SetCtrlAttribute (panelHandle, PANEL_ANALOGTABLE, ATTR_VISIBLE, 0);
	SetCtrlAttribute (panelHandle, PANEL_DIGTABLE, ATTR_VISIBLE, 0);	
 	SetCtrlAttribute (panelHandle, PANEL_TIMETABLE, ATTR_VISIBLE, 0);
	page=GetPage();
	
//	GetCtrlVal(panelHandle, PANEL_TGL_NUMERICTABLE,&val);
//	if(val==0) 
//	{ 
//			SetDisplayType(VAL_CELL_NUMERIC);
//	}
//	else 
//	{
//		SetDisplayType(VAL_CELL_PICTURE);
//	}

	GetTableCellAttribute (panelHandle, PANEL_ANALOGTABLE, MakePoint(1,1),
						   ATTR_CELL_TYPE, &celltype);
	
	
	if(celltype==2) {ispicture=1;}
	else { ispicture=0;}
	
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
		for(j=1;j<=NUMBERANALOGCHANNELS;j++)
		{
			SetTableCellVal (panelHandle, PANEL_ANALOGTABLE, MakePoint(i,j), 0);
			if(ispicture==0)
			{
				SetTableCellVal(panelHandle,PANEL_ANALOGTABLE,MakePoint(i,j),AnalogTable[i][j][page].fval);
			}
			
			
			SetTableCellAttribute (panelHandle, PANEL_ANALOGTABLE, MakePoint(i,j),ATTR_CELL_DIMMED, 0);
			SetTableCellAttribute (panelHandle, PANEL_DIGTABLE, MakePoint(i,j),ATTR_CELL_DIMMED, 0);
			//check what's happening...care about off, static, changing
			// step.  Vfinal=0 and last Vfinal=0  -> off
			// step	  Vfinal=Vconst and last Vconst=Vconst
			// changing mode>0			
			cmode=AnalogTable[i][j][page].fcn;
			vlast=AnalogTable[i-1][j][page].fval;
			vnow=AnalogTable[i][j][page].fval;			

			SetTableCellAttribute (panelHandle, PANEL_ANALOGTABLE,MakePoint(i,j), ATTR_TEXT_BGCOLOR,ColourTable[j]);
			if(cmode==1)
			{
				if(vnow==0)
				{
					SetTableCellAttribute (panelHandle, PANEL_ANALOGTABLE,MakePoint(i,j), ATTR_TEXT_BGCOLOR,ColourTable[j]);
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
				SetTableCellAttribute (panelHandle, PANEL_ANALOGTABLE,MakePoint(i,j), ATTR_TEXT_BGCOLOR,ColourTable[j]);
			}
			if(vnow==0&&cmode==1)
			{
				SetTableCellAttribute (panelHandle, PANEL_ANALOGTABLE,MakePoint(i,j), ATTR_TEXT_BGCOLOR,ColourTable[j]);
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
				SetTableCellAttribute (panelHandle, PANEL_DIGTABLE,MakePoint(i,j), ATTR_TEXT_BGCOLOR,ColourTable[j]);
			}
			//Done digital drawing.
		}
		
		//update DDS row
		
		SetTableCellVal (panelHandle, PANEL_ANALOGTABLE, MakePoint(i,25), 0);
		if(ispicture==0)
		{
			SetTableCellVal(panelHandle,PANEL_ANALOGTABLE,MakePoint(i,25),ddstable[i][page].start_frequency);
		}
		
		
		SetTableCellAttribute (panelHandle, PANEL_ANALOGTABLE, MakePoint(i,25),ATTR_CELL_DIMMED, 0);
		SetTableCellAttribute (panelHandle, PANEL_DIGTABLE, MakePoint(i,25),ATTR_CELL_DIMMED, 0);
		
		if (ddstable[i][page].amplitude == 0.0 || ddstable[i][page].is_stop)
		{
			//make grey
			SetTableCellAttribute (panelHandle, PANEL_ANALOGTABLE,MakePoint(i,25), ATTR_TEXT_BGCOLOR,VAL_WHITE);
		}
		else
		{
			if (ddstable[i][page].start_frequency>ddstable[i][page].end_frequency)
			{
				//ramping down
				SetTableCellAttribute (panelHandle, PANEL_ANALOGTABLE,MakePoint(i,25), ATTR_TEXT_BGCOLOR,VAL_BLUE);          
			}
			else
			{
				//ramping up
				SetTableCellAttribute (panelHandle, PANEL_ANALOGTABLE,MakePoint(i,25), ATTR_TEXT_BGCOLOR,VAL_GREEN);      
			}
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
			if(ispicture==0) {picmode=0;}
			if(ispicture==1) {picmode=2;}
			if(dimset==1) {picmode=2;}

			for(j=1;j<=NUMBERANALOGCHANNELS;j++)
			{
				SetTableCellAttribute (panelHandle, PANEL_ANALOGTABLE, MakePoint(i,j),ATTR_CELL_DIMMED, dimset);
				SetTableCellAttribute (panelHandle, PANEL_DIGTABLE, MakePoint(i,j),ATTR_CELL_DIMMED, dimset);
				SetTableCellAttribute (panelHandle, PANEL_ANALOGTABLE, MakePoint(i,j),ATTR_CELL_TYPE,picmode);
			}
		}
	}
	SetCtrlAttribute (panelHandle, PANEL_ANALOGTABLE, ATTR_VISIBLE, analogtable_visible);
	SetCtrlAttribute (panelHandle, PANEL_DIGTABLE, ATTR_VISIBLE, digtable_visible);
	SetCtrlAttribute (panelHandle, PANEL_TIMETABLE, ATTR_VISIBLE, 1);
	
	if(currentpage==10)
	{
		SetCtrlAttribute (panelHandle, PANEL_NUM_INSERTIONPAGE, ATTR_DIMMED, 0);
		SetCtrlAttribute (panelHandle, PANEL_NUM_INSERTIONCOL, ATTR_DIMMED, 0);
	}
	else
	{
		SetCtrlAttribute (panelHandle, PANEL_NUM_INSERTIONPAGE, ATTR_DIMMED, 1);
		SetCtrlAttribute (panelHandle, PANEL_NUM_INSERTIONCOL, ATTR_DIMMED, 1);
	}
	
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
			SetCtrlVal (panelHandle, PANEL_TB_SHOWPHASE8, 0);
			SetCtrlVal (panelHandle, PANEL_TB_SHOWPHASE9, 0);
			SetCtrlVal (panelHandle, PANEL_TB_SHOWPHASE10, 0);			
			SetCtrlAttribute (panelHandle, PANEL_LABEL_1, ATTR_VISIBLE, 1);
			SetCtrlAttribute (panelHandle, PANEL_LABEL_2, ATTR_VISIBLE, 0);
			SetCtrlAttribute (panelHandle, PANEL_LABEL_3, ATTR_VISIBLE, 0);
			SetCtrlAttribute (panelHandle, PANEL_LABEL_4, ATTR_VISIBLE, 0);
			SetCtrlAttribute (panelHandle, PANEL_LABEL_5, ATTR_VISIBLE, 0);
			SetCtrlAttribute (panelHandle, PANEL_LABEL_6, ATTR_VISIBLE, 0);
			SetCtrlAttribute (panelHandle, PANEL_LABEL_7, ATTR_VISIBLE, 0);
			SetCtrlAttribute (panelHandle, PANEL_LABEL_8, ATTR_VISIBLE, 0); 
			SetCtrlAttribute (panelHandle, PANEL_LABEL_9, ATTR_VISIBLE, 0); 
			SetCtrlAttribute (panelHandle, PANEL_LABEL_10, ATTR_VISIBLE, 0); 
			currentpage=1;
			ChangedVals=TRUE;
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
			SetCtrlVal (panelHandle, PANEL_TB_SHOWPHASE8, 0);
			SetCtrlVal (panelHandle, PANEL_TB_SHOWPHASE9, 0);
			SetCtrlVal (panelHandle, PANEL_TB_SHOWPHASE10, 0);			
			SetCtrlAttribute (panelHandle, PANEL_LABEL_1, ATTR_VISIBLE, 0);
			SetCtrlAttribute (panelHandle, PANEL_LABEL_2, ATTR_VISIBLE, 1);
			SetCtrlAttribute (panelHandle, PANEL_LABEL_3, ATTR_VISIBLE, 0);
			SetCtrlAttribute (panelHandle, PANEL_LABEL_4, ATTR_VISIBLE, 0);
			SetCtrlAttribute (panelHandle, PANEL_LABEL_5, ATTR_VISIBLE, 0);
			SetCtrlAttribute (panelHandle, PANEL_LABEL_6, ATTR_VISIBLE, 0);
			SetCtrlAttribute (panelHandle, PANEL_LABEL_7, ATTR_VISIBLE, 0);
			SetCtrlAttribute (panelHandle, PANEL_LABEL_8, ATTR_VISIBLE, 0); 
			SetCtrlAttribute (panelHandle, PANEL_LABEL_9, ATTR_VISIBLE, 0); 
			SetCtrlAttribute (panelHandle, PANEL_LABEL_10, ATTR_VISIBLE, 0); 
			ChangedVals=TRUE; 
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
			SetCtrlVal (panelHandle, PANEL_TB_SHOWPHASE8, 0);
			SetCtrlVal (panelHandle, PANEL_TB_SHOWPHASE9, 0);
			SetCtrlVal (panelHandle, PANEL_TB_SHOWPHASE10, 0);			
			SetCtrlAttribute (panelHandle, PANEL_LABEL_1, ATTR_VISIBLE, 0);
			SetCtrlAttribute (panelHandle, PANEL_LABEL_2, ATTR_VISIBLE, 0);
			SetCtrlAttribute (panelHandle, PANEL_LABEL_3, ATTR_VISIBLE, 1);
			SetCtrlAttribute (panelHandle, PANEL_LABEL_4, ATTR_VISIBLE, 0);
			SetCtrlAttribute (panelHandle, PANEL_LABEL_5, ATTR_VISIBLE, 0);
			SetCtrlAttribute (panelHandle, PANEL_LABEL_6, ATTR_VISIBLE, 0);
			SetCtrlAttribute (panelHandle, PANEL_LABEL_7, ATTR_VISIBLE, 0);
			SetCtrlAttribute (panelHandle, PANEL_LABEL_8, ATTR_VISIBLE, 0); 
			SetCtrlAttribute (panelHandle, PANEL_LABEL_9, ATTR_VISIBLE, 0); 
			SetCtrlAttribute (panelHandle, PANEL_LABEL_10, ATTR_VISIBLE, 0); 
			ChangedVals=TRUE; 
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
			SetCtrlVal (panelHandle, PANEL_TB_SHOWPHASE8, 0);
			SetCtrlVal (panelHandle, PANEL_TB_SHOWPHASE9, 0);
			SetCtrlVal (panelHandle, PANEL_TB_SHOWPHASE10, 0);			
			SetCtrlAttribute (panelHandle, PANEL_LABEL_1, ATTR_VISIBLE, 0);
			SetCtrlAttribute (panelHandle, PANEL_LABEL_2, ATTR_VISIBLE, 0);
			SetCtrlAttribute (panelHandle, PANEL_LABEL_3, ATTR_VISIBLE, 0);
			SetCtrlAttribute (panelHandle, PANEL_LABEL_4, ATTR_VISIBLE, 1);
			SetCtrlAttribute (panelHandle, PANEL_LABEL_5, ATTR_VISIBLE, 0);
			SetCtrlAttribute (panelHandle, PANEL_LABEL_6, ATTR_VISIBLE, 0);
			SetCtrlAttribute (panelHandle, PANEL_LABEL_7, ATTR_VISIBLE, 0);
			SetCtrlAttribute (panelHandle, PANEL_LABEL_8, ATTR_VISIBLE, 0); 
			SetCtrlAttribute (panelHandle, PANEL_LABEL_9, ATTR_VISIBLE, 0); 
			SetCtrlAttribute (panelHandle, PANEL_LABEL_10, ATTR_VISIBLE, 0); 
			ChangedVals=TRUE; 
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
			SetCtrlVal (panelHandle, PANEL_TB_SHOWPHASE8, 0);
			SetCtrlVal (panelHandle, PANEL_TB_SHOWPHASE9, 0);
			SetCtrlVal (panelHandle, PANEL_TB_SHOWPHASE10, 0);			
			SetCtrlAttribute (panelHandle, PANEL_LABEL_1, ATTR_VISIBLE, 0);
			SetCtrlAttribute (panelHandle, PANEL_LABEL_2, ATTR_VISIBLE, 0);
			SetCtrlAttribute (panelHandle, PANEL_LABEL_3, ATTR_VISIBLE, 0);
			SetCtrlAttribute (panelHandle, PANEL_LABEL_4, ATTR_VISIBLE, 0);
			SetCtrlAttribute (panelHandle, PANEL_LABEL_5, ATTR_VISIBLE, 1);
			SetCtrlAttribute (panelHandle, PANEL_LABEL_6, ATTR_VISIBLE, 0);
			SetCtrlAttribute (panelHandle, PANEL_LABEL_7, ATTR_VISIBLE, 0);
			SetCtrlAttribute (panelHandle, PANEL_LABEL_8, ATTR_VISIBLE, 0); 
			SetCtrlAttribute (panelHandle, PANEL_LABEL_9, ATTR_VISIBLE, 0); 
			SetCtrlAttribute (panelHandle, PANEL_LABEL_10, ATTR_VISIBLE, 0); 
			ChangedVals=TRUE; 
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
			SetCtrlVal (panelHandle, PANEL_TB_SHOWPHASE8, 0);
			SetCtrlVal (panelHandle, PANEL_TB_SHOWPHASE9, 0);
			SetCtrlVal (panelHandle, PANEL_TB_SHOWPHASE10, 0);			
			SetCtrlAttribute (panelHandle, PANEL_LABEL_1, ATTR_VISIBLE, 0);
			SetCtrlAttribute (panelHandle, PANEL_LABEL_2, ATTR_VISIBLE, 0);
			SetCtrlAttribute (panelHandle, PANEL_LABEL_3, ATTR_VISIBLE, 0);
			SetCtrlAttribute (panelHandle, PANEL_LABEL_4, ATTR_VISIBLE, 0);
			SetCtrlAttribute (panelHandle, PANEL_LABEL_5, ATTR_VISIBLE, 0);
			SetCtrlAttribute (panelHandle, PANEL_LABEL_6, ATTR_VISIBLE, 1);
			SetCtrlAttribute (panelHandle, PANEL_LABEL_7, ATTR_VISIBLE, 0);
		 	SetCtrlAttribute (panelHandle, PANEL_LABEL_8, ATTR_VISIBLE, 0); 
			SetCtrlAttribute (panelHandle, PANEL_LABEL_9, ATTR_VISIBLE, 0); 
			SetCtrlAttribute (panelHandle, PANEL_LABEL_10, ATTR_VISIBLE, 0); 
			ChangedVals=TRUE; 
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
			SetCtrlVal (panelHandle, PANEL_TB_SHOWPHASE8, 0);
			SetCtrlVal (panelHandle, PANEL_TB_SHOWPHASE9, 0);
			SetCtrlVal (panelHandle, PANEL_TB_SHOWPHASE10, 0);			
			SetCtrlAttribute (panelHandle, PANEL_LABEL_1, ATTR_VISIBLE, 0);
			SetCtrlAttribute (panelHandle, PANEL_LABEL_2, ATTR_VISIBLE, 0);
			SetCtrlAttribute (panelHandle, PANEL_LABEL_3, ATTR_VISIBLE, 0);
			SetCtrlAttribute (panelHandle, PANEL_LABEL_4, ATTR_VISIBLE, 0);
			SetCtrlAttribute (panelHandle, PANEL_LABEL_5, ATTR_VISIBLE, 0);
			SetCtrlAttribute (panelHandle, PANEL_LABEL_6, ATTR_VISIBLE, 0);
			SetCtrlAttribute (panelHandle, PANEL_LABEL_7, ATTR_VISIBLE, 1);
			SetCtrlAttribute (panelHandle, PANEL_LABEL_8, ATTR_VISIBLE, 0); 
			SetCtrlAttribute (panelHandle, PANEL_LABEL_9, ATTR_VISIBLE, 0); 
			SetCtrlAttribute (panelHandle, PANEL_LABEL_10, ATTR_VISIBLE, 0); 
			ChangedVals=TRUE; 
			currentpage=7;
			DrawNewTable(isdimmed);
			break;
		}
	return 0;
}

int CVICALLBACK TOGGLE8_CALLBACK (int panel, int control, int event,
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
			SetCtrlVal (panelHandle, PANEL_TB_SHOWPHASE7, 0);
			SetCtrlVal (panelHandle, PANEL_TB_SHOWPHASE8, 1);
			SetCtrlVal (panelHandle, PANEL_TB_SHOWPHASE9, 0);
			SetCtrlVal (panelHandle, PANEL_TB_SHOWPHASE10, 0);			
			SetCtrlAttribute (panelHandle, PANEL_LABEL_1, ATTR_VISIBLE, 0);
			SetCtrlAttribute (panelHandle, PANEL_LABEL_2, ATTR_VISIBLE, 0);
			SetCtrlAttribute (panelHandle, PANEL_LABEL_3, ATTR_VISIBLE, 0);
			SetCtrlAttribute (panelHandle, PANEL_LABEL_4, ATTR_VISIBLE, 0);
			SetCtrlAttribute (panelHandle, PANEL_LABEL_5, ATTR_VISIBLE, 0);
			SetCtrlAttribute (panelHandle, PANEL_LABEL_6, ATTR_VISIBLE, 0);
			SetCtrlAttribute (panelHandle, PANEL_LABEL_7, ATTR_VISIBLE, 0);
			SetCtrlAttribute (panelHandle, PANEL_LABEL_8, ATTR_VISIBLE, 1); 
			SetCtrlAttribute (panelHandle, PANEL_LABEL_9, ATTR_VISIBLE, 0); 
			SetCtrlAttribute (panelHandle, PANEL_LABEL_10, ATTR_VISIBLE, 0); 
			ChangedVals=TRUE; 
			currentpage=8;
			DrawNewTable(isdimmed);
			break;
		}
	return 0;
}


int CVICALLBACK TOGGLE9_CALLBACK (int panel, int control, int event,
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
			SetCtrlVal (panelHandle, PANEL_TB_SHOWPHASE7, 0);
			SetCtrlVal (panelHandle, PANEL_TB_SHOWPHASE8, 0);
			SetCtrlVal (panelHandle, PANEL_TB_SHOWPHASE9, 1);
			SetCtrlVal (panelHandle, PANEL_TB_SHOWPHASE10, 0);			
			SetCtrlAttribute (panelHandle, PANEL_LABEL_1, ATTR_VISIBLE, 0);
			SetCtrlAttribute (panelHandle, PANEL_LABEL_2, ATTR_VISIBLE, 0);
			SetCtrlAttribute (panelHandle, PANEL_LABEL_3, ATTR_VISIBLE, 0);
			SetCtrlAttribute (panelHandle, PANEL_LABEL_4, ATTR_VISIBLE, 0);
			SetCtrlAttribute (panelHandle, PANEL_LABEL_5, ATTR_VISIBLE, 0);
			SetCtrlAttribute (panelHandle, PANEL_LABEL_6, ATTR_VISIBLE, 0);
			SetCtrlAttribute (panelHandle, PANEL_LABEL_7, ATTR_VISIBLE, 0);
			SetCtrlAttribute (panelHandle, PANEL_LABEL_8, ATTR_VISIBLE, 0); 
			SetCtrlAttribute (panelHandle, PANEL_LABEL_9, ATTR_VISIBLE, 1); 
			SetCtrlAttribute (panelHandle, PANEL_LABEL_10, ATTR_VISIBLE, 0); 
			ChangedVals=TRUE; 
			currentpage=9;
			DrawNewTable(isdimmed);
			break;
		}
	return 0;
}


int CVICALLBACK TOGGLE10_CALLBACK (int panel, int control, int event,
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
			SetCtrlVal (panelHandle, PANEL_TB_SHOWPHASE7, 0);
			SetCtrlVal (panelHandle, PANEL_TB_SHOWPHASE8, 0);
			SetCtrlVal (panelHandle, PANEL_TB_SHOWPHASE9, 0);
			SetCtrlVal (panelHandle, PANEL_TB_SHOWPHASE10, 1);
			
			SetCtrlAttribute (panelHandle, PANEL_LABEL_1, ATTR_VISIBLE, 0);
			SetCtrlAttribute (panelHandle, PANEL_LABEL_2, ATTR_VISIBLE, 0);
			SetCtrlAttribute (panelHandle, PANEL_LABEL_3, ATTR_VISIBLE, 0);
			SetCtrlAttribute (panelHandle, PANEL_LABEL_4, ATTR_VISIBLE, 0);
			SetCtrlAttribute (panelHandle, PANEL_LABEL_5, ATTR_VISIBLE, 0);
			SetCtrlAttribute (panelHandle, PANEL_LABEL_6, ATTR_VISIBLE, 0);
			SetCtrlAttribute (panelHandle, PANEL_LABEL_7, ATTR_VISIBLE, 0);
			SetCtrlAttribute (panelHandle, PANEL_LABEL_8, ATTR_VISIBLE, 0); 
			SetCtrlAttribute (panelHandle, PANEL_LABEL_9, ATTR_VISIBLE, 0); 
			SetCtrlAttribute (panelHandle, PANEL_LABEL_10, ATTR_VISIBLE, 1); 
	
			ChangedVals=TRUE; 
			currentpage=10;
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
  	GetCtrlVal (panelHandle, PANEL_CHECKBOX_8, &bool);
	ischecked[8]=bool;
  	GetCtrlVal (panelHandle, PANEL_CHECKBOX_9, &bool);
	ischecked[9]=bool;
  	GetCtrlVal (panelHandle, PANEL_CHECKBOX_10, &bool);
	ischecked[10]=bool;
	
	
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
	int ctrlmode;
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
				
				for(j=1;j<=NUMBERANALOGCHANNELS;j++)
				{
					tscaleold=AnalogTable[i][j][page].tscale;	// use this  and next line to auto-scale the time
					AnalogTable[i][j][page].tscale=tscaleold*ratio;
			//		AnalogTable[i][j][page].tscale=dval;	  //use this line to force timescale to period
				}
				
				//set the delta time value for the dds array
				ddstable[i][page].delta_time = dval/1000;
				
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
			SaveLastGuiSettings();
			ChangedVals=TRUE;
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
//*********************************************************************************

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
	int MetaDigitalArray[NUMBERDIGITALCHANNELS+1][500];
//	struct AnVals{
//		int		fcn;		//fcn is an integer refering to a function to use.
//						// 0-step, 1-linear, 2- exp, 3- 'S' curve
//		double 	fval;		//the final value
//		double	tscale;		//the timescale to approach final value
//		} MetaAnalogArray[16][500];
	struct AnVals MetaAnalogArray[NUMBERANALOGCHANNELS+1][500];
	ddsoptions_struct MetaDDSArray[500];
	int i,j,k,mindex,tsize;
	int insertpage,insertcolumn,x,y,lastpagenum,FinalPageToUse;
	BOOL nozerofound,nozerofound_2;
	//*********
	int ResetToZeroAtEnd[30];
	
	//****************
	isdimmed=TRUE;
	lastpagenum=10;
	GetCtrlVal (panelHandle, PANEL_NUM_INSERTIONPAGE, &insertpage);
	GetCtrlVal (panelHandle, PANEL_NUM_INSERTIONCOL, &insertcolumn);
	//Lets build the times list first...so we know how long it will be.
	//check each page...find used columns and dim out unused....(with 0 or negative values)
	SetCtrlAttribute(panelHandle,PANEL_ANALOGTABLE,ATTR_TABLE_MODE,VAL_COLUMN); 
	mindex=0;
	FinalPageToUse=NUMBEROFPAGES-2;//issues with '0 or 1 indexing'
	if(insertpage==NUMBEROFPAGES-1)
	{
		FinalPageToUse++;
	}
	//go through for each page
	for(k=1;k<=FinalPageToUse;k++)// numberofpages-1 because last page is 'mobile'
	{
		nozerofound=1;
		if(ischecked[k]==1) //if the page is selected
		{
			//go through for each time column
			for(i=1;i<14;i++)
			{
	//			printf("%d\t%d\n",k,i);
				if((i==insertcolumn)&&(k==insertpage)&&(k!=NUMBEROFPAGES-1))
				{
					nozerofound_2=TRUE;
					if(ischecked[lastpagenum]==1)
					{
						for(x=1;x<=14;x++)
						{
							if((nozerofound==TRUE)&&(nozerofound_2==TRUE)&&(TimeArray[x][lastpagenum]>0))
							{
								mindex++;
								MetaTimeArray[mindex]=TimeArray[x][lastpagenum];
								for(y=1;y<=NUMBERANALOGCHANNELS;y++)
								{
									MetaAnalogArray[y][mindex].fcn=AnalogTable[x][y][lastpagenum].fcn;
									MetaAnalogArray[y][mindex].fval=AnalogTable[x][y][lastpagenum].fval;
									MetaAnalogArray[y][mindex].tscale=AnalogTable[x][y][lastpagenum].tscale;
									MetaDigitalArray[y][mindex]=DigTableValues[x][y][lastpagenum];
								}
								MetaDDSArray[mindex] = ddstable[x][lastpagenum];  
							}
							else if(TimeArray[x][lastpagenum]==0)
							{
								nozerofound_2=0;
							}
						}
					}
					
/*end A */		}
				
			
				if((nozerofound==1)&&(TimeArray[i][k]>0)) 
				//ignore all columns after the first
				// time 0
				{
					mindex++; //increase the number of columns counter
					MetaTimeArray[mindex]=TimeArray[i][k];
					//go through for each analog channel
					for(j=1;j<=NUMBERANALOGCHANNELS;j++)
					{
						MetaAnalogArray[j][mindex].fcn=AnalogTable[i][j][k].fcn;
						MetaAnalogArray[j][mindex].fval=AnalogTable[i][j][k].fval;
						MetaAnalogArray[j][mindex].tscale=AnalogTable[i][j][k].tscale;
						MetaDigitalArray[j][mindex]=DigTableValues[i][j][k];
					}
					/* ddsoptions_struct contains floats and ints, so shallow copy is ok */
					MetaDDSArray[mindex] = ddstable[i][k];
				}
				else if (TimeArray[i][k]==0) 
				{
					nozerofound=0;
				}
			}
		}
	}
	isdimmed=TRUE;
	
/*  Add 1 column to end of the meta array to take care of zeroing etc....
	mindex++;

	GetMenuBarAttribute (MENU, MENU_ALLLOW, ATTR_CHECKED,&Checked_All_Low );
	if(Checked_All_Low==TRUE)
	{
		for(j=1;j<=NUMBERANALOGCHANNELS:j++)
		{
			MetaAnalogArray[j][mindex].fval=0;
			MetaAnalogArray[j][mindex].fcn=1;//set to step;
			MetaAnalogArray[j][mindex].tscale=1;
			MetaDigitalArray[j][mindex]=FALSE;
		}
	}
	
	GetMenuBarAttribute (MENU, MENU_BYCHANNEL, ATTR_CHECKED,&Checked_By_Value );
	if(Checked_By_Value==TRUE)
	{
		for(j=1;j<=NUMBERANALOGCHANNELS;j++)
		{
			if(AChName[j].resettozero==TRUE)
			{
				MetaAnalogArray[j][mindex].fval=0;
			}
			else
			{
				MetaAnalogArray[j][mindex].fval=MetaAnalogArray[j][mindex-1];	
			}
			if(DChName[j].resetlow==TRUE)
			{
				MetaDigitalArray[j][mindex]=FALSE;
			}
			else
			{
				MetaDigitalArray[j][mindex]=MetaDigitalArray[j][mindex-1];	
			}
		}
	}
	
	GetMenuBarAttribute (MENU, MENU_HOLDVALUE, ATTR_CHECKED,&Checked_Hold_Value );
	if(Checked_Hold_Value==TRUE)
	{
		for(j=1;j<=NUMBERANALOGCHANNELS;j++)
		{
				MetaAnalogArray[j][mindex].fval=MetaAnalogArray[j][mindex-1];	
				MetaDigitalArray[j][mindex]=MetaDigitalArray[j][mindex-1];
		}
	}
*/	
	DrawNewTable(isdimmed);
	tsize=mindex; //tsize is the number of columns
	BuildUpdateList(MetaTimeArray,MetaAnalogArray,MetaDigitalArray,MetaDDSArray,tsize);

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
			for(j=1;j<=NUMBERANALOGCHANNELS;j++)
			{
				for(k=0;k<NUMBEROFPAGES;k++)
				{
					AnalogTable[i][j][k].fcn=1;		
					AnalogTable[i][j][k].fval=0;
					AnalogTable[i][j][k].tscale=0;
					DigTableValues[i][j][k]=0;
					TimeArray[i][k]=0;
					ddstable[i][k].start_frequency=0;
					ddstable[i][k].end_frequency=0;
					ddstable[i][k].amplitude=0;
					ddstable[i][k].is_stop=0;
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
		ShiftColumn(cpoint.x,1);
		DrawNewTable(0);
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
		for(j=1;j<=NUMBERANALOGCHANNELS;j++)
		{
			DigTableValues[xstart-dir*i][j][page]=DigTableValues[xstart-dir*i-dir][j][page];
			AnalogTable[xstart-dir*i][j][page].fcn=AnalogTable[xstart-dir*i-dir][j][page].fcn  ;
			AnalogTable[xstart-dir*i][j][page].fval=AnalogTable[xstart-dir*i-dir][j][page].fval ;
			AnalogTable[xstart-dir*i][j][page].tscale=AnalogTable[xstart-dir*i-dir][j][page].tscale;
		}
		ddstable[xstart-dir*i][page].start_frequency=ddstable[xstart-dir*i-dir][page].start_frequency;
		ddstable[xstart-dir*i][page].end_frequency=ddstable[xstart-dir*i-dir][page].end_frequency;
		ddstable[xstart-dir*i][page].amplitude=ddstable[xstart-dir*i-dir][page].amplitude;
		ddstable[xstart-dir*i][page].is_stop=ddstable[xstart-dir*i-dir][page].is_stop;
	}
	//if we inserted a column, then set all values to zero 
	// prompt and ask if we want to duplicate the selected column
	if(dir==1) 
	{
		TimeArray[col][page]=0;
		for(j=1;j<=NUMBERANALOGCHANNELS;j++)
		{
			DigTableValues[col][j][page]=0;
			AnalogTable[col][j][page].fcn=1;
			AnalogTable[col][j][page].fval=0;
			AnalogTable[col][j][page].tscale=1;
		}
		ddstable[col][page].start_frequency=0;
		ddstable[col][page].end_frequency=0;
		ddstable[col][page].amplitude=0;
		ddstable[col][page].is_stop=FALSE;
		status=ConfirmPopup("Duplicate","Do you want to duplicate the selected column?");
		if (status==1)
		{
			for(j=1;j<=NUMBERANALOGCHANNELS;j++)
			{
				TimeArray[col][page]=TimeArray[col+1][page];
				DigTableValues[col][j][page]=DigTableValues[col+1][j][page];
				AnalogTable[col][j][page].fcn=AnalogTable[col+1][j][page].fcn;
				AnalogTable[col][j][page].fval=AnalogTable[col+1][j][page].fval;
				AnalogTable[col][j][page].tscale=AnalogTable[col+1][j][page].tscale;
			}
			ddstable[col][page].start_frequency=ddstable[col+1][page].start_frequency;
			ddstable[col][page].end_frequency=ddstable[col+1][page].end_frequency;
			ddstable[col][page].amplitude=ddstable[col+1][page].amplitude;
			ddstable[col][page].is_stop=ddstable[col+1][page].is_stop;
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
	for(j=1;j<=NUMBERANALOGCHANNELS;j++)
	{
		AnalogClip[j].fcn=AnalogTable[cpoint.x][j][page].fcn;	
		AnalogClip[j].fval=AnalogTable[cpoint.x][j][page].fval;
		AnalogClip[j].tscale=AnalogTable[cpoint.x][j][page].tscale;
		DigClip[j]=DigTableValues[cpoint.x][j][page];
	}
	ddsclip.start_frequency=ddstable[cpoint.x][page].start_frequency;
	ddsclip.end_frequency=ddstable[cpoint.x][page].end_frequency;
	ddsclip.amplitude=ddstable[cpoint.x][page].amplitude;
	ddsclip.is_stop=ddstable[cpoint.x][page].is_stop;
	DrawNewTable(0);

}
//**********************************************************************************

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
	for(j=1;j<=NUMBERANALOGCHANNELS;j++)
	{
		AnalogTable[cpoint.x][j][page].fcn=AnalogClip[j].fcn;	
		AnalogTable[cpoint.x][j][page].fval=AnalogClip[j].fval;
		AnalogTable[cpoint.x][j][page].tscale=AnalogClip[j].tscale;
		DigTableValues[cpoint.x][j][page]=DigClip[j];
	}
	ddstable[cpoint.x][page].start_frequency=ddsclip.start_frequency;
	ddstable[cpoint.x][page].end_frequency=ddsclip.end_frequency;
	ddstable[cpoint.x][page].amplitude=ddsclip.amplitude;
	ddstable[cpoint.x][page].is_stop=ddsclip.is_stop;
	DrawNewTable(0);
}
//**********************************************************************************

int CVICALLBACK TGLNUMERIC_CALLBACK (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	int val=0;
	switch (event)
		{
		case EVENT_COMMIT:
			// find out if the button is pressed or not
			// change buttons to appropriate mode and draw new table
			GetCtrlVal(panelHandle, PANEL_TGL_NUMERICTABLE,&val);
			if(val==0) 
			{ 
				SetDisplayType(VAL_CELL_NUMERIC);
			}
			else 
			{
				SetDisplayType(VAL_CELL_PICTURE);
			}
			
			
			break;
		}
	return 0;
}
//**********************************************************************************
void CVICALLBACK DDSSETUP_CALLBACK (int menuBar, int menuItem, void *callbackData,
		int panel)
{
	LoadDDSSettings();
	DisplayPanel(panelHandle5);
}

//**********************************************************************************
void LoadArrays(char savedname[500],int csize)
{
	FILE *fdata;
	int i=0,j=0,k=0;
	int xval=16,yval=16,zval=10;
	char buff[500]="",buff2[100]="";
	strncat(buff,savedname,csize-4);
	strcat(buff,".arr");
	if((fdata=fopen(buff,"r"))==NULL)
	{
		MessagePopup("Load Error","Failed to load data arrays");
	//	exit(1);
	}
	//The Plan:
	//write the dimensionality of the array....
	//szve as x,y,z...
	//scan order is Page,X,Y..
	//So, scanning by channel at each step....
	//Need time info tooo...
	// So...after dim...save appropriate time array. (page ,x)
	// save structures.
	//locked dimensions...16,16,10	
	//The dimensions
	fread(&xval,sizeof xval, 1,fdata);
	fread(&yval,sizeof yval, 1,fdata);
	fread(&zval,sizeof zval, 1,fdata);
	//now for the times.	
	fread(&TimeArray,sizeof TimeArray,1,fdata);
	//and the analog data
	fread(&AnalogTable,sizeof AnalogTable,1,fdata);
	fread(&DigTableValues,sizeof DigTableValues,1,fdata);
	fread(&AChName,sizeof AChName,1,fdata);
	fread(&DChName,sizeof DChName,1,fdata);
	fread(&ddstable,sizeof ddstable,1,fdata);
	fread(&buff2,sizeof buff2,1,fdata);
	SetCtrlAttribute (panelHandle, PANEL_TB_SHOWPHASE1,ATTR_OFF_TEXT, buff2);
	SetCtrlAttribute (panelHandle, PANEL_TB_SHOWPHASE1,ATTR_ON_TEXT, buff2);
	fread(&buff2,sizeof buff2,1,fdata);
	SetCtrlAttribute (panelHandle, PANEL_TB_SHOWPHASE2,ATTR_OFF_TEXT, buff2);
	SetCtrlAttribute (panelHandle, PANEL_TB_SHOWPHASE2,ATTR_ON_TEXT, buff2);
	fread(&buff2,sizeof buff2,1,fdata);
	SetCtrlAttribute (panelHandle, PANEL_TB_SHOWPHASE3,ATTR_OFF_TEXT, buff2);
	SetCtrlAttribute (panelHandle, PANEL_TB_SHOWPHASE3,ATTR_ON_TEXT, buff2);
	fread(&buff2,sizeof buff2,1,fdata);
	SetCtrlAttribute (panelHandle, PANEL_TB_SHOWPHASE4,ATTR_OFF_TEXT, buff2);
	SetCtrlAttribute (panelHandle, PANEL_TB_SHOWPHASE4,ATTR_ON_TEXT, buff2);
	fread(&buff2,sizeof buff2,1,fdata);
	SetCtrlAttribute (panelHandle, PANEL_TB_SHOWPHASE5,ATTR_OFF_TEXT, buff2);
	SetCtrlAttribute (panelHandle, PANEL_TB_SHOWPHASE5,ATTR_ON_TEXT, buff2);
	fread(&buff2,sizeof buff2,1,fdata);
	SetCtrlAttribute (panelHandle, PANEL_TB_SHOWPHASE6,ATTR_OFF_TEXT, buff2);
	SetCtrlAttribute (panelHandle, PANEL_TB_SHOWPHASE6,ATTR_ON_TEXT, buff2);
	fread(&buff2,sizeof buff2,1,fdata);
	SetCtrlAttribute (panelHandle, PANEL_TB_SHOWPHASE7,ATTR_OFF_TEXT, buff2);
	SetCtrlAttribute (panelHandle, PANEL_TB_SHOWPHASE7,ATTR_ON_TEXT, buff2);
	fclose(fdata);

}

//*****************************************************************************************
void SaveArrays(char savedname[500],int csize)
{
	FILE *fdata;
	int i=0,j=0,k=0;
	int xval=16,yval=16,zval=10;
	char buff[500],buff2[100];
	strncpy(buff,savedname,csize-4);
	strcat(buff,".arr");
	if((fdata=fopen(buff,"w"))==NULL)
	{
	//	InsertListItem(panelHandle,PANEL_DEBUG,-1,buff,1);
		MessagePopup("Save Error","Failed to save data arrays");
	}
	//The Plan:
	//write the dimensionality of the array....
	//szve as x,y,z...
	//scan order is Page,X,Y..
	//So, scanning by channel at each step....
	//Need time info tooo...
	// So...after dim...save appropriate time array. (page ,x)
	// save structures.
	//locked dimensions...16,16,10	
	//The dimensions
	fwrite(&xval,sizeof xval, 1,fdata);
	fwrite(&yval,sizeof yval, 1,fdata);
	fwrite(&zval,sizeof zval, 1,fdata);
	//now for the times.	
	fwrite(&TimeArray,sizeof TimeArray,1,fdata);
	//and the analog data
	fwrite(&AnalogTable,sizeof AnalogTable,1,fdata);
	fwrite(&DigTableValues,sizeof DigTableValues,1,fdata);
																	   
	fwrite(&AChName,sizeof AChName,1,fdata);
	fwrite(&DChName,sizeof DChName,1,fdata);
	fwrite(&ddstable,sizeof ddstable,1,fdata);
	GetCtrlAttribute (panelHandle, PANEL_TB_SHOWPHASE1,ATTR_OFF_TEXT, buff2);
	fwrite(&buff2,sizeof buff2,1,fdata);
	GetCtrlAttribute (panelHandle, PANEL_TB_SHOWPHASE2,ATTR_OFF_TEXT, buff2);
	fwrite(&buff2,sizeof buff2,1,fdata);
	GetCtrlAttribute (panelHandle, PANEL_TB_SHOWPHASE3,ATTR_OFF_TEXT, buff2);
	fwrite(&buff2,sizeof buff2,1,fdata);
	GetCtrlAttribute (panelHandle, PANEL_TB_SHOWPHASE4,ATTR_OFF_TEXT, buff2);
	fwrite(&buff2,sizeof buff2,1,fdata);
	GetCtrlAttribute (panelHandle, PANEL_TB_SHOWPHASE5,ATTR_OFF_TEXT, buff2);
	fwrite(&buff2,sizeof buff2,1,fdata);
	GetCtrlAttribute (panelHandle, PANEL_TB_SHOWPHASE6,ATTR_OFF_TEXT, buff2);
	fwrite(&buff2,sizeof buff2,1,fdata);
	GetCtrlAttribute (panelHandle, PANEL_TB_SHOWPHASE7,ATTR_OFF_TEXT, buff2);
	fwrite(&buff2,sizeof buff2,1,fdata);
	fclose(fdata);
}


//**********************************************************************************
void BuildUpdateList(double TMatrix[],struct AnVals AMat[NUMBERANALOGCHANNELS+1][500],int DMat[NUMBERDIGITALCHANNELS+1][500],ddsoptions_struct DDSArray[500], int numtimes)
//void BuildUpdateList(double TMatrix[],struct AnVals AMat[16][500],double DMat[16][500])

{
//PLAN:  	first...for each Timeblock:  check how many are steps and how many aren't
//  	    calculate all step changes.
//			Loop unitltimes done
//				calc new changes for nonstep
	BOOL UseCompression,ArraysToDebug; 
	int *UpdateNum;
	int *ChNum;
	float *ChVal;
	FILE *fp;
	int NewTimeMat[500];
	int i=0,j=0,k=0,m=0,n=0,tau=0,p=0,imax;
	int nupcurrent=0,nuptotal=0,checkresettozero=0;
	int usefcn=0,LastDVal=0,LastDVal2=0,digchannel;  //Bool
	int UsingFcns[NUMBERANALOGCHANNELS+1]={0},count=0,ucounter=0,digval,digval2,counter,channel;
	double LastAval[NUMBERANALOGCHANNELS+1]={0},NewAval,TempChVal,TempChVal2;
	int ResetToZeroAtEnd[30]; //1-24 for analog, ...but for now, if [1]=1 then all zero, else no change
	int timemult,t,c,bigger;
	double cycletime=0;
	int GlobalDelay=40000;
	char buff[100];
	int repeat=0,timesum=0;
	static int didboot=0;
	static int didprocess=0;
	int memused;
	int timeused;
	int tmp_dds;
	dds_cmds_ptr dds_cmd_seq = NULL;
	int digchannelsum;
	int newcount=0;
	// variables for timechannel optimization
	int ZeroThreshold=50;
	int lastfound=0;
	int ii=0,jj=0,kk=0,tt=0; // variables for loops
	int start_offset=0;
// done optimization loop variables
	
	//Change run button appearance
	
	time_t tstart,tstop;
	//TODO:	make sure it writes the final value....currently might not.
	// Change the color of the Run button, so we know the program/ADwin is active
	//
	
	SetCtrlAttribute (panelHandle, PANEL_CMD_RUN,ATTR_CMD_BUTTON_COLOR, VAL_GREEN);            
   // DisplayPanel(panelHandle);
   	tstart=clock();
	timemult=(int)(1/EventPeriod); //number of adwin events per millisecond
	GlobalDelay=EventPeriod/AdwinTick; //??
	ClearListCtrl(panelHandle,PANEL_DEBUG); //clear debug list control
		 
	//make a new time list...using the needed step size.i.e in number of ADWin events
		
	for (i=1;i<=numtimes;i++) 
	{
		NewTimeMat[i]=(int)(TMatrix[i]*timemult); //number of Adwin events in column i
		timesum=timesum+NewTimeMat[i]; //total number of Adwin events
	}
	
	cycletime=(double)timesum/(double)timemult/10000;
	sprintf(buff,"timesum %d",timesum);
	InsertListItem(panelHandle,PANEL_DEBUG,-1,buff,1);
	//dynamically allocate the memory for the time array (instead of using a static array:UpdateNum)
	//We are making an assumption about how many programmable points we may need to use.
	//For now assume that number of channel updates <= 4* #of events.

	if(ChangedVals==TRUE) //reupdate the ADWIN array, if user values have changed
    {	
    	/* Update the array of DDS commands
		EventPeriod is in ms, create_command_array in s, so convert units */
//		dds_cmd_seq = create_dds_cmd_sequence(DDSArray, numtimes,DDSFreq.PLLmult, 
//		DDSFreq.extclock,EventPeriod/1000);
    
	//	SaveLastGuiSettings();
		UpdateNum=calloc((int)((double)timesum*1.2),sizeof(int));
		if(!UpdateNum){ exit(1); }
		ChNum=calloc((int)((double)timesum*4),sizeof(int));
		if(!UpdateNum){ exit(1); }
		ChVal=calloc((int)((double)timesum*4),sizeof(double));
		if(!UpdateNum){ exit(1); }
		
		
		//Go through for each column that needs to be updated
		
		//Important Variables:
		//count: Number of Adwin events run up until the current position
		//nupcurrent: number of updates for the current Adwin event
		//nuptotal: current position in the channel/value column
		//*********************Initialize outputs to zero********************
/*		for(i=1;i<=NUMBERANALOGCHANNELS;i++)
		{
			ChNum[i]=AChName[i].chnum;
			ChVal[i]=AChName[i].tbias;
		}
		//digital lines
		ChNum[NUMBERANALOGCHANNELS+1]=33;
		ChNum[NUMBERANALOGCHANNELS+2]=34;
		ChVal[NUMBERANALOGCHANNELS+1]=0;
		ChVal[NUMBERANALOGCHANNELS+2]=0;
		nuptotal=26;
		nupcurrent=26;
		UpdateNum[1]=2;
		UpdateNum[2]=2;
		UpdateNum[3]=2;
		UpdateNum[4]=2;
		UpdateNum[5]=2;
		UpdateNum[6]=2;
		UpdateNum[7]=2;
		UpdateNum[8]=2;
		UpdateNum[9]=2;
		UpdateNum[10]=2;
		UpdateNum[11]=2;
		UpdateNum[12]=2;
		UpdateNum[13]=2;

		count=13;   
		start_offset=13;		*/
		//*********************Done initializing outputs to zero********************
	
		for (i=1;i<=numtimes;i++)
		{
			// find out how many channels need updating this round...
			// if its a fcn, keep a list of UsingFcns, and change it now
			nupcurrent=0;
			usefcn=0;
			
			for (j=1;j<=NUMBERANALOGCHANNELS;j++)	// scan over the analog channel..find update ones.
			{
				LastAval[j]=-99;
				if(!(AMat[j][i].fval==AMat[j][i-1].fval))
				{
					nupcurrent++;
					nuptotal++;
					ChNum[nuptotal]=AChName[j].chnum;
					NewAval=CalcFcnValue(AMat[j][i].fcn,AMat[j][i-1].fval,AMat[j][i].fval,AMat[j][i].tscale,0.0);
	// use 1 of the following 2 lines.  Line 1 for output =entry (in V) on panel
	// line 2 for scaled output.  Maybe rewrite to do a function call with a more complex formula.
//					ChVal[nuptotal]=NewAval;
 					TempChVal=AChName[j].tbias+NewAval*AChName[j].tfcn;
 					ChVal[nuptotal]=CheckIfWithinLimits(TempChVal,j);
 					
					if(!(AMat[j][i].fcn==1))
					{
						usefcn++;
						UsingFcns[usefcn]=j;	// mark these lines for special attention..more complex	
					}
				}
			}	//now the digital value
			digval=0;
			digval2=0;
			for(k=1;k<=NUMBERDIGITALCHANNELS;k++)
			{
				digchannel=DChName[k].chnum;
				if(digchannel<=16)
				{
					digval=digval+DMat[k][i]*pow(2,DChName[k].chnum-1);
				}
				if((digchannel>=17)&&(digchannel<=24))
				{
					digval2=digval2+DMat[k][i]*pow(2,(DChName[k].chnum-16)-1);
				}
			}
			if(!(digval==LastDVal))
			{
				nupcurrent++;
				nuptotal++;
				ChNum[nuptotal]=33;		   //***********change this!!! to 99, or 98 or 255 or something 
				ChVal[nuptotal]=digval;		// also update the ADBasic code 
			}
			LastDVal=digval;
			if(!(digval2==LastDVal2))
			{
				nupcurrent++;
				nuptotal++;
				ChNum[nuptotal]=34;		   //***********change this!!! to 99, or 98 or 255 or something 
				ChVal[nuptotal]=digval2;		// also update the ADBasic code 
			}
			LastDVal2=digval2;
			count++;
			UpdateNum[count]=nupcurrent;
			
			//end of first scan  
			//now do the remainder of the loop...but just the compicated fcns
			t=0;
			while(t<NewTimeMat[i]-1)
			{
				t++;
				k=0;
				nupcurrent=0;
				//do the DDS
		/*		tmp_dds = get_dds_cmd(dds_cmd_seq, count-1-start_offset);  //dds translator(zero base) runs 1 behind this counter
				if (tmp_dds>=0)
				{
					nupcurrent++;
					nuptotal++;
					ChNum[nuptotal] = 31; //dummy channel
					ChVal[nuptotal] = tmp_dds;
				
				} //done the DDS
		*/		
				while(k<usefcn)
				{
					k++;
					c=UsingFcns[k];
					NewAval=CalcFcnValue(AMat[c][i].fcn,AMat[c][i-1].fval,AMat[c][i].fval,AMat[c][i].tscale,t);
//					TempChVal=AChName[j].tbias+NewAval*AChName[j].tfcn;

					TempChVal=AChName[c].tbias+NewAval*AChName[c].tfcn;

					TempChVal2=CheckIfWithinLimits(TempChVal,c);
					if(fabs(TempChVal2-LastAval[k])>0.001)
					{
						nupcurrent++;
						nuptotal++;
						ChNum[nuptotal]=AChName[c].chnum;
						ChVal[nuptotal]=AChName[c].tbias+NewAval*AChName[c].tfcn;
						LastAval[k]=TempChVal2;
					}
				}
				count++;
				UpdateNum[count]=nupcurrent;
			}//Done this element of the TMatrix
		}//done scanning over times array
		//draw in DEBUG box
		bigger=count;
		if(nuptotal>bigger) {bigger=nuptotal;}
		GetMenuBarAttribute (menuHandle, MENU_PREFS_COMPRESSION, ATTR_CHECKED, &UseCompression);     
		GetMenuBarAttribute (menuHandle, MENU_PREFS_SHOWARRAY, ATTR_CHECKED, &ArraysToDebug);     		
		newcount=0;
		if(UseCompression)
		{
			OptimizeTimeLoop(UpdateNum,count,&newcount);
		}
		
		
		if(ArraysToDebug)
		{
			//fp=fopen("outarraymerge.txt","w");
			imax=newcount;
			if(newcount==0) {imax=count;}
			if(imax>1000){imax=1000;}
			for(i=1;i<imax+1;i++)
			{
				sprintf(buff,"%d :  %d    %d    %f",i,UpdateNum[i],ChNum[i],ChVal[i]);
				InsertListItem(panelHandle,PANEL_DEBUG,-1,buff,1);
			//	fprintf(fp,"%d,%d,%d,%f\n",i,UpdateNum[i],ChNum[i],ChVal[i]);
			}
		//	fclose(fp);
		}
		
		
		tstop=clock();
		
		#ifdef PRINT_TO_DEBUG
			sprintf(buff,"count %d",count);
			InsertListItem(panelHandle,PANEL_DEBUG,-1,buff,1);
			sprintf(buff,"compressed count %d",newcount);
			InsertListItem(panelHandle,PANEL_DEBUG,-1,buff,1);
			sprintf(buff,"updates %d",nuptotal);
			InsertListItem(panelHandle,PANEL_DEBUG,-1,buff,1);
	
	
			sprintf(buff,"time to generate arrays:   %d",tstop-tstart);
			InsertListItem(panelHandle,PANEL_DEBUG,-1,buff,1);
			
		#endif
		
		if(didboot==0)
		{
			Boot("C:\\ADWIN\\ADWIN10.BTL",0);           
			didboot=1;
		}
		if (didprocess==0)
		{
			processnum=Load_Process("TransferData.TA1");
			didprocess=1;
		}
		for(p-1;p<=NUMBERANALOGCHANNELS;p++) {ResetToZeroAtEnd[p]=AChName[p].resettozero;}
	
		if(UseCompression)
		{
			SetPar(1,newcount);  	//Let ADwin know how many counts (read as Events) we will be using.		
			SetData_Long(1,UpdateNum,1,newcount+1);
		}
		else
		{
		  	SetPar(1,count);  	//Let ADwin know how many counts (read as Events) we will be using.
			SetData_Long(1,UpdateNum,1,count+1);
		}
		
		SetPar(2,GlobalDelay);
		SetData_Long(2,ChNum,1,nuptotal+1);
		SetData_Float(3,ChVal,1,nuptotal+1);
		// determine if we should reset values to zero after a cycle
		GetMenuBarAttribute (menuHandle, MENU_SETTINGS_RESETZERO, ATTR_CHECKED,&checkresettozero);
		for(i=1;i<=NUMBERANALOGCHANNELS;i++)
		{	
			ResetToZeroAtEnd[i]=AChName[i].resettozero;
		}
		digchannelsum=0;
		for(i=1;i<=16;i++)
		{
			if(DChName[i].resettolow==1) {digchannelsum+=2^(i-1);}	
		}
		ResetToZeroAtEnd[25]=digchannelsum;// lower 16 digital channels    
		for(i=17;i<=NUMBERDIGITALCHANNELS;i++)
		digchannelsum=0;
		{
			if(DChName[i].resettolow==1) {digchannelsum+=2^(i-17);}	
		}
		ResetToZeroAtEnd[26]=digchannelsum;// digital channels 17-24
		//ResetToZeroAtEnd[25]=0;// lower 16 digital channels    
		//ResetToZeroAtEnd[26]=0;// digital channels 17-24
		ResetToZeroAtEnd[27]=0;// master override....if ==1 then reset none
		if(checkresettozero==0) { ResetToZeroAtEnd[27]=1;}

		SetData_Long(4,ResetToZeroAtEnd,1,NUMBERANALOGCHANNELS+6);
		// done ealuating channels that are reset to  zero (low)
		ChangedVals=0;
	}
	tstop=clock();         
	timeused=tstop-tstart;     
	t=Start_Process(processnum);
	tstop=clock();         	
	sprintf(buff,"Time to transfer and start ADwin:   %d",timeused);
	
	InsertListItem(panelHandle,PANEL_DEBUG,-1,buff,1);
	memused=(count+2*nuptotal)*4;//in bytes
	sprintf(buff,"MB of data sent:   %f",(double)memused/1000000);
	InsertListItem(panelHandle,PANEL_DEBUG,-1,buff,1);
	sprintf(buff,"Transfer Rate:   %f   MB/s",(double)memused/(double)timeused/1000);
	InsertListItem(panelHandle,PANEL_DEBUG,-1,buff,1);

	SetCtrlAttribute (panelHandle, PANEL_CMD_RUN,ATTR_CMD_BUTTON_COLOR, 0x00B0B0B0);
	//DisplayPanel(panelHandle);
	//re-enable the timer if necessary
	GetCtrlVal(panelHandle,PANEL_TOGGLEREPEAT,&repeat);
	if(repeat==1)
	{
		SetCtrlAttribute(panelHandle,PANEL_TIMER,ATTR_ENABLED,1);
		SetCtrlAttribute (panelHandle, PANEL_TIMER, ATTR_INTERVAL,cycletime);
		ResetTimer(panelHandle,PANEL_TIMER);
	}
	free(UpdateNum);
	free(ChNum);
	free(ChVal);
}

//********************************************************************************************
double CalcFcnValue(int fcn,double Vinit,double Vfinal, double timescale,double telapsed)
{
	double value=-99,amplitude,slope,aconst,bconst,tms;
	if (timescale<=0) {timescale=1;}
	tms=telapsed*EventPeriod;
	switch(fcn)
	{
		case 1 ://step function
			value=Vfinal;
			break;
		case 2 ://linear ramp
			amplitude=Vfinal-Vinit;
			slope=amplitude/timescale;
			if(tms>timescale) { value=Vfinal;}
			else {value=Vinit+slope*tms;}
			break;
		case 3 ://exponential
			amplitude=Vfinal-Vinit;
			value=Vfinal-amplitude*exp(-tms/timescale);
			break;
		case 4 :
			amplitude=Vfinal-Vinit;
			aconst=3*amplitude/pow(timescale,2);
			bconst=-2*amplitude/pow(timescale,3);
			if(tms>timescale) 
			{
				value=Vfinal;
			}
			else
			{
				value=Vinit+(aconst*pow(tms,2)+bconst*pow(tms,3));
			}
			break;
	}
	// Check if the value exceeds the limits.
	return value;
}
//********************************************************************************************

int CVICALLBACK DISPLAYDIAL_CALLBACK (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	int dialval=0;
	switch (event)
		{
		case EVENT_COMMIT:
			GetCtrlVal (panelHandle, PANEL_DISPLAYDIAL, &dialval);
			// Now change the size of the tables depending on the dial value.
			

			SetChannelDisplayed(dialval);

			break;
		}
	return 0;
}


/************************************************************************
Author: Stefan
-------
Date Created: August 2004
-------
Description: Resizes the analog table on the gui
-------
Return Value: void
-------
Parameters: new top, left and height values for the list box
*************************************************************************/
void ReshapeAnalogTable( int top,int left,int height)
{
	int j,istext=0,celltype=0;
	
	
	
	//resize the analog table and all it's related list boxes
  	SetCtrlAttribute (panelHandle, PANEL_ANALOGTABLE, ATTR_HEIGHT, height);
  	SetCtrlAttribute (panelHandle, PANEL_ANALOGTABLE, ATTR_LEFT, left);
	SetCtrlAttribute (panelHandle, PANEL_ANALOGTABLE, ATTR_TOP, top);
	
	SetCtrlAttribute (panelHandle, PANEL_TBL_ANAMES, ATTR_LEFT, left-165);
	SetCtrlAttribute (panelHandle, PANEL_TBL_ANAMES, ATTR_HEIGHT, height);
	SetCtrlAttribute (panelHandle, PANEL_TBL_ANAMES, ATTR_TOP, top);   
	
	SetCtrlAttribute (panelHandle, PANEL_TBL_ANALOGUNITS, ATTR_HEIGHT, height);  
	SetCtrlAttribute (panelHandle, PANEL_TBL_ANALOGUNITS, ATTR_TOP, top);
	SetCtrlAttribute (panelHandle, PANEL_TBL_ANALOGUNITS, ATTR_LEFT, left+705);  
	

	for (j=1;j<=NUMBERANALOGCHANNELS+1;j++)
	{
		SetTableRowAttribute (panelHandle, PANEL_ANALOGTABLE, j,ATTR_SIZE_MODE, VAL_USE_EXPLICIT_SIZE);
		SetTableRowAttribute (panelHandle, PANEL_ANALOGTABLE, j, ATTR_ROW_HEIGHT, (height)/(NUMBERANALOGCHANNELS+1));
	    SetTableRowAttribute (panelHandle, PANEL_TBL_ANAMES, j,ATTR_SIZE_MODE, VAL_USE_EXPLICIT_SIZE);
		SetTableRowAttribute (panelHandle, PANEL_TBL_ANAMES, j, ATTR_ROW_HEIGHT, (height)/(NUMBERANALOGCHANNELS+1));
		SetTableRowAttribute (panelHandle, PANEL_TBL_ANALOGUNITS, j,ATTR_SIZE_MODE, VAL_USE_EXPLICIT_SIZE);
		SetTableRowAttribute (panelHandle, PANEL_TBL_ANALOGUNITS, j, ATTR_ROW_HEIGHT, (height)/(NUMBERANALOGCHANNELS+1));
	}
}


/************************************************************************
Author: Stefan
-------
Date Created: August 2004
-------
Description: Resizes the digital table on the gui
-------
Return Value: void
-------
Parameters: new top, left and height values for the list box
*************************************************************************/
void ReshapeDigitalTable( int top,int left,int height)
{
 int j;
  	
  	SetCtrlAttribute (panelHandle, PANEL_DIGTABLE, ATTR_HEIGHT, height);
  	SetCtrlAttribute (panelHandle, PANEL_DIGTABLE, ATTR_LEFT, left);
	SetCtrlAttribute (panelHandle, PANEL_DIGTABLE, ATTR_TOP, top);
	SetCtrlAttribute (panelHandle, PANEL_TBL_DIGNAMES, ATTR_TOP, top); 
	SetCtrlAttribute (panelHandle, PANEL_TBL_DIGNAMES, ATTR_LEFT, left-165);
	SetCtrlAttribute (panelHandle, PANEL_TBL_DIGNAMES, ATTR_HEIGHT, height);
	
	for (j=1;j<=NUMBERDIGITALCHANNELS;j++)
	{
		SetTableRowAttribute (panelHandle, PANEL_DIGTABLE, j, ATTR_ROW_HEIGHT, (height)/(NUMBERDIGITALCHANNELS));
		SetTableRowAttribute (panelHandle, PANEL_TBL_DIGNAMES, j, ATTR_ROW_HEIGHT, (height)/(NUMBERDIGITALCHANNELS));
	}

}


/************************************************************************
Author: David McKay
-------
Date Created: August 23, 2004
-------
Date Modified: August 23, 2004
-------
Description: Sets how to display the data, graphically or numerically
-------
Return Value: void
-------
Parameter1: 
int display_setting: type of display
VAL_CELL_NUMERIC : graphic
VAL_CELL_PICTURE : numeric
*************************************************************************/
void SetDisplayType(int display_setting)
{

	int i,j;
	
	for(i=1;i<=14;i++)
	{
		for(j=1;j<=NUMBERANALOGCHANNELS;j++)
		{
			SetTableCellAttribute (panelHandle, PANEL_ANALOGTABLE, MakePoint(i,j),
				   ATTR_CELL_TYPE, display_setting);
		 }
	}
	DrawNewTable(0);
	
	//set button status
	if (display_setting == VAL_CELL_NUMERIC)
	{
		SetCtrlVal(panelHandle, PANEL_TGL_NUMERICTABLE, 0);
	}
	else if (display_setting == VAL_CELL_PICTURE)
	{
		SetCtrlVal(panelHandle, PANEL_TGL_NUMERICTABLE, 1);
	}	


}


/************************************************************************
Author: David McKay
-------
Date Created: August 23, 2004
-------
Date Modified: August 23, 2004
-------
Description: Sets which channel to display: analog, digital or both
-------
Return Value: void
-------
Parameter1: 
int display_setting: channel to display
1: both
2: analog
3: digital
*************************************************************************/
void SetChannelDisplayed(int display_setting)
{
	int toppos1,toppos2,leftpos,heightpos;
	//hide everything
	SetCtrlAttribute (panelHandle, PANEL_DIGTABLE, ATTR_VISIBLE, 0);
	SetCtrlAttribute (panelHandle, PANEL_ANALOGTABLE, ATTR_VISIBLE, 0);
	SetCtrlAttribute (panelHandle, PANEL_TBL_ANAMES, ATTR_VISIBLE, 0);    
	SetCtrlAttribute (panelHandle, PANEL_TBL_ANALOGUNITS, ATTR_VISIBLE, 0);    
	SetCtrlAttribute (panelHandle, PANEL_TBL_DIGNAMES, ATTR_VISIBLE, 0);    	   
	//ATTR_LABEL_VISIBLE 
	
	switch (display_setting)   
		{
		case 1:		//both
			heightpos=380;
			toppos1=155;
			leftpos=170;
			toppos2=1280-100-heightpos;
			toppos2=155+380+50; 
			ReshapeAnalogTable(toppos1,170,heightpos);   //passed top, left and height
			ReshapeDigitalTable(toppos2,170,heightpos);
								
			SetCtrlAttribute (panelHandle, PANEL_DIGTABLE, ATTR_VISIBLE, 1);
			SetCtrlAttribute (panelHandle, PANEL_ANALOGTABLE, ATTR_VISIBLE, 1);
			SetCtrlAttribute (panelHandle, PANEL_TBL_ANAMES, ATTR_VISIBLE, 1);    
			SetCtrlAttribute (panelHandle, PANEL_TBL_ANALOGUNITS, ATTR_VISIBLE, 1); 
			SetCtrlAttribute (panelHandle, PANEL_TBL_DIGNAMES, ATTR_VISIBLE, 1);
			
			break;
		
		case 2:		//analog table
			
			ReshapeAnalogTable(155,170,600);   //passed top, left and height
			
				
			SetCtrlAttribute (panelHandle, PANEL_ANALOGTABLE, ATTR_VISIBLE, 1);
			SetCtrlAttribute (panelHandle, PANEL_TBL_ANAMES, ATTR_VISIBLE, 1);    
			SetCtrlAttribute (panelHandle, PANEL_TBL_ANALOGUNITS, ATTR_VISIBLE, 1);
			break;
		
		case 3:		 // digital table
			
		
			 //passed top, left and height
			ReshapeDigitalTable(155,170,600);
				
			SetCtrlAttribute (panelHandle, PANEL_DIGTABLE, ATTR_VISIBLE, 1);
			SetCtrlAttribute (panelHandle, PANEL_TBL_DIGNAMES, ATTR_VISIBLE, 1);		 
			break;
		
		}
		
		SetCtrlVal(panelHandle, PANEL_DISPLAYDIAL, display_setting);
		
	DrawNewTable(0);
	
	return;
	
}
//***********************************************************************************************
double CheckIfWithinLimits(double OutputVoltage, int linenumber)
{
	double NewOutput;
	NewOutput=OutputVoltage;
	if(OutputVoltage>AChName[linenumber].maxvolts) NewOutput=AChName[linenumber].maxvolts;
	if(OutputVoltage<AChName[linenumber].minvolts) NewOutput=AChName[linenumber].minvolts;
	return NewOutput;
}
//***********************************************************************************************
void CVICALLBACK RESETZERO_CALLBACK (int menuBar, int menuItem, void *callbackData,
		int panel)
{
	int ischecked=0;
	GetMenuBarAttribute (menuHandle, MENU_SETTINGS_RESETZERO, ATTR_CHECKED,&ischecked);
	SetMenuBarAttribute (menuHandle, MENU_SETTINGS_RESETZERO, ATTR_CHECKED,abs(ischecked-1));
}
//***********************************************************************************************
void SaveLastGuiSettings(void)
{
	// Save settings:  First look for file .ini  This will be a simple 1 line file stating the name of the last file
	//saved.  Load this up and use as the starting name in the file dialog.
	char fname[100]="c:\\LastGui.pan";
	SavePanelState(PANEL, fname, 1);
	SaveArrays(fname, strlen(fname));
	
}
//********************************************************************************************************************

void CVICALLBACK EXPORT_PANEL_CALLBACK (int menuBar, int menuItem, void *callbackData,
		int panel)
{
	FILE *fexport;
	int i=0,j=0,k=0;
	int xval=16,yval=16,zval=10;
	char buff[500],buff2[100],fexportname[200],buff3[31],bigbuff[2000];
	char fcnmode[6]="SLEJ";   // step, linear, exponential, const-jerk
	double MetaTimeArray[500];
	int MetaDigitalArray[NUMBERDIGITALCHANNELS+1][500];
	struct AnVals MetaAnalogArray[NUMBERANALOGCHANNELS+1][500];
	double DDSfreq1[500], DDSfreq2[500],DDScurrent[500],DDSstop[500];
	int mindex,nozerofound,tsize;
	fcnmode[0]='S';fcnmode[1]='L';fcnmode[2]='E';fcnmode[3]='J';
	
	FileSelectPopup ("", "*.export", "", "Export Panel?", VAL_SAVE_BUTTON, 0, 0, 1, 1,fexportname );
	if((fexport=fopen(fexportname,"w"))==NULL)
	{
		MessagePopup("Save Error","Failed to save configuration file");
	}
	
	isdimmed=1;
	//Lets build the times list first...so we know how long it will be.
	//check each page...find used columns and dim out unused....(with 0 or negative values)
	SetCtrlAttribute(panelHandle,PANEL_ANALOGTABLE,ATTR_TABLE_MODE,VAL_COLUMN); 
	mindex=0;
	for(k=1;k<=NUMBEROFPAGES;k++)				//go through for each page       
	{
		nozerofound=1;
		if(ischecked[k]==1) //if the page is selected
		{
		//go through for each time column
			for(i=1;i<14;i++)
			{
				if((nozerofound==1)&&(TimeArray[i][k]>0)) 
				//ignore all columns after the first time=0
				{
					mindex++; //increase the number of columns counter
					MetaTimeArray[mindex]=TimeArray[i][k];
						
					//go through for each analog channel
					for(j=1;j<=NUMBERANALOGCHANNELS;j++)
					{
						MetaAnalogArray[j][mindex].fcn=AnalogTable[i][j][k].fcn;
						MetaAnalogArray[j][mindex].fval=AnalogTable[i][j][k].fval;
						MetaAnalogArray[j][mindex].tscale=AnalogTable[i][j][k].tscale;
						MetaDigitalArray[j][mindex]=DigTableValues[i][j][k];
						DDScurrent[mindex]=ddstable[i][k].amplitude;
						DDSfreq1[mindex]=ddstable[i][k].start_frequency;
						DDSfreq2[mindex]=ddstable[i][k].end_frequency;						
						DDSstop[mindex]=ddstable[i][k].is_stop;
					}
				}
				else if (TimeArray[i][k]==0) 
				{
					nozerofound=0;
				}
			}
		}
	}

	tsize=mindex; //tsize is the number of columns
	// now write to file
	// write header
	sprintf(bigbuff,"Time(ms)");
	for(i=1;i<=tsize;i++)
	{  
		strcat(bigbuff,",");
		sprintf(buff,"%f",MetaTimeArray[i]);
		strcat(bigbuff,buff);
	}
	strcat(bigbuff,"\n");
	fprintf(fexport,bigbuff);
	//done header, now write analog lines
	for(j=1;j<=24;j++)
	{
		sprintf(bigbuff,AChName[j].chname);
		for(i=1;i<=tsize;i++)
		{  
			strcat(bigbuff,",");
			strncat(bigbuff,fcnmode+MetaAnalogArray[j][i].fcn-1,1);
			sprintf(buff,"%3.2f",MetaAnalogArray[j][i].fval);  
			strcat(bigbuff,buff);
		}
		strcat(bigbuff,"\n");
		fprintf(fexport,bigbuff);
	}
	//done analog lines, now write DDS
	
	sprintf(bigbuff,"DDS");
	for(i=1;i<=tsize;i++)
	{
		strcat(bigbuff,",");
		if(DDSstop[i]==TRUE) 
		{
			strcat(bigbuff,"OFF");
		}
		else
		{
			strcat(bigbuff,"ma");
			sprintf(buff,"%4.2f",DDScurrent[i]);
			strcat(bigbuff,buff);
			strcat(bigbuff,"_FA");
			sprintf(buff,"%4.2f",DDSfreq1[i]);
			strcat(bigbuff,buff);
			strcat(bigbuff,"_FB");
			sprintf(buff,"%4.2f",DDSfreq2[i]);
			strcat(bigbuff,buff);
		}	
	
	}
	fprintf(fexport,bigbuff);     
	//done DDS, now do digital
	for(j=1;j<=24;j++)
	{
		sprintf(bigbuff,DChName[j].chname);
		for(i=1;i<=tsize;i++)
		{  
			strcat(bigbuff,",");
			sprintf(buff,"%d",MetaDigitalArray[j][i]);  
		
			strcat(bigbuff,buff);
		}
		strcat(bigbuff,"\n");
		fprintf(fexport,bigbuff);
	}
	

	fclose(fexport);
	
}

//***********************************************************************************************

void CVICALLBACK CONFIG_EXPORT_CALLBACK (int menuBar, int menuItem, void *callbackData,
		int panel)
{

	FILE *fconfig;
	int i=0,j=0,k=0;
	int xval=16,yval=16,zval=10;
	char buff[500],buff2[100],fconfigname[200],buff3[31];

	

	FileSelectPopup ("", "*.config", "", "Save Configuration", VAL_SAVE_BUTTON, 0, 0, 1, 1,fconfigname );
	
	
	if((fconfig=fopen(fconfigname,"w"))==NULL)
	{
	//	InsertListItem(panelHandle,PANEL_DEBUG,-1,buff,1);
		MessagePopup("Save Error","Failed to save configuration file");
	}
	// write out analog channel info
	sprintf(buff,"Analog Channels\n");
	fprintf(fconfig,buff);
	sprintf(buff,"Row,Channel,Name,tbias,tfcn,MaxVolts,MinVolts,Units\n");
	fprintf(fconfig,buff);
	for(i=1;i<=24;i++)
	{
		strncpy(buff3,AChName[i].chname,30);
		sprintf(buff,"%d,%d,%s,%f,%f,%f,%f,%s\n",i,AChName[i].chnum,buff3,AChName[i].tbias
			,AChName[i].tfcn,AChName[i].maxvolts,AChName[i].minvolts,AChName[i].units);
		fprintf(fconfig,buff);
	}
	// Write out digital Channel info
	sprintf(buff,"Digital Channels\n");
	fprintf(fconfig,buff);
	sprintf(buff,"Row,Channel,Name\n");
	fprintf(fconfig,buff);
	
	for(i=1;i<=24;i++)
	{
		sprintf(buff,"%d,%d,%s\n",i,DChName[i].chnum,DChName[i].chname);
		fprintf(fconfig,buff);
	}
	
	
	fclose(fconfig);
}


void CVICALLBACK MENU_ALLLOW_CALLBACK (int menuBar, int menuItem, void *callbackData,
		int panel)
{
}

void CVICALLBACK MENU_HOLD_CALLBACK (int menuBar, int menuItem, void *callbackData,
		int panel)
{
}

void CVICALLBACK MENU_BYCHANNEL_CALLBACK (int menuBar, int menuItem, void *callbackData,
		int panel)
{
}

void OptimizeTimeLoop(int *UpdateNum,int count, int *newcount)
{
	int i=0,k=0; // i is the counter through the original UpdateNum list
	int j=0; // t is the counter through the NewUpdateNum list 
	int t=0;
	int LowZeroThreshold,HighZeroThreshold;
	int LastFound=0;
	int numberofzeros;
	i=1;
	t=1;
	LowZeroThreshold=10;	  // minimum number of consecutive zero's to encounter before optimizing
	HighZeroThreshold=10000;  // maximum number of consecutive zero's to optimize 
							//  We do not want to exceed the counter on the ADwin
							//  ADwin uses a 40MHz clock, 1 ms implies counter set to 40,000
	do 
	{
		//find the number of zero's, without updating i
		
		if((UpdateNum[i]==0)&&(i<=count)) //point A
		{
			j=1;
			do 
			{ 
				LastFound=UpdateNum[i+j];
				if(LastFound!=0)
				{
					if(j>=LowZeroThreshold)
					{
						numberofzeros=j;
						while(numberofzeros>HighZeroThreshold)
						{
							UpdateNum[t]=-HighZeroThreshold;
							t=t+1;
							numberofzeros=numberofzeros-HighZeroThreshold;
						} 
						UpdateNum[t]=-numberofzeros;
						t++;
					}
					else
					{
						for(k=1;k<j;k++) 
						{   
							UpdateNum[t]=0;
							t=t+1;
						}
						UpdateNum[t]=0;
					}
					
				}
				j=j+1;	 
			}while((LastFound==0)&&((i+j)<=(count+10)));

			i=i+j-2;
		}	//endA
		else
		{
			UpdateNum[t]=UpdateNum[i];
		}
		i=i+1;
		t=t+1;
	}while(i<count+1);
	//output
	// clean up, set everything to 0 from t to count
//	for (k=t;k<=count;k++) {UpdateNum[k]=0;}
	*newcount=t;
}

int CVICALLBACK NUM_INSERTIONPAGE_CALLBACK (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
		{
		case EVENT_COMMIT:
			ChangedVals=TRUE;
		}
	return 0;
}

int CVICALLBACK NUM_INSERTIONCOL_CALLBACK (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
		{
		case EVENT_COMMIT:
			ChangedVals=TRUE;
			break;
		}
	return 0;
}

void CVICALLBACK COMPRESSION_CALLBACK (int menuBar, int menuItem, void *callbackData,
		int panel)
{
	BOOL UseCompression;
	GetMenuBarAttribute (menuHandle, MENU_PREFS_COMPRESSION, ATTR_CHECKED, &UseCompression);
	if(UseCompression)
	{
		SetMenuBarAttribute (menuHandle, MENU_PREFS_COMPRESSION, ATTR_CHECKED, FALSE);
	}
	else
	{
		SetMenuBarAttribute (menuHandle, MENU_PREFS_COMPRESSION, ATTR_CHECKED, TRUE);
	}
}


void CVICALLBACK SHOWARRAY_CALLBACK (int menuBar, int menuItem, void *callbackData,
		int panel)
{
	BOOL ShowArray;
	GetMenuBarAttribute (menuHandle, MENU_PREFS_SHOWARRAY, ATTR_CHECKED, &ShowArray);
	if(ShowArray)
	{
		SetMenuBarAttribute (menuHandle, MENU_PREFS_SHOWARRAY, ATTR_CHECKED, FALSE);
	}
	else
	{
		SetMenuBarAttribute (menuHandle, MENU_PREFS_SHOWARRAY, ATTR_CHECKED, TRUE);
	}


}
