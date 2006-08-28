#include <ansi_c.h>
#include <cvirte.h>		
#include <userint.h>
#include "ScanTableLoader.h"
#include "ScanTableLoader2.h" 
#include "vars.h"
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
	
	int steps=0,mode,iters,STCELLNUMS;
	double first,last;
	char buff[50]="";
	char buff2[200]="";
	
	
	switch (event)
		{
		case EVENT_COMMIT:
		
			GetNumTableRows (panelHandle,PANEL_SCAN_TABLE,&STCELLNUMS);
			GetCtrlVal (panelHandle8,SL_PANEL_SCAN_TYPE, &mode);
			GetCtrlVal (panelHandle8,SL_PANEL_STEP_NUM, &steps);
			GetCtrlVal (panelHandle8,SL_PANEL_SCAN_INIT, &first);
			GetCtrlVal (panelHandle8,SL_PANEL_SCAN_FIN, &last);
			GetCtrlVal (panelHandle8,SL_PANEL_SCAN_ITER, &iters);
				
			if ((steps+1)*iters+1>STCELLNUMS||steps<1)
			{
				sprintf(buff,"%d",STCELLNUMS-1);
				strcat(buff2,"# of (Steps+1)*Iters/Step must be between 1-");
				strcat(buff2,buff);
				
				ConfirmPopup("USER ERROR",buff2);
				HidePanel(panelHandle8);
			}
			else if(iters<1)
			{
				ConfirmPopup("USER ERROR","Number of iterations must be greater than 1");
				HidePanel(panelHandle8);
			}
			else
			{
				
				switch(mode)
				{
					case 1:
					LoadLinearRamp(steps,first,last,iters,STCELLNUMS);
					break;
					case 2:
					LoadExpRamp(steps,first,last,iters,STCELLNUMS);
					break;
					
				}
				HidePanel(panelHandle8);
			}
			
				break;
			}
	return 0;
}

 void LoadLinearRamp(int steps,double first,double last,int iter,int STCELLNUMS)
 {
 	//Loads a linear ramp into the scan table with first and last values in steps
    
 	int i,j;
 	double slope;
 	
 	slope = (double)((last-first)/steps);
 	
 	for(i=0;i<=steps;i++)
 	{
 		
 		for(j=0;j<iter;j++)
 		{
 			SetTableCellVal(panelHandle, PANEL_SCAN_TABLE, MakePoint(1, i*iter+j+1), slope*i+first);
 			//printf("Cell: %d\tNum: %f\n",i*iter+j+1,slope*i+first); testing
 		}
 	
 	}
 	
 	SetTableCellVal(panelHandle, PANEL_SCAN_TABLE, MakePoint(1, (steps+1)*iter+1), -1000.0);
 	
 	for (i=(steps+1)*iter+2;i<=STCELLNUMS;i++)
 		SetTableCellVal(panelHandle, PANEL_SCAN_TABLE, MakePoint(1, i),0.0);
 	
 }
 
 void LoadExpRamp(int steps,double first,double last,int iter,int STCELLNUMS)
 {
    //Loads an exponential ramp into the scan table with first and last values in steps
    //Creates Values Following Form f(x)=last-amplitude*exp(bx)
    //+/- 1% Accuracy 
    
    int i,j;
    double amplitude,b;
    
    amplitude = last-first;
    
    b=log(0.01)/steps;
    	
    for (i=0;i<steps;i++)
    {
    	for(j=0;j<iter;j++)
    		SetTableCellVal(panelHandle, PANEL_SCAN_TABLE, MakePoint(1, i*iter+j+1), last-amplitude*exp(b*(double)i));
    }
    
    for(j=1;j<=iter;j++)
    	SetTableCellVal(panelHandle, PANEL_SCAN_TABLE, MakePoint(1, steps*iter+j), last);
    
    SetTableCellVal(panelHandle, PANEL_SCAN_TABLE, MakePoint(1, (steps+1)*iter+1), -1000.0);
    
    for (i=(steps+1)*iter+2;i<=STCELLNUMS;i++)
 		SetTableCellVal(panelHandle, PANEL_SCAN_TABLE, MakePoint(1, i),0.0);
    
 }
 

