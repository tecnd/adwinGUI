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
#define  PANEL_CHECKBOX_7                2
#define  PANEL_CHECKBOX_6                3
#define  PANEL_CHECKBOX_5                4
#define  PANEL_CHECKBOX_4                5
#define  PANEL_CHECKBOX_3                6
#define  PANEL_CHECKBOX_2                7
#define  PANEL_CHECKBOX                  8
#define  PANEL_LABEL_7                   9
#define  PANEL_LABEL_6                   10
#define  PANEL_LABEL_5                   11
#define  PANEL_LABEL_4                   12
#define  PANEL_LABEL_3                   13
#define  PANEL_LABEL_2                   14
#define  PANEL_LABEL_1                   15
#define  PANEL_TIMETABLE                 16      /* callback function: TIMETABLE_CALLBACK */
#define  PANEL_ANALOGTABLE               17      /* callback function: ANALOGTABLE_CALLBACK */
#define  PANEL_QUITBUTTON                18      /* callback function: QuitCallback */
#define  PANEL_DIGTABLE                  19      /* callback function: DIGTABLE_CALLBACK */
#define  PANEL_TB_SHOWPHASE7             20      /* callback function: TOGGLE7_CALLBACK */
#define  PANEL_TB_SHOWPHASE6             21      /* callback function: TOGGLE6_CALLBACK */
#define  PANEL_TB_SHOWPHASE5             22      /* callback function: TOGGLE5_CALLBACK */
#define  PANEL_TB_SHOWPHASE4             23      /* callback function: TOGGLE4_CALLBACK */
#define  PANEL_TB_SHOWPHASE3             24      /* callback function: TOGGLE3_CALLBACK */
#define  PANEL_TB_SHOWPHASE2             25      /* callback function: TOGGLE2_CALLBACK */
#define  PANEL_TB_SHOWPHASE1             26      /* callback function: TOGGLE1_CALLBACK */
#define  PANEL_CMD_RUN                   27      /* callback function: CMD_RUN_CALLBACK */
#define  PANEL_DEBUG                     28
#define  PANEL_TOGGLEREPEAT              29
#define  PANEL_CMDSTOP                   30      /* callback function: CMDSTOP_CALLBACK */
#define  PANEL_TGL_NUMERICTABLE          31      /* callback function: TGLNUMERIC_CALLBACK */
#define  PANEL_DISPLAYDIAL               32      /* callback function: DISPLAYDIAL_CALLBACK */
#define  PANEL_TBL_ANALOGUNITS           33
#define  PANEL_TBL_DIGNAMES              34
#define  PANEL_TBL_ANAMES                35
#define  PANEL_TIMER                     36      /* callback function: TIMER_CALLBACK */


     /* Menu Bars, Menus, and Menu Items: */

