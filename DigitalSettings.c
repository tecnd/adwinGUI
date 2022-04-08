#include "vars.h"
#include "GUIDesign.h"

//********************************************************************************************
void SetDigitalChannels(void)
/*
Set the digital channel list (on main panel) with the information contained in the DChName array
*/
{
	for (int i = 1; i <= NUMBERDIGITALCHANNELS; i++)
	{
		SetTableCellAttribute(panelHandle, PANEL_TBL_DIGNAMES, MakePoint(1, i), ATTR_CTRL_VAL, DChName[i].chname);
		SetTableCellAttribute(panelHandle, PANEL_TBL_DIGNAMES, MakePoint(2, i), ATTR_CTRL_VAL, DChName[i].chnum);
	}
}
