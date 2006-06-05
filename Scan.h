/**************************************************************************/
/* LabWindows/CVI User Interface Resource (UIR) Include File              */
/* Copyright (c) National Instruments 2006. All Rights Reserved.          */
/*                                                                        */
/* WARNING: Do not add to, delete from, or otherwise modify the contents  */
/*          of this include file.                                         */
/**************************************************************************/

#include <userint.h>

#ifdef __cplusplus
    extern "C" {
#endif

     /* Panels and Controls: */

#define  SCANPANEL                       1
#define  SCANPANEL_NUM_ITERATIONS        2
#define  SCANPANEL_NUM_SCANSTEP          3
#define  SCANPANEL_NUM_SCANEND           4
#define  SCANPANEL_NUM_SCANSTART         5
#define  SCANPANEL_NUM_ROW               6
#define  SCANPANEL_NUM_DURATION          7
#define  SCANPANEL_NUM_CHANNEL           8
#define  SCANPANEL_NUM_PAGE              9
#define  SCANPANEL_NUM_COLUMN            10
#define  SCANPANEL_CMD_SCAN_CANCEL       11      /* callback function: CALLBACK_SCAN_CANCEL */
#define  SCANPANEL_CMD_DDSFLOORSCANOK    12      /* callback function: CALLBACK_DDSFLOORSCANOK */
#define  SCANPANEL_CMD_DDSSCANOK         13      /* callback function: CALLBACK_DDSSCANOK */
#define  SCANPANEL_CMD_TIMESCANOK        14      /* callback function: CALLBACK_TIMESCANOK */
#define  SCANPANEL_CMD_SCANOK            15      /* callback function: CALLBACK_SCANOK */
#define  SCANPANEL_RING_MODE             16
#define  SCANPANEL_DECORATION            17
#define  SCANPANEL_DECORATION_2          18
#define  SCANPANEL_NUM_TIMESTART         19
#define  SCANPANEL_NUM_TIMEEND           20
#define  SCANPANEL_NUM_TIMESTEP          21
#define  SCANPANEL_NUM_TIMEITER          22
#define  SCANPANEL_NUM_DDSFLOOREND       23
#define  SCANPANEL_NUM_DDSSTART          24
#define  SCANPANEL_NUM_DDSFLOORSTART     25
#define  SCANPANEL_NUM_DDSEND            26
#define  SCANPANEL_NUM_DDSFLOORITER      27
#define  SCANPANEL_NUM_DDSFLOORSTEP      28
#define  SCANPANEL_NUM_DDSBASEFREQ       29
#define  SCANPANEL_NUM_DDSCURRENT        30
#define  SCANPANEL_NUM_DDSITER           31
#define  SCANPANEL_NUM_DDSSTEP           32
#define  SCANPANEL_CMD_GETSCANVALS       33      /* callback function: CMD_GETSCANVALS_CALLBACK */
#define  SCANPANEL_CHECK_USE_LIST        34      /* callback function: CHECK_USE_LIST_CALLBACK */
#define  SCANPANEL_DECORATION_3          35
#define  SCANPANEL_DECORATION_4          36


     /* Menu Bars, Menus, and Menu Items: */

          /* (no menu bars in the resource file) */


     /* Callback Prototypes: */ 

int  CVICALLBACK CALLBACK_DDSFLOORSCANOK(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK CALLBACK_DDSSCANOK(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK CALLBACK_SCAN_CANCEL(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK CALLBACK_SCANOK(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK CALLBACK_TIMESCANOK(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK CHECK_USE_LIST_CALLBACK(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK CMD_GETSCANVALS_CALLBACK(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif
