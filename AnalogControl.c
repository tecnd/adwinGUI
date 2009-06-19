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
Clicking on Set Value, reads the values from the control and stores them in the AnalogTable

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
	int itemp=0,cmode,next_fcn,chk_page;				// variables for reading/writing the control mode
	int docheck=0,valuesgood=1;		// booleans.  docheck=1 if we need to check the time settings
									// valuesgood=1 if okay to write data to AnalogTable array..sets to 0 if prompt denied
	double dtemp,ttemp,usedtimescale; //usedtimescale=timescale written in AnalogTable to control ramping, exponential etc.
	double timematrixlength;		// duration of the current column.
	double max,min;					// max and min acceptable values used for coercive range checking
	Point pval;
	switch (event)
		{
								 
		case EVENT_COMMIT:
			valuesgood=0;
			
			//Retrieve Control/Data Values from Panel
			GetCtrlVal (panelHandle4,CTRL_PANEL_RING_CTRLMODE, &itemp);
			cmode=itemp;
			GetCtrlVal (panelHandle4,CTRL_PANEL_NUMFINALVAL, &dtemp);
			GetCtrlVal (panelHandle4,CTRL_PANEL_NUMTIMESCALE, &ttemp);
			usedtimescale=dtemp;
			AnalogTable[currentx][currenty][currentpage].fcn=itemp;
			
			if(itemp!=6)
			{
				AnalogTable[currentx][currenty][currentpage].fval=dtemp;			
				AnalogTable[currentx][currenty][currentpage].tscale=ttemp;
			}
		
			
			max=(AChName[currenty].maxvolts-AChName[currenty].tbias)/AChName[currenty].tfcn;
			min=(AChName[currenty].minvolts-AChName[currenty].tbias)/AChName[currenty].tfcn;
			GetCtrlVal (panelHandle4,CTRL_PANEL_NUMFINALVAL, &dtemp);
			if(dtemp>max)
				SetCtrlVal (panelHandle4,CTRL_PANEL_NUMFINALVAL, max);  	
			else if (dtemp<min)
				SetCtrlVal (panelHandle4,CTRL_PANEL_NUMFINALVAL, min); 
			else
			{
				AnalogTable[currentx][currenty][currentpage].fval=dtemp;			
				valuesgood=1;
			}
			
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
			
			// Code below used to allow ramps to have shorter time constants... basically disabled now
			if (docheck==1)
			{
				//valuesgood=ConfirmPopup("Overlong array selected","Do you want to overwrite the following array?");	
				//valuesgood=1;  // just assume timing is right.
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
	// Sets all relevant info into the DDS Control Panel based on the current cell selection
	
	SetCtrlVal(panelHandle4,CTRL_PANEL_STRUNITS, AChName[currenty].units);
	SetCtrlVal(panelHandle4,CTRL_PANEL_STR_CHNAME,AChName[currenty].chname);
	SetCtrlVal (panelHandle4,CTRL_PANEL_RING_CTRLMODE,AnalogTable[currentx][currenty][currentpage].fcn);
	
	if(AnalogTable[currentx][currenty][currentpage].fcn!=6)
		SetCtrlVal (panelHandle4,CTRL_PANEL_NUMFINALVAL, AnalogTable[currentx][currenty][currentpage].fval);
	else
		SetCtrlVal (panelHandle4,CTRL_PANEL_NUMFINALVAL,findLastVal(currenty,currentx, currentpage));	
	
	SetCtrlVal (panelHandle4,CTRL_PANEL_NUMTIMESCALE, AnalogTable[currentx][currenty][currentpage].tscale);
	SetCtrlVal(panelHandle4,CTRL_PANEL_NUMINITVAL,findLastVal(currenty,currentx, currentpage));
}

//***************************************************************************

int CVICALLBACK RING_CTRLMODE_CALLBACK (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
/* activated when using the control mode ring control.
Cycles between the different control modes and changes the timescale of the ramp
to an appropriate value for the mode. 
If adding another function, just add another case, and modify the ring control on the 
AnalogControl.uir file, and add function handling in CalcFcnVal() in GUIDesign.c

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
			
		case 5: //S
			SetCtrlAttribute (panelHandle4, CTRL_PANEL_NUMFINALVAL, ATTR_LABEL_TEXT,"Amplitude");
			SetCtrlAttribute (panelHandle4, CTRL_PANEL_NUMTIMESCALE, ATTR_LABEL_TEXT,"Frequency");
			break;
	
	}
			
	return 0;
}
//***************************************************************************

int CVICALLBACK CMD_ANCANCEL_CALLBACK (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
// Cancel and don't make any changes.  
{
	switch (event)
		{
		case EVENT_COMMIT:
			 HidePanel(panelHandle4);
			break;
		}
	return 0;
}


