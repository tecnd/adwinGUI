#include <userint.h>
#include "AnalogSettings.h"
#include "main.h"
//#include "AnalogSettings.h"
//#include <userint.h>
//#include <ansi_c.h>
//#define ALLOC_GLOBALS
//#include <ansi_c.h>

//#include <cvirte.h>		
//#include <userint.h>
//#include "GUIDesign.c"

// Calling globals from main.c
extern int panelHandle,panelHandle2;
extern  struct AnalogTableValues{
	int		fcn;		//fcn is an integer refering to a function to use.
						// 0-step, 1-linear, 2- exp, 3- 'S' curve
	double 	fval;		//the final value
	double	tscale;		//the timescale to approach final value
	} AnalogTable[17][17][10];
	
extern struct AnalogChannelProperties{
	int		chnum;		// channel number 1-8 DAC1	9-16 DAC2
	char    chname[50]; // name to appear on the panel
	char	units[50];
	double  tfcn;		// Transfer function.  i.e. 10V out = t G/cm etc...
	}  AChName[16];

//************************************************************************
int CVICALLBACK CMD_ALLOWCHANGE_CALLBACK (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	SetCtrlAttribute (panelHandle2, ANLGPANEL_CMD_SETCHANGES,   ATTR_VISIBLE, 1);
	switch (event)
		{
		case EVENT_COMMIT:
			
			SetCtrlAttribute (panelHandle2, ANLGPANEL_NUM_ACHANNEL, ATTR_CTRL_MODE, VAL_HOT);
			SetCtrlAttribute (panelHandle2, ANLGPANEL_STR_CHANNELNAME, ATTR_CTRL_MODE, VAL_HOT);
			SetCtrlAttribute (panelHandle2, ANLGPANEL_STRING_UNITS, ATTR_CTRL_MODE, VAL_HOT);
			SetCtrlAttribute (panelHandle2, ANLGPANEL_NUM_CHANNELPROP, ATTR_CTRL_MODE, VAL_HOT);
			
			break;
		}
	return 0;
}
//************************************************************************

int CVICALLBACK CMD_SETCHANGES_CALLBACK (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{   
	char buff[51]="",buff2[51]="";
	int channel=0,line=0;
	double prop=0;
	switch (event)
		{
		case EVENT_COMMIT:
		
			GetCtrlVal (panelHandle2, ANLGPANEL_NUM_ACHANNEL, &channel);
			GetCtrlVal (panelHandle2, ANLGPANEL_NUM_ACH_LINE, &line);
			GetCtrlVal (panelHandle2, ANLGPANEL_NUM_CHANNELPROP, &prop);
			GetCtrlVal (panelHandle2, ANLGPANEL_STR_CHANNELNAME, buff);
			GetCtrlVal (panelHandle2, ANLGPANEL_STRING_UNITS, buff2);
			AChName[line].chnum=channel;
			AChName[line].tfcn=prop;
			sprintf(AChName[line].chname,buff);
			sprintf(AChName[line].units,buff2);
			SetAnalogChannels();
		//	HidePanel(panelHandle2);
			//Next we will update the channel list and redisplay ;)
			break;
		}
	return 0;
}
//************************************************************************

int CVICALLBACK CMD_DONEANALOG_CALLBACK (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
																		 
	switch (event)
		{
		case EVENT_COMMIT:
			SetCtrlAttribute (panelHandle2, ANLGPANEL_NUM_ACHANNEL, ATTR_CTRL_MODE, VAL_INDICATOR);
			SetCtrlAttribute (panelHandle2, ANLGPANEL_STR_CHANNELNAME, ATTR_CTRL_MODE, VAL_INDICATOR);
			SetCtrlAttribute (panelHandle2, ANLGPANEL_STRING_UNITS, ATTR_CTRL_MODE, VAL_INDICATOR);
			SetCtrlAttribute (panelHandle2, ANLGPANEL_NUM_CHANNELPROP, ATTR_CTRL_MODE, VAL_INDICATOR);
			SetCtrlAttribute (panelHandle2, ANLGPANEL_CMD_SETCHANGES,   ATTR_VISIBLE, 0);
			HidePanel(panelHandle2);
			break;
		}
	return 0;
}
//************************************************************************

void SetAnalogChannels()
{
int i=0,j=0,k=0,line=0;
char numbuff[20]="";
for(i=1;i<=16;i++)
{
	ReplaceListItem (panelHandle, PANEL_LB_ANAMES, i-1, AChName[i].chname,AChName[i].chnum);
	sprintf(numbuff,"%d",AChName[i].chnum);
	ReplaceListItem (panelHandle, PANEL_LB_ACHANNELS,i-1, numbuff ,AChName[i].chnum);
	ReplaceListItem (panelHandle, PANEL_LB_AUNITS,i-1, AChName[i].units ,AChName[i].chnum);
	
}
}
//************************************************************************
 void ReadAnalogChannels()
{
int i=0,j=0,k=0,line=0;
char buff[50]="";
for(i=1;i<=16;i++)
{
	GetLabelFromIndex (panelHandle, PANEL_LB_ANAMES, i-1, buff);
	sprintf(AChName[i].chname,buff);	
	GetLabelFromIndex (panelHandle, PANEL_LB_ACHANNELS, i-1, buff);
	AChName[i].chnum=atoi(buff);
	GetLabelFromIndex (panelHandle, PANEL_LB_AUNITS, i-1, buff);
	sprintf(AChName[i].units,buff);
	//ReplaceListItem (panelHandle, PANEL_LB_ANAMES, i-1, AChName[i].chname,AChName[i].chnum);
	//sprintf(numbuff,"%d",AChName[i].chnum);
	//ReplaceListItem (panelHandle, PANEL_LB_ACHANNELS,i-1, numbuff ,AChName[i].chnum);
	//ReplaceListItem (panelHandle, PANEL_LB_AUNITS,i-1, AChName[i].units ,AChName[i].chnum);
	
}
}
//********************************************************************
int CVICALLBACK NUM_ACH_LINE_CALLBACK (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	int line=0;
	switch (event)
		{
		case EVENT_COMMIT:
			
			GetCtrlVal (panelHandle2, ANLGPANEL_NUM_ACH_LINE, &line);
			
			SetCtrlVal (panelHandle2, ANLGPANEL_NUM_ACHANNEL, AChName[line].chnum);
//			SetCtrlVal (panelHandle2, ANLGPANEL_NUM_ACH_LINE, &line);
			SetCtrlVal (panelHandle2, ANLGPANEL_NUM_CHANNELPROP,AChName[line].tfcn );
			SetCtrlVal (panelHandle2, ANLGPANEL_STR_CHANNELNAME, AChName[line].chname);
			SetCtrlVal (panelHandle2, ANLGPANEL_STRING_UNITS, AChName[line].units);
			break;
		}
	return 0;
}
