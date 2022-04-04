#include "vars.h"
#include "GUIDesign.h"

//************************************************************************
// Update the main panel to display new values in the channel listing
void SetAnalogChannels()
{
	int i=0;
	for(i=1;i<=NUMBERANALOGCHANNELS;i++)
	{

		SetTableCellVal (panelHandle, PANEL_TBL_ANAMES, MakePoint(1,i), AChName[i].chname);
		SetTableCellVal (panelHandle, PANEL_TBL_ANAMES, MakePoint(2,i),AChName[i].chnum);
		SetTableCellVal (panelHandle, PANEL_TBL_ANALOGUNITS, MakePoint(1,i), AChName[i].units);
	}
}

