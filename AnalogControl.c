#include <userint.h>
#include "AnalogControl.h"
#include "main.h"
//Info on the panel
//PanelName		PANEL_CTRL
//Control Mode listbox				CTRL_PANEL_LB_CTRLMODE
//Final Voltage NUmeric control		CTRL_PANEL_NUMFINALVAL
// Units indicator					CTRL_PANEL_STRUNITS
//Timescale	numeric control			CTRL_PANEL_NUMTIMESCALE
//Command button to set values		CTRL_PANEL_CMD_SETANALOG 

extern int panelHandle4,panelHandle;
extern struct AnalogTableValues{
	int		fcn;		//fcn is an integer refering to a function to use.// 0-step, 1-linear, 2- exp, 3- 'S' curve
	double 	fval;		//the final value
	double	tscale;		//the timescale to approach final value
	} AnalogTable[17][17][10];  // read as x,y,page
	
extern struct AnalogChannelProperties{
	int		chnum;		// channel number 1-8 DAC1	9-16 DAC2
	char    chname[50]; // name to appear on the panel
	char	units[50];
	double  tfcn;		// Transfer function.  i.e. 10V out = t G/cm etc...
	}  AChName[16];	

extern double TimeArray[17][10];
extern int isdimmed;	
extern int currentpage,currentx,currenty;

//***********************************************************************
int CVICALLBACK CMD_SETANALAOG_CALLBACK (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	int itemp=0,cmode,docheck=0,valuesgood=1;
	double dtemp=0.0,usedtimescale,timematrixlength;
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
			if(cmode=0) {docheck=0;}
			
			
			if (docheck==1)
			{
				valuesgood=ConfirmPopup("Overlong array selected","Do you want to overwrite the following array?");	
				if(valuesgood==1)
				{
				 //code to update the 2 columns
				
				}
			}
			if(valuesgood==1) 
			{
				HidePanel(panelHandle4);
				DrawNewTable(isdimmed);
				break;
			}
			
		}
	return 0;
}
//***************************************************************************
void SetControlPanel(void)
{
	SetCtrlVal(panelHandle4,CTRL_PANEL_STRUNITS, AChName[currenty].units);
	SetCtrlVal(panelHandle4,CTRL_PANEL_STR_CHNAME,AChName[currenty].chname);
	SetCtrlVal (panelHandle4,CTRL_PANEL_NUMFINALVAL, AnalogTable[currentx][currenty][currentpage].fval);
	SetCtrlVal (panelHandle4,CTRL_PANEL_NUMTIMESCALE, AnalogTable[currentx][currenty][currentpage].tscale);
	SetCtrlVal (panelHandle4,CTRL_PANEL_RING_CTRLMODE,AnalogTable[currentx][currenty][currentpage].fcn);
	SetCtrlVal(panelHandle4,CTRL_PANEL_NUMINITVAL,AnalogTable[currentx-1][currenty][currentpage].fval);
}

//***************************************************************************

int CVICALLBACK RING_CTRLMODE_CALLBACK (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	int ctrlmode=0;
	double timescales=0,newtime=1,Vdiff=0,ctrlvfinal;
	GetCtrlVal (panelHandle4,CTRL_PANEL_RING_CTRLMODE, &ctrlmode);
	GetCtrlVal (panelHandle4,CTRL_PANEL_NUMFINALVAL, &ctrlvfinal);
	timescales=TimeArray[currentx][currentpage];
	Vdiff=ctrlvfinal-AnalogTable[currentx-1][currenty][currentpage].fval;
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
			}
	return 0;
}
//***************************************************************************
