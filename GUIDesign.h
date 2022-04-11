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

#define  PANEL                            1       /* callback function: PANEL_CALLBACK */
#define  PANEL_TIMETABLE                  2       /* control type: table, callback function: TIMETABLE_CALLBACK */
#define  PANEL_ANALOGTABLE                3       /* control type: table, callback function: ANALOGTABLE_CALLBACK */
#define  PANEL_DIGTABLE                   4       /* control type: table, callback function: DIGTABLE_CALLBACK */
#define  PANEL_CMD_SCAN                   5       /* control type: command, callback function: CMD_SCAN_CALLBACK */
#define  PANEL_CMD_RUN                    6       /* control type: command, callback function: CMD_RUN_CALLBACK */
#define  PANEL_TOGGLEREPEAT               7       /* control type: textButton, callback function: (none) */
#define  PANEL_CMDSTOP                    8       /* control type: command, callback function: CMDSTOP_CALLBACK */
#define  PANEL_TBL_ANALOGUNITS            9       /* control type: table, callback function: (none) */
#define  PANEL_TBL_DIGNAMES               10      /* control type: table, callback function: (none) */
#define  PANEL_TBL_ANAMES                 11      /* control type: table, callback function: (none) */
#define  PANEL_TIMER                      12      /* control type: timer, callback function: TIMER_CALLBACK */
#define  PANEL_SCAN_TABLE                 13      /* control type: table, callback function: (none) */
#define  PANEL_CMD_COMMENTS               14      /* control type: command, callback function: CMD_COMMENTS_CALLBACK */

#define  SUBPANEL2                        2
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
#define  MENU_FILE_EXPORT_PYTHON          7       /* callback function: EXPORT_PYTHON_CALLBACK */
#define  MENU_FILE_QuitCallback           8       /* callback function: EXIT */
#define  MENU_SETTINGS                    9
#define  MENU_SETTINGS_ANALOG             10      /* callback function: ANALOGSET_CALLBACK */
#define  MENU_SETTINGS_DIGITAL            11      /* callback function: DIGITALSET_CALLBACK */
#define  MENU_SETTINGS_BOOTADWIN          12      /* callback function: BOOTADWIN_CALLBACK */
#define  MENU_SETTINGS_CLEARPANEL         13      /* callback function: CLEARPANEL_CALLBACK */
#define  MENU_SETTINGS_RESETZERO          14      /* callback function: RESETZERO_CALLBACK */
#define  MENU_SETTINGS_RESETZERO_SUBMENU  15
#define  MENU_SETTINGS_RESETZERO_SETLOW   16      /* callback function: MENU_ALLLOW_CALLBACK */
#define  MENU_SETTINGS_RESETZERO_HOLD     17      /* callback function: MENU_HOLD_CALLBACK */
#define  MENU_SETTINGS_RESETZERO_BYCHNL   18      /* callback function: MENU_BYCHANNEL_CALLBACK */
#define  MENU_SETTINGS_SCANSETTING        19      /* callback function: SCANSETTING_CALLBACK */
#define  MENU_EDITMATRIX                  20
#define  MENU_EDITMATRIX_INSERTCOLUMN     21      /* callback function: INSERTCOLUMN_CALLBACK */
#define  MENU_EDITMATRIX_DELETECOLUMN     22      /* callback function: DELETECOLUMN_CALLBACK */
#define  MENU_EDITMATRIX_COPYCOLUMN       23      /* callback function: COPYCOLUMN_CALLBACK */
#define  MENU_EDITMATRIX_PASTECOLUMN      24      /* callback function: PASTECOLUMN_CALLBACK */
#define  MENU_PREFS                       25
#define  MENU_PREFS_COMPRESSION           26      /* callback function: COMPRESSION_CALLBACK */
#define  MENU_PREFS_SIMPLETIMING          27      /* callback function: SIMPLETIMING_CALLBACK */


     /* Callback Prototypes: */

void CVICALLBACK ANALOGSET_CALLBACK(int menubar, int menuItem, void *callbackData, int panel);
int  CVICALLBACK ANALOGTABLE_CALLBACK(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
void CVICALLBACK BOOTADWIN_CALLBACK(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK CLEARPANEL_CALLBACK(int menubar, int menuItem, void *callbackData, int panel);
int  CVICALLBACK CMD_COMMENTS_CALLBACK(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
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
void CVICALLBACK EXPORT_PYTHON_CALLBACK(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK INSERTCOLUMN_CALLBACK(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK MENU_ALLLOW_CALLBACK(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK MENU_BYCHANNEL_CALLBACK(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK MENU_CALLBACK(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK MENU_HOLD_CALLBACK(int menubar, int menuItem, void *callbackData, int panel);
int  CVICALLBACK PANEL_CALLBACK(int panel, int event, void *callbackData, int eventData1, int eventData2);
void CVICALLBACK PASTECOLUMN_CALLBACK(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK RESETZERO_CALLBACK(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK SCANSETTING_CALLBACK(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK SIMPLETIMING_CALLBACK(int menubar, int menuItem, void *callbackData, int panel);
int  CVICALLBACK TIMER_CALLBACK(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK TIMETABLE_CALLBACK(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif