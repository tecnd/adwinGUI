#include <userint.h>
#include "Oscilloscope.h"


int CVICALLBACK PANEL_CALLBACK (int panel, int event, void *callbackData,
								int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_CLOSE:
			HidePanel(GetActivePanel());
			break;
	}
	return 0;
}
