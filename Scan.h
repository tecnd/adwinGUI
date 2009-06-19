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
#define  SCANPANEL_CMD_LASSCANOK         13      /* callback function: CALLBACK_LASSCANOK */
#define  SCANPANEL_CMD_DDSSCANOK         14      /* callback function: CALLBACK_DDSSCANOK */
#define  SCANPANEL_CMD_TIMESCANOK        15      /* callback function: CALLBACK_TIMESCANOK */
#define  SCANPANEL_CMD_SCANOK            16      /* callback function: CALLBACK_SCANOK */
#define  SCANPANEL_RING_MODE             17
#define  SCANPANEL_DECORATION            18
#define  SCANPANEL_DECORATION_2          19
#define  SCANPANEL_NUM_TIMESTART         20
#define  SCANPANEL_NUM_TIMEEND           21
#define  SCANPANEL_NUM_TIMESTEP          22
#define  SCANPANEL_NUM_TIMEITER          23
#define  SCANPANEL_NUM_DDSFLOOREND       24
#define  SCANPANEL_NUM_DDSSTART          25
#define  SCANPANEL_NUM_DDSFLOORSTART     26
#define  SCANPANEL_NUM_DDSEND            27
#define  SCANPANEL_NUM_DDSFLOORITER      28
#define  SCANPANEL_NUM_DDSFLOORSTEP      29
#define  SCANPANEL_NUM_DDSBASEFREQ       30
#define  SCANPANEL_NUM_DDSCURRENT        31
#define  SCANPANEL_NUM_LASITER           32
#define  SCANPANEL_NUM_LASSTEP           33
#define  SCANPANEL_NUM_DDSITER           34
#define  SCANPANEL_NUM_DDSSTEP           35
#define  SCANPANEL_CMD_GETSCANVALS       36      /* callback function: CMD_GETSCANVALS_CALLBACK */
#define  SCANPANEL_CHECK_USE_LIST        37      /* callback function: CHECK_USE_LIST_CALLBACK */
#define  SCANPANEL_DECORATION_3          38
#define  SCANPANEL_DECORATION_4          39
#define  SCANPANEL_NUM_LASEND            40
#define  SCANPANEL_NUM_LASSTART          41
#define  SCANPANEL_DECORATION_5          42
#define  SCANPANEL_TXT_LASIDENT          43


     /* Menu Bars, Menus, and Menu Items: */

          /* (no menu bars in the resource file) */


     /* Callback Prototypes: */ 

int  CVICALLBACK CALLBACK_DDSFLOORSCANOK(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK CALLBACK_DDSSCANOK(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK CALLBACK_LASSCANOK(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK CALLBACK_SCAN_CANCEL(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK CALLBACK_SCANOK(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK CALLBACK_TIMESCANOK(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK CHECK_USE_LIST_CALLBACK(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK CMD_GETSCANVALS_CALLBACK(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif
