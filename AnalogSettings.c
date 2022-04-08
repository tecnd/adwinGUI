#include "vars.h"
#include "GUIDesign.h"

//************************************************************************
// Update the main panel to display new values in the channel listing
void SetAnalogChannels()
{
	for(int i = 1; i <= NUMBERANALOGCHANNELS; i++)
	{

		SetTableCellAttribute(panelHandle, PANEL_TBL_ANAMES, MakePoint(1,i), ATTR_CTRL_VAL, AChName[i].chname);
		SetTableCellAttribute(panelHandle, PANEL_TBL_ANAMES, MakePoint(2,i), ATTR_CTRL_VAL, AChName[i].chnum);
		SetTableCellAttribute(panelHandle, PANEL_TBL_ANALOGUNITS, MakePoint(1,i), ATTR_CTRL_VAL, AChName[i].units);
	}
}

