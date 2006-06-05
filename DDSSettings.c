
#include "DDSSettings.h"
#include "DDSSettings2.h"

/*
Allows us to change the internal clock frequency of the DDS.
Really not needed.
*/
int CVICALLBACK SETFREQ_CALLBACK (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	
	switch (event)
		{
		case EVENT_COMMIT:
			SaveDDSSettings();
			LoadDDSSettings();
			break;
		}
	return 0;
}

int CVICALLBACK SETPLL_CALLBACK (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{   
	switch (event)
		{
		case EVENT_COMMIT:
			SaveDDSSettings();
			LoadDDSSettings();
			break;
		}
	return 0;
}

int CVICALLBACK DDSDONE_CALLBACK (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	
	
	switch (event)
		{
		case EVENT_COMMIT:
			
			SaveDDSSettings();
			HidePanel(panelHandle5);
			break;
		}
	return 0;
}


void LoadDDSSettings(void)
{

	SetCtrlVal (panelHandle5, DDSPANEL_NUM_DDSCLOCK,DDSFreq.clock);
	SetCtrlVal (panelHandle5, DDSPANEL_NUM_PLLMULTIPLIER,DDSFreq.PLLmult);
	SetCtrlVal (panelHandle5, DDSPANEL_NUM_DDSBASEFREQ,DDSFreq.extclock);
	SetCtrlVal (panelHandle5, DDSPANEL_NUM_DDS2_CLOCK,DDS2_CLOCK);
	SetCtrlVal (panelHandle5, DDSPANEL_NUM_DDS3_CLOCK,DDS3CLOCK);
	return;
	
}

void SaveDDSSettings(void)
{

	int temp1;
	double temp2;
	
	GetCtrlVal (panelHandle5, DDSPANEL_NUM_PLLMULTIPLIER, &temp1);
	GetCtrlVal (panelHandle5, DDSPANEL_NUM_DDSBASEFREQ, &temp2);
	
	DDSFreq.extclock = temp2;
	DDSFreq.PLLmult = temp1;
	DDSFreq.clock = DDSFreq.extclock * DDSFreq.PLLmult;
	
	return;
	
}
