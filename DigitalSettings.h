/**************************************************************************/
/* LabWindows/CVI User Interface Resource (UIR) Include File              */
/* Copyright (c) National Instruments 2004. All Rights Reserved.          */
/*                                                                        */
/* WARNING: Do not add to, delete from, or otherwise modify the contents  */
/*          of this include file.                                         */
/**************************************************************************/

#include <userint.h>

#ifdef __cplusplus
    extern "C" {
#endif

     /* Panels and Controls: */

#define  DIGPANEL                        1
#define  DIGPANEL_STR_DIGCHANNELNAME     2
#define  DIGPANEL_NUM_DIGCH_LINE         3       /* callback function: NUM_DIG_LINE_CALLBACK */
#define  DIGPANEL_NUM_DIGCHANNEL         4
#define  DIGPANEL_CMD_DIGALLOWCHANGE     5       /* callback function: CMD_DIGALLOWCHANGE_CALLBACK */
#define  DIGPANEL_CMD_DIGSETCHANGES      6       /* callback function: CMD_DIGSETCHANGES_CALLBACK */
#define  DIGPANEL_CMD_DONEDIG            7       /* callback function: CMD_DONEDIG_CALLBACK */
#define  DIGPANEL_CHK_DIGRESET           8       /* callback function: CHKDIGRESET_CALLBACK */


     /* Menu Bars, Menus, and Menu Items: */

          /* (no menu bars in the resource file) */


     /* Callback Prototypes: */ 

int  CVICALLBACK CHKDIGRESET_CALLBACK(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK CMD_DIGALLOWCHANGE_CALLBACK(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK CMD_DIGSETCHANGES_CALLBACK(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK CMD_DONEDIG_CALLBACK(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK NUM_DIG_LINE_CALLBACK(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif
