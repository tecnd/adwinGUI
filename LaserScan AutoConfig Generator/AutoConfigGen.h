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

#define  MainPanel                       1
#define  MainPanel_LASER_RING            2       /* callback function: LASERSELECT_CALLBACK */
#define  MainPanel_SCANEND               3
#define  MainPanel_SCANSTART             4
#define  MainPanel_FLASH_LOAD            5       /* callback function: FLASH_LOAD_CALLBACK */
#define  MainPanel_RAM_LOAD              6       /* callback function: RAM_LOAD_CALLBACK */
#define  MainPanel_CONFIG_NUM            7       /* callback function: CONFIG_NUM_CALLBACK */
#define  MainPanel_RESETBUTTON           8       /* callback function: RESET_CALLBACK */
#define  MainPanel_EXITBUTTON            9       /* callback function: EXIT_CALLBACK */
#define  MainPanel_NUM_SCANTIME          10
#define  MainPanel_UNITTXT               11
#define  MainPanel_FLASH_LED             12
#define  MainPanel_RAM_LED               13
#define  MainPanel_TEXTMSG_2             14
#define  MainPanel_TIMER                 15      /* callback function: TIMER_CALLBACK */
#define  MainPanel_FLASH_TEST            16      /* callback function: FLASH_TEST_CALLBACK */
#define  MainPanel_CONSTFREQRAD          17      /* callback function: ConstFreqCallback */
#define  MainPanel_DACSCANRAD            18      /* callback function: DACScanCallback */
#define  MainPanel_DDSSCANRAD            19      /* callback function: DDSScanCallback */
#define  MainPanel_TEXTMSG_3             20


     /* Menu Bars, Menus, and Menu Items: */

#define  Settings                        1
#define  Settings_SETTINGS_MENU          2
#define  Settings_SETTINGS_MENU_LSET_LOAD 3      /* callback function: LSET_LOAD_CALLBACK */
#define  Settings_SETTINGS_MENU_LSET_SAVE 4      /* callback function: LSET_SAVE_CALLBACK */
#define  Settings_SETTINGS_MENU_LSET_EDIT 5      /* callback function: LSET_EDIT_CALLBACK */


     /* Callback Prototypes: */ 

int  CVICALLBACK CONFIG_NUM_CALLBACK(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK ConstFreqCallback(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK DACScanCallback(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK DDSScanCallback(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK EXIT_CALLBACK(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK FLASH_LOAD_CALLBACK(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK FLASH_TEST_CALLBACK(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK LASERSELECT_CALLBACK(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
void CVICALLBACK LSET_EDIT_CALLBACK(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK LSET_LOAD_CALLBACK(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK LSET_SAVE_CALLBACK(int menubar, int menuItem, void *callbackData, int panel);
int  CVICALLBACK RAM_LOAD_CALLBACK(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK RESET_CALLBACK(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK TIMER_CALLBACK(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif
