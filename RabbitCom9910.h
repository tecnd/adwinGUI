/**************************************************************************/
/* LabWindows/CVI User Interface Resource (UIR) Include File              */
/* Copyright (c) National Instruments 2008. All Rights Reserved.          */
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
#define  PANEL_SENDBUTTON                2       /* callback function: Sendfreq */
#define  PANEL_FREQUENCY1                3
#define  PANEL_FREQUENCY2                4
#define  PANEL_QUITBUTTON                5       /* callback function: Shutdown */
#define  PANEL_CLEARBUTTON               6       /* callback function: Clear */
#define  PANEL_FINALIZEBUTTON            7       /* callback function: Finalize */
#define  PANEL_SETUPDDSBUTTON            8       /* callback function: SetupDDS */
#define  PANEL_SWEEPTIME                 9
#define  PANEL_TEXTMSG                   10
#define  PANEL_TEXTMSG_2                 11


     /* Menu Bars, Menus, and Menu Items: */

          /* (no menu bars in the resource file) */


     /* Callback Prototypes: */ 

int  CVICALLBACK Clear(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK Finalize(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK Sendfreq(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK SetupDDS(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK Shutdown(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif
