/**************************************************************************/
/* LabWindows/CVI User Interface Resource (UIR) Include File              */
/* Copyright (c) National Instruments 2005. All Rights Reserved.          */
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
#define  SCANPANEL_CMD_SCANOK            12      /* callback function: CALLBACK_SCANOK */
#define  SCANPANEL_RING_MODE             13


     /* Menu Bars, Menus, and Menu Items: */

          /* (no menu bars in the resource file) */


     /* Callback Prototypes: */ 

int  CVICALLBACK CALLBACK_SCAN_CANCEL(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK CALLBACK_SCANOK(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif
