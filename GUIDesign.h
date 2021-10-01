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
#define  PANEL_CHECKBOX_10                2       /* control type: radioButton, callback function: (none) */
#define  PANEL_CHECKBOX_9                 3       /* control type: radioButton, callback function: (none) */
#define  PANEL_CHECKBOX_8                 4       /* control type: radioButton, callback function: (none) */
#define  PANEL_CHECKBOX_7                 5       /* control type: radioButton, callback function: (none) */
#define  PANEL_CHECKBOX_6                 6       /* control type: radioButton, callback function: (none) */
#define  PANEL_CHECKBOX_5                 7       /* control type: radioButton, callback function: (none) */
#define  PANEL_CHECKBOX_4                 8       /* control type: radioButton, callback function: (none) */
#define  PANEL_CHECKBOX_3                 9       /* control type: radioButton, callback function: (none) */
#define  PANEL_CHECKBOX_2                 10      /* control type: radioButton, callback function: (none) */
#define  PANEL_CHECKBOX                   11      /* control type: radioButton, callback function: (none) */
#define  PANEL_TIMETABLE                  12      /* control type: table, callback function: TIMETABLE_CALLBACK */
#define  PANEL_ANALOGTABLE                13      /* control type: table, callback function: ANALOGTABLE_CALLBACK */
#define  PANEL_TB_SHOWPHASE10             14      /* control type: textButton, callback function: TOGGLE10_CALLBACK */
#define  PANEL_TB_SHOWPHASE9              15      /* control type: textButton, callback function: TOGGLE9_CALLBACK */
#define  PANEL_DIGTABLE                   16      /* control type: table, callback function: DIGTABLE_CALLBACK */
#define  PANEL_TB_SHOWPHASE8              17      /* control type: textButton, callback function: TOGGLE8_CALLBACK */
#define  PANEL_TB_SHOWPHASE7              18      /* control type: textButton, callback function: TOGGLE7_CALLBACK */
#define  PANEL_TB_SHOWPHASE6              19      /* control type: textButton, callback function: TOGGLE6_CALLBACK */
#define  PANEL_TB_SHOWPHASE5              20      /* control type: textButton, callback function: TOGGLE5_CALLBACK */
#define  PANEL_TB_SHOWPHASE4              21      /* control type: textButton, callback function: TOGGLE4_CALLBACK */
#define  PANEL_TB_SHOWPHASE3              22      /* control type: textButton, callback function: TOGGLE3_CALLBACK */
#define  PANEL_TB_SHOWPHASE2              23      /* control type: textButton, callback function: TOGGLE2_CALLBACK */
#define  PANEL_TB_SHOWPHASE1              24      /* control type: textButton, callback function: TOGGLE1_CALLBACK */
#define  PANEL_CMD_SCAN                   25      /* control type: command, callback function: CMD_SCAN_CALLBACK */
#define  PANEL_CMD_RUN                    26      /* control type: command, callback function: CMD_RUN_CALLBACK */
#define  PANEL_DEBUG                      27      /* control type: listBox, callback function: (none) */
#define  PANEL_TOGGLEREPEAT               28      /* control type: textButton, callback function: (none) */
#define  PANEL_CMDSTOP                    29      /* control type: command, callback function: CMDSTOP_CALLBACK */
#define  PANEL_TGL_NUMERICTABLE           30      /* control type: textButton, callback function: TGLNUMERIC_CALLBACK */
#define  PANEL_DISPLAYDIAL                31      /* control type: slide, callback function: DISPLAYDIAL_CALLBACK */
#define  PANEL_TBL_ANALOGUNITS            32      /* control type: table, callback function: (none) */
#define  PANEL_TBL_DIGNAMES               33      /* control type: table, callback function: (none) */
#define  PANEL_TBL_ANAMES                 34      /* control type: table, callback function: (none) */
#define  PANEL_TIMER                      35      /* control type: timer, callback function: TIMER_CALLBACK */
#define  PANEL_NUM_INSERTIONCOL           36      /* control type: numeric, callback function: NUM_INSERTIONCOL_CALLBACK */
#define  PANEL_NUM_INSERTIONPAGE          37      /* control type: numeric, callback function: NUM_INSERTIONPAGE_CALLBACK */
#define  PANEL_NUM_DDS3_OFFSET            38      /* control type: numeric, callback function: (none) */
#define  PANEL_NUM_DDS2_OFFSET            39      /* control type: numeric, callback function: (none) */
#define  PANEL_NUM_DDS_OFFSET             40      /* control type: numeric, callback function: (none) */
#define  PANEL_SCAN_TABLE                 41      /* control type: table, callback function: (none) */
#define  PANEL_CANVAS_END                 42      /* control type: canvas, callback function: (none) */
#define  PANEL_CANVAS_START               43      /* control type: canvas, callback function: (none) */
#define  PANEL_DECORATION                 44      /* control type: deco, callback function: (none) */
#define  PANEL_NUM_LOOP_REPS              45      /* control type: numeric, callback function: (none) */
#define  PANEL_NUM_LOOPCOL2               46      /* control type: numeric, callback function: (none) */
#define  PANEL_NUM_LOOPPAGE2              47      /* control type: numeric, callback function: (none) */
#define  PANEL_NUM_LOOPCOL1               48      /* control type: numeric, callback function: (none) */
#define  PANEL_NUM_LOOPPAGE1              49      /* control type: numeric, callback function: (none) */
#define  PANEL_SWITCH_LOOP                50      /* control type: binary, callback function: SWITCH_LOOP_CALLBACK */
#define  PANEL_DECORATION_2               51      /* control type: deco, callback function: (none) */
#define  PANEL_CANVAS_LOOPLINE            52      /* control type: canvas, callback function: (none) */
#define  PANEL_Looping                    53      /* control type: textMsg, callback function: (none) */

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
#define  MENU_SETTINGS_DDSSETUP           11      /* callback function: DDSSETUP_CALLBACK */
#define  MENU_SETTINGS_CONTROLTEXT        12
#define  MENU_SETTINGS_CONTROLTEXT_SUBMENU 13
#define  MENU_SETTINGS_CONTROLTEXT_TITLE1 14      /* callback function: TITLE1_CALLBACK */
#define  MENU_SETTINGS_CONTROLTEXT_TITLE2 15      /* callback function: TITLE2_CALLBACK */
#define  MENU_SETTINGS_CONTROLTEXT_TITLE3 16      /* callback function: TITLE3_CALLBACK */
#define  MENU_SETTINGS_CONTROLTEXT_TITLE4 17      /* callback function: TITLE4_CALLBACK */
#define  MENU_SETTINGS_CONTROLTEXT_TITLE5 18      /* callback function: TITLE5_CALLBACK */
#define  MENU_SETTINGS_CONTROLTEXT_TITLE6 19      /* callback function: TITLE6_CALLBACK */
#define  MENU_SETTINGS_CONTROLTEXT_TITLE7 20      /* callback function: TITLE7_CALLBACK */
#define  MENU_SETTINGS_CONTROLTEXT_TITLE8 21      /* callback function: TITLE8_CALLBACK */
#define  MENU_SETTINGS_CONTROLTEXT_TITLE9 22      /* callback function: TITLE9_CALLBACK */
#define  MENU_SETTINGS_CONTROLTEXT_TITLEX 23      /* callback function: TITLEX_CALLBACK */
#define  MENU_SETTINGS_DEBUG              24      /* callback function: MENU_DEBUG_CALLBACK */
#define  MENU_SETTINGS_BOOTADWIN          25      /* callback function: BOOTADWIN_CALLBACK */
#define  MENU_SETTINGS_CLEARPANEL         26      /* callback function: CLEARPANEL_CALLBACK */
#define  MENU_SETTINGS_RESETZERO          27      /* callback function: RESETZERO_CALLBACK */
#define  MENU_SETTINGS_RESETZERO_SUBMENU  28
#define  MENU_SETTINGS_RESETZERO_SETLOW   29      /* callback function: MENU_ALLLOW_CALLBACK */
#define  MENU_SETTINGS_RESETZERO_HOLD     30      /* callback function: MENU_HOLD_CALLBACK */
#define  MENU_SETTINGS_RESETZERO_BYCHNL   31      /* callback function: MENU_BYCHANNEL_CALLBACK */
#define  MENU_SETTINGS_SCANSETTING        32      /* callback function: SCANSETTING_CALLBACK */
#define  MENU_SETTINGS_NOTECHECK          33      /* callback function: NOTECHECK_CALLBACK */
#define  MENU_EDITMATRIX                  34
#define  MENU_EDITMATRIX_INSERTCOLUMN     35      /* callback function: INSERTCOLUMN_CALLBACK */
#define  MENU_EDITMATRIX_DELETECOLUMN     36      /* callback function: DELETECOLUMN_CALLBACK */
#define  MENU_EDITMATRIX_COPYCOLUMN       37      /* callback function: COPYCOLUMN_CALLBACK */
#define  MENU_EDITMATRIX_PASTECOLUMN      38      /* callback function: PASTECOLUMN_CALLBACK */
#define  MENU_UPDATEPERIOD                39
#define  MENU_UPDATEPERIOD_SETGD5         40      /* callback function: SETGD5_CALLBACK */
#define  MENU_UPDATEPERIOD_SETGD10        41      /* callback function: SETGD10_CALLBACK */
#define  MENU_UPDATEPERIOD_SETGD100       42      /* callback function: SETGD100_CALLBACK */
#define  MENU_UPDATEPERIOD_SETGD1000      43      /* callback function: SETGD1000_CALLBACK */
#define  MENU_PREFS                       44
#define  MENU_PREFS_COMPRESSION           45      /* callback function: COMPRESSION_CALLBACK */
#define  MENU_PREFS_SIMPLETIMING          46      /* callback function: SIMPLETIMING_CALLBACK */
#define  MENU_PREFS_SHOWARRAY             47      /* callback function: SHOWARRAY_CALLBACK */
#define  MENU_PREFS_DDS_OFF               48      /* callback function: DDS_OFF_CALLBACK */
#define  MENU_PREFS_STREAM_SETTINGS       49      /* callback function: STREAM_CALLBACK */


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
void CVICALLBACK DDS_OFF_CALLBACK(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK DDSSETUP_CALLBACK(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK DELETECOLUMN_CALLBACK(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK DIGITALSET_CALLBACK(int menubar, int menuItem, void *callbackData, int panel);
int  CVICALLBACK DIGTABLE_CALLBACK(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK DISPLAYDIAL_CALLBACK(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
void CVICALLBACK EXIT(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK EXPORT_PANEL_CALLBACK(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK INSERTCOLUMN_CALLBACK(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK MENU_ALLLOW_CALLBACK(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK MENU_BYCHANNEL_CALLBACK(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK MENU_CALLBACK(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK MENU_DEBUG_CALLBACK(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK MENU_HOLD_CALLBACK(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK NOTECHECK_CALLBACK(int menubar, int menuItem, void *callbackData, int panel);
int  CVICALLBACK NUM_INSERTIONCOL_CALLBACK(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK NUM_INSERTIONPAGE_CALLBACK(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
void CVICALLBACK PASTECOLUMN_CALLBACK(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK RESETZERO_CALLBACK(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK SCANSETTING_CALLBACK(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK SETGD1000_CALLBACK(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK SETGD100_CALLBACK(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK SETGD10_CALLBACK(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK SETGD5_CALLBACK(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK SHOWARRAY_CALLBACK(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK SIMPLETIMING_CALLBACK(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK STREAM_CALLBACK(int menubar, int menuItem, void *callbackData, int panel);
int  CVICALLBACK SWITCH_LOOP_CALLBACK(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
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
void CVICALLBACK TITLE8_CALLBACK(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK TITLE9_CALLBACK(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK TITLEX_CALLBACK(int menubar, int menuItem, void *callbackData, int panel);
int  CVICALLBACK TOGGLE10_CALLBACK(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK TOGGLE1_CALLBACK(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK TOGGLE2_CALLBACK(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK TOGGLE3_CALLBACK(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK TOGGLE4_CALLBACK(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK TOGGLE5_CALLBACK(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK TOGGLE6_CALLBACK(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK TOGGLE7_CALLBACK(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK TOGGLE8_CALLBACK(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK TOGGLE9_CALLBACK(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif