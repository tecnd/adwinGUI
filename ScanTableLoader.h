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

#define  SL_PANEL                        1
#define  SL_PANEL_SCAN_TYPE              2
#define  SL_PANEL_SL_OK                  3       /* callback function: SCAN_LOAD_OK */
#define  SL_PANEL_SL_CANCEL              4       /* callback function: SCAN_LOAD_CANCEL */
#define  SL_PANEL_ITER_NUM               5
#define  SL_PANEL_SCAN_INIT              6
#define  SL_PANEL_SCAN_FIN               7


     /* Menu Bars, Menus, and Menu Items: */

          /* (no menu bars in the resource file) */


     /* Callback Prototypes: */ 

int  CVICALLBACK SCAN_LOAD_CANCEL(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK SCAN_LOAD_OK(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif
