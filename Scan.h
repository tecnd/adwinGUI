/**************************************************************************/
/* LabWindows/CVI User Interface Resource (UIR) Include File              */
/*                                                                        */
/* WARNING: Do not add to, delete from, or otherwise modify the contents  */
/*          of this include file.                                         */
/**************************************************************************/

#include <userint.h>

#ifdef __cplusplus
    extern "C" {
#endif

     /* Panels and Controls: */

#define  SCANPANEL                        1
#define  SCANPANEL_NUM_ITERATIONS         2       /* control type: numeric, callback function: (none) */
#define  SCANPANEL_NUM_SCANSTEP           3       /* control type: numeric, callback function: (none) */
#define  SCANPANEL_NUM_SCANEND            4       /* control type: numeric, callback function: (none) */
#define  SCANPANEL_NUM_SCANSTART          5       /* control type: numeric, callback function: (none) */
#define  SCANPANEL_NUM_ROW                6       /* control type: numeric, callback function: (none) */
#define  SCANPANEL_NUM_DURATION           7       /* control type: numeric, callback function: (none) */
#define  SCANPANEL_NUM_CHANNEL            8       /* control type: numeric, callback function: (none) */
#define  SCANPANEL_NUM_PAGE               9       /* control type: numeric, callback function: (none) */
#define  SCANPANEL_NUM_COLUMN             10      /* control type: numeric, callback function: (none) */
#define  SCANPANEL_CMD_SCAN_CANCEL        11      /* control type: command, callback function: CALLBACK_SCAN_CANCEL */
#define  SCANPANEL_CMD_TIMESCANOK         12      /* control type: command, callback function: CALLBACK_TIMESCANOK */
#define  SCANPANEL_CMD_SCANOK             13      /* control type: command, callback function: CALLBACK_SCANOK */
#define  SCANPANEL_RING_MODE              14      /* control type: ring, callback function: (none) */
#define  SCANPANEL_DECORATION             15      /* control type: deco, callback function: (none) */
#define  SCANPANEL_DECORATION_2           16      /* control type: deco, callback function: (none) */
#define  SCANPANEL_NUM_TIMESTART          17      /* control type: numeric, callback function: (none) */
#define  SCANPANEL_NUM_TIMEEND            18      /* control type: numeric, callback function: (none) */
#define  SCANPANEL_NUM_TIMESTEP           19      /* control type: numeric, callback function: (none) */
#define  SCANPANEL_NUM_TIMEITER           20      /* control type: numeric, callback function: (none) */
#define  SCANPANEL_CMD_GETSCANVALS        21      /* control type: command, callback function: CMD_GETSCANVALS_CALLBACK */
#define  SCANPANEL_CHECK_USE_LIST         22      /* control type: radioButton, callback function: CHECK_USE_LIST_CALLBACK */
#define  SCANPANEL_DECORATION_3           23      /* control type: deco, callback function: (none) */
#define  SCANPANEL_DECORATION_4           24      /* control type: deco, callback function: (none) */
#define  SCANPANEL_TEXTMSG                25      /* control type: textMsg, callback function: (none) */


     /* Control Arrays: */

          /* (no control arrays in the resource file) */


     /* Menu Bars, Menus, and Menu Items: */

          /* (no menu bars in the resource file) */


     /* Callback Prototypes: */

int  CVICALLBACK CALLBACK_SCAN_CANCEL(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK CALLBACK_SCANOK(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK CALLBACK_TIMESCANOK(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK CHECK_USE_LIST_CALLBACK(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK CMD_GETSCANVALS_CALLBACK(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif