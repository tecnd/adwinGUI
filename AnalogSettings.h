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

#define  ANLGPANEL                       1
#define  ANLGPANEL_STRING_UNITS          2
#define  ANLGPANEL_STR_CHANNELNAME       3
#define  ANLGPANEL_NUM_CHANNELPROP       4
#define  ANLGPANEL_NUM_ACH_LINE          5       /* callback function: NUM_ACH_LINE_CALLBACK */
#define  ANLGPANEL_NUM_ACHANNEL          6
#define  ANLGPANEL_CMD_ALLOWCHANGE       7       /* callback function: CMD_ALLOWCHANGE_CALLBACK */
#define  ANLGPANEL_CMD_SETCHANGES        8       /* callback function: CMD_SETCHANGES_CALLBACK */
#define  ANLGPANEL_CMD_DONEANALOG        9       /* callback function: CMD_DONEANALOG_CALLBACK */


     /* Menu Bars, Menus, and Menu Items: */

          /* (no menu bars in the resource file) */


     /* Callback Prototypes: */ 

int  CVICALLBACK CMD_ALLOWCHANGE_CALLBACK(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK CMD_DONEANALOG_CALLBACK(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK CMD_SETCHANGES_CALLBACK(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK NUM_ACH_LINE_CALLBACK(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif
