/**
@file AnalogSettings.c
@brief Sets the AnalogChannelProperties for each analog line.
*/
#include "AnalogSettings.h"

#include <ansi_c.h>
#include <userint.h>

#include "AnalogSettings2.h"
#include "GUIDesign.h"
#include "vars.h"

/**
@brief Callback for the Allow Changes button. Switches components from Indicator
to Hot so changes can be made. Adds flag that values have been changed so ADwin
array is rebuilt on next run.
*/
int CVICALLBACK CMD_ALLOWCHANGE_CALLBACK(int panel, int control, int event,
                                         void* callbackData, int eventData1,
                                         int eventData2) {
  switch (event) {
    case EVENT_COMMIT:
      SetCtrlAttribute(panelHandle2, ANLGPANEL_CMD_SETCHANGES, ATTR_VISIBLE, 1);
      SetCtrlAttribute(panelHandle2, ANLGPANEL_NUM_ACHANNEL, ATTR_CTRL_MODE,
                       VAL_HOT);
      SetCtrlAttribute(panelHandle2, ANLGPANEL_STR_CHANNELNAME, ATTR_CTRL_MODE,
                       VAL_HOT);
      SetCtrlAttribute(panelHandle2, ANLGPANEL_STRING_UNITS, ATTR_CTRL_MODE,
                       VAL_HOT);
      SetCtrlAttribute(panelHandle2, ANLGPANEL_NUM_CHANNELPROP, ATTR_CTRL_MODE,
                       VAL_HOT);
      SetCtrlAttribute(panelHandle2, ANLGPANEL_NUM_CHANNELBIAS, ATTR_CTRL_MODE,
                       VAL_HOT);
      SetCtrlAttribute(panelHandle2, ANLGPANEL_CHKBOX_RESET, ATTR_CTRL_MODE,
                       VAL_HOT);
      SetCtrlAttribute(panelHandle2, ANLGPANEL_NUM_MINV, ATTR_CTRL_MODE,
                       VAL_HOT);
      SetCtrlAttribute(panelHandle2, ANLGPANEL_NUM_MAXV, ATTR_CTRL_MODE,
                       VAL_HOT);
      ChangedVals = TRUE;
      break;
  }
  return 0;
}

/**
@brief Callback for the Set Changes button. Takes inputs from the panel and
updates the relevant AnalogChannelProperties in AChName.
*/
int CVICALLBACK CMD_SETCHANGES_CALLBACK(int panel, int control, int event,
                                        void* callbackData, int eventData1,
                                        int eventData2) {
  char buff[51] = "", buff2[51] = "";
  int channel = 0, line = 0, val = 0;
  double prop = 0, bias = 0, vmin = 0, vmax = 0;
  switch (event) {
    case EVENT_COMMIT:
      GetCtrlVal(panelHandle2, ANLGPANEL_NUM_ACHANNEL, &channel);
      GetCtrlVal(panelHandle2, ANLGPANEL_NUM_ACH_LINE, &line);
      GetCtrlVal(panelHandle2, ANLGPANEL_NUM_CHANNELPROP, &prop);
      GetCtrlVal(panelHandle2, ANLGPANEL_NUM_CHANNELBIAS, &bias);
      GetCtrlVal(panelHandle2, ANLGPANEL_STR_CHANNELNAME, buff);
      GetCtrlVal(panelHandle2, ANLGPANEL_STRING_UNITS, buff2);
      GetCtrlVal(panelHandle2, ANLGPANEL_CHKBOX_RESET, &val);
      GetCtrlVal(panelHandle2, ANLGPANEL_NUM_MINV, &vmin);
      GetCtrlVal(panelHandle2, ANLGPANEL_NUM_MAXV, &vmax);

      AChName[line].resettozero = val;
      AChName[line].chnum = channel;
      AChName[line].tfcn = prop;
      AChName[line].tbias = bias;
      AChName[line].maxvolts = vmax;
      AChName[line].minvolts = vmin;
      sprintf(AChName[line].chname, buff);
      sprintf(AChName[line].units, buff2);
      SetAnalogChannels();
      // Next we will update the channel list and redisplay ;)
      break;
  }
  return 0;
}

