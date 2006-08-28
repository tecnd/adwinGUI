#include <cvirte.h>		
#include <userint.h>
#include "NumSet.h"
#include "vars.h"
#include "GUIDesign.h"

//Added by David Burns 22 June, 2006
//Currently, Numset is used to allow the User to alter the number of cells in the ScanTable
//The Panel was designed to allow the user to enter an integer in general. This could be adapted to be used for 
//multiple purposes if needed.

int CVICALLBACK NumSet_Ok (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	int currentnum,setnum,diff;
	
	switch (event)
		{
		case EVENT_COMMIT:
			GetNumTableRows (panelHandle,PANEL_SCAN_TABLE,&currentnum);
			GetCtrlVal (panelHandle9,NS_PANEL_SETNUM, &setnum);
			diff=setnum-currentnum;
			if(diff>=0)
				InsertTableRows (panelHandle, PANEL_SCAN_TABLE, -1, diff, VAL_CELL_NUMERIC); //adds diff rows to end of table
			else
				DeleteTableRows(panelHandle, PANEL_SCAN_TABLE, currentnum+diff+1, -1); //deletes diff rows from bottom of table
			
			HidePanel(panelHandle9);
			break;
			}
	return 0;
}

int CVICALLBACK NumSet_Cancel (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
		{
		case EVENT_COMMIT:
			HidePanel(panelHandle9);
			break;
		}
	return 0;
}
