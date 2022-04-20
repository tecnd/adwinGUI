/**
@file DigitalSettings.c
@brief Sets the DigitalChannelProperties for each digital line.
*/
#include "DigitalSettings.h"

#include <ansi_c.h>
#include <userint.h>

#include "DigitalSettings2.h"
#include "GUIDesign.h"
#include "vars.h"

/**
@brief Callback for the Line number box. Updates the displayed information as we
change the line number.
*/
int CVICALLBACK NUM_DIG_LINE_CALLBACK(int panel, int control, int event,
                                      void* callbackData, int eventData1,
                                      int eventData2) {
  int line = 0;
  switch (event) {
    case EVENT_COMMIT:
      GetCtrlVal(panelHandle3, DIGPANEL_NUM_DIGCH_LINE, &line);
      SetCtrlVal(panelHandle3, DIGPANEL_NUM_DIGCHANNEL, DChName[line].chnum);
      SetCtrlVal(panelHandle3, DIGPANEL_STR_DIGCHANNELNAME,
                 DChName[line].chname);
      SetCtrlVal(panelHandle3, DIGPANEL_CHK_DIGRESET, DChName[line].resettolow);
      break;
  }
  return 0;
}

/**
@brief Callback for the Allow Changes button. Switches components from Indicator
to Hot so changes can be made. Adds flag that values have been changed so ADwin
array is rebuilt on next run.
*/
int CVICALLBACK CMD_DIGALLOWCHANGE_CALLBACK(int panel, int control, int event,
                                            void* callbackData, int eventData1,
                                            int eventData2) {
  switch (event) {
    case EVENT_COMMIT:
      SetCtrlAttribute(panelHandle3, DIGPANEL_CMD_DIGSETCHANGES, ATTR_VISIBLE,
                       1);
      SetCtrlAttribute(panelHandle3, DIGPANEL_NUM_DIGCHANNEL, ATTR_CTRL_MODE,
                       VAL_HOT);
      SetCtrlAttribute(panelHandle3, DIGPANEL_STR_DIGCHANNELNAME,
                       ATTR_CTRL_MODE, VAL_HOT);
      SetCtrlAttribute(panelHandle3, DIGPANEL_CHK_DIGRESET, ATTR_CTRL_MODE,
                       VAL_HOT);
      ChangedVals = TRUE;
      break;
  }
  return 0;
}

/**
@brief Callback for the Set Changes button. Takes inputs from the panel and
updates the relevant DigitalChaannelProperties in DChName.
*/
int CVICALLBACK CMD_DIGSETCHANGES_CALLBACK(int panel, int control, int event,
                                           void* callbackData, int eventData1,
                                           int eventData2) {
  int channel = 0, line = 0, resetlow = 0;
  char buff[50];
  switch (event) {
    case EVENT_COMMIT:
      GetCtrlVal(panelHandle3, DIGPANEL_NUM_DIGCHANNEL, &channel);
      GetCtrlVal(panelHandle3, DIGPANEL_NUM_DIGCH_LINE, &line);
      GetCtrlVal(panelHandle3, DIGPANEL_STR_DIGCHANNELNAME, buff);
      GetCtrlVal(panelHandle3, DIGPANEL_CHK_DIGRESET, &resetlow);
      DChName[line].chnum = channel;
      sprintf(DChName[line].chname, buff);
      DChName[line].resettolow = resetlow;
      SetDigitalChannels();
      break;
  }
  return 0;
}

/**
@brief Callback for the Done button. Sets all controls to Indicator and hides
the panel.
*/
int CVICALLBACK CMD_DONEDIG_CALLBACK(int panel, int control, int event,
                                     void* callbackData, int eventData1,
                                     int eventData2) {
  switch (event) {
    case EVENT_COMMIT:
      SetCtrlAttribute(panelHandle3, DIGPANEL_NUM_DIGCHANNEL, ATTR_CTRL_MODE,
                       VAL_INDICATOR);
      SetCtrlAttribute(panelHandle3, DIGPANEL_STR_DIGCHANNELNAME,
                       ATTR_CTRL_MODE, VAL_INDICATOR);
      SetCtrlAttribute(panelHandle3, DIGPANEL_CMD_DIGSETCHANGES, ATTR_VISIBLE,
                       0);
      SetCtrlAttribute(panelHandle3, DIGPANEL_CHK_DIGRESET, ATTR_CTRL_MODE,
                       VAL_INDICATOR);
      HidePanel(panelHandle3);
      break;
  }
  return 0;
}

/**
@brief Updates the main panel to display new values in DChName.
*/
void SetDigitalChannels(void) {
  for (int i = 1; i <= NUMBERDIGITALCHANNELS; i++) {
    SetTableCellAttribute(panelHandle, PANEL_TBL_DIGNAMES, MakePoint(1, i),
                          ATTR_CTRL_VAL, DChName[i].chname);
    SetTableCellAttribute(panelHandle, PANEL_TBL_DIGNAMES, MakePoint(2, i),
                          ATTR_CTRL_VAL, DChName[i].chnum);
  }
}
