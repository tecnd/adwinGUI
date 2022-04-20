/**
@file AnalogControl.c
@brief This panel is used to set the value of an AnalogTableValues in
AnalogTable. It is called by double-clicking on an analog table cell.
*/

#include "AnalogControl.h"

#include <ansi_c.h>
#include <toolbox.h>
#include <userint.h>

#include "AnalogControl2.h"
#include "GUIDesign2.h"
#include "vars.h"

/**
@brief Set button callback. On commit, reads data off of
the panel controls and update the entry in the AnalogTable array.
@todo If timescale exceeds the time column, update the following cells
as well until total timescale is used.
*/
int CVICALLBACK CMD_SETANALAOG_CALLBACK(int panel, int control, int event,
                                        void* callbackData, int eventData1,
                                        int eventData2) {
  int itemp = 0;  // variables for reading/writing the control mode
  double dtemp, ttemp;
  switch (event) {
    case EVENT_COMMIT:
      // Retrieve Control/Data Values from Panel
      GetCtrlVal(panelHandle4, CTRL_PANEL_RING_CTRLMODE, &itemp);
      GetCtrlVal(panelHandle4, CTRL_PANEL_NUMFINALVAL, &dtemp);
      GetCtrlVal(panelHandle4, CTRL_PANEL_NUMTIMESCALE, &ttemp);
      AnalogTable[currentx][currenty][currentpage].fcn = itemp;

      if (itemp != 6) {
        AnalogTable[currentx][currenty][currentpage].fval = dtemp;
        AnalogTable[currentx][currenty][currentpage].tscale = ttemp;
      }

      GetCtrlVal(panelHandle4, CTRL_PANEL_NUMFINALVAL, &dtemp);
      AnalogTable[currentx][currenty][currentpage].fval = dtemp;

      GetCtrlVal(panelHandle4, CTRL_PANEL_NUMTIMESCALE, &dtemp);
      AnalogTable[currentx][currenty][currentpage].tscale = dtemp;

      HidePanel(panelHandle4);
      DrawNewTable(0);
      break;
  }
  return 0;
}

/**
@brief Fill in indicators on the panel.
*/
void SetControlPanel(void) {
  SetCtrlVal(panelHandle4, CTRL_PANEL_STRUNITS, AChName[currenty].units);
  SetCtrlVal(panelHandle4, CTRL_PANEL_STR_CHNAME, AChName[currenty].chname);
  SetCtrlVal(panelHandle4, CTRL_PANEL_NUMFINALVAL,
             AnalogTable[currentx][currenty][currentpage].fval);
  SetCtrlVal(panelHandle4, CTRL_PANEL_NUMTIMESCALE,
             AnalogTable[currentx][currenty][currentpage].tscale);
  SetCtrlVal(panelHandle4, CTRL_PANEL_RING_CTRLMODE,
             AnalogTable[currentx][currenty][currentpage].fcn);

  // find the last value
  // This loop can perform better if it starts from the current cell and
  // searches backwards instead of starting from the beginning and searching
  // forwards
  BOOL FOUNDVAL = 0;
  int cx = 1;
  int cz = 1;
  double lastvalue = 0;
  while (!FOUNDVAL) {
    if (cz > NUMBEROFPAGES) {
      break;  // didn't find it, end the loop
    }
    if (TimeArray[cx][cz] == 0) {
      cz++;  // if you see a 0 time, go to next page
      cx = 1;
    } else if (TimeArray[cx][cz] < 0) {
      // if you see a negative time, skip to next value
      cx++;
      if (cx >= NUMBEROFCOLUMNS) {
        cx = 1;
        cz++;
      }
    } else {
      // if positive time, record current val
      if ((cx >= currentx) && (cz >= currentpage)) {
        FOUNDVAL = TRUE;  // found current value
      } else {
        lastvalue = AnalogTable[cx][currenty][cz].fval;
      }
      cx++;
      if (cx >= NUMBEROFCOLUMNS) {
        cx = 1;
        cz++;
      }
    }
  }
  SetCtrlVal(panelHandle4, CTRL_PANEL_NUMINITVAL, lastvalue);
}

/**
@brief Callback for the Control Mode ring control.
Cycles between the different control modes and changes the timescale of the ramp
to an appropriate value for the mode.

If adding another function, just add another case, modify the ring control
on the AnalogControl.uir file, and add function handling in CalcFcnVal() in
GUIDesign.c
*/
int CVICALLBACK RING_CTRLMODE_CALLBACK(int panel, int control, int event,
                                       void* callbackData, int eventData1,
                                       int eventData2) {
  int ctrlmode = 0;
  double timescales = 0, ctrlvfinal;

  GetCtrlVal(panelHandle4, CTRL_PANEL_RING_CTRLMODE, &ctrlmode);
  GetCtrlVal(panelHandle4, CTRL_PANEL_NUMFINALVAL, &ctrlvfinal);
  timescales = TimeArray[currentx][currentpage];
  double Vdiff =
      ctrlvfinal - AnalogTable[currentx - 1][currenty][currentpage].fval;
  SetCtrlAttribute(panelHandle4, CTRL_PANEL_NUMFINALVAL, ATTR_LABEL_TEXT,
                   "Final Value");
  SetCtrlAttribute(panelHandle4, CTRL_PANEL_NUMTIMESCALE, ATTR_LABEL_TEXT,
                   "Time Scale");
  switch (ctrlmode) {
    case 1: // Step
      SetCtrlVal(panelHandle4, CTRL_PANEL_NUMTIMESCALE, 0.0);
      break;
    case 2: // Linear ramp
      SetCtrlVal(panelHandle4, CTRL_PANEL_NUMTIMESCALE, timescales);
      break;
    case 3: // Exponential ramp
      SetCtrlVal(panelHandle4, CTRL_PANEL_NUMTIMESCALE, timescales);
      if (FloatCompare(&Vdiff, &(float){0.}) != 0) {
        double newtime = timescales / fabs(log(fabs(Vdiff)) - log(0.001));
        //  newtime=fabs(timescales/(log(0.001/Vdiff)));
        SetCtrlVal(panelHandle4, CTRL_PANEL_NUMTIMESCALE, newtime);
      }
      break;
    case 4: // S curve
      SetCtrlVal(panelHandle4, CTRL_PANEL_NUMTIMESCALE, timescales);
      break;

    case 5: // Sine wave
      SetCtrlAttribute(panelHandle4, CTRL_PANEL_NUMFINALVAL, ATTR_LABEL_TEXT,
                       "Amplitude");
      SetCtrlAttribute(panelHandle4, CTRL_PANEL_NUMTIMESCALE, ATTR_LABEL_TEXT,
                       "Frequency");
      break;
  }

  return 0;
}

/**
@brief Callback for the Cancel button.
*/
int CVICALLBACK CMD_ANCANCEL_CALLBACK(int panel, int control, int event,
                                      void* callbackData, int eventData1,
                                      int eventData2) {
  switch (event) {
    case EVENT_COMMIT:
      HidePanel(panelHandle4);
      break;
  }
  return 0;
}
