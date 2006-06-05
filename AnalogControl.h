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

#define  CTRL_PANEL                      1
#define  CTRL_PANEL_NUMINITVAL           2
#define  CTRL_PANEL_NUMFINALVAL          3
#define  CTRL_PANEL_STRUNITS             4
#define  CTRL_PANEL_NUMTIMESCALE         5
#define  CTRL_PANEL_CMD_SETANALOG        6       /* callback function: CMD_SETANALAOG_CALLBACK */
#define  CTRL_PANEL_STR_CHNAME           7
#define  CTRL_PANEL_RING_CTRLMODE        8       /* callback function: RING_CTRLMODE_CALLBACK */
#define  CTRL_PANEL_CMD_ANCANCEL         9       /* callback function: CMD_ANCANCEL_CALLBACK */


     /* Menu Bars, Menus, and Menu Items: */

          /* (no menu bars in the resource file) */


     /* Callback Prototypes: */ 

int  CVICALLBACK CMD_ANCANCEL_CALLBACK(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK CMD_SETANALAOG_CALLBACK(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK RING_CTRLMODE_CALLBACK(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif
