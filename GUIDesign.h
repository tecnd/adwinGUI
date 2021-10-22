/**************************************************************************/
/* LabWindows/CVI User Interface Resource (UIR) Include File              */
/*                                                                        */
/* WARNING: Do not add to, delete from, or otherwise modify the contents  */
/*          of this include file.                                         */
/**************************************************************************/

#include <userint.h>

#ifdef __cplusplus
    extern "C" {
#endif

     /* Panels and Controls: */

#define  PANEL                            1
#define  PANEL_TIMETABLE                  2       /* control type: table, callback function: TIMETABLE_CALLBACK */
#define  PANEL_ANALOGTABLE                3       /* control type: table, callback function: ANALOGTABLE_CALLBACK */
#define  PANEL_DIGTABLE                   4       /* control type: table, callback function: DIGTABLE_CALLBACK */
#define  PANEL_CMD_SCAN                   5       /* control type: command, callback function: CMD_SCAN_CALLBACK */
#define  PANEL_CMD_RUN                    6       /* control type: command, callback function: CMD_RUN_CALLBACK */
#define  PANEL_DEBUG                      7       /* control type: listBox, callback function: (none) */
#define  PANEL_TOGGLEREPEAT               8       /* control type: textButton, callback function: (none) */
#define  PANEL_CMDSTOP                    9       /* control type: command, callback function: CMDSTOP_CALLBACK */
#define  PANEL_TBL_ANALOGUNITS            10      /* control type: table, callback function: (none) */
#define  PANEL_TBL_DIGNAMES               11      /* control type: table, callback function: (none) */
#define  PANEL_TBL_ANAMES                 12      /* control type: table, callback function: (none) */
#define  PANEL_TIMER                      13      /* control type: timer, callback function: TIMER_CALLBACK */
#define  PANEL_SCAN_TABLE                 14      /* control type: table, callback function: (none) */

#define  SUBPANEL1                        2
#define  SUBPANEL1_TEXTBOX                2       /* control type: textBox, callback function: (none) */

#define  SUBPANEL2                        3
#define  SUBPANEL2_NUM_SCANITER           2       /* control type: numeric, callback function: (none) */
#define  SUBPANEL2_NUM_SCANSTEP           3       /* control type: numeric, callback function: (none) */
#define  SUBPANEL2_NUM_SCANVAL            4       /* control type: numeric, callback function: (none) */


     /* Control Arrays: */

          /* (no control arrays in the resource file) */


     /* Menu Bars, Menus, and Menu Items: */

#define  MENU                             1
#define  MENU_FILE                        2
#define  MENU_FILE_LOADSET                3       /* callback function: MENU_CALLBACK */
#define  MENU_FILE_SAVESET                4       /* callback function: MENU_CALLBACK */
#define  MENU_FILE_MENU_EXPORT_CHANNEL    5       /* callback function: CONFIG_EXPORT_CALLBACK */
#define  MENU_FILE_MENU_EXPORT_PANEL      6       /* callback function: EXPORT_PANEL_CALLBACK */
#define  MENU_FILE_QuitCallback           7       /* callback function: EXIT */
#define  MENU_SETTINGS                    8
#define  MENU_SETTINGS_ANALOG             9       /* callback function: ANALOGSET_CALLBACK */
#define  MENU_SETTINGS_DIGITAL            10      /* callback function: DIGITALSET_CALLBACK */
#define  MENU_SETTINGS_CONTROLTEXT        11
#define  MENU_SETTINGS_CONTROLTEXT_SUBMENU 12
#define  MENU_SETTINGS_CONTROLTEXT_TITLE1 13      /* callback function: TITLE1_CALLBACK */
#define  MENU_SETTINGS_CONTROLTEXT_TITLE2 14      /* callback function: TITLE2_CALLBACK */
#define  MENU_SETTINGS_CONTROLTEXT_TITLE3 15      /* callback function: TITLE3_CALLBACK */
#define  MENU_SETTINGS_CONTROLTEXT_TITLE4 16      /* callback function: TITLE4_CALLBACK */
#define  MENU_SETTINGS_CONTROLTEXT_TITLE5 17      /* callback function: TITLE5_CALLBACK */
#define  MENU_SETTINGS_CONTROLTEXT_TITLE6 18      /* callback function: TITLE6_CALLBACK */
#define  MENU_SETTINGS_CONTROLTEXT_TITLE7 19      /* callback function: TITLE7_CALLBACK */
#define  MENU_SETTINGS_CONTROLTEXT_TITLE8 20      /* callback function: TITLE8_CALLBACK */
#define  MENU_SETTINGS_CONTROLTEXT_TITLE9 21      /* callback function: TITLE9_CALLBACK */
#define  MENU_SETTINGS_CONTROLTEXT_TITLEX 22      /* callback function: TITLEX_CALLBACK */
#define  MENU_SETTINGS_DEBUG              23      /* callback function: MENU_DEBUG_CALLBACK */
#define  MENU_SETTINGS_BOOTADWIN          24      /* callback function: BOOTADWIN_CALLBACK */
#define  MENU_SETTINGS_CLEARPANEL         25      /* callback function: CLEARPANEL_CALLBACK */
#define  MENU_SETTINGS_RESETZERO          26      /* callback function: RESETZERO_CALLBACK */
#define  MENU_SETTINGS_RESETZERO_SUBMENU  27
#define  MENU_SETTINGS_RESETZERO_SETLOW   28      /* callback function: MENU_ALLLOW_CALLBACK */
#define  MENU_SETTINGS_RESETZERO_HOLD     29      /* callback function: MENU_HOLD_CALLBACK */
#define  MENU_SETTINGS_RESETZERO_BYCHNL   30      /* callback function: MENU_BYCHANNEL_CALLBACK */
#define  MENU_SETTINGS_SCANSETTING        31      /* callback function: SCANSETTING_CALLBACK */
#define  MENU_SETTINGS_NOTECHECK          32      /* callback function: NOTECHECK_CALLBACK */
#define  MENU_EDITMATRIX                  33
#define  MENU_EDITMATRIX_INSERTCOLUMN     34      /* callback function: INSERTCOLUMN_CALLBACK */
#define  MENU_EDITMATRIX_DELETECOLUMN     35      /* callback function: DELETECOLUMN_CALLBACK */
#define  MENU_EDITMATRIX_COPYCOLUMN       36      /* callback function: COPYCOLUMN_CALLBACK */
#define  MENU_EDITMATRIX_PASTECOLUMN      37      /* callback function: PASTECOLUMN_CALLBACK */
#define  MENU_UPDATEPERIOD                38
#define  MENU_UPDATEPERIOD_SETGD5         39      /* callback function: SETGD5_CALLBACK */
#define  MENU_UPDATEPERIOD_SETGD10        40      /* callback function: SETGD10_CALLBACK */
#define  MENU_UPDATEPERIOD_SETGD100       41      /* callback function: SETGD100_CALLBACK */
#define  MENU_UPDATEPERIOD_SETGD1000      42      /* callback function: SETGD1000_CALLBACK */
#define  MENU_PREFS                       43
#define  MENU_PREFS_COMPRESSION           44      /* callback function: COMPRESSION_CALLBACK */
#define  MENU_PREFS_SIMPLETIMING          45      /* callback function: SIMPLETIMING_CALLBACK */
#define  MENU_PREFS_SHOWARRAY             46      /* callback function: SHOWARRAY_CALLBACK */


     /* Callback Prototypes: */

void CVICALLBACK ANALOGSET_CALLBACK(int menubar, int menuItem, void *callbackData, int panel);
int  CVICALLBACK ANALOGTABLE_CALLBACK(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
void CVICALLBACK BOOTADWIN_CALLBACK(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK CLEARPANEL_CALLBACK(int menubar, int menuItem, void *callbackData, int panel);
int  CVICALLBACK CMD_RUN_CALLBACK(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK CMD_SCAN_CALLBACK(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK CMDSTOP_CALLBACK(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
void CVICALLBACK COMPRESSION_CALLBACK(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK CONFIG_EXPORT_CALLBACK(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK COPYCOLUMN_CALLBACK(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK DELETECOLUMN_CALLBACK(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK DIGITALSET_CALLBACK(int menubar, int menuItem, void *callbackData, int panel);
int  CVICALLBACK DIGTABLE_CALLBACK(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
void CVICALLBACK EXIT(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK EXPORT_PANEL_CALLBACK(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK INSERTCOLUMN_CALLBACK(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK MENU_ALLLOW_CALLBACK(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK MENU_BYCHANNEL_CALLBACK(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK MENU_CALLBACK(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK MENU_DEBUG_CALLBACK(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK MENU_HOLD_CALLBACK(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK NOTECHECK_CALLBACK(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK PASTECOLUMN_CALLBACK(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK RESETZERO_CALLBACK(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK SCANSETTING_CALLBACK(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK SETGD1000_CALLBACK(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK SETGD100_CALLBACK(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK SETGD10_CALLBACK(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK SETGD5_CALLBACK(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK SHOWARRAY_CALLBACK(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK SIMPLETIMING_CALLBACK(int menubar, int menuItem, void *callbackData, int panel);
int  CVICALLBACK TIMER_CALLBACK(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK TIMETABLE_CALLBACK(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
void CVICALLBACK TITLE1_CALLBACK(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK TITLE2_CALLBACK(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK TITLE3_CALLBACK(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK TITLE4_CALLBACK(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK TITLE5_CALLBACK(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK TITLE6_CALLBACK(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK TITLE7_CALLBACK(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK TITLE8_CALLBACK(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK TITLE9_CALLBACK(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK TITLEX_CALLBACK(int menubar, int menuItem, void *callbackData, int panel);


#ifdef __cplusplus
    }
#endif