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

#define  MAINPANEL                       1
#define  MAINPANEL_LAS0BOX3              2
#define  MAINPANEL_LAS0BOX2              3
#define  MAINPANEL_LAS0BOX1              4
#define  MAINPANEL_LAS0BOX0              5
#define  MAINPANEL_EXITBUTTON            6       /* callback function: Exit_Callback */
#define  MAINPANEL_LED3                  7
#define  MAINPANEL_LED2                  8
#define  MAINPANEL_LED1                  9
#define  MAINPANEL_LED0                  10
#define  MAINPANEL_FORCEBUTTON           11      /* callback function: ForceCon */
#define  MAINPANEL_CLEARBUTTON           12      /* callback function: ClearTextBoxes */
#define  MAINPANEL_HBTTIMER              13      /* callback function: HBTimerTick */


     /* Menu Bars, Menus, and Menu Items: */

#define  SETTINGS                        1
#define  SETTINGS_SET                    2
#define  SETTINGS_SET_LOAD               3       /* callback function: Load_Settings */


     /* Callback Prototypes: */ 

int  CVICALLBACK ClearTextBoxes(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK Exit_Callback(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK ForceCon(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK HBTimerTick(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
void CVICALLBACK Load_Settings(int menubar, int menuItem, void *callbackData, int panel);


#ifdef __cplusplus
    }
#endif
