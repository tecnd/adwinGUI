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
#define  PANEL_LABEL_6                    12      /* control type: table, callback function: (none) */
#define  PANEL_LABEL_7                    13      /* control type: table, callback function: (none) */
#define  PANEL_LABEL_8                    14      /* control type: table, callback function: (none) */
#define  PANEL_LABEL_9                    15      /* control type: table, callback function: (none) */
#define  PANEL_LABEL_5                    16      /* control type: table, callback function: (none) */
#define  PANEL_LABEL_4                    17      /* control type: table, callback function: (none) */
#define  PANEL_LABEL_3                    18      /* control type: table, callback function: (none) */
#define  PANEL_LABEL_2                    19      /* control type: table, callback function: (none) */
#define  PANEL_LABEL_1                    20      /* control type: table, callback function: (none) */
#define  PANEL_LABEL_10                   21      /* control type: table, callback function: (none) */
#define  PANEL_TIMETABLE                  22      /* control type: table, callback function: (none) */
#define  PANEL_ANALOGTABLE                23      /* control type: table, callback function: (none) */
#define  PANEL_TB_SHOWPHASE10             24      /* control type: textButton, callback function: TOGGLE10_CALLBACK */
#define  PANEL_TB_SHOWPHASE9              25      /* control type: textButton, callback function: TOGGLE9_CALLBACK */
#define  PANEL_DIGTABLE                   26      /* control type: table, callback function: (none) */
#define  PANEL_TB_SHOWPHASE8              27      /* control type: textButton, callback function: TOGGLE8_CALLBACK */
#define  PANEL_TB_SHOWPHASE7              28      /* control type: textButton, callback function: TOGGLE7_CALLBACK */
#define  PANEL_TB_SHOWPHASE6              29      /* control type: textButton, callback function: TOGGLE6_CALLBACK */
#define  PANEL_TB_SHOWPHASE5              30      /* control type: textButton, callback function: TOGGLE5_CALLBACK */
#define  PANEL_TB_SHOWPHASE4              31      /* control type: textButton, callback function: TOGGLE4_CALLBACK */
#define  PANEL_TB_SHOWPHASE3              32      /* control type: textButton, callback function: TOGGLE3_CALLBACK */
#define  PANEL_TB_SHOWPHASE2              33      /* control type: textButton, callback function: TOGGLE2_CALLBACK */
#define  PANEL_TB_SHOWPHASE1              34      /* control type: textButton, callback function: TOGGLE1_CALLBACK */
#define  PANEL_CMD_SCAN                   35      /* control type: command, callback function: (none) */
#define  PANEL_CMD_RUN                    36      /* control type: command, callback function: (none) */
#define  PANEL_DEBUG                      37      /* control type: listBox, callback function: (none) */
#define  PANEL_TOGGLEREPEAT               38      /* control type: textButton, callback function: (none) */
#define  PANEL_CMDSTOP                    39      /* control type: command, callback function: (none) */
#define  PANEL_TGL_NUMERICTABLE           40      /* control type: textButton, callback function: (none) */
#define  PANEL_DISPLAYDIAL                41      /* control type: slide, callback function: (none) */
#define  PANEL_TBL_ANALOGUNITS            42      /* control type: table, callback function: (none) */
#define  PANEL_TBL_DIGNAMES               43      /* control type: table, callback function: (none) */
#define  PANEL_TBL_ANAMES                 44      /* control type: table, callback function: (none) */
#define  PANEL_TIMER                      45      /* control type: timer, callback function: (none) */
#define  PANEL_NUM_INSERTIONCOL           46      /* control type: numeric, callback function: (none) */
#define  PANEL_NUM_INSERTIONPAGE          47      /* control type: numeric, callback function: (none) */
#define  PANEL_NUM_DDS3_OFFSET            48      /* control type: numeric, callback function: (none) */
#define  PANEL_NUM_DDS2_OFFSET            49      /* control type: numeric, callback function: (none) */
#define  PANEL_NUM_DDS_OFFSET             50      /* control type: numeric, callback function: (none) */
#define  PANEL_SCAN_TABLE                 51      /* control type: table, callback function: (none) */
#define  PANEL_CANVAS_END                 52      /* control type: canvas, callback function: (none) */
#define  PANEL_CANVAS_START               53      /* control type: canvas, callback function: (none) */
#define  PANEL_DECORATION                 54      /* control type: deco, callback function: (none) */
#define  PANEL_NUM_LOOP_REPS              55      /* control type: numeric, callback function: (none) */
#define  PANEL_NUM_LOOPCOL2               56      /* control type: numeric, callback function: (none) */
#define  PANEL_NUM_LOOPPAGE2              57      /* control type: numeric, callback function: (none) */
#define  PANEL_NUM_LOOPCOL1               58      /* control type: numeric, callback function: (none) */
#define  PANEL_NUM_LOOPPAGE1              59      /* control type: numeric, callback function: (none) */
#define  PANEL_SWITCH_LOOP                60      /* control type: binary, callback function: (none) */
#define  PANEL_DECORATION_2               61      /* control type: deco, callback function: (none) */
#define  PANEL_CANVAS_LOOPLINE            62      /* control type: canvas, callback function: (none) */
#define  PANEL_Looping                    63      /* control type: textMsg, callback function: (none) */

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
#define  MENU_FILE_QuitCallback           5       /* callback function: EXIT */


     /* Callback Prototypes: */

void CVICALLBACK EXIT(int menubar, int menuItem, void *callbackData, int panel);
void CVICALLBACK MENU_CALLBACK(int menubar, int menuItem, void *callbackData, int panel);
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