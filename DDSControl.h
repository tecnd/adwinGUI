/**************************************************************************/
/* LabWindows/CVI User Interface Resource (UIR) Include File              */
/* Copyright (c) National Instruments 2004. All Rights Reserved.          */
/*                                                                        */
/* WARNING: Do not add to, delete from, or otherwise modify the contents  */
/*          of this include file.                                         */
/**************************************************************************/

#include <userint.h>

#ifdef __cplusplus
    extern "C" {
#endif

     /* Panels and Controls: */

#define  PANEL                           1
#define  PANEL_NUM_CURRENT               2
#define  PANEL_NUM_FREQ1                 3
#define  PANEL_NUM_FREQ2                 4
#define  PANEL_CMD_ONOFF                 5       /* callback function: CMD_ONOFF_CALLBACK */
#define  PANEL_CMD_OK                    6       /* callback function: CMD_OK_CALLBACK */
#define  PANEL_CMD_CANCEL                7       /* callback function: CMD_CANCEL_CALLBACK */


     /* Menu Bars, Menus, and Menu Items: */

          /* (no menu bars in the resource file) */


     /* Callback Prototypes: */ 

int  CVICALLBACK CMD_CANCEL_CALLBACK(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK CMD_OK_CALLBACK(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK CMD_ONOFF_CALLBACK(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif
