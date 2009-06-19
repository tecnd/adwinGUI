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

#define  NS_PANEL                        1
#define  NS_PANEL_SETNUM                 2
#define  NS_PANEL_NS_OK                  3       /* callback function: NumSet_Ok */
#define  NS_PANEL_NS_CANCEL              4       /* callback function: NumSet_Cancel */
#define  NS_PANEL_TEXTMSG                5


     /* Menu Bars, Menus, and Menu Items: */

          /* (no menu bars in the resource file) */


     /* Callback Prototypes: */ 

int  CVICALLBACK NumSet_Cancel(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK NumSet_Ok(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif
