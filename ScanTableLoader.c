/**
@file ScanTableLoader.c
@brief Generates values for the scan table using either linear or exponential
ramping. A value of -1000 indicates the end of the ramp.
*/

#include "ScanTableLoader.h"

#include <ansi_c.h>
#include <cvirte.h>
#include <userint.h>

#include "GUIDesign.h"
#include "ScanTableLoader2.h"
#include "vars.h"

/**
@brief Callback for the Cancel button. Hides the panel without loading anything
to the scan table.
*/
int CVICALLBACK SCAN_LOAD_CANCEL(int panel, int control, int event,
                                 void* callbackData, int eventData1,
                                 int eventData2) {
  switch (event) {
    case EVENT_COMMIT:
      HidePanel(panelHandle8);

      break;
  }
  return 0;
}

/**
@brief Callback for the OK button. Loads values into the scan table based on the
given parameters.
*/
int CVICALLBACK SCAN_LOAD_OK(int panel, int control, int event,
                             void* callbackData, int eventData1,
                             int eventData2) {
  int steps = 0, mode;
  double first, last;

  switch (event) {
    case EVENT_COMMIT:

      GetCtrlVal(panelHandle8, SL_PANEL_SCAN_TYPE, &mode);
      GetCtrlVal(panelHandle8, SL_PANEL_ITER_NUM, &steps);
      GetCtrlVal(panelHandle8, SL_PANEL_SCAN_INIT, &first);
      GetCtrlVal(panelHandle8, SL_PANEL_SCAN_FIN, &last);

      if (steps > 30 || steps < 1) {
        ConfirmPopup("USER ERROR", "# of Steps must be between 1-30");
        HidePanel(panelHandle8);
      } else {
        switch (mode) {
          case 1:
            LoadLinearRamp(steps, first, last);
            break;
          case 2:
            LoadExpRamp(steps, first, last);
            break;
        }
        HidePanel(panelHandle8);
      }

      break;
  }
  return 0;
}

/**
@brief Loads a linear ramp into the scan table.
@param steps Number of steps to ramp.
@param first The starting value to ramp from.
@param last The final value to ramp to.
*/
void LoadLinearRamp(int steps, double first, double last) {
  double slope = (last - first) / steps;

  for (int i = 0; i <= steps; i++) {
    SetTableCellVal(panelHandle, PANEL_SCAN_TABLE, MakePoint(1, i + 1),
                    slope * i);
  }
  SetTableCellVal(panelHandle, PANEL_SCAN_TABLE, MakePoint(1, steps + 2),
                  -1000.0);
}

/**
@brief Loads an exponential ramp into the scan table.
@param steps Number of steps to ramp.
@param first The starting value to ramp from.
@param last The final value to ramp to.

Creates values using the formula f(x) = last - amplitude * exp(bx) with +/- 1%
accuracy.
*/
void LoadExpRamp(int steps, double first, double last) {
  double amplitude = last - first;
  double b = log(0.01) / steps;

  for (int i = 0; i < steps; i++) {
    SetTableCellVal(panelHandle, PANEL_SCAN_TABLE, MakePoint(1, i + 1),
                    last - amplitude * exp(b * i));
  }
  SetTableCellVal(panelHandle, PANEL_SCAN_TABLE, MakePoint(1, steps + 1), last);
  SetTableCellVal(panelHandle, PANEL_SCAN_TABLE, MakePoint(1, steps + 2),
                  -1000.0);
}