#define  MENU                            1
#define  MENU_FILE                       2
#define  MENU_FILE_LOADSET               3       /* callback function: MENU_CALLBACK */
#define  MENU_FILE_SAVESET               4       /* callback function: MENU_CALLBACK */
#define  MENU_FILE_BOOTLOAD              5       /* callback function: BOOTLOAD_CALLBACK */
#define  MENU_FILE_MENU_EXPORT_CHANNEL   6       /* callback function: CONFIG_EXPORT_CALLBACK */
#define  MENU_FILE_MENU_EXPORT_PANEL     7       /* callback function: EXPORT_PANEL_CALLBACK */
#define  MENU_SETTINGS                   8
#define  MENU_SETTINGS_ANALOG            9       /* callback function: ANALOGSET_CALLBACK */
#define  MENU_SETTINGS_DIGITAL           10      /* callback function: DIGITALSET_CALLBACK */
#define  MENU_SETTINGS_DDSSETUP          11      /* callback function: DDSSETUP_CALLBACK */
#define  MENU_SETTINGS_CONTROLTEXT       12
#define  MENU_SETTINGS_CONTROLTEXT_SUBMEN 13
#define  MENU_SETTINGS_CONTROLTEXT_TITLE1 14     /* callback function: TITLE1_CALLBACK */
#define  MENU_SETTINGS_CONTROLTEXT_TITLE2 15     /* callback function: TITLE2_CALLBACK */
#define  MENU_SETTINGS_CONTROLTEXT_TITLE3 16     /* callback function: TITLE3_CALLBACK */
#define  MENU_SETTINGS_CONTROLTEXT_TITLE4 17     /* callback function: TITLE4_CALLBACK */
#define  MENU_SETTINGS_CONTROLTEXT_TITLE5 18     /* callback function: TITLE5_CALLBACK */
#define  MENU_SETTINGS_CONTROLTEXT_TITLE6 19     /* callback function: TITLE6_CALLBACK */
#define  MENU_SETTINGS_CONTROLTEXT_TITLE7 20     /* callback function: TITLE7_CALLBACK */
#define  MENU_SETTINGS_DEBUG             21      /* callback function: MENU_DEBUG_CALLBACK */
#define  MENU_SETTINGS_BOOTADWIN         22      /* callback function: BOOTADWIN_CALLBACK */
#define  MENU_SETTINGS_CLEARPANEL        23      /* callback function: CLEARPANEL_CALLBACK */
#define  MENU_SETTINGS_RESETZERO         24      /* callback function: RESETZERO_CALLBACK */
#define  MENU_EDITMATRIX                 25
#define  MENU_EDITMATRIX_INSERTCOLUMN    26      /* callback function: INSERTCOLUMN_CALLBACK */
#define  MENU_EDITMATRIX_DELETECOLUMN    27      /* callback function: DELETECOLUMN_CALLBACK */
#define  MENU_EDITMATRIX_COPYCOLUMN      28      /* callback function: COPYCOLUMN_CALLBACK */
#define  MENU_EDITMATRIX_PASTECOLUMN     29      /* callback function: PASTECOLUMN_CALLBACK */
#define  MENU_UPDATEPERIOD               30
#define  MENU_UPDATEPERIOD_SETGD5        31      /* callback function: SETGD5_CALLBACK */
#define  MENU_UPDATEPERIOD_SETGD10       32      /* callback function: SETGD10_CALLBACK */
#define  MENU_UPDATEPERIOD_SETGD100      33      /* callback function: SETGD100_CALLBACK */
#define  MENU_UPDATEPERIOD_SETGD1000     34      /* callback function: SETGD1000_CALLBACK */


     /* Callback Prototypes: */ 

void CVICALLBACK ANALOGSET_CALLBACK(int menubar, int menuItem, void *callbackData, int panel);
int  CVICALLBACK ANALOGTABLE_CALLBACK(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
void CVICALLBACK BOOTADWIN_CALLBACK(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK BOOTLOAD_CALLBACK(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK CLEARPANEL_CALLBACK(int menubar, int menuItem, void *callbackData, int panel);
int  CVICALLBACK CMD_RUN_CALLBACK(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK CMDSTOP_CALLBACK(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
void CVICALLBACK CONFIG_EXPORT_CALLBACK(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK COPYCOLUMN_CALLBACK(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK DDSSETUP_CALLBACK(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK DELETECOLUMN_CALLBACK(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK DIGITALSET_CALLBACK(int menubar, int menuItem, void *callbackData, int panel);
int  CVICALLBACK DIGTABLE_CALLBACK(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK DISPLAYDIAL_CALLBACK(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
void CVICALLBACK EXPORT_PANEL_CALLBACK(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK INSERTCOLUMN_CALLBACK(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK MENU_CALLBACK(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK MENU_DEBUG_CALLBACK(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK PASTECOLUMN_CALLBACK(int menubar, int menuItem, void *callbackData, int panel);
int  CVICALLBACK QuitCallback(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
void CVICALLBACK RESETZERO_CALLBACK(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK SETGD1000_CALLBACK(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK SETGD100_CALLBACK(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK SETGD10_CALLBACK(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK SETGD5_CALLBACK(int menubar, int menuItem, void *callbackData, int panel);
int  CVICALLBACK TGLNUMERIC_CALLBACK(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK TIMER_CALLBACK(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK TIMETABLE_CALLBACK(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
void CVICALLBACK TITLE1_CALLBACK(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK TITLE2_CALLBACK(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK TITLE3_CALLBACK(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK TITLE4_CALLBACK(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK TITLE5_CALLBACK(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK TITLE6_CALLBACK(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK TITLE7_CALLBACK(int menubar, int menuItem, void *callbackData, int panel);
int  CVICALLBACK TOGGLE1_CALLBACK(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK TOGGLE2_CALLBACK(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK TOGGLE3_CALLBACK(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK TOGGLE4_CALLBACK(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK TOGGLE5_CALLBACK(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK TOGGLE6_CALLBACK(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK TOGGLE7_CALLBACK(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif
