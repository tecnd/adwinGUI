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

#define  PANEL                           1
#define  PANEL_CHECKBOX_10               2
#define  PANEL_CHECKBOX_9                3
#define  PANEL_CHECKBOX_8                4
#define  PANEL_CHECKBOX_7                5
#define  PANEL_CHECKBOX_6                6
#define  PANEL_CHECKBOX_5                7
#define  PANEL_CHECKBOX_4                8
#define  PANEL_CHECKBOX_3                9
#define  PANEL_CHECKBOX_2                10
#define  PANEL_CHECKBOX                  11
#define  PANEL_LABEL_7                   12
#define  PANEL_LABEL_6                   13
#define  PANEL_LABEL_10                  14
#define  PANEL_LABEL_9                   15
#define  PANEL_LABEL_8                   16
#define  PANEL_LABEL_5                   17
#define  PANEL_LABEL_4                   18
#define  PANEL_LABEL_3                   19
#define  PANEL_LABEL_2                   20
#define  PANEL_LABEL_1                   21
#define  PANEL_TIMETABLE                 22      /* callback function: TIMETABLE_CALLBACK */
#define  PANEL_ANALOGTABLE               23      /* callback function: ANALOGTABLE_CALLBACK */
#define  PANEL_TB_SHOWPHASE10            24      /* callback function: TOGGLE10_CALLBACK */
#define  PANEL_QUITBUTTON                25      /* callback function: QuitCallback */
#define  PANEL_TB_SHOWPHASE9             26      /* callback function: TOGGLE9_CALLBACK */
#define  PANEL_DIGTABLE                  27      /* callback function: DIGTABLE_CALLBACK */
#define  PANEL_TB_SHOWPHASE8             28      /* callback function: TOGGLE8_CALLBACK */
#define  PANEL_TB_SHOWPHASE7             29      /* callback function: TOGGLE7_CALLBACK */
#define  PANEL_TB_SHOWPHASE6             30      /* callback function: TOGGLE6_CALLBACK */
#define  PANEL_TB_SHOWPHASE5             31      /* callback function: TOGGLE5_CALLBACK */
#define  PANEL_TB_SHOWPHASE4             32      /* callback function: TOGGLE4_CALLBACK */
#define  PANEL_TB_SHOWPHASE3             33      /* callback function: TOGGLE3_CALLBACK */
#define  PANEL_TB_SHOWPHASE2             34      /* callback function: TOGGLE2_CALLBACK */
#define  PANEL_TB_SHOWPHASE1             35      /* callback function: TOGGLE1_CALLBACK */
#define  PANEL_CMD_SCAN                  36      /* callback function: CMD_SCAN_CALLBACK */
#define  PANEL_CMD_RUN                   37      /* callback function: CMD_RUN_CALLBACK */
#define  PANEL_DEBUG                     38
#define  PANEL_TOGGLEREPEAT              39
#define  PANEL_CMDSTOP                   40      /* callback function: CMDSTOP_CALLBACK */
#define  PANEL_TGL_NUMERICTABLE          41      /* callback function: TGLNUMERIC_CALLBACK */
#define  PANEL_DISPLAYDIAL               42      /* callback function: DISPLAYDIAL_CALLBACK */
#define  PANEL_TBL_ANALOGUNITS           43
#define  PANEL_TBL_DIGNAMES              44
#define  PANEL_TBL_ANAMES                45
#define  PANEL_TIMER                     46      /* callback function: TIMER_CALLBACK */
#define  PANEL_NUM_INSERTIONCOL          47      /* callback function: NUM_INSERTIONCOL_CALLBACK */
#define  PANEL_NUM_INSERTIONPAGE         48      /* callback function: NUM_INSERTIONPAGE_CALLBACK */
#define  PANEL_NUM_DDS3_OFFSET           49
#define  PANEL_NUM_DDS2_OFFSET           50
#define  PANEL_NUM_DDS_OFFSET            51
#define  PANEL_LABNOTE_TXT               52
#define  PANEL_NUM_SCANITER              53
#define  PANEL_NUM_SCANSTEP              54
#define  PANEL_NUM_SCANVAL               55
#define  PANEL_SCAN_TABLE                56
#define  PANEL_DECORATION_BOX            57


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
#define  MENU_SETTINGS_CONTROLTEXT_TITLE8 21     /* callback function: TITLE8_CALLBACK */
#define  MENU_SETTINGS_CONTROLTEXT_TITLE9 22     /* callback function: TITLE9_CALLBACK */
#define  MENU_SETTINGS_CONTROLTEXT_TITLEX 23     /* callback function: TITLEX_CALLBACK */
#define  MENU_SETTINGS_DEBUG             24      /* callback function: MENU_DEBUG_CALLBACK */
#define  MENU_SETTINGS_BOOTADWIN         25      /* callback function: BOOTADWIN_CALLBACK */
#define  MENU_SETTINGS_CLEARPANEL        26      /* callback function: CLEARPANEL_CALLBACK */
#define  MENU_SETTINGS_RESETZERO         27      /* callback function: RESETZERO_CALLBACK */
#define  MENU_SETTINGS_RESETZERO_SUBMENU 28
#define  MENU_SETTINGS_RESETZERO_SETLOW  29      /* callback function: MENU_ALLLOW_CALLBACK */
#define  MENU_SETTINGS_RESETZERO_HOLD    30      /* callback function: MENU_HOLD_CALLBACK */
#define  MENU_SETTINGS_RESETZERO_BYCHNL  31      /* callback function: MENU_BYCHANNEL_CALLBACK */
#define  MENU_SETTINGS_SCANSETTING       32      /* callback function: SCANSETTING_CALLBACK */
#define  MENU_SETTINGS_NOTECHECK         33      /* callback function: NOTECHECK_CALLBACK */
#define  MENU_EDITMATRIX                 34
#define  MENU_EDITMATRIX_INSERTCOLUMN    35      /* callback function: INSERTCOLUMN_CALLBACK */
#define  MENU_EDITMATRIX_DELETECOLUMN    36      /* callback function: DELETECOLUMN_CALLBACK */
#define  MENU_EDITMATRIX_COPYCOLUMN      37      /* callback function: COPYCOLUMN_CALLBACK */
#define  MENU_EDITMATRIX_PASTECOLUMN     38      /* callback function: PASTECOLUMN_CALLBACK */
#define  MENU_UPDATEPERIOD               39
#define  MENU_UPDATEPERIOD_SETGD5        40      /* callback function: SETGD5_CALLBACK */
#define  MENU_UPDATEPERIOD_SETGD10       41      /* callback function: SETGD10_CALLBACK */
#define  MENU_UPDATEPERIOD_SETGD100      42      /* callback function: SETGD100_CALLBACK */
#define  MENU_UPDATEPERIOD_SETGD1000     43      /* callback function: SETGD1000_CALLBACK */
#define  MENU_PREFS                      44
#define  MENU_PREFS_COMPRESSION          45      /* callback function: COMPRESSION_CALLBACK */
#define  MENU_PREFS_SIMPLETIMING         46      /* callback function: SIMPLETIMING_CALLBACK */
#define  MENU_PREFS_SHOWARRAY            47      /* callback function: SHOWARRAY_CALLBACK */
#define  MENU_PREFS_DDS_OFF              48      /* callback function: DDS_OFF_CALLBACK */
#define  MENU_PREFS_STREAM_SETTINGS      49      /* callback function: STREAM_CALLBACK */


     /* Callback Prototypes: */ 

void CVICALLBACK ANALOGSET_CALLBACK(int menubar, int menuItem, void *callbackData, int panel);
int  CVICALLBACK ANALOGTABLE_CALLBACK(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
void CVICALLBACK BOOTADWIN_CALLBACK(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK BOOTLOAD_CALLBACK(int menubar, int menuItem, void *callbackData, int panel);
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
int  CVICALLBACK QuitCallback(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
void CVICALLBACK RESETZERO_CALLBACK(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK SCANSETTING_CALLBACK(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK SETGD1000_CALLBACK(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK SETGD100_CALLBACK(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK SETGD10_CALLBACK(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK SETGD5_CALLBACK(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK SHOWARRAY_CALLBACK(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK SIMPLETIMING_CALLBACK(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK STREAM_CALLBACK(int menubar, int menuItem, void *callbackData, int panel);
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
