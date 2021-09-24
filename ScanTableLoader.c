#include <userint.h>
#include "vars.h"
#include <ansi_c.h>
#include <cvirte.h>
#include "ScanTableLoader.h"
#include "ScanTableLoader2.h"
#include "GUIDesign.h"


int CVICALLBACK SCAN_LOAD_CANCEL (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
		{
		case EVENT_COMMIT:
			HidePanel(panelHandle8);

			break;
		}
	return 0;
}

int CVICALLBACK SCAN_LOAD_OK (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	//Loads Scan Tables Based on Selections made in the ScanTableLoader Panel

	int steps=0,mode;
	double first,last;


	switch (event)
		{
		case EVENT_COMMIT:

			GetCtrlVal (panelHandle8,SL_PANEL_SCAN_TYPE, &mode);
			GetCtrlVal (panelHandle8,SL_PANEL_ITER_NUM, &steps);
			GetCtrlVal (panelHandle8,SL_PANEL_SCAN_INIT, &first);
			GetCtrlVal (panelHandle8,SL_PANEL_SCAN_FIN, &last);

			if (steps>30||steps<1)
			{
				ConfirmPopup("USER ERROR","# of Steps must be between 1-30");
				HidePanel(panelHandle8);
			}
			else
			{

				switch(mode)
				{
					case 1:
					LoadLinearRamp(steps,first,last);
					break;
					case 2:
					LoadExpRamp(steps,first,last);
					break;

				}
				HidePanel(panelHandle8);
			}

				break;
			}
	return 0;
}

 void LoadLinearRamp(int steps,double first,double last)
 {
 	//Loads a linear ramp into the scan table with first and last values in steps

 	int i;
 	double slope;

 	slope = (double)((last-first)/steps);

 	for(i=0;i<=steps;i++)
 		SetTableCellVal(panelHandle, PANEL_SCAN_TABLE, MakePoint(1, i+1), slope*i);

 	SetTableCellVal(panelHandle, PANEL_SCAN_TABLE, MakePoint(1, steps+2), -1000.0);

 }

 void LoadExpRamp(int steps,double first,double last)
 {
    //Loads an exponential ramp into the scan table with first and last values in steps
    //Creates Values Following Form f(x)=last-amplitude*exp(bx)
    //+/- 1% Accuracy

    int i;
    double amplitude,b;

    amplitude = last-first;

    b=log(0.01)/steps;

    for (i=0;i<steps;i++)
    	SetTableCellVal(panelHandle, PANEL_SCAN_TABLE, MakePoint(1, i+1), last-amplitude*exp(b*(double)i));

    SetTableCellVal(panelHandle, PANEL_SCAN_TABLE, MakePoint(1, steps+1), last);
    SetTableCellVal(panelHandle, PANEL_SCAN_TABLE, MakePoint(1, steps+2), -1000.0);

 }


