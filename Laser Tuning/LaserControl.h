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

#define  PANEL                           1
#define  PANEL_LAS_LAST_VAL              2
#define  PANEL_LAS_TARGET_FREQ           3
#define  PANEL_LAS_CONTROL_MODE          4
#define  PANEL_CANCEL_LAS_CONT           5       /* callback function: CancelLasConCALLBACK */
#define  PANEL_SET_LAS_CON               6       /* callback function: SetLasConCALLBACK */
#define  PANEL_LAS_NAME                  7


     /* Menu Bars, Menus, and Menu Items: */

          /* (no menu bars in the resource file) */


     /* Callback Prototypes: */ 

int  CVICALLBACK CancelLasConCALLBACK(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK SetLasConCALLBACK(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif
