#include "vars.h"
#include "GUIDesign.h"

//********************************************************************************************
void SetDigitalChannels(void)
/*
Set the digital channel list (on main panel) with the information contained in the DChName array
*/
{
	int i = 0;
	for (i = 1; i <= NUMBERDIGITALCHANNELS; i++)
	{
		SetTableCellVal(panelHandle, PANEL_TBL_DIGNAMES, MakePoint(1, i), DChName[i].chname);
		SetTableCellVal(panelHandle, PANEL_TBL_DIGNAMES, MakePoint(2, i), DChName[i].chnum);
	}
}
