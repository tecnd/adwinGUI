#include <userint.h>

#include "AnalogControl.h"
#include "AnalogControl2.h"

//Info on the panel
//PanelName		PANEL_CTRL
//Control Mode listbox				CTRL_PANEL_LB_CTRLMODE
//Final Voltage NUmeric control		CTRL_PANEL_NUMFINALVAL
// Units indicator					CTRL_PANEL_STRUNITS
//Timescale	numeric control			CTRL_PANEL_NUMTIMESCALE
//Command button to set values		CTRL_PANEL_CMD_SETANALOG 

/*
This panel is used to set the value of an analog control.  It is called by double-clicking on an
analog table cell.

*/



//***********************************************************************
int CVICALLBACK CMD_SETANALAOG_CALLBACK (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
/* Panel button to set analog data. 
Reads data off of the panel controls, ringbox and numberboxes.
Writes data to the AnalogTable array
NOT DONE:  IF TIMESCALE IS TOO LONG THEN OVERWRITE THE FOLLOWING CELL
*/
{
	int itemp=0,cmode;				// variables for reading/writing the control mode
	int docheck=0,valuesgood=1;		// booleans.  docheck=1 if we need to check the time settings
									// valuesgood=1 if okay to write data to AnalogTable array..sets to 0 if prompt denied
	double dtemp=0.0,usedtimescale; //usedtimescale=timescale written in AnalogTable to control ramping, exponential etc.
	double timematrixlength;		// duration of the current column.
	switch (event)
		{
								 
		case EVENT_COMMIT:
			valuesgood=1;
			GetCtrlVal (panelHandle4,CTRL_PANEL_RING_CTRLMODE, &itemp);
			cmode=itemp;
			AnalogTable[currentx][currenty][currentpage].fcn=itemp;
			GetCtrlVal (panelHandle4,CTRL_PANEL_NUMFINALVAL, &dtemp);
			AnalogTable[currentx][currenty][currentpage].fval=dtemp;			
			GetCtrlVal (panelHandle4,CTRL_PANEL_NUMTIMESCALE, &dtemp);
			usedtimescale=dtemp;
			AnalogTable[currentx][currenty][currentpage].tscale=dtemp;
			// Check if the timescale may be too long, if so prompt for 
			//overwrite of the next block.
			timematrixlength=TimeArray[currentx][currentpage];
			docheck=0;
			if((cmode==1)&&(usedtimescale>timematrixlength)) {docheck=1;}
			if((cmode==2)&&(usedtimescale>timematrixlength)) {docheck=1;}
			if((cmode==3)&&(usedtimescale>timematrixlength/3)) {docheck=1;}
			if(cmode==0) {docheck=0;}
			
			
			if (docheck==1)
			{
				//valuesgood=ConfirmPopup("Overlong array selected","Do you want to overwrite the following array?");	
				valuesgood=1;  // just assume timing is right.
				if(valuesgood==1)
				{
				 //code to update the 2 columns
				
				}
			}
			if(valuesgood==1)  // everything checks out, return to main panel 
			{
				HidePanel(panelHandle4);
				DrawNewTable(1);
				break;
			}
			
			
		}
	return 0;
}
//***************************************************************************
void SetControlPanel(void)
{
	double lastvalue;
	BOOL FOUNDVAL=0;
	int cx,cy,cz;
	
	SetCtrlVal(panelHandle4,CTRL_PANEL_STRUNITS, AChName[currenty].units);
	SetCtrlVal(panelHandle4,CTRL_PANEL_STR_CHNAME,AChName[currenty].chname);
	SetCtrlVal (panelHandle4,CTRL_PANEL_NUMFINALVAL, AnalogTable[currentx][currenty][currentpage].fval);
	SetCtrlVal (panelHandle4,CTRL_PANEL_NUMTIMESCALE, AnalogTable[currentx][currenty][currentpage].tscale);
	SetCtrlVal (panelHandle4,CTRL_PANEL_RING_CTRLMODE,AnalogTable[currentx][currenty][currentpage].fcn);

	// find the last value
	cx=1;
	cz=1;
	lastvalue=0;
	while(!FOUNDVAL)
	{
		if(TimeArray[cx][cz]==0) {cz++;cx=1;}
		if(TimeArray[cx][cz]<0) 
		{
			cx++;  
			if(cx>=14) {cx=1;cz++;}
		}
		if(TimeArray[cx][cz]>0)
		{	
			
			if((cx>=currentx)&&(cz>=currentpage)) 
			{	
				FOUNDVAL=TRUE;
			}
			else
			{
				lastvalue=AnalogTable[cx][currenty][cz].fval;
			}
			cx++;
			if(cx>=14) {cx=1;cz++;}
		}
	}
	
	SetCtrlVal(panelHandle4,CTRL_PANEL_NUMINITVAL,lastvalue);
}

//***************************************************************************

int CVICALLBACK RING_CTRLMODE_CALLBACK (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
/* activated when using the control mode ring control.
Cycles between the different control modes and changes the timescale of the ramp
to an appropriate value for the mode. 
*/
{
	int ctrlmode=0;
	double timescales=0,newtime=1,Vdiff=0,ctrlvfinal;
	
	GetCtrlVal (panelHandle4,CTRL_PANEL_RING_CTRLMODE, &ctrlmode);
	GetCtrlVal (panelHandle4,CTRL_PANEL_NUMFINALVAL, &ctrlvfinal);
	timescales=TimeArray[currentx][currentpage];
	Vdiff=ctrlvfinal-AnalogTable[currentx-1][currenty][currentpage].fval;
	SetCtrlAttribute (panelHandle4, CTRL_PANEL_NUMFINALVAL, ATTR_LABEL_TEXT,"Final Value");
	SetCtrlAttribute (panelHandle4, CTRL_PANEL_NUMTIMESCALE, ATTR_LABEL_TEXT,"Time Scale");
	switch (event)
		{
		case EVENT_LEFT_CLICK:
			break;
		case EVENT_LEFT_DOUBLE_CLICK:
	
			break;
			case EVENT_LOST_FOCUS:
			
		
			case EVENT_COMMIT:
			break;
		
		}
	switch (ctrlmode)
	{
		case 1://stepwise
			SetCtrlVal (panelHandle4,CTRL_PANEL_NUMTIMESCALE, 0.0);
			break;
		case 2://linear ramp
			SetCtrlVal (panelHandle4,CTRL_PANEL_NUMTIMESCALE, timescales);
			break;
		case 3://exponential
			SetCtrlVal (panelHandle4,CTRL_PANEL_NUMTIMESCALE, timescales);
			if(!(Vdiff==0))
			{
				newtime=timescales/fabs(log(fabs(Vdiff))-log(0.001));
			//	newtime=fabs(timescales/(log(0.001/Vdiff)));
				SetCtrlVal (panelHandle4,CTRL_PANEL_NUMTIMESCALE, newtime);
			}
			break;//const. jerk
		case 4:
			SetCtrlVal (panelHandle4,CTRL_PANEL_NUMTIMESCALE,timescales);
			break;
			
		case 5:
			SetCtrlAttribute (panelHandle4, CTRL_PANEL_NUMFINALVAL, ATTR_LABEL_TEXT,"Amplitude");
		
			SetCtrlAttribute (panelHandle4, CTRL_PANEL_NUMTIMESCALE, ATTR_LABEL_TEXT,"Frequency");
			break;
	
	}
			
	return 0;
}
//***************************************************************************

int CVICALLBACK CMD_ANCANCEL_CALLBACK (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
		{
		case EVENT_COMMIT:
			 HidePanel(panelHandle4);
			break;
		}
	return 0;
}