/**
@brief Callback for the Done button. Sets all controls to Indicator and hides
the panel.
*/
int CVICALLBACK CMD_DONEANALOG_CALLBACK(int panel, int control, int event,
                                        void* callbackData, int eventData1,
                                        int eventData2) {
  switch (event) {
    case EVENT_COMMIT:
      SetCtrlAttribute(panelHandle2, ANLGPANEL_NUM_ACHANNEL, ATTR_CTRL_MODE,
                       VAL_INDICATOR);
      SetCtrlAttribute(panelHandle2, ANLGPANEL_STR_CHANNELNAME, ATTR_CTRL_MODE,
                       VAL_INDICATOR);
      SetCtrlAttribute(panelHandle2, ANLGPANEL_STRING_UNITS, ATTR_CTRL_MODE,
                       VAL_INDICATOR);
      SetCtrlAttribute(panelHandle2, ANLGPANEL_NUM_CHANNELPROP, ATTR_CTRL_MODE,
                       VAL_INDICATOR);
      SetCtrlAttribute(panelHandle2, ANLGPANEL_NUM_CHANNELBIAS, ATTR_CTRL_MODE,
                       VAL_INDICATOR); /* added by Seth, Oct 28, 2004 */
      SetCtrlAttribute(panelHandle2, ANLGPANEL_CHKBOX_RESET, ATTR_CTRL_MODE,
                       VAL_INDICATOR);
      SetCtrlAttribute(panelHandle2, ANLGPANEL_NUM_MINV, ATTR_CTRL_MODE,
                       VAL_INDICATOR);
      SetCtrlAttribute(panelHandle2, ANLGPANEL_NUM_MAXV, ATTR_CTRL_MODE,
                       VAL_INDICATOR);

      SetCtrlAttribute(panelHandle2, ANLGPANEL_CMD_SETCHANGES, ATTR_VISIBLE, 0);
      HidePanel(panelHandle2);
      break;
  }
  return 0;
}

/**
@brief Updates the main panel to display new values in AChName.
*/
void SetAnalogChannels(void) {
  for (int i = 1; i <= NUMBERANALOGCHANNELS; i++) {
    SetTableCellAttribute(panelHandle, PANEL_TBL_ANAMES, MakePoint(1, i),
                          ATTR_CTRL_VAL, AChName[i].chname);
    SetTableCellAttribute(panelHandle, PANEL_TBL_ANAMES, MakePoint(2, i),
                          ATTR_CTRL_VAL, AChName[i].chnum);
    SetTableCellAttribute(panelHandle, PANEL_TBL_ANALOGUNITS, MakePoint(1, i),
                          ATTR_CTRL_VAL, AChName[i].units);
  }
}

/**
@brief Callback for the Line number box. Updates the displayed information as we
change the line number.
*/
int CVICALLBACK NUM_ACH_LINE_CALLBACK(int panel, int control, int event,
                                      void* callbackData, int eventData1,
                                      int eventData2) {
  int line = 0;
  switch (event) {
    case EVENT_COMMIT:

      GetCtrlVal(panelHandle2, ANLGPANEL_NUM_ACH_LINE, &line);

      SetCtrlVal(panelHandle2, ANLGPANEL_NUM_ACHANNEL, AChName[line].chnum);
      SetCtrlVal(panelHandle2, ANLGPANEL_NUM_CHANNELPROP, AChName[line].tfcn);
      SetCtrlVal(panelHandle2, ANLGPANEL_NUM_CHANNELBIAS,
                 AChName[line].tbias); /* added by Seth, Oct 28, 2004 */
      SetCtrlVal(panelHandle2, ANLGPANEL_STR_CHANNELNAME, AChName[line].chname);
      SetCtrlVal(panelHandle2, ANLGPANEL_STRING_UNITS, AChName[line].units);
      SetCtrlVal(panelHandle2, ANLGPANEL_CHKBOX_RESET,
                 AChName[line].resettozero);
      SetCtrlVal(panelHandle2, ANLGPANEL_NUM_MINV, AChName[line].minvolts);
      SetCtrlVal(panelHandle2, ANLGPANEL_NUM_MAXV, AChName[line].maxvolts);
      break;
  }

  return 0;
}
