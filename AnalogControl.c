#include <userint.h>
#include <ansi_c.h>
#include <toolbox.h>
#include "vars.h"
#include "GUIDesign2.h"
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
int CVICALLBACK CMD_SETANALAOG_CALLBACK(int panel, int control, int event,
										void *callbackData, int eventData1, int eventData2)
/* Panel button to set analog data.
Reads data off of the panel controls, ringbox and numberboxes.
Writes data to the AnalogTable array
NOT DONE:  IF TIMESCALE IS TOO LONG THEN OVERWRITE THE FOLLOWING CELL
*/
{
	int itemp = 0; // variables for reading/writing the control mode
	double dtemp, ttemp;
	switch (event)
	{

	case EVENT_COMMIT:
		//Retrieve Control/Data Values from Panel
		GetCtrlVal(panelHandle4, CTRL_PANEL_RING_CTRLMODE, &itemp);
		GetCtrlVal(panelHandle4, CTRL_PANEL_NUMFINALVAL, &dtemp);
		GetCtrlVal(panelHandle4, CTRL_PANEL_NUMTIMESCALE, &ttemp);
		AnalogTable[currentx][currenty][currentpage].fcn = itemp;

		if (itemp != 6)
		{
			AnalogTable[currentx][currenty][currentpage].fval = dtemp;
			AnalogTable[currentx][currenty][currentpage].tscale = ttemp;
		}

		GetCtrlVal(panelHandle4, CTRL_PANEL_NUMFINALVAL, &dtemp);
		AnalogTable[currentx][currenty][currentpage].fval = dtemp;

		GetCtrlVal(panelHandle4, CTRL_PANEL_NUMTIMESCALE, &dtemp);
		AnalogTable[currentx][currenty][currentpage].tscale = dtemp;

		HidePanel(panelHandle4);
		DrawNewTable(isdimmed);
		break;
	}
	return 0;
}
//***************************************************************************
void SetControlPanel(void)
{
	// Reads the value of the last active column before this one and displays it on the panel.
	double lastvalue;
	BOOL FOUNDVAL = 0;
	int cx, cz;

	SetCtrlVal(panelHandle4, CTRL_PANEL_STRUNITS, AChName[currenty].units);
	SetCtrlVal(panelHandle4, CTRL_PANEL_STR_CHNAME, AChName[currenty].chname);
	SetCtrlVal(panelHandle4, CTRL_PANEL_NUMFINALVAL, AnalogTable[currentx][currenty][currentpage].fval);
	SetCtrlVal(panelHandle4, CTRL_PANEL_NUMTIMESCALE, AnalogTable[currentx][currenty][currentpage].tscale);
	SetCtrlVal(panelHandle4, CTRL_PANEL_RING_CTRLMODE, AnalogTable[currentx][currenty][currentpage].fcn);

	// find the last value
	cx = 1;
	cz = 1;
	lastvalue = 0;
	while (!FOUNDVAL)
	{
		if (cz > NUMBEROFPAGES)
		{
			break; // didn't find it, end the loop
		}
		if (TimeArray[cx][cz] == 0)
		{
			cz++; // if you see a 0 time, go to next page
			cx = 1;
		}
		else if (TimeArray[cx][cz] < 0) // if you see a negative time, skip to next value
		{
			cx++;
			if (cx >= NUMBEROFCOLUMNS)
			{
				cx = 1;
				cz++;
			}
		}
		else // if positive time, record current val
		{
			if ((cx >= currentx) && (cz >= currentpage))
			{
				FOUNDVAL = TRUE; // found current value
			}
			else
			{
				lastvalue = AnalogTable[cx][currenty][cz].fval;
			}
			cx++;
			if (cx >= NUMBEROFCOLUMNS)
			{
				cx = 1;
				cz++;
			}
		}
	}

	SetCtrlVal(panelHandle4, CTRL_PANEL_NUMINITVAL, lastvalue);
}

//***************************************************************************

int CVICALLBACK RING_CTRLMODE_CALLBACK(int panel, int control, int event,
									   void *callbackData, int eventData1, int eventData2)
/* activated when using the control mode ring control.
Cycles between the different control modes and changes the timescale of the ramp
to an appropriate value for the mode.
If adding another function, just add another case, and modify the ring control on the
AnalogControl.uir file, and add function handling in CalcFcnVal() in GUIDesign.c

*/
{
	int ctrlmode = 0;
	double timescales = 0, ctrlvfinal;

	GetCtrlVal(panelHandle4, CTRL_PANEL_RING_CTRLMODE, &ctrlmode);
	GetCtrlVal(panelHandle4, CTRL_PANEL_NUMFINALVAL, &ctrlvfinal);
	timescales = TimeArray[currentx][currentpage];
	double Vdiff = ctrlvfinal - AnalogTable[currentx - 1][currenty][currentpage].fval;
	SetCtrlAttribute(panelHandle4, CTRL_PANEL_NUMFINALVAL, ATTR_LABEL_TEXT, "Final Value");
	SetCtrlAttribute(panelHandle4, CTRL_PANEL_NUMTIMESCALE, ATTR_LABEL_TEXT, "Time Scale");
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
	case 1: //stepwise
		SetCtrlVal(panelHandle4, CTRL_PANEL_NUMTIMESCALE, 0.0);
		break;
	case 2: //linear ramp
		SetCtrlVal(panelHandle4, CTRL_PANEL_NUMTIMESCALE, timescales);
		break;
	case 3: //exponential
		SetCtrlVal(panelHandle4, CTRL_PANEL_NUMTIMESCALE, timescales);
		if (FloatCompare(&Vdiff, &(float){0.}) != 0)
		{
			double newtime = timescales / fabs(log(fabs(Vdiff)) - log(0.001));
			//	newtime=fabs(timescales/(log(0.001/Vdiff)));
			SetCtrlVal(panelHandle4, CTRL_PANEL_NUMTIMESCALE, newtime);
		}
		break; //const. jerk
	case 4:
		SetCtrlVal(panelHandle4, CTRL_PANEL_NUMTIMESCALE, timescales);
		break;

	case 5: //S
		SetCtrlAttribute(panelHandle4, CTRL_PANEL_NUMFINALVAL, ATTR_LABEL_TEXT, "Amplitude");
		SetCtrlAttribute(panelHandle4, CTRL_PANEL_NUMTIMESCALE, ATTR_LABEL_TEXT, "Frequency");
		break;
	}

	return 0;
}
//***************************************************************************

int CVICALLBACK CMD_ANCANCEL_CALLBACK(int panel, int control, int event,
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
