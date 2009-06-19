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

#define  MAIN_PANEL                      1
#define  MAIN_PANEL_DISCON_BUTTON        2       /* callback function: DISCONNECT */
#define  MAIN_PANEL_CON_BUTTON           3       /* callback function: CONNECT */
#define  MAIN_PANEL_SEND_BUTTON          4       /* callback function: SEND_DATA */
#define  MAIN_PANEL_CONNECTION_LED       5
#define  MAIN_PANEL_EXIT_BUTTON          6       /* callback function: QUIT */
#define  MAIN_PANEL_HEART_LED            7
#define  MAIN_PANEL_OUT_CMD              8
#define  MAIN_PANEL_OUT_DATA1            9
#define  MAIN_PANEL_OUT_DATA2            10
#define  MAIN_PANEL_OUT_DATA3            11
#define  MAIN_PANEL_CMDBLOCKTICK         12
#define  MAIN_PANEL_TCP_HEART_TIMER      13      /* callback function: TCP_HEART_TIME_CALLBACK */


     /* Menu Bars, Menus, and Menu Items: */

          /* (no menu bars in the resource file) */


     /* Callback Prototypes: */ 

int  CVICALLBACK CONNECT(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK DISCONNECT(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK QUIT(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK SEND_DATA(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK TCP_HEART_TIME_CALLBACK(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif
