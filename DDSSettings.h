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

#define  DDSPANEL                        1
#define  DDSPANEL_NUM_PLLMULTIPLIER      2       /* callback function: SETPLL_CALLBACK */
#define  DDSPANEL_NUM_DDSBASEFREQ        3       /* callback function: SETFREQ_CALLBACK */
#define  DDSPANEL_NUM_DDSCLOCK           4
#define  DDSPANEL_CMD_DDSDONE            5       /* callback function: DDSDONE_CALLBACK */
#define  DDSPANEL_NUM_DDS3_CLOCK         6
#define  DDSPANEL_NUM_DDS2_CLOCK         7


     /* Menu Bars, Menus, and Menu Items: */

          /* (no menu bars in the resource file) */


     /* Callback Prototypes: */ 

int  CVICALLBACK DDSDONE_CALLBACK(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK SETFREQ_CALLBACK(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK SETPLL_CALLBACK(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif
