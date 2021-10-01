
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
//
//  "testTable.h"
//
//  Declaration and include file for UI build functions
//

//  Date: 10-01-2021
//

//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------

#ifndef _TESTTABLE_INCLUDED
#define _TESTTABLE_INCLUDED

#ifdef __cplusplus
    extern "C" {            /* Assume C declarations for C++ */
#endif  /* __cplusplus */

//----------------------------------------------------------------------------------------------------
//  Include required headers
//----------------------------------------------------------------------------------------------------

#include <cvidef.h> // definition of CVICALLBACK

//----------------------------------------------------------------------------------------------------
//  Prototype UI build functions.
//----------------------------------------------------------------------------------------------------

int BuildPANEL (int hParentPanel);
int BuildSUBPANEL1 (int hParentPanel);
int BuildSUBPANEL2 (int hParentPanel);
int BuildMENU (int hParentPanel);

//-------------------------------------------------------------------------------------------------
// Prototype callbacks for Panel: PANEL
// ------------------------------------------------------------------------------------------------

// Control: PANEL_TIMETABLE
int CVICALLBACK TIMETABLE_CALLBACK (int panelHandle, int control, int event, void *callbackData, int eventData1, int eventData2);

// Control: PANEL_ANALOGTABLE
int CVICALLBACK ANALOGTABLE_CALLBACK (int panelHandle, int control, int event, void *callbackData, int eventData1, int eventData2);

// Control: PANEL_TB_SHOWPHASE10
int CVICALLBACK TOGGLE10_CALLBACK (int panelHandle, int control, int event, void *callbackData, int eventData1, int eventData2);

// Control: PANEL_TB_SHOWPHASE9
int CVICALLBACK TOGGLE9_CALLBACK (int panelHandle, int control, int event, void *callbackData, int eventData1, int eventData2);

// Control: PANEL_DIGTABLE
int CVICALLBACK DIGTABLE_CALLBACK (int panelHandle, int control, int event, void *callbackData, int eventData1, int eventData2);

// Control: PANEL_TB_SHOWPHASE8
int CVICALLBACK TOGGLE8_CALLBACK (int panelHandle, int control, int event, void *callbackData, int eventData1, int eventData2);

// Control: PANEL_TB_SHOWPHASE7
int CVICALLBACK TOGGLE7_CALLBACK (int panelHandle, int control, int event, void *callbackData, int eventData1, int eventData2);

// Control: PANEL_TB_SHOWPHASE6
int CVICALLBACK TOGGLE6_CALLBACK (int panelHandle, int control, int event, void *callbackData, int eventData1, int eventData2);

// Control: PANEL_TB_SHOWPHASE5
int CVICALLBACK TOGGLE5_CALLBACK (int panelHandle, int control, int event, void *callbackData, int eventData1, int eventData2);

// Control: PANEL_TB_SHOWPHASE4
int CVICALLBACK TOGGLE4_CALLBACK (int panelHandle, int control, int event, void *callbackData, int eventData1, int eventData2);

// Control: PANEL_TB_SHOWPHASE3
int CVICALLBACK TOGGLE3_CALLBACK (int panelHandle, int control, int event, void *callbackData, int eventData1, int eventData2);

// Control: PANEL_TB_SHOWPHASE2
int CVICALLBACK TOGGLE2_CALLBACK (int panelHandle, int control, int event, void *callbackData, int eventData1, int eventData2);

// Control: PANEL_TB_SHOWPHASE1
int CVICALLBACK TOGGLE1_CALLBACK (int panelHandle, int control, int event, void *callbackData, int eventData1, int eventData2);

// Control: PANEL_CMD_SCAN
int CVICALLBACK CMD_SCAN_CALLBACK (int panelHandle, int control, int event, void *callbackData, int eventData1, int eventData2);

// Control: PANEL_CMD_RUN
int CVICALLBACK CMD_RUN_CALLBACK (int panelHandle, int control, int event, void *callbackData, int eventData1, int eventData2);

// Control: PANEL_CMDSTOP
int CVICALLBACK CMDSTOP_CALLBACK (int panelHandle, int control, int event, void *callbackData, int eventData1, int eventData2);

// Control: PANEL_TGL_NUMERICTABLE
int CVICALLBACK TGLNUMERIC_CALLBACK (int panelHandle, int control, int event, void *callbackData, int eventData1, int eventData2);

// Control: PANEL_DISPLAYDIAL
int CVICALLBACK DISPLAYDIAL_CALLBACK (int panelHandle, int control, int event, void *callbackData, int eventData1, int eventData2);

// Control: PANEL_TIMER
int CVICALLBACK TIMER_CALLBACK (int panelHandle, int control, int event, void *callbackData, int eventData1, int eventData2);

// Control: PANEL_NUM_INSERTIONCOL
int CVICALLBACK NUM_INSERTIONCOL_CALLBACK (int panelHandle, int control, int event, void *callbackData, int eventData1, int eventData2);

// Control: PANEL_NUM_INSERTIONPAGE
int CVICALLBACK NUM_INSERTIONPAGE_CALLBACK (int panelHandle, int control, int event, void *callbackData, int eventData1, int eventData2);

// Control: PANEL_SWITCH_LOOP
int CVICALLBACK SWITCH_LOOP_CALLBACK (int panelHandle, int control, int event, void *callbackData, int eventData1, int eventData2);

//-------------------------------------------------------------------------------------------------
// Prototype callbacks for Panel: SUBPANEL1
// ------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
// Prototype callbacks for Panel: SUBPANEL2
// ------------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------------
// Prototype callbacks for Menubar: MENU
// ------------------------------------------------------------------------------------------------

// Menu item: MENU_FILE_LOADSET
void CVICALLBACK MENU_CALLBACK (int menubarHandle, int menuItemID, void *callbackPtr, int panelHandle);

// Menu item: MENU_FILE_SAVESET
void CVICALLBACK MENU_CALLBACK (int menubarHandle, int menuItemID, void *callbackPtr, int panelHandle);

// Menu item: MENU_FILE_MENU_EXPORT_CHANNEL
void CVICALLBACK CONFIG_EXPORT_CALLBACK (int menubarHandle, int menuItemID, void *callbackPtr, int panelHandle);

// Menu item: MENU_FILE_MENU_EXPORT_PANEL
void CVICALLBACK EXPORT_PANEL_CALLBACK (int menubarHandle, int menuItemID, void *callbackPtr, int panelHandle);

// Menu item: MENU_FILE_QuitCallback
void CVICALLBACK EXIT (int menubarHandle, int menuItemID, void *callbackPtr, int panelHandle);

// Menu item: MENU_SETTINGS_ANALOG
void CVICALLBACK ANALOGSET_CALLBACK (int menubarHandle, int menuItemID, void *callbackPtr, int panelHandle);

// Menu item: MENU_SETTINGS_DIGITAL
void CVICALLBACK DIGITALSET_CALLBACK (int menubarHandle, int menuItemID, void *callbackPtr, int panelHandle);

// Menu item: MENU_SETTINGS_DDSSETUP
void CVICALLBACK DDSSETUP_CALLBACK (int menubarHandle, int menuItemID, void *callbackPtr, int panelHandle);

// Menu item: MENU_SETTINGS_CONTROLTEXT_TITLE1
void CVICALLBACK TITLE1_CALLBACK (int menubarHandle, int menuItemID, void *callbackPtr, int panelHandle);

// Menu item: MENU_SETTINGS_CONTROLTEXT_TITLE2
void CVICALLBACK TITLE2_CALLBACK (int menubarHandle, int menuItemID, void *callbackPtr, int panelHandle);

// Menu item: MENU_SETTINGS_CONTROLTEXT_TITLE3
void CVICALLBACK TITLE3_CALLBACK (int menubarHandle, int menuItemID, void *callbackPtr, int panelHandle);

// Menu item: MENU_SETTINGS_CONTROLTEXT_TITLE4
void CVICALLBACK TITLE4_CALLBACK (int menubarHandle, int menuItemID, void *callbackPtr, int panelHandle);

// Menu item: MENU_SETTINGS_CONTROLTEXT_TITLE5
void CVICALLBACK TITLE5_CALLBACK (int menubarHandle, int menuItemID, void *callbackPtr, int panelHandle);

// Menu item: MENU_SETTINGS_CONTROLTEXT_TITLE6
void CVICALLBACK TITLE6_CALLBACK (int menubarHandle, int menuItemID, void *callbackPtr, int panelHandle);

// Menu item: MENU_SETTINGS_CONTROLTEXT_TITLE7
void CVICALLBACK TITLE7_CALLBACK (int menubarHandle, int menuItemID, void *callbackPtr, int panelHandle);

// Menu item: MENU_SETTINGS_CONTROLTEXT_TITLE8
void CVICALLBACK TITLE8_CALLBACK (int menubarHandle, int menuItemID, void *callbackPtr, int panelHandle);

// Menu item: MENU_SETTINGS_CONTROLTEXT_TITLE9
void CVICALLBACK TITLE9_CALLBACK (int menubarHandle, int menuItemID, void *callbackPtr, int panelHandle);

// Menu item: MENU_SETTINGS_CONTROLTEXT_TITLEX
void CVICALLBACK TITLEX_CALLBACK (int menubarHandle, int menuItemID, void *callbackPtr, int panelHandle);

// Menu item: MENU_SETTINGS_DEBUG
void CVICALLBACK MENU_DEBUG_CALLBACK (int menubarHandle, int menuItemID, void *callbackPtr, int panelHandle);

// Menu item: MENU_SETTINGS_BOOTADWIN
void CVICALLBACK BOOTADWIN_CALLBACK (int menubarHandle, int menuItemID, void *callbackPtr, int panelHandle);

// Menu item: MENU_SETTINGS_CLEARPANEL
void CVICALLBACK CLEARPANEL_CALLBACK (int menubarHandle, int menuItemID, void *callbackPtr, int panelHandle);

// Menu item: MENU_SETTINGS_RESETZERO
void CVICALLBACK RESETZERO_CALLBACK (int menubarHandle, int menuItemID, void *callbackPtr, int panelHandle);

// Menu item: MENU_SETTINGS_RESETZERO_SETLOW
void CVICALLBACK MENU_ALLLOW_CALLBACK (int menubarHandle, int menuItemID, void *callbackPtr, int panelHandle);

// Menu item: MENU_SETTINGS_RESETZERO_HOLD
void CVICALLBACK MENU_HOLD_CALLBACK (int menubarHandle, int menuItemID, void *callbackPtr, int panelHandle);

// Menu item: MENU_SETTINGS_RESETZERO_BYCHNL
void CVICALLBACK MENU_BYCHANNEL_CALLBACK (int menubarHandle, int menuItemID, void *callbackPtr, int panelHandle);

// Menu item: MENU_SETTINGS_SCANSETTING
void CVICALLBACK SCANSETTING_CALLBACK (int menubarHandle, int menuItemID, void *callbackPtr, int panelHandle);

// Menu item: MENU_SETTINGS_NOTECHECK
void CVICALLBACK NOTECHECK_CALLBACK (int menubarHandle, int menuItemID, void *callbackPtr, int panelHandle);

// Menu item: MENU_EDITMATRIX_INSERTCOLUMN
void CVICALLBACK INSERTCOLUMN_CALLBACK (int menubarHandle, int menuItemID, void *callbackPtr, int panelHandle);

// Menu item: MENU_EDITMATRIX_DELETECOLUMN
void CVICALLBACK DELETECOLUMN_CALLBACK (int menubarHandle, int menuItemID, void *callbackPtr, int panelHandle);

// Menu item: MENU_EDITMATRIX_COPYCOLUMN
void CVICALLBACK COPYCOLUMN_CALLBACK (int menubarHandle, int menuItemID, void *callbackPtr, int panelHandle);

// Menu item: MENU_EDITMATRIX_PASTECOLUMN
void CVICALLBACK PASTECOLUMN_CALLBACK (int menubarHandle, int menuItemID, void *callbackPtr, int panelHandle);

// Menu item: MENU_UPDATEPERIOD_SETGD5
void CVICALLBACK SETGD5_CALLBACK (int menubarHandle, int menuItemID, void *callbackPtr, int panelHandle);

// Menu item: MENU_UPDATEPERIOD_SETGD10
void CVICALLBACK SETGD10_CALLBACK (int menubarHandle, int menuItemID, void *callbackPtr, int panelHandle);

// Menu item: MENU_UPDATEPERIOD_SETGD100
void CVICALLBACK SETGD100_CALLBACK (int menubarHandle, int menuItemID, void *callbackPtr, int panelHandle);

// Menu item: MENU_UPDATEPERIOD_SETGD1000
void CVICALLBACK SETGD1000_CALLBACK (int menubarHandle, int menuItemID, void *callbackPtr, int panelHandle);

// Menu item: MENU_PREFS_COMPRESSION
void CVICALLBACK COMPRESSION_CALLBACK (int menubarHandle, int menuItemID, void *callbackPtr, int panelHandle);

// Menu item: MENU_PREFS_SIMPLETIMING
void CVICALLBACK SIMPLETIMING_CALLBACK (int menubarHandle, int menuItemID, void *callbackPtr, int panelHandle);

// Menu item: MENU_PREFS_SHOWARRAY
void CVICALLBACK SHOWARRAY_CALLBACK (int menubarHandle, int menuItemID, void *callbackPtr, int panelHandle);

// Menu item: MENU_PREFS_DDS_OFF
void CVICALLBACK DDS_OFF_CALLBACK (int menubarHandle, int menuItemID, void *callbackPtr, int panelHandle);

// Menu item: MENU_PREFS_STREAM_SETTINGS
void CVICALLBACK STREAM_CALLBACK (int menubarHandle, int menuItemID, void *callbackPtr, int panelHandle);


//-------------------------------------------------------------------------------------------------
// Declare identifiers for Panel:  PANEL
// ------------------------------------------------------------------------------------------------

extern int PANEL_CHECKBOX_10                        ;     // control identifier
extern int PANEL_CHECKBOX_9                         ;     // control identifier
extern int PANEL_CHECKBOX_8                         ;     // control identifier
extern int PANEL_CHECKBOX_7                         ;     // control identifier
extern int PANEL_CHECKBOX_6                         ;     // control identifier
extern int PANEL_CHECKBOX_5                         ;     // control identifier
extern int PANEL_CHECKBOX_4                         ;     // control identifier
extern int PANEL_CHECKBOX_3                         ;     // control identifier
extern int PANEL_CHECKBOX_2                         ;     // control identifier
extern int PANEL_CHECKBOX                           ;     // control identifier
extern int PANEL_LABEL_6                            ;     // control identifier
extern int PANEL_LABEL_7                            ;     // control identifier
extern int PANEL_LABEL_8                            ;     // control identifier
extern int PANEL_LABEL_9                            ;     // control identifier
extern int PANEL_LABEL_5                            ;     // control identifier
extern int PANEL_LABEL_4                            ;     // control identifier
extern int PANEL_LABEL_3                            ;     // control identifier
extern int PANEL_LABEL_2                            ;     // control identifier
extern int PANEL_LABEL_1                            ;     // control identifier
extern int PANEL_LABEL_10                           ;     // control identifier
extern int PANEL_TIMETABLE                          ;     // control identifier
extern int PANEL_ANALOGTABLE                        ;     // control identifier
extern int PANEL_TB_SHOWPHASE10                     ;     // control identifier
extern int PANEL_TB_SHOWPHASE9                      ;     // control identifier
extern int PANEL_DIGTABLE                           ;     // control identifier
extern int PANEL_TB_SHOWPHASE8                      ;     // control identifier
extern int PANEL_TB_SHOWPHASE7                      ;     // control identifier
extern int PANEL_TB_SHOWPHASE6                      ;     // control identifier
extern int PANEL_TB_SHOWPHASE5                      ;     // control identifier
extern int PANEL_TB_SHOWPHASE4                      ;     // control identifier
extern int PANEL_TB_SHOWPHASE3                      ;     // control identifier
extern int PANEL_TB_SHOWPHASE2                      ;     // control identifier
extern int PANEL_TB_SHOWPHASE1                      ;     // control identifier
extern int PANEL_CMD_SCAN                           ;     // control identifier
extern int PANEL_CMD_RUN                            ;     // control identifier
extern int PANEL_DEBUG                              ;     // control identifier
extern int PANEL_TOGGLEREPEAT                       ;     // control identifier
extern int PANEL_CMDSTOP                            ;     // control identifier
extern int PANEL_TGL_NUMERICTABLE                   ;     // control identifier
extern int PANEL_DISPLAYDIAL                        ;     // control identifier
extern int PANEL_TBL_ANALOGUNITS                    ;     // control identifier
extern int PANEL_TBL_DIGNAMES                       ;     // control identifier
extern int PANEL_TBL_ANAMES                         ;     // control identifier
extern int PANEL_TIMER                              ;     // control identifier
extern int PANEL_NUM_INSERTIONCOL                   ;     // control identifier
extern int PANEL_NUM_INSERTIONPAGE                  ;     // control identifier
extern int PANEL_NUM_DDS3_OFFSET                    ;     // control identifier
extern int PANEL_NUM_DDS2_OFFSET                    ;     // control identifier
extern int PANEL_NUM_DDS_OFFSET                     ;     // control identifier
extern int PANEL_SCAN_TABLE                         ;     // control identifier
extern int PANEL_CANVAS_END                         ;     // control identifier
extern int PANEL_CANVAS_START                       ;     // control identifier
extern int PANEL_DECORATION                         ;     // control identifier
extern int PANEL_NUM_LOOP_REPS                      ;     // control identifier
extern int PANEL_NUM_LOOPCOL2                       ;     // control identifier
extern int PANEL_NUM_LOOPPAGE2                      ;     // control identifier
extern int PANEL_NUM_LOOPCOL1                       ;     // control identifier
extern int PANEL_NUM_LOOPPAGE1                      ;     // control identifier
extern int PANEL_SWITCH_LOOP                        ;     // control identifier
extern int PANEL_DECORATION_2                       ;     // control identifier
extern int PANEL_CANVAS_LOOPLINE                    ;     // control identifier
extern int PANEL_Looping                            ;     // control identifier

//-------------------------------------------------------------------------------------------------
// Declare identifiers for Panel:  SUBPANEL1
// ------------------------------------------------------------------------------------------------

extern int SUBPANEL1_TEXTBOX                        ;     // control identifier

//-------------------------------------------------------------------------------------------------
// Declare identifiers for Panel:  SUBPANEL2
// ------------------------------------------------------------------------------------------------

extern int SUBPANEL2_NUM_SCANITER                   ;     // control identifier
extern int SUBPANEL2_NUM_SCANSTEP                   ;     // control identifier
extern int SUBPANEL2_NUM_SCANVAL                    ;     // control identifier

//-------------------------------------------------------------------------------------------------
// Declare identifiers for Menubar:  MENU
// ------------------------------------------------------------------------------------------------

extern int MENU_FILE                                ;     // menu identifier
extern int MENU_FILE_LOADSET                        ;     // menu item identifier
extern int MENU_FILE_SAVESET                        ;     // menu item identifier
extern int MENU_FILE_MENU_EXPORT_CHANNEL            ;     // menu item identifier
extern int MENU_FILE_MENU_EXPORT_PANEL              ;     // menu item identifier
extern int MENU_FILE_QuitCallback                   ;     // menu item identifier
extern int MENU_SETTINGS                            ;     // menu identifier
extern int MENU_SETTINGS_ANALOG                     ;     // menu item identifier
extern int MENU_SETTINGS_DIGITAL                    ;     // menu item identifier
extern int MENU_SETTINGS_DDSSETUP                   ;     // menu item identifier
extern int MENU_SETTINGS_CONTROLTEXT                ;     // menu item identifier
extern int MENU_SETTINGS_CONTROLTEXT_SUB            ;     // submenu identifier 
extern int MENU_SETTINGS_CONTROLTEXT_TITLE1         ;     // menu item identifier
extern int MENU_SETTINGS_CONTROLTEXT_TITLE2         ;     // menu item identifier
extern int MENU_SETTINGS_CONTROLTEXT_TITLE3         ;     // menu item identifier
extern int MENU_SETTINGS_CONTROLTEXT_TITLE4         ;     // menu item identifier
extern int MENU_SETTINGS_CONTROLTEXT_TITLE5         ;     // menu item identifier
extern int MENU_SETTINGS_CONTROLTEXT_TITLE6         ;     // menu item identifier
extern int MENU_SETTINGS_CONTROLTEXT_TITLE7         ;     // menu item identifier
extern int MENU_SETTINGS_CONTROLTEXT_TITLE8         ;     // menu item identifier
extern int MENU_SETTINGS_CONTROLTEXT_TITLE9         ;     // menu item identifier
extern int MENU_SETTINGS_CONTROLTEXT_TITLEX         ;     // menu item identifier
extern int MENU_SETTINGS_DEBUG                      ;     // menu item identifier
extern int MENU_SETTINGS_BOOTADWIN                  ;     // menu item identifier
extern int MENU_SETTINGS_CLEARPANEL                 ;     // menu item identifier
extern int MENU_SETTINGS_RESETZERO                  ;     // menu item identifier
extern int MENU_SETTINGS_RESETZERO_SUB              ;     // submenu identifier 
extern int MENU_SETTINGS_RESETZERO_SETLOW           ;     // menu item identifier
extern int MENU_SETTINGS_RESETZERO_HOLD             ;     // menu item identifier
extern int MENU_SETTINGS_RESETZERO_BYCHNL           ;     // menu item identifier
extern int MENU_SETTINGS_SCANSETTING                ;     // menu item identifier
extern int MENU_SETTINGS_NOTECHECK                  ;     // menu item identifier
extern int MENU_EDITMATRIX                          ;     // menu identifier
extern int MENU_EDITMATRIX_INSERTCOLUMN             ;     // menu item identifier
extern int MENU_EDITMATRIX_DELETECOLUMN             ;     // menu item identifier
extern int MENU_EDITMATRIX_COPYCOLUMN               ;     // menu item identifier
extern int MENU_EDITMATRIX_PASTECOLUMN              ;     // menu item identifier
extern int MENU_UPDATEPERIOD                        ;     // menu identifier
extern int MENU_UPDATEPERIOD_SETGD5                 ;     // menu item identifier
extern int MENU_UPDATEPERIOD_SETGD10                ;     // menu item identifier
extern int MENU_UPDATEPERIOD_SETGD100               ;     // menu item identifier
extern int MENU_UPDATEPERIOD_SETGD1000              ;     // menu item identifier
extern int MENU_PREFS                               ;     // menu identifier
extern int MENU_PREFS_COMPRESSION                   ;     // menu item identifier
extern int MENU_PREFS_SIMPLETIMING                  ;     // menu item identifier
extern int MENU_PREFS_SHOWARRAY                     ;     // menu item identifier
extern int MENU_PREFS_DDS_OFF                       ;     // menu item identifier
extern int MENU_PREFS_STREAM_SETTINGS               ;     // menu item identifier

#ifdef __cplusplus
    }
#endif

#endif // _TESTTABLE_INCLUDED