#include <userint.h>
#include "vars.h"
#include "Oscilloscope.h"


int CVICALLBACK PANEL_CALLBACK (int panel, int event, void *callbackData,
								int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_GOT_FOCUS:
			int xArr[] = {0, 1, 2};
			int yArr[] = {0, -1, 1};
			PlotXY(oscilloscopeHandle, PANEL_GRAPH, xArr, yArr, 3, VAL_INTEGER, VAL_INTEGER, VAL_THIN_STEP, VAL_NO_POINT, VAL_SOLID, 1, VAL_RED);
			break;
		case EVENT_CLOSE:
			HidePanel(oscilloscopeHandle);
			break;
	}
	return 0;
}
