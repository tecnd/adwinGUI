#include <userint.h>
#include "DigitalSettings.h"
#include "main.h"
extern int	panelHandle,panelHandle3;
extern struct DigitalChannelProperties{
	int		chnum;		// digital line to control
	char 	chname[50];	// name of the channel on the panel
	}	DChName[33];
//********************************************************************************************
int CVICALLBACK NUM_DIG_LINE_CALLBACK (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	int line=0;
	switch (event)
		{
		case EVENT_COMMIT:
			GetCtrlVal (panelHandle3, DIGPANEL_NUM_DIGCH_LINE, &line);
			
			SetCtrlVal (panelHandle3, DIGPANEL_NUM_DIGCHANNEL, DChName[line].chnum);
		//	SetCtrlVal (panelHandle3, DIGPANEL_NUM_DIGCH_LINE, &line);
			SetCtrlVal (panelHandle3, DIGPANEL_STR_DIGCHANNELNAME, DChName[line].chname);
			break;
		}
	return 0;
}
//********************************************************************************************
int CVICALLBACK CMD_DIGALLOWCHANGE_CALLBACK (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	SetCtrlAttribute (panelHandle3, DIGPANEL_CMD_DIGSETCHANGES,   ATTR_VISIBLE, 1);
	switch (event)
		{
		case EVENT_COMMIT:
			
			SetCtrlAttribute (panelHandle3, DIGPANEL_NUM_DIGCHANNEL, ATTR_CTRL_MODE, VAL_HOT);
			SetCtrlAttribute (panelHandle3, DIGPANEL_STR_DIGCHANNELNAME, ATTR_CTRL_MODE, VAL_HOT);
			break;
		}
	return 0;
}
//********************************************************************************************
int CVICALLBACK CMD_DIGSETCHANGES_CALLBACK (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{	
	int channel=0,line=0;
	char buff[50];
	switch (event)
		{
		case EVENT_COMMIT:
			GetCtrlVal (panelHandle3, DIGPANEL_NUM_DIGCHANNEL, &channel);
			GetCtrlVal (panelHandle3, DIGPANEL_NUM_DIGCH_LINE, &line);
			GetCtrlVal (panelHandle3, DIGPANEL_STR_DIGCHANNELNAME, buff);
			DChName[line].chnum=channel;
			sprintf(DChName[line].chname,buff);
			SetDigitalChannels();
			break;
		}
	return 0;
}
//********************************************************************************************
int CVICALLBACK CMD_DONEDIG_CALLBACK (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
		{
		case EVENT_COMMIT:
			SetCtrlAttribute (panelHandle3, DIGPANEL_NUM_DIGCHANNEL, ATTR_CTRL_MODE, VAL_INDICATOR);
			SetCtrlAttribute (panelHandle3, DIGPANEL_STR_DIGCHANNELNAME, ATTR_CTRL_MODE, VAL_INDICATOR);
			SetCtrlAttribute (panelHandle3, DIGPANEL_CMD_DIGSETCHANGES,   ATTR_VISIBLE, 0);
			HidePanel(panelHandle3);
			break;
		}
	return 0;
}
//********************************************************************************************



void SetDigitalChannels(void)
{
	int i=0,j=0,k=0,line=0;
	char numbuff[20]="";
	for(i=1;i<=16;i++)
	{
		ReplaceListItem (panelHandle, PANEL_LB_DIGNAMES, i-1, DChName[i].chname,DChName[i].chnum);
		sprintf(numbuff,"%d",DChName[i].chnum);
		ReplaceListItem (panelHandle, PANEL_LB_DIGLINES, i-1, numbuff, DChName[i].chnum);
	}
}

//********************************************************************************************
void ReadDigitalChannels(void)
{
	int i=0,j=0,k=0,line=0;
    char buff[50]="";
    for(i=1;i<=16;i++)
    {
	    GetLabelFromIndex (panelHandle, PANEL_LB_DIGNAMES, i-1, buff);
	    sprintf(DChName[i].chname,buff);	
    	GetLabelFromIndex (panelHandle, PANEL_LB_DIGLINES, i-1, buff);
   		DChName[i].chnum=atoi(buff);
    }
}

