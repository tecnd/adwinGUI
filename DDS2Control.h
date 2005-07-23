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

#define  DDS2PANEL                       1
#define  DDS2PANEL_NUM2_CURRENT          2
#define  DDS2PANEL_NUM2_FREQ1            3
#define  DDS2PANEL_NUM2_FREQ2            4
#define  DDS2PANEL_CMD2_ONOFF            5       /* callback function: CMD2_ONOFF_CALLBACK */
#define  DDS2PANEL_CMD2_OK               6       /* callback function: CMD2_OK_CALLBACK */
#define  DDS2PANEL_CMD2_CANCEL           7       /* callback function: CMD2_CANCEL_CALLBACK */


     /* Menu Bars, Menus, and Menu Items: */

          /* (no menu bars in the resource file) */


     /* Callback Prototypes: */ 

int  CVICALLBACK CMD2_CANCEL_CALLBACK(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK CMD2_OK_CALLBACK(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK CMD2_ONOFF_CALLBACK(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif
