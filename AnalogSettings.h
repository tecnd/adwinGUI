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

#define  ANLGPANEL                        1
#define  ANLGPANEL_NUM_ACHANNEL           2       /* control type: numeric, callback function: (none) */
#define  ANLGPANEL_STR_CHANNELNAME        3       /* control type: string, callback function: (none) */
#define  ANLGPANEL_STRING_UNITS           4       /* control type: string, callback function: (none) */
#define  ANLGPANEL_NUM_CHANNELBIAS        5       /* control type: numeric, callback function: (none) */
#define  ANLGPANEL_NUM_CHANNELPROP        6       /* control type: numeric, callback function: (none) */
#define  ANLGPANEL_CHKBOX_RESET           7       /* control type: radioButton, callback function: (none) */
#define  ANLGPANEL_NUM_MAXV               8       /* control type: numeric, callback function: (none) */
#define  ANLGPANEL_NUM_MINV               9       /* control type: numeric, callback function: (none) */
#define  ANLGPANEL_NUM_ACH_LINE           10      /* control type: numeric, callback function: NUM_ACH_LINE_CALLBACK */
#define  ANLGPANEL_CMD_ALLOWCHANGE        11      /* control type: command, callback function: CMD_ALLOWCHANGE_CALLBACK */
#define  ANLGPANEL_CMD_SETCHANGES         12      /* control type: command, callback function: CMD_SETCHANGES_CALLBACK */
#define  ANLGPANEL_CMD_DONEANALOG         13      /* control type: command, callback function: CMD_DONEANALOG_CALLBACK */


     /* Control Arrays: */

          /* (no control arrays in the resource file) */


     /* Menu Bars, Menus, and Menu Items: */

          /* (no menu bars in the resource file) */


     /* Callback Prototypes: */

int  CVICALLBACK CMD_ALLOWCHANGE_CALLBACK(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK CMD_DONEANALOG_CALLBACK(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK CMD_SETCHANGES_CALLBACK(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK NUM_ACH_LINE_CALLBACK(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif