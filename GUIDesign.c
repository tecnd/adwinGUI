/**
@file GUIDesign.c
@brief Contains functions and callbacks for primary GUI. Handles data processing
and communication with the ADwin.
*/
// Modified by Seth Aubin on August 2, 2010
// change to the "ADbasic binary file: "TransferData_August02_2010.TB1"
// purpose: activate DIO 1 and DIO 2 outputs.

// 2006
// March 9:  Reorder the routines to more closely match the order in which they
// are executed.
// Applies to the 'engine' but not the cosmetic/table handling routnes

#include "GUIDesign.h"

#include <ansi_c.h>
#include <toolbox.h>
#include <userint.h>
#include <utility.h>

#include "Adwin.h"
#include "AnalogControl2.h"
#include "AnalogSettings2.h"
#include "DigitalSettings2.h"
#include "GUIDesign2.h"
#include "Scan.h"
#include "ScanTableLoader.h"
#include "main.h"
#include "scan2.h"
#include "vars.h"

// Forward declarations of functions
void BuildUpdateList(double[500],
                     struct AnalogTableValues[NUMBERANALOGCHANNELS + 1][500],
                     int[NUMBERDIGITALCHANNELS + 1][500], int);
int OptimizeTimeLoop(int*, int);
void ShiftColumn(int, int, int);
void RunOnce(void);
void SaveArrays(char*, int);
void LoadArrays(char*, int);
void ExportPanel(char*, int);
double CalcFcnValue(int, double, double, double, double, double);
double CheckIfWithinLimits(double, int);
void UpdateScanValue(int);
void ExportScanBuffer(void);

// Clipboard to hold data from copy/paste cells
double TimeClip;
int ClipColumn = -1;
struct AnalogTableValues AnalogClip[NUMBERANALOGCHANNELS + 1];
int DigClip[NUMBERDIGITALCHANNELS + 1];

/**
@brief Returns if the page is checked.
@param page The page number
@return 0 if not checked
@return 1 if checked
@author Kerry Wang
*/
BOOL IsPageChecked(int page) {
  BOOL bool;
  GetCtrlVal(panelHandle, CheckboxArray[page], &bool);
  return bool;
}

/**
@brief Callback for the Run button. Disables scanning and activates the timer if
Repeat is enabled. Calls RunOnce() to run the ADwin.
*/
int CVICALLBACK CMD_RUN_CALLBACK(int panel, int control, int event,
                                 void* callbackData, int eventData1,
                                 int eventData2) {
  int repeat = 0;
  switch (event) {
    case EVENT_COMMIT:
      PScan.Scan_Active = FALSE;
      // Forces the BuildUpdateList() routine to generate
      // new data for the ADwin
      ChangedVals = TRUE;
      GetCtrlVal(panelHandle, PANEL_TOGGLEREPEAT,
                 &repeat);  // reads the state of the "repeat" switch
      if (repeat == TRUE) {
        SetCtrlAttribute(panelHandle, PANEL_TIMER, ATTR_ENABLED, 1);
        // activate timer:  calls TIMER_CALLBACK to restart the RunOnce commands
        // after a set time.
      } else {
        SetCtrlAttribute(panelHandle, PANEL_TIMER, ATTR_ENABLED, 0);
        // deactivate timer
      }
      RunOnce();  // starts the routine to build the ADwin data.
      break;
  }

  return 0;
}

/**
@brief Callback for the Scan button. Pretty much a copy of CMD_RUN_CALLBACK(),
but activates the Scan flag and resets the scan counter.
@todo could be integrated into the CMD_RUN routine.... but this works
*/
int CVICALLBACK CMD_SCAN_CALLBACK(int panel, int control, int event,
                                  void* callbackData, int eventData1,
                                  int eventData2) {
  int repeat = 0;
  switch (event) {
    case EVENT_COMMIT:
      UpdateScanValue(TRUE);  // sending value of 1 resets the scan counter.
      PScan.Scan_Active = TRUE;
      ChangedVals = TRUE;
      repeat = TRUE;
      SetCtrlVal(panelHandle, PANEL_TOGGLEREPEAT,
                 repeat);  // sets "repeat" button to active
      SetCtrlAttribute(panelHandle, PANEL_TIMER, ATTR_ENABLED,
                       1);  // Turn on the timer
      RunOnce();            // starts the routine to build the ADwin data.
      break;
  }

  return 0;
}

/**
@brief Callback for the timer object. When the timer's countdown reaches 0,
disable itself and call RunOnce() again.

The timer is activated in CMD_RUN_CALLBACK(), CMD_SCAN_CALLBACK(), and
BuildUpdateList(). It gets deactivated here and when we hit CMDSTOP_CALLBACK().
*/
int CVICALLBACK TIMER_CALLBACK(int panel, int control, int event,
                               void* callbackData, int eventData1,
                               int eventData2) {
  switch (event) {
    case EVENT_TIMER_TICK:
      SetCtrlAttribute(panelHandle, PANEL_TIMER, ATTR_ENABLED, FALSE);
      // disable timer and re-enable in the runonce (or update list) loop, if
      // the repeat butn is pressed. reset the timer too and set a timer time of
      // 50ms?
      if (PScan.Scan_Active == TRUE) {
        UpdateScanValue(FALSE);
      }
      RunOnce();

      break;
  }
  return 0;
}

/**
@brief Callback for the Stop button. Turns off the timer object and the repeat
button.

Lets the ADwin finish its current program.  Interrupting the program partway can
be bad for the equipment as the variables are not cleared in memory and updates
can get out of sync.
@author Stefan Myrskog
*/
int CVICALLBACK CMDSTOP_CALLBACK(int panel, int control, int event,
                                 void* callbackData, int eventData1,
                                 int eventData2) {
  SetCtrlAttribute(panelHandle_sub2, SUBPANEL2, ATTR_VISIBLE,
                   0);  // hide the SCAN display panel
  switch (event) {
    case EVENT_COMMIT:
      SetCtrlAttribute(panelHandle, PANEL_TIMER, ATTR_ENABLED, 0);
      SetCtrlVal(panelHandle, PANEL_TOGGLEREPEAT, 0);
      // check to see if we need to export a Scan history
      if (PScan.Scan_Active == TRUE) {
        ExportScanBuffer();
      }
      break;
  }
  return 0;
}

/**
@brief Flattens the time, analog, and digital tables from multiple "pages" to a
single long page. Passes the tables to BuildUpdateList() to send to the ADwin.
@todo Change meta arrays to be malloc'd at runtime, or use dynamic lists.
@todo Change name to be more descriptive.
*/
void RunOnce(void) {
  // could/should change these following defs and use malloc instead, but they
  // should never exceed.. 170 or so.
  // initialize the 0th element even though we're not using it,
  // otherwise will raise uninitialized exception
  double MetaTimeArray[500] = {0};
  int MetaDigitalArray[NUMBERDIGITALCHANNELS + 1][500] = {0};
  struct AnalogTableValues MetaAnalogArray[NUMBERANALOGCHANNELS + 1][500] = {0};

  int mindex = 0;  // Index in the meta arrays
  // go through for each page
  for (int page = 1; page <= NUMBEROFPAGES; page++) {
    // if the page is selected (checkbox is checked)
    if (IsPageChecked(page)) {
      // go through for each column
      for (int col = 1; col <= NUMBEROFCOLUMNS; col++) {
        // Ignore columns with negative times
        if (TimeArray[col][page] > 0) {
          mindex++;  // increase the number of columns counter
          MetaTimeArray[mindex] = TimeArray[col][page];

          // go through for each analog channel
          for (int channel = 1; channel <= NUMBERANALOGCHANNELS; channel++) {
            // Sets MetaArray with appropriate fcn val when "same" selected
            if (AnalogTable[col][channel][page].fcn == 6) {
              // on the right for repeat function, currently set to hold val
              MetaAnalogArray[channel][mindex].fcn = 1;
              MetaAnalogArray[channel][mindex].fval =
                  MetaAnalogArray[channel][mindex - 1].fval;
              MetaAnalogArray[channel][mindex].tscale =
                  MetaAnalogArray[channel][mindex - 1].tscale;
            } else {
              MetaAnalogArray[channel][mindex].fcn =
                  AnalogTable[col][channel][page].fcn;
              MetaAnalogArray[channel][mindex].fval =
                  AnalogTable[col][channel][page].fval;
              MetaAnalogArray[channel][mindex].tscale =
                  AnalogTable[col][channel][page].tscale;
            }
          }
          for (int channel = 1; channel <= NUMBERDIGITALCHANNELS; channel++) {
            MetaDigitalArray[channel][mindex] =
                DigTableValues[col][channel][page];
          }
        }
        // Skip to next page on time 0
        else if (TimeArray[col][page] == 0) {
          break;
        }
      }
    }
  }
  DrawNewTable(1);
  // Send the new arrays to BuildUpdateList()
  BuildUpdateList(MetaTimeArray, MetaAnalogArray, MetaDigitalArray, mindex);
}

/**
@brief Builds the data from the arrays into ADwin-friendly format.
@param TMatrix[] Stores the interval time of each column
@param AMat[] Stores info located in the analog table
@param DMat[] Stores info located in the digital table

Each array can have up to 499 elements. Note that elements are 1-indexed, the
0-th index is undefined and should not be accessed.

@param numtimes The actual number of valid update period elements.

We generate the following 3 arrays:
- UpdateNum
  - Each entry is the number of channel updates (i.e. ChNum/ChVal pairs) to be
performed on the next ADwin trigger, which comes once every 10 microseconds.
- ChNum
  - Each entry is the channel number to be updated. Synchronous with ChVal.
Channel number definitions as follows:
  - 1-32: Analog lines. ChVal is the voltage, from -10V to 10V.
  - 101, 102: Digital cards, with 32 lines each. ChVal is a 32-bit integer, one
bit for each line, starting from LSB.
- ChVal
  - Each entry is the value to be written to a channel. Synchronous with ChNum.
*/
void BuildUpdateList(
    double TMatrix[500],
    struct AnalogTableValues AMat[NUMBERANALOGCHANNELS + 1][500],
    int DMat[NUMBERDIGITALCHANNELS + 1][500], int numtimes) {
  BOOL UseCompression;
  int NewTimeMat[500] = {0};
  int UsingFcns[NUMBERANALOGCHANNELS + 1] = {0};
  double cycletime = 0;
  int repeat = 0, timesum = 0;

  // Change run button appearance while operating
  SetCtrlAttribute(panelHandle, PANEL_CMD_RUN, ATTR_CMD_BUTTON_COLOR,
                   VAL_GREEN);

  int timemult = RoundRealToNearestInteger(
      1 / EVENTPERIOD);  // number of adwin upates per ms

  // make a new time list...converting the TimeTable from milliseconds to number
  // of events (numtimes=total #of columns)
  for (int i = 1; i <= numtimes; i++) {
    NewTimeMat[i] = RoundRealToNearestInteger(
        TMatrix[i] * timemult);         // number of Adwin events in column i
    timesum = timesum + NewTimeMat[i];  // total number of Adwin events
  }

  cycletime =
      timesum * EVENTPERIOD / 1000;  // Total duration of the cycle, in seconds

  // reupdate the ADWIN array if the user values have changed
  if (ChangedVals == TRUE) {
    // dynamically allocate the memory for the time array (instead of using a
    // static array:UpdateNum) We are making an assumption about how many
    // programmable points we may need to use. For now assume that number of
    // channel updates <= 4* #of events, serious overestimate

    int* UpdateNum = calloc(timesum + 1, sizeof *UpdateNum);
    if (!UpdateNum) {
      exit(1);
    }
    int* ChNum = calloc(timesum * 4, sizeof *ChNum);
    if (!ChNum) {
      exit(1);
    }
    float* ChVal = calloc(timesum * 4, sizeof *ChVal);
    if (!ChVal) {
      exit(1);
    }

    int nuptotal = 0;
    int count = 0;
    double NewAval, TempChVal, TempChVal2;
    double LastAval[NUMBERANALOGCHANNELS + 1] = {0};
    long ResetToZeroAtEnd[NUMBERANALOGCHANNELS + 6];
    static int didboot = 0;
    static int didprocess = 0;

    // Go through for each column that needs to be updated

    // Important Variables:
    // count: Number of Adwin events until the current position
    // nupcurrent: number of updates for the current Adwin event
    // nuptotal: current position in the channel/value column
    int LastDVal = 0;
    int LastDVal2 = 0;
    // make sure to always update on first column to
    // prevent lingering from past loops in repeat mode
    BOOL firstCol = TRUE;
    for (int i = 1; i <= numtimes; i++) {
      // find out how many channels need updating this round...
      // if it's a non-step fcn, then keep a list of UsingFcns, and change it
      // now
      int nupcurrent = 0;
      int usefcn = 0;

      // scan over the analog channel..find updated values by comparing to old
      // values.
      for (int j = 1; j <= NUMBERANALOGCHANNELS; j++) {
        LastAval[j] = -99;
        if (AMat[j][i].fval != AMat[j][i - 1].fval) {
          nupcurrent++;
          nuptotal++;
          ChNum[nuptotal] = AChName[j].chnum;
          NewAval =
              CalcFcnValue(AMat[j][i].fcn, AMat[j][i - 1].fval, AMat[j][i].fval,
                           AMat[j][i].tscale, 0.0, TMatrix[i]);

          TempChVal = AChName[j].tbias + NewAval * AChName[j].tfcn;
          ChVal[nuptotal] = CheckIfWithinLimits(TempChVal, j);

          if (AMat[j][i].fcn != 1) {
            usefcn++;
            // mark these lines for special attention..more complex
            UsingFcns[usefcn] = j;
          }
        }
      }  // done scanning the analog values.
      // now the digital value
      int digval = 0;
      int digval2 = 0;
      for (int row = 1; row <= NUMBERDIGITALCHANNELS; row++) {
        int digchannel = DChName[row].chnum;

        if (digchannel <= 32) {
          // Set bits using logical OR. Technically left-shifting a 32 bit
          // signed int by 31 bits is undefined behavior, but it works as
          // expected on modern machines
          digval |= (DMat[row][i] << (digchannel - 1));
        } else if ((digchannel >= 101) && (digchannel <= 132)) {
          digval2 |= DMat[row][i] << (digchannel - 101);
        }
      }  // finished computing current digital data

      if (firstCol || digval != LastDVal) {
        nupcurrent++;
        nuptotal++;
        ChNum[nuptotal] = 101;
        ChVal[nuptotal] = digval;
      }
      LastDVal = digval;
      if (firstCol || digval2 != LastDVal2) {
        nupcurrent++;
        nuptotal++;
        ChNum[nuptotal] = 102;
        ChVal[nuptotal] = digval2;
      }
      LastDVal2 = digval2;

      firstCol = FALSE;
      count++;
      UpdateNum[count] = nupcurrent;

      // end of first scan
      // now do the remainder of the loop...but just the complicated fcns, i.e.
      // ramps, sine wave

      int t = 0;
      while (t < NewTimeMat[i] - 1) {
        t++;
        int k = 0;
        nupcurrent = 0;

        while (k < usefcn) {
          k++;
          int c = UsingFcns[k];
          NewAval =
              CalcFcnValue(AMat[c][i].fcn, AMat[c][i - 1].fval, AMat[c][i].fval,
                           AMat[c][i].tscale, t, TMatrix[i]);
          TempChVal = AChName[c].tbias + NewAval * AChName[c].tfcn;
          TempChVal2 = CheckIfWithinLimits(TempChVal, c);

          // only update if the ADwin will output a new value.
          // ADwin is 16 bit, +/-10 V, to 1 bit resolution implies dV=20V/2^16
          // ~=  0.3mV
          if (fabs(TempChVal2 - LastAval[k]) > 0.0003) {
            nupcurrent++;
            nuptotal++;
            ChNum[nuptotal] = AChName[c].chnum;
            ChVal[nuptotal] = AChName[c].tbias + NewAval * AChName[c].tfcn;
            LastAval[k] = TempChVal2;
          }
        }
        count++;
        UpdateNum[count] = nupcurrent;
      }  // Done this element of the TMatrix
    }    // done scanning over times array

    // read some menu options
    GetMenuBarAttribute(menuHandle, MENU_PREFS_COMPRESSION, ATTR_CHECKED,
                        &UseCompression);
    int newcount = 0;

    if (UseCompression) {
      newcount = OptimizeTimeLoop(UpdateNum, count);
    }

    // tstop = clock();

    if (didboot == FALSE)  // is the ADwin booted?  if not, then boot
    {
      Boot("ADbasic\\ADwin11.btl", 0);
      didboot = 1;
    }
    if (didprocess == FALSE)  // is the ADwin process already loaded?
    {
      Load_Process("ADbasic\\TransferDataExternalClock.TB1");
      didprocess = 1;
    }

    if (UseCompression) {
      // Let ADwin know how many counts (read as Events)
      // we will be using.
      SetPar(1, newcount);
      SetData_Long(1, UpdateNum, 1, newcount + 1);
    } else {
      // Let ADwin know how many counts (read as Events) we
      // will be using.
      SetPar(1, count);
      SetData_Long(1, UpdateNum, 1, count + 1);
    }

    // Send the Array to the AdWin Sequencer
    int GlobalDelay = 3000;  // 3000 * 3.33...ns = 0.01 ms ticks. No longer
                             // necessary with external trigger, but still here
                             // in case we ever need to use the internal clock
    SetPar(2, GlobalDelay);
    SetData_Long(2, ChNum, 1, nuptotal + 1);
    SetData_Float(3, ChVal, 1, nuptotal + 1);

    for (int i = 1; i <= NUMBERANALOGCHANNELS; i++) {
      ResetToZeroAtEnd[i - 1] = AChName[i].resettozero;
    }

    SetData_Long(4, ResetToZeroAtEnd, 1, NUMBERANALOGCHANNELS);
    // done evaluating channels that are reset to  zero (low)
    ChangedVals = 0;
    free(UpdateNum);
    free(ChNum);
    free(ChVal);
  }

  Start_Process(1);  // start the process on ADwin

  SetCtrlAttribute(panelHandle, PANEL_CMD_RUN, ATTR_CMD_BUTTON_COLOR,
                   0x00B0B0B0);
  // re-enable the timer if necessary
  GetCtrlVal(panelHandle, PANEL_TOGGLEREPEAT, &repeat);
  if ((PScan.Scan_Active == TRUE) && (PScan.ScanDone == TRUE)) {
    repeat = FALSE;  // remember to reset the front panel repeat button
    SetCtrlVal(panelHandle, PANEL_TOGGLEREPEAT, repeat);
  }
  if (repeat == TRUE) {
    SetCtrlAttribute(panelHandle, PANEL_TIMER, ATTR_ENABLED, 1);
    SetCtrlAttribute(panelHandle, PANEL_TIMER, ATTR_INTERVAL, cycletime);
    ResetTimer(panelHandle, PANEL_TIMER);
  }
}

/**
@brief Given an analog function setting and a time, calculate the analog value.
@param fcn Function type, see AnalogTableValues.fcn.
@param Vinit For ramps and S curve, voltage at telapsed = 0. Not used for step
and sine wave.
@param Vfinal For step, voltage to step to. For ramps and S curve, voltage at
telapsed >= timescale. For sine wave, the amplitude.
@param timescale For ramps and S curve, time to reach Vfinal. For sine wave, the
frequency. Not used for step.
@param telapsed The time elapsed since the start of the cell.
@param celltime Total time duration of this cell
@return The resulting analog value.
@todo What is SimpleTiming? Seems turned on by default.
*/
double CalcFcnValue(int fcn, double Vinit, double Vfinal, double timescale,
                    double telapsed, double celltime) {
  BOOL UseSimpleTiming;
  GetMenuBarAttribute(menuHandle, MENU_PREFS_SIMPLETIMING, ATTR_CHECKED,
                      &UseSimpleTiming);
  double value = -99;
  double amplitude;
  if (UseSimpleTiming == TRUE) {
    timescale = celltime - EVENTPERIOD;
  }
  if (timescale <= 0) {
    timescale = 1;
  }
  double tms = telapsed * EVENTPERIOD;
  // add commands here to select 'simple timing'

  switch (fcn) {
    case 1:  // step function
      value = Vfinal;
      break;
    case 2:  // linear ramp
      amplitude = Vfinal - Vinit;
      double slope = amplitude / timescale;
      if (tms > timescale) {
        value = Vfinal;
      } else {
        value = Vinit + slope * tms;
      }
      break;
    case 3:  // exponential
      amplitude = Vfinal - Vinit;
      double newtime = timescale;
      if (UseSimpleTiming == TRUE) {
        newtime = timescale / fabs(log(fabs(amplitude)) - log(0.001));
      }
      value = Vfinal - amplitude * exp(-tms / newtime);
      break;
    case 4:  // S curve
      amplitude = Vfinal - Vinit;
      double aconst = 3 * amplitude / pow(timescale, 2);
      double bconst = -2 * amplitude / pow(timescale, 3);
      if (tms > timescale) {
        value = Vfinal;
      } else {
        value = Vinit + (aconst * pow(tms, 2) + bconst * pow(tms, 3));
      }
      break;
    case 5:
      // generate a sinewave.  Use Vfinal as the amplitude and timescale
      // as the frequency
      // ignore the 'Simple Timing' option...use the user entered value.

      amplitude = Vfinal;
      // consider it to be Hertz (tms is time in milliseconds)
      value = amplitude * sin(2 * Pi() * timescale * tms / 1000);
  }
  // Check if the value exceeds the allowed voltage limits.
  return value;
}

/**
@brief Compresses the UpdateNum list by replacing long strings of 0's with a
single negative number representing the amount replaced. i.e. if we see 2000 0's
in a row, replace with -2000 instead.
@param UpdateNum Array to be optimized (1-indexed)
@param count Length of UpdateNum
@return Length of UpdateNum after optimization, including index 0
*/
int OptimizeTimeLoop(int* UpdateNum, int count) {
  int i = 1;  // i is the counter through the original UpdateNum list
  int t = 1;  // t is the counter through the NewUpdateNum list
  int LowZeroThreshold =
      0;  // minimum number of consecutive zero's to encounter before optimizing
  int HighZeroThreshold =
      100000;  // maximum number of consecutive zero's to optimize

  // Loop through UpdateNum[]
  while (i <= count) {
    if (UpdateNum[i] != 0) {
      UpdateNum[t] = UpdateNum[i];
      i++;
      t++;
    } else  // found a 0
    {       // now we need to scan to find the # of zeros
      int j = 1;
      while (((i + j) < (count + 1)) && (UpdateNum[i + j] == 0)) {
        j++;
      }  // if this fails, then AA[i+j]!=0
      if ((i + j) < (count + 1)) {
        int numberofzeros = j;
        if (numberofzeros <= LowZeroThreshold) {
          for (int k = 1; k <= numberofzeros; k++) {
            UpdateNum[t] = 0;
            t++;
            i++;
          }
        } else {
          while (numberofzeros > HighZeroThreshold) {
            numberofzeros = numberofzeros - HighZeroThreshold;
            UpdateNum[t] = -HighZeroThreshold;
            t++;
          }
          UpdateNum[t] = -numberofzeros;
          t++;
          UpdateNum[t] = UpdateNum[i + j];
          t++;
          i = i + j + 1;
        }
      } else {
        UpdateNum[t] = -(count + 1 - i - j);
        i = i + j + 1;
      }
    }
  }
  return t;
}

// June 7, 2005 :  Completed Scan capability, added on-screen display of scan
// progress.
// May 11, 2005:  added capability to change time and DDS settings too.
// Redesigned Scan structure May 03, 2005 existing problem: if the final value
// isn't exactly reached by the steps, then the last stage is skipped and the
// cycle doesn't end
// has to do with numsteps.  Should be programmed with ceiling(), not abs
/**
@brief Black magic
@todo Write documentation
*/
void UpdateScanValue(int Reset) {
  int cx, cy, cz;
  double cellval, nextcell;
  BOOL UseList;
  int hour, minute, second;
  static BOOL ScanUp;
  static int timesdid, counter;

  cx = PScan.Column;
  cy = PScan.Row;
  cz = PScan.Page;

  SetCtrlAttribute(panelHandle_sub2, SUBPANEL2, ATTR_VISIBLE, 1);

  // if Use_List is checked, then read values off of the SCAN_TABLE on the main
  // panel.
  GetCtrlVal(panelHandle7, SCANPANEL_CHECK_USE_LIST, &UseList);
  // Initialization on first iteration
  if (Reset == TRUE) {
    counter = 0;
    for (int i = 0; i < 1000; i++) {
      ScanBuffer[i].Step = 0;
      ScanBuffer[i].Iteration = 0;
      ScanBuffer[i].Value = 0;
    }

    // Copy information from the appropriate scan mode to the variables.
    switch (PScan.ScanMode) {
      case 0:  // set to analog
        ScanVal.End = PScan.Analog.End_Of_Scan;
        ScanVal.Start = PScan.Analog.Start_Of_Scan;
        ScanVal.Step = PScan.Analog.Scan_Step_Size;
        ScanVal.Iterations = PScan.Analog.Iterations_Per_Step;
        break;
      case 1:  // time scan
        ScanVal.End = PScan.Time.End_Of_Scan;
        ScanVal.Start = PScan.Time.Start_Of_Scan;
        ScanVal.Step = PScan.Time.Scan_Step_Size;
        ScanVal.Iterations = PScan.Time.Iterations_Per_Step;
        break;
    }

    // if we are set to use the scan list instead of a linear
    // scan, then read first value
    if (UseList) {
      GetTableCellVal(panelHandle, PANEL_SCAN_TABLE, MakePoint(1, 1), &cellval);
      ScanVal.Start = cellval;
    }
    timesdid = 0;
    ScanVal.Current_Step = 0;
    ScanVal.Current_Iteration = -1;
    ScanVal.Current_Value = ScanVal.Start;

    // determine the sign of the step and correct if necessary
    if (ScanVal.End >= ScanVal.Start) {
      ScanUp = TRUE;
      if (ScanVal.Step < 0) {
        ScanVal.Step = -ScanVal.Step;
      }
    } else {
      ScanUp = FALSE;  // ie. we scan downwards
      if (ScanVal.Step > 0) {
        ScanVal.Step = -ScanVal.Step;
      }
    }
  }  // Done setting/resetting values

  // numsteps to depend on mode
  if (UseList) {
    // UseList=TRUE .... therefore using table of Scan Values
    ScanVal.Current_Iteration++;
    if (ScanVal.Current_Iteration >= ScanVal.Iterations) {
      ScanVal.Current_Iteration = 0;
      ScanVal.Current_Step++;
      // read next element of scan list
      GetTableCellVal(panelHandle, PANEL_SCAN_TABLE,
                      MakePoint(1, ScanVal.Current_Step + 1), &cellval);
      // indicate which element of the list we are currently using
      SetTableCellAttribute(panelHandle, PANEL_SCAN_TABLE,
                            MakePoint(1, ScanVal.Current_Step + 1),
                            ATTR_TEXT_BGCOLOR, VAL_LT_GRAY);
      SetTableCellAttribute(panelHandle, PANEL_SCAN_TABLE,
                            MakePoint(1, ScanVal.Current_Step),
                            ATTR_TEXT_BGCOLOR, VAL_WHITE);
      ScanVal.Current_Value = cellval;
      ChangedVals = TRUE;
    }
  }

  else {
    // UseList=FALSE.... therefor assume linear scanning
    // calculate number of steps in the ramp
    int numsteps = ceil(fabs((ScanVal.Start - ScanVal.End) / ScanVal.Step));
    PScan.ScanDone = FALSE;
    timesdid++;
    ScanVal.Current_Iteration++;

    if ((ScanVal.Current_Iteration >= ScanVal.Iterations) &&
        (ScanVal.Current_Step < numsteps))  // update the step at correct time
    {
      ScanVal.Current_Iteration = 0;
      ScanVal.Current_Step++;
      ScanVal.Current_Value = ScanVal.Current_Value + ScanVal.Step;
      ChangedVals = TRUE;
    }
    // if we are at the last step, then set the scan value to the last value (in
    // case the step size causes the scan to go too far
    if ((ScanVal.Current_Value >= ScanVal.End) && (ScanUp == TRUE)) {
      ScanVal.Current_Value = ScanVal.End;
    }

    if ((ScanVal.Current_Value <= ScanVal.End) && (ScanUp == FALSE)) {
      ScanVal.Current_Value = ScanVal.End;
    }
  }

  // insert current scan values into the tables , so they are included in the
  // next BuildUpdateList
  switch (PScan.ScanMode) {
    case 0:  // Analog value
      AnalogTable[cx][cy][cz].fval = ScanVal.Current_Value;
      AnalogTable[cx][cy][cz].fcn = PScan.Analog.Analog_Mode;
      break;
    case 1:  // Time duration
      TimeArray[cx][cz] = ScanVal.Current_Value;
      break;
  }

  // Record current scan information into a string buffer, so we can write it to
  // disk later.
  GetSystemTime(&hour, &minute, &second);
  ScanBuffer[counter].Step = ScanVal.Current_Step;
  ScanBuffer[counter].Iteration = ScanVal.Current_Iteration;
  ScanBuffer[counter].Value = ScanVal.Current_Value;
  ScanBuffer[0].BufferSize = counter;
  sprintf(ScanBuffer[counter].Time, "%d:%d:%d", hour, minute, second);
  counter++;

  // display current scan parameters on screen
  SetCtrlVal(panelHandle_sub2, SUBPANEL2_NUM_SCANVAL, ScanVal.Current_Value);
  SetCtrlVal(panelHandle_sub2, SUBPANEL2_NUM_SCANSTEP, ScanVal.Current_Step);
  SetCtrlVal(panelHandle_sub2, SUBPANEL2_NUM_SCANITER,
             ScanVal.Current_Iteration);

  // check for end condition
  if (UseList)  // Scanning ends if we program -999 into a cell of the Scan List
  {
    GetTableCellVal(panelHandle, PANEL_SCAN_TABLE,
                    MakePoint(1, ScanVal.Current_Step + 2), &nextcell);
    if ((nextcell <= -999) &&
        (ScanVal.Current_Iteration >= ScanVal.Iterations - 1)) {
      PScan.ScanDone = TRUE;
    }
  }

  else  // not using the ScanTable
  {
    if (ScanUp) {
      if ((ScanVal.Current_Value >= ScanVal.End) &&
          (ScanVal.Current_Iteration >= ScanVal.Iterations - 1)) {  // Done Scan
        PScan.ScanDone = TRUE;  // Flag used in RunOnce() to initiate a stop
      }
    } else {
      if ((ScanVal.Current_Value <= ScanVal.End) &&
          (ScanVal.Current_Iteration >= ScanVal.Iterations - 1)) {  // Done Scan
        PScan.ScanDone = TRUE;  // Flag used in RunOnce() to initiate a stop
      }
    }
  }  // done checking the scan is done

  // if the scan is done, then cleanup and write the starting values back into
  // the tables
  if (PScan.ScanDone == TRUE) {  // reset initial values in the tables
    AnalogTable[cx][cy][cz].fval = PScan.Analog.Start_Of_Scan;
    TimeArray[cx][cz] = PScan.Time.Start_Of_Scan;

    // hide the information panel
    SetCtrlAttribute(panelHandle_sub2, SUBPANEL2, ATTR_VISIBLE, 0);
    ExportScanBuffer();  // prompt to write out information
  }
}

/**
@brief Loads panel values from .pan and .arr files.
*/
void LoadSettings(void) {
  int status = ConfirmPopup(
      "Load",
      "Are you sure you want to load a new panel?\nUnsaved data will be lost!");
  if (status == 0) {
    return;  // Don't load
  }
  char fsavename[500];
  // prompt for a file, if selected then load the Panel and Arrays
  status = FileSelectPopupEx("C:\\UserDate\\Data", "*.pan", "", "Load Settings",
                             VAL_LOAD_BUTTON, 0, 0, fsavename);
  if (status != VAL_EXISTING_FILE_SELECTED) {
    return;
  }
  if (RecallPanelState(panelHandle, fsavename, 1) < 0) {
    MessagePopup("Load error", "Failed to load from file");
    return;
  }
  RecallPanelState(commentsHandle, fsavename, 2);
  LoadArrays(fsavename, strlen(fsavename));
  SetPanelAttribute(panelHandle, ATTR_TITLE, fsavename);
  // Reset button state and redraw table by simulating click on first page
  // button
  TOGGLE_CALLBACK(panelHandle, ButtonArray[1], EVENT_COMMIT, NULL, 0, 0);
}

/**
@brief Saves panel values to .pan and .arr files.
*/
void SaveSettings(void) {
  char fsavename[500];

  int status =
      FileSelectPopupEx("C:\\UserDate\\Data", "*.pan", "", "Save Settings",
                        VAL_SAVE_BUTTON, 0, 0, fsavename);
  if (status != VAL_NO_FILE_SELECTED) {
    SavePanelState(panelHandle, fsavename, 1);
    SavePanelState(commentsHandle, fsavename, 2);
    SaveArrays(fsavename, strlen(fsavename));
    SetPanelAttribute(panelHandle, ATTR_TITLE, fsavename);
  } else {
    MessagePopup("File Error", "No file was selected");
  }
}

/**
@brief Helper function to alternate color every three rows
@param index 1-based index to get color for
@return Hex code for gray or light gray, depending on index
@author Kerry Wang
*/
int ColorPicker(int index) {
  index--;  // correct for 1-based indices
  if ((index / 3) % 2)
    return VAL_GRAY;
  else
    return 0x00B0B0B0;
}

/**
@brief Redraws analog and digital tables.
@param isdimmed Whether or not to dim disabled columns
*/
void DrawNewTable(int isdimmed) {
  if (IsPageChecked(currentpage) == FALSE) {  // dim the tables
    SetCtrlAttribute(panelHandle, PANEL_ANALOGTABLE, ATTR_DIMMED, 1);
    SetCtrlAttribute(panelHandle, PANEL_DIGTABLE, ATTR_DIMMED, 1);
    SetCtrlAttribute(panelHandle, PANEL_TIMETABLE, ATTR_DIMMED, 1);
  } else {  // undim the tables
    SetCtrlAttribute(panelHandle, PANEL_ANALOGTABLE, ATTR_DIMMED, 0);
    SetCtrlAttribute(panelHandle, PANEL_DIGTABLE, ATTR_DIMMED, 0);
    SetCtrlAttribute(panelHandle, PANEL_TIMETABLE, ATTR_DIMMED, 0);
  }

  for (int i = 1; i <= NUMBEROFCOLUMNS; i++)  // scan over the columns
  {
    SetTableCellAttribute(panelHandle, PANEL_TIMETABLE, MakePoint(i, 1),
                          ATTR_CELL_DIMMED, 0);
    // scan over analog channels
    for (int j = 1; j <= NUMBERANALOGCHANNELS; j++) {
      int cmode = AnalogTable[i][j][currentpage].fcn;
      double vnow = AnalogTable[i][j][currentpage].fval;

      if (cmode != 6) {
        // write the ending value into the cell
        SetTableCellAttribute(panelHandle, PANEL_ANALOGTABLE, MakePoint(i, j),
                              ATTR_CTRL_VAL, vnow);
      } else if (i == 1 && currentpage == 1) {
        ConfirmPopup("User Error",
                     "Do not use \"Same as Previous\" Setting for Column 1 "
                     "Page 1.\nThis results in unstable behaviour.\nResetting "
                     "Cell Function to default: step");
        SetTableCellAttribute(panelHandle, PANEL_ANALOGTABLE, MakePoint(i, j),
                              ATTR_CTRL_VAL, 0.0);
        cmode = 1;
        AnalogTable[1][j][1].fcn = cmode;
        AnalogTable[1][j][1].fval = 0.0;
        AnalogTable[1][j][1].tscale = 0.0;
      } else {
        // 888 indicates cell will take value of previous cell
        SetTableCellAttribute(panelHandle, PANEL_ANALOGTABLE, MakePoint(i, j),
                              ATTR_CTRL_VAL, 888.0);
      }

      // get Analog table parameters for that cell... e.g. start/end values,
      // function to get there
      SetTableCellAttribute(panelHandle, PANEL_ANALOGTABLE, MakePoint(i, j),
                            ATTR_TEXT_COLOR, VAL_BLACK);
      // Change the cell color depending on the function type
      switch (cmode) {
        case 1:  // step
          if (vnow == 0) {
            SetTableCellAttribute(panelHandle, PANEL_ANALOGTABLE,
                                  MakePoint(i, j), ATTR_TEXT_BGCOLOR,
                                  ColorPicker(j));
          } else {
            SetTableCellAttribute(panelHandle, PANEL_ANALOGTABLE,
                                  MakePoint(i, j), ATTR_TEXT_BGCOLOR, VAL_RED);
          }
          break;

        case 2:  // linear ramp
          SetTableCellAttribute(panelHandle, PANEL_ANALOGTABLE, MakePoint(i, j),
                                ATTR_TEXT_BGCOLOR, VAL_GREEN);
          break;

        case 3:  // exponential ramp
          SetTableCellAttribute(panelHandle, PANEL_ANALOGTABLE, MakePoint(i, j),
                                ATTR_TEXT_BGCOLOR, VAL_BLUE);
          SetTableCellAttribute(panelHandle, PANEL_ANALOGTABLE, MakePoint(i, j),
                                ATTR_TEXT_COLOR, VAL_WHITE);
          break;

        case 4:  // constant jerk function
          SetTableCellAttribute(panelHandle, PANEL_ANALOGTABLE, MakePoint(i, j),
                                ATTR_TEXT_BGCOLOR, VAL_MAGENTA);
          break;

        case 5:  // Sine wave output
          SetTableCellAttribute(panelHandle, PANEL_ANALOGTABLE, MakePoint(i, j),
                                ATTR_TEXT_BGCOLOR, VAL_CYAN);
          break;

        case 6:  // Same as Previous Output
          SetTableCellAttribute(panelHandle, PANEL_ANALOGTABLE, MakePoint(i, j),
                                ATTR_TEXT_BGCOLOR, VAL_YELLOW);
          break;

        default:
          SetTableCellAttribute(panelHandle, PANEL_ANALOGTABLE, MakePoint(i, j),
                                ATTR_TEXT_BGCOLOR, ColorPicker(j));
          break;
      }
    }
    // scan over digital channels
    for (int j = 1; j <= NUMBERDIGITALCHANNELS; j++) {
      // if a digital value is high, colour the cell red
      if (DigTableValues[i][j][currentpage] == 1) {
        SetTableCellAttribute(panelHandle, PANEL_DIGTABLE, MakePoint(i, j),
                              ATTR_TEXT_BGCOLOR, VAL_RED);
      } else {
        SetTableCellAttribute(panelHandle, PANEL_DIGTABLE, MakePoint(i, j),
                              ATTR_TEXT_BGCOLOR, ColorPicker(j));
      }
    }  // Done digital drawing.

    // update the times row
    SetTableCellAttribute(panelHandle, PANEL_TIMETABLE, MakePoint(i, 1),
                          ATTR_CTRL_VAL, TimeArray[i][currentpage]);
  }
  // So far, all columns are undimmed
  // now check if we need to dim out any columns(of timetable,AnalogTable and
  // DigTable
  if (isdimmed) {
    SetTableCellRangeAttribute(panelHandle, PANEL_ANALOGTABLE,
                               VAL_TABLE_ENTIRE_RANGE, ATTR_CELL_DIMMED,
                               TRUE);  // First, dim all cells
    SetTableCellRangeAttribute(panelHandle, PANEL_DIGTABLE,
                               VAL_TABLE_ENTIRE_RANGE, ATTR_CELL_DIMMED, TRUE);
    SetTableCellRangeAttribute(panelHandle, PANEL_TIMETABLE,
                               VAL_TABLE_ENTIRE_RANGE, ATTR_CELL_DIMMED, TRUE);
    for (int col = 1; col <= NUMBEROFCOLUMNS; col++) {
      if (TimeArray[col][currentpage] == 0) {
        // If we find a zero, stop and leave the rest dimmed
        break;
      } else if (TimeArray[col][currentpage] > 0) {
        // If time is positive, undim the column
        SetTableCellAttribute(panelHandle, PANEL_TIMETABLE, MakePoint(col, 1),
                              ATTR_CELL_DIMMED, FALSE);
        SetTableCellRangeAttribute(panelHandle, PANEL_ANALOGTABLE,
                                   VAL_TABLE_COLUMN_RANGE(col),
                                   ATTR_CELL_DIMMED, FALSE);
        SetTableCellRangeAttribute(panelHandle, PANEL_DIGTABLE,
                                   VAL_TABLE_COLUMN_RANGE(col),
                                   ATTR_CELL_DIMMED, FALSE);
      }
    }
  } else {
    SetTableCellRangeAttribute(panelHandle, PANEL_ANALOGTABLE,
                               VAL_TABLE_ENTIRE_RANGE, ATTR_CELL_DIMMED,
                               FALSE);  // Undim everything
    SetTableCellRangeAttribute(panelHandle, PANEL_DIGTABLE,
                               VAL_TABLE_ENTIRE_RANGE, ATTR_CELL_DIMMED, FALSE);
    SetTableCellRangeAttribute(panelHandle, PANEL_TIMETABLE,
                               VAL_TABLE_ENTIRE_RANGE, ATTR_CELL_DIMMED, FALSE);
  }

  if ((currentpage == PScan.Page) && (PScan.Scan_Active == TRUE)) {
    // display the cell active for a parameter scan
    switch (PScan.ScanMode) {
      case 0:
        SetTableCellAttribute(panelHandle, PANEL_ANALOGTABLE,
                              MakePoint(PScan.Column, PScan.Row),
                              ATTR_TEXT_BGCOLOR, VAL_DK_YELLOW);
        break;
      case 1:
        SetTableCellAttribute(panelHandle, PANEL_TIMETABLE,
                              MakePoint(PScan.Column, 1), ATTR_TEXT_BGCOLOR,
                              VAL_DK_YELLOW);
        break;
      case 2:
        SetTableCellAttribute(panelHandle, PANEL_ANALOGTABLE,
                              MakePoint(PScan.Column, 25), ATTR_TEXT_BGCOLOR,
                              VAL_DK_YELLOW);
        break;
    }
  }
}

/**
@brief Callback for the analog channel settings menu option.
@todo Should be merged into MENU_CALLBACK().
*/
void CVICALLBACK ANALOGSET_CALLBACK(int menuBar, int menuItem,
                                    void* callbackData, int panel) {
  ChangedVals = TRUE;
  DisplayPanel(panelHandle2);
}

/**
@brief Callback for the digital channel settings menu option.
@todo Should be merged into MENU_CALLBACK().
*/
void CVICALLBACK DIGITALSET_CALLBACK(int menuBar, int menuItem,
                                     void* callbackData, int panel) {
  ChangedVals = TRUE;
  DisplayPanel(panelHandle3);
}

/**
@brief Callback for the menu bar. Currently handles the saving and loading
options.
*/
void CVICALLBACK MENU_CALLBACK(int menuBar, int menuItem, void* callbackData,
                               int panel) {
  switch (menuItem) {
    case MENU_FILE_SAVESET:
      SaveSettings();
      break;

    case MENU_FILE_LOADSET:
      LoadSettings();
      break;
  }
}

/**
@brief Callback for the analog table. On double-click, opens the analog control
for the clicked cell.
*/
int CVICALLBACK ANALOGTABLE_CALLBACK(int panel, int control, int event,
                                     void* callbackData, int eventData1,
                                     int eventData2) {
  ChangedVals = TRUE;
  switch (event) {
    case EVENT_LEFT_DOUBLE_CLICK:
      // Update last-clicked cell
      Point cpoint = {0, 0};
      GetActiveTableCell(panelHandle, PANEL_ANALOGTABLE, &cpoint);
      currentx = cpoint.x;
      currenty = cpoint.y;
      // now set the indicators in the control panel title..ie units
      SetControlPanel();
      // Display indices in window title
      char buff[80] = "";
      sprintf(buff, "x:%d y:%d z:%d", currentx, currenty, currentpage);
      SetPanelAttribute(panelHandle4, ATTR_TITLE, buff);
      // Read the mouse position and open window there.
      int pixleft = 0, pixtop = 0;
      GetGlobalMouseState(NULL, &pixleft, &pixtop, NULL, NULL, NULL);
      SetPanelAttribute(panelHandle4, ATTR_LEFT, pixleft);
      SetPanelAttribute(panelHandle4, ATTR_TOP, pixtop);
      DisplayPanel(panelHandle4);
      break;
  }
  return 0;
}

/**
@brief Callback for the page buttons. Left-click to go to that page, right-click to rename the page.
@param control ID of the clicked button.
*/
int CVICALLBACK TOGGLE_CALLBACK(int panel, int control, int event,
                                void* callbackData, int eventData1,
                                int eventData2) {
  switch (event) {
    case EVENT_COMMIT:
      for (int i = 1; i <= NUMBEROFPAGES; i++) {
        // Go through ButtonArray, set the clicked button to on and all others to off
        SetCtrlVal(panelHandle, ButtonArray[i], control == ButtonArray[i]);
        if (control == ButtonArray[i]) {
          setVisibleLabel(i);
          currentpage = i;
        }
      }
      ChangedVals = TRUE;
      DrawNewTable(0);
      break;
    case EVENT_RIGHT_CLICK:
      // Get index of clicked button in ButtonArray
      int i = 1;
      for (i = 1; i <= NUMBEROFPAGES; i++) {
        if (ButtonArray[i] == control) {
          break;
        }
      }
      char buff[80];
      char prompt[46];
      sprintf(prompt, "Enter a new label for phase %d control button", i);
      int status = PromptPopup("Enter control button label", prompt, buff,
                               sizeof buff - 2);
      if (status == 0) {
        SetCtrlAttribute(panelHandle, ButtonArray[i], ATTR_ON_TEXT, buff);
        SetCtrlAttribute(panelHandle, ButtonArray[i], ATTR_OFF_TEXT, buff);
      }
      break;
  }
  return 0;
}

/**
@brief Callback for the page enable checkboxes. On toggle, dims or undims the tables.
*/
int CVICALLBACK CHECKBOX_CALLBACK(int panel, int control, int event,
                                  void* callbackData, int eventData1,
                                  int eventData2) {
  switch (event) {
    case EVENT_COMMIT:
      DrawNewTable(0);
      break;
  }
  return 0;
}

/**
@brief Callback for the digital table. On double-click, toggles the clicked cell and updates the entry in DigTableValues.
*/
int CVICALLBACK DIGTABLE_CALLBACK(int panel, int control, int event,
                                  void* callbackData, int eventData1,
                                  int eventData2) {
  ChangedVals = TRUE;
  switch (event) {
    case EVENT_LEFT_DOUBLE_CLICK:
      Point pval = {0, 0};
      GetActiveTableCell(panelHandle, PANEL_DIGTABLE, &pval);
      int digval = DigTableValues[pval.x][pval.y][currentpage];
      if (digval == 0) {
        DigTableValues[pval.x][pval.y][currentpage] = 1;
      } else {
        DigTableValues[pval.x][pval.y][currentpage] = 0;
      }
      DrawNewTable(0);
      break;
  }
  return 0;
}

/**
@brief Callback for the time table. Updates the time table and all analog cells in that column.
*/
int CVICALLBACK TIMETABLE_CALLBACK(int panel, int control, int event,
                                   void* callbackData, int eventData1,
                                   int eventData2) {
  ChangedVals = TRUE;
  switch (event) {
    case EVENT_COMMIT:
      for (int i = 1; i <= NUMBEROFCOLUMNS; i++) {
        double dval;
        GetTableCellVal(panelHandle, PANEL_TIMETABLE, MakePoint(i, 1), &dval);
        TimeArray[i][currentpage] = dval;
        // now rescale the time scale for waveform fcn. Go over all analog
        // channels
        double oldtime = TimeArray[i][currentpage];
        double ratio = dval / oldtime;
        if (oldtime == 0) {
          ratio = 1;
        }

        for (int j = 1; j <= NUMBERANALOGCHANNELS; j++) {
          double tscaleold =
              AnalogTable[i][j][currentpage]
                  .tscale;  // use this and next line to auto-scale the time
          AnalogTable[i][j][currentpage].tscale = tscaleold * ratio;
        }
      }
      break;
    case EVENT_LEFT_DOUBLE_CLICK:
      DrawNewTable(0);
      break;
  }
  return 0;
}

/**
@brief Callback for the Boot Adwin menu option.
@todo Should be merged into MENU_CALLBACK().
*/
void CVICALLBACK BOOTADWIN_CALLBACK(int menuBar, int menuItem,
                                    void* callbackData, int panel) {
  Boot("ADbasic\\ADwin11.btl", 0);
  Load_Process("ADbasic\\TransferDataExternalClock.TB1");
}

/**
@brief Callback for the Clear Panel menu option.
@todo Should be merged into MENU_CALLBACK().
*/
void CVICALLBACK CLEARPANEL_CALLBACK(int menuBar, int menuItem,
                                     void* callbackData, int panel) {
  // add code to clear all the analog and digital information.....
  int status =
      ConfirmPopup("Clear Panel", "Do you really want to clear the panel?");
  if (status == 1) {
    ChangedVals = 1;
    for (int col = 1; col <= NUMBEROFCOLUMNS; col++) {
      for (int channel = 1; channel <= NUMBERANALOGCHANNELS; channel++) {
        for (int page = 0; page <= NUMBEROFPAGES; page++) {
          AnalogTable[col][channel][page].fcn = 1;
          AnalogTable[col][channel][page].fval = 0;
          AnalogTable[col][channel][page].tscale = 0;
          DigTableValues[col][channel][page] = 0;
          TimeArray[col][page] = 0;
        }
      }
    }
    DrawNewTable(0);
  }
}

/**
@brief Callback for the Insert Column menu option. Calls ShiftColumn() in insert mode.
@todo Should be merged into MENU_CALLBACK().
*/
void CVICALLBACK INSERTCOLUMN_CALLBACK(int menuBar, int menuItem,
                                       void* callbackData, int panel) {
  Point cpoint = {0, 0};
  GetActiveTableCell(panelHandle, PANEL_TIMETABLE, &cpoint);
  char message[100];
  sprintf(message, "Really insert column at %d? The last column will be lost!", cpoint.x);
  int status = ConfirmPopup("Insert column", message);
  if (status == 1) {
    ShiftColumn(cpoint.x, currentpage, -1);  // -1 for insert mode
  }
}

/**
@brief Callback for the Delete Column menu option. Calls ShiftColumn() in delete mode.
@todo Should be merged into MENU_CALLBACK().
*/
void CVICALLBACK DELETECOLUMN_CALLBACK(int menuBar, int menuItem,
                                       void* callbackData, int panel) {
  Point cpoint = {0, 0};
  GetActiveTableCell(panelHandle, PANEL_TIMETABLE, &cpoint);
  char message[40];
  sprintf(message, "Really delete column %d?", cpoint.x);
  int status = ConfirmPopup("Delete column", message);
  if (status == 1) {
    ShiftColumn(cpoint.x, currentpage, 1);  // 1 for delete mode
  }
}

/**
@brief Inserts or deletes a column on a page by shifting columns left or right.
@param col 1-based index to insert/delete a column at.
@param page Page to insert/delete column on.
@param dir -1 to insert, 1 to delete.

When inserting, the last column on the page is lost.
*/
void ShiftColumn(int col, int page, int dir) {
  int start = 0;
  int zerocol = 0;
  if (dir == -1) {
    // Insertion: Starting from the end, each column copies its left neighbor
    // Then zero out selected column
    start = NUMBEROFCOLUMNS;
    zerocol = col;
  } else if (dir == 1) {
    // Deletion: Starting from the selected column, each column copies its right neighbor
    // Then zero out last column
    start = col;
    zerocol = NUMBEROFCOLUMNS;
  } else {
    Assert(0);
  }

  // Shift columns left or right depending on dir
  for (int i = 0; i < NUMBEROFCOLUMNS - col; i++) {
    TimeArray[start + dir * i][page] = TimeArray[start + dir * (i + 1)][page];

    for (int j = 1; j <= NUMBERANALOGCHANNELS; j++) {
      AnalogTable[start + dir * i][j][page].fcn =
          AnalogTable[start + dir * (i + 1)][j][page].fcn;
      AnalogTable[start + dir * i][j][page].fval =
          AnalogTable[start + dir * (i + 1)][j][page].fval;
      AnalogTable[start + dir * i][j][page].tscale =
          AnalogTable[start + dir * (i + 1)][j][page].tscale;
    }

    for (int j = 1; j <= NUMBERDIGITALCHANNELS; j++) {
      DigTableValues[start + dir * i][j][page] =
          DigTableValues[start + dir * (i + 1)][j][page];
    }
  }

  // Zero out zerocol
  if (dir == 1) {
    TimeArray[zerocol][page] = 0;
  }

  for (int j = 1; j <= NUMBERANALOGCHANNELS; j++) {
    AnalogTable[zerocol][j][page].fcn = 1;
    AnalogTable[zerocol][j][page].fval = 0;
    AnalogTable[zerocol][j][page].tscale = 1;
  }

  for (int j = 1; j <= NUMBERDIGITALCHANNELS; j++) {
    DigTableValues[zerocol][j][page] = 0;
  }
  ChangedVals = TRUE;
  DrawNewTable(0);
}

void CVICALLBACK COPYCOLUMN_CALLBACK(int menuBar, int menuItem,
                                     void* callbackData, int panel) {
  // All attributes of active column are replaced with those of the global
  // "clip" variables (from ClipColumn)

  char buff[20] = "", buff2[100] = "";
  Point cpoint = {0, 0};

  GetActiveTableCell(panelHandle, PANEL_TIMETABLE, &cpoint);

  // User Confirmation of Selected Column
  sprintf(buff, "%d", cpoint.x);
  strcat(buff2, "Confirm Copy of column ");
  strcat(buff2, buff);
  int status = ConfirmPopup("Array Manipulation:Copy", buff2);

  if (status == 1) {
    ClipColumn = cpoint.x;
    TimeClip = TimeArray[cpoint.x][currentpage];
    for (int j = 1; j <= NUMBERANALOGCHANNELS; j++) {
      AnalogClip[j].fcn = AnalogTable[cpoint.x][j][currentpage].fcn;
      AnalogClip[j].fval = AnalogTable[cpoint.x][j][currentpage].fval;
      AnalogClip[j].tscale = AnalogTable[cpoint.x][j][currentpage].tscale;
    }

    for (int j = 1; j <= NUMBERDIGITALCHANNELS; j++) {
      DigClip[j] = DigTableValues[cpoint.x][j][currentpage];
    }
    DrawNewTable(0);  // draws undimmed table if already dimmed
  }
}

/**
Replaces all the values in the selected column with the global "clip" values
*/
void CVICALLBACK PASTECOLUMN_CALLBACK(int menuBar, int menuItem,
                                      void* callbackData, int panel) {
  Point cpoint = {0, 0};
  ChangedVals = 1;

  // Ensures a column has been copied to the clipboard
  if (ClipColumn > 0) {
    char buff[100] = "";
    // User Confirmation of Copy Function
    GetActiveTableCell(panelHandle, PANEL_TIMETABLE, &cpoint);
    sprintf(
        buff,
        "Confirm Copy of Column %d to %d?\nContents of Column %d will be lost.",
        ClipColumn, cpoint.x, cpoint.x);
    int status = ConfirmPopup("Paste Column", buff);

    if (status == 1) {
      TimeArray[cpoint.x][currentpage] = TimeClip;
      for (int j = 1; j <= NUMBERANALOGCHANNELS; j++) {
        AnalogTable[cpoint.x][j][currentpage].fcn = AnalogClip[j].fcn;
        AnalogTable[cpoint.x][j][currentpage].fval = AnalogClip[j].fval;
        AnalogTable[cpoint.x][j][currentpage].tscale = AnalogClip[j].tscale;
        DigTableValues[cpoint.x][j][currentpage] = DigClip[j];
      }
      DrawNewTable(0);
    }
  } else
    ConfirmPopup("Copy Column", "No Column Selected");
}

/** Loads all Panel attributes and values which are not saved automatically by
the NI function SavePanelState. the values are stored in the .arr file by
SaveArrays.

Note that if the lengths of any of the data arrays are changed previous saves
will not be able to be loaded. If necessary See AdwinGUI Panel Converter V11-V12
(created June 01, 2006)

@param savedname[]
@param csize
*/
void LoadArrays(char savedname[500], int csize) {
  FILE* fdata;
  char buff[500] = "";
  char buttonName[80];
  strncat(buff, savedname, csize - 4);
  strcat(buff, ".arr");
  if ((fdata = fopen(buff, "rb")) == NULL) {
    MessagePopup("Load Error", "Failed to load data arrays");
  }
  // now for the times.
  fread(&TimeArray, (sizeof TimeArray), 1, fdata);
  // and the analog data
  fread(&AnalogTable, (sizeof AnalogTable), 1, fdata);
  fread(&DigTableValues, (sizeof DigTableValues), 1, fdata);
  fread(&AChName, (sizeof AChName), 1, fdata);
  fread(&DChName, sizeof DChName, 1, fdata);

  for (int page = 1; page <= NUMBEROFPAGES; page++) {
    fread(&buttonName, sizeof buttonName, 1, fdata);
    SetCtrlAttribute(panelHandle, ButtonArray[page], ATTR_ON_TEXT, buttonName);
    SetCtrlAttribute(panelHandle, ButtonArray[page], ATTR_OFF_TEXT, buttonName);
  }

  fclose(fdata);

  SetAnalogChannels();
  SetDigitalChannels();
}

/**
Saves all Panel attributes and values which are not saved automatically by the
NI function SavePanelState. The values are stored in the .arr file.

Note that if the lengths of any of the data arrays are changed previous saves
will not be able to be laoded. If necessary See AdwinGUI Panel Converter V11-V12
(created June 01, 2006)

@param savedname[]
@param csize
*/
void SaveArrays(char savedname[500], int csize) {
  FILE* fdata;
  char buff[500];
  char buttonName[80];
  strncpy(buff, savedname, csize - 4);
  buff[csize - 4] = 0;
  strcat(buff, ".arr");
  if ((fdata = fopen(buff, "wb")) == NULL) {
    char buff2[100];
    strcpy(buff2, "Failed to save data arrays. \n Panel Filename received\n");
    strcat(buff2, buff);
    MessagePopup("Save Error", buff2);
  }
  // now for the times.
  fwrite(&TimeArray, sizeof TimeArray, 1, fdata);
  // and the analog data
  fwrite(&AnalogTable, sizeof AnalogTable, 1, fdata);
  fwrite(&DigTableValues, sizeof DigTableValues, 1, fdata);

  fwrite(&AChName, sizeof AChName, 1, fdata);
  fwrite(&DChName, sizeof DChName, 1, fdata);

  for (int page = 1; page <= NUMBEROFPAGES; page++) {
    GetCtrlAttribute(panelHandle, ButtonArray[page], ATTR_ON_TEXT, buttonName);
    fwrite(&buttonName, sizeof buttonName, 1, fdata);
  }

  fclose(fdata);
}

double CheckIfWithinLimits(double OutputVoltage, int linenumber) {
  double NewOutput;
  NewOutput = OutputVoltage;
  if (OutputVoltage > AChName[linenumber].maxvolts)
    NewOutput = AChName[linenumber].maxvolts;
  if (OutputVoltage < AChName[linenumber].minvolts)
    NewOutput = AChName[linenumber].minvolts;
  return NewOutput;
}

void CVICALLBACK EXPORT_PANEL_CALLBACK(int menuBar, int menuItem,
                                       void* callbackData, int panel) {
  char fexportname[260];

  int status = FileSelectPopupEx("", "*.export", "", "Export Panel?",
                                 VAL_SAVE_BUTTON, 0, 0, fexportname);
  if (status == VAL_NO_FILE_SELECTED) {
    return;
  }
  ExportPanel(fexportname, strlen(fexportname));
}

/**
Export the panel to a file and open it in the Python reader
@author Kerry Wang
*/
void CVICALLBACK EXPORT_PYTHON_CALLBACK(int menuBar, int menuItem,
                                        void* callbackData, int panel) {
  char* appdata = getenv("localappdata");
  char* fexportname = StrDup(appdata);
  AppendString(&fexportname, "\\Temp\\pyreader.export", -1);
  ExportPanel(fexportname, strlen(fexportname));
  char* launchname = StrDup("pythonw3.exe pyreader\\pyreader.py ");
  AppendString(&launchname, appdata, -1);
  AppendString(&launchname, "\\Temp\\pyreader.export", -1);
  LaunchExecutableEx(launchname, LE_SHOWNORMAL, NULL);
}

void ExportPanel(char fexportname[200], int fnamesize) {
  FILE* fexport;
  char buff[500], bigbuff[2000];
  char fcnmode[] =
      "SLEJWH";  // step, linear, exponential, const-jerk, sine wave, hold
  double MetaTimeArray[500] = {0};
  int MetaDigitalArray[NUMBERDIGITALCHANNELS + 1][500] = {0};
  struct AnalogTableValues MetaAnalogArray[NUMBERANALOGCHANNELS + 1][500] = {0};
  int mindex, tsize;

  if ((fexport = fopen(fexportname, "w")) == NULL) {
    MessagePopup("Save Error", "Failed to save configuration file");
  }

  // Lets build the times list first...so we know how long it will be.
  // check each page...find used columns and dim out unused....(with 0 or
  // negative values)
  SetCtrlAttribute(panelHandle, PANEL_ANALOGTABLE, ATTR_TABLE_MODE, VAL_COLUMN);
  mindex = 0;

  // go through for each page
  for (int page = 1; page <= NUMBEROFPAGES; page++) {
    if (IsPageChecked(page)) {
      // if the page is selected
      // go through for each time column
      for (int col = 1; col <= NUMBEROFCOLUMNS; col++) {
        // ignore all columns after the first time=0
        if (TimeArray[col][page] == 0) {
          break;
        }
        // ignore columns with negative time
        else if (TimeArray[col][page] > 0) {
          mindex++;  // increase the number of columns counter
          MetaTimeArray[mindex] = TimeArray[col][page];

          // go through for each analog channel
          for (int channel = 1; channel <= NUMBERANALOGCHANNELS; channel++) {
            MetaAnalogArray[channel][mindex].fcn =
                AnalogTable[col][channel][page].fcn;
            MetaAnalogArray[channel][mindex].fval =
                AnalogTable[col][channel][page].fval;
            MetaAnalogArray[channel][mindex].tscale =
                AnalogTable[col][channel][page].tscale;
          }
          // go through each digital channel
          for (int channel = 1; channel <= NUMBERDIGITALCHANNELS; channel++) {
            MetaDigitalArray[channel][mindex] =
                DigTableValues[col][channel][page];
          }
        }
      }
    }
  }
  tsize = mindex;  // tsize is the number of columns
  // now write to file
  // write header
  sprintf(bigbuff, "T");
  for (int i = 1; i <= tsize; i++) {
    strcat(bigbuff, ",");
    sprintf(buff, "%.2f", MetaTimeArray[i]);
    strcat(bigbuff, buff);
  }
  strcat(bigbuff, "\n");
  fprintf(fexport, bigbuff);
  // done header, now write analog lines
  char* string;
  for (int j = 1; j <= NUMBERANALOGCHANNELS; j++) {
    string = StrDup("A,");
    AppendString(&string, AChName[j].chname, 50);
    AppendString(&string, ",", 1);
    sprintf(buff, "%d", AChName[j].chnum);
    AppendString(&string, buff, 3);
    for (int i = 1; i <= tsize; i++) {
      AppendString(&string, ",", 1);
      AppendString(&string, fcnmode + MetaAnalogArray[j][i].fcn - 1, 1);
      sprintf(buff, "%#.2f", MetaAnalogArray[j][i].fval);
      AppendString(&string, buff, -1);
      AppendString(&string, "/", 1);
      sprintf(buff, "%#.2f", MetaAnalogArray[j][i].tscale);
      AppendString(&string, buff, -1);
    }
    AppendString(&string, "\n", 1);
    fprintf(fexport, string);
  }

  // now do digital
  for (int j = 1; j <= NUMBERDIGITALCHANNELS; j++) {
    sprintf(bigbuff, "D,");
    strncat(bigbuff, DChName[j].chname, 50);
    strncat(bigbuff, ",", 1);
    sprintf(buff, "%d", DChName[j].chnum);
    strncat(bigbuff, buff, 3);
    for (int i = 1; i <= tsize; i++) {
      strcat(bigbuff, ",");
      sprintf(buff, "%d", MetaDigitalArray[j][i]);
      strcat(bigbuff, buff);
    }
    strncat(bigbuff, "\n", 1);
    fprintf(fexport, bigbuff);
  }
  fclose(fexport);
}

void CVICALLBACK CONFIG_EXPORT_CALLBACK(int menuBar, int menuItem,
                                        void* callbackData, int panel) {
  FILE* fconfig;
  char buff[500], fconfigname[200], buff3[31];

  FileSelectPopup("", "*.config", "", "Save Configuration", VAL_SAVE_BUTTON, 0,
                  0, 1, 1, fconfigname);

  if ((fconfig = fopen(fconfigname, "w")) == NULL) {
    MessagePopup("Save Error", "Failed to save configuration file");
  }
  // write out analog channel info
  sprintf(buff, "Analog Channels\n");
  fprintf(fconfig, buff);
  sprintf(buff, "Row,Channel,Name,tbias,tfcn,MaxVolts,MinVolts,Units\n");
  fprintf(fconfig, buff);
  for (int i = 1; i <= NUMBERANALOGCHANNELS; i++) {
    strncpy(buff3, AChName[i].chname, 30);
    sprintf(buff, "%d,%d,%s,%f,%f,%f,%f,%s\n", i, AChName[i].chnum, buff3,
            AChName[i].tbias, AChName[i].tfcn, AChName[i].maxvolts,
            AChName[i].minvolts, AChName[i].units);
    fprintf(fconfig, buff);
  }
  // Write out digital Channel info
  sprintf(buff, "Digital Channels\n");
  fprintf(fconfig, buff);
  sprintf(buff, "Row,Channel,Name\n");
  fprintf(fconfig, buff);

  for (int i = 1; i <= NUMBERDIGITALCHANNELS; i++) {
    sprintf(buff, "%d,%d,%s\n", i, DChName[i].chnum, DChName[i].chname);
    fprintf(fconfig, buff);
  }

  fclose(fconfig);
}

void CVICALLBACK COMPRESSION_CALLBACK(int menuBar, int menuItem,
                                      void* callbackData, int panel) {
  BOOL UseCompression;
  GetMenuBarAttribute(menuHandle, MENU_PREFS_COMPRESSION, ATTR_CHECKED,
                      &UseCompression);
  if (UseCompression) {
    SetMenuBarAttribute(menuHandle, MENU_PREFS_COMPRESSION, ATTR_CHECKED,
                        FALSE);
  } else {
    SetMenuBarAttribute(menuHandle, MENU_PREFS_COMPRESSION, ATTR_CHECKED, TRUE);
  }
}

void CVICALLBACK SIMPLETIMING_CALLBACK(int menuBar, int menuItem,
                                       void* callbackData, int panel) {
  BOOL Simple_Timing;
  GetMenuBarAttribute(menuHandle, MENU_PREFS_SIMPLETIMING, ATTR_CHECKED,
                      &Simple_Timing);
  if (Simple_Timing) {
    SetMenuBarAttribute(menuHandle, MENU_PREFS_SIMPLETIMING, ATTR_CHECKED,
                        FALSE);
  } else {
    SetMenuBarAttribute(menuHandle, MENU_PREFS_SIMPLETIMING, ATTR_CHECKED,
                        TRUE);
  }
}

void CVICALLBACK SCANSETTING_CALLBACK(int menuBar, int menuItem,
                                      void* callbackData, int panel) {
  InitializeScanPanel();
}

/**
Callback function to open the comments window.
@author Kerry Wang
**/
int CVICALLBACK CMD_COMMENTS_CALLBACK(int panel, int control, int event,
                                      void* callbackData, int eventData1,
                                      int eventData2) {
  switch (event) {
    case EVENT_COMMIT:
      DisplayPanel(commentsHandle);
      break;
  }

  return 0;
}

void ExportScanBuffer(void) {
  char fbuffername[250];

  int status = FileSelectPopup("", "*.scan", "", "Save Scan Buffer",
                               VAL_SAVE_BUTTON, 0, 0, 1, 1, fbuffername);
  if (status > 0) {
    FILE* fbuffer;
    if ((fbuffer = fopen(fbuffername, "w")) == NULL) {
      MessagePopup("Save Error", "Failed to save configuration file");
    }
    char buff[500];
    switch (PScan.ScanMode) {
      default:
      case 0:
        sprintf(buff, "Analog Scan\nRow,%d,Column,%d,Page,%d\n", PScan.Row,
                PScan.Column, PScan.Page);
        break;
      case 1:
        sprintf(buff, "Time Scan\nColumn,%d,Page,%d\n", PScan.Column,
                PScan.Page);
        break;
    }
    fprintf(fbuffer, buff);
    sprintf(buff, "Cycle,Value,Step,Iteration,Time\n");
    fprintf(fbuffer, buff);
    for (int i = 0; i <= ScanBuffer[0].BufferSize; i++) {
      double val = ScanBuffer[i].Value;
      int step = ScanBuffer[i].Step;
      int iter = ScanBuffer[i].Iteration;
      char date[100];
      sprintf(date, ScanBuffer[i].Time);
      sprintf(buff, "%d,%f,%d,%d,%s\n", i, val, step, iter, date);
      fprintf(fbuffer, buff);
    }
    fclose(fbuffer);
  }
}

/**
Copies the value of the active DigitalTabel value into the clipboard
*/
void CVICALLBACK Dig_Cell_Copy(int panelHandle, int controlID, int MenuItemID,
                               void* callbackData) {
  Point pval = {0, 0};

  GetActiveTableCell(panelHandle, PANEL_DIGTABLE, &pval);
  DigClip[0] = DigTableValues[pval.x][pval.y][currentpage];
}

/**
Pastes the value store in DigClip[0] by Dig_Cell_Copy into the selected Digital
Table Cells
*/
void CVICALLBACK Dig_Cell_Paste(int panelHandle, int controlID, int MenuItemID,
                                void* callbackData) {
  Rect selection;
  Point pval = {0, 0};

  GetTableSelection(
      panelHandle, PANEL_DIGTABLE,
      &selection);  // note: returns a 0 to all values if only 1 cell selected

  // Pasting into multiple cells
  if (selection.top > 0) {
    for (pval.y = selection.top;
         pval.y <= selection.top + (selection.height - 1); pval.y++) {
      for (pval.x = selection.left;
           pval.x <= selection.left + (selection.width - 1); pval.x++) {
        DigTableValues[pval.x][pval.y][currentpage] = DigClip[0];
      }
    }
  }
  // Pasting into single cell
  else if (selection.top == 0) {
    GetActiveTableCell(panelHandle, PANEL_DIGTABLE, &pval);
    DigTableValues[pval.x][pval.y][currentpage] = DigClip[0];
  }
  DrawNewTable(0);
}

/**
This function copies the contents of the active AnalogTable Cell to the
Clipboard Globals

Handles Analog Channels, it is called by right clicking on the Analog Table and
Selecing "Copy"
*/
void CVICALLBACK Analog_Cell_Copy(int panelHandle, int controlID,
                                  int MenuItemID, void* callbackData) {
  Point pval = {0, 0};

  GetActiveTableCell(panelHandle, PANEL_ANALOGTABLE, &pval);

  if (pval.y <= NUMBERANALOGCHANNELS) {
    AnalogClip[0].fcn = AnalogTable[pval.x][pval.y][currentpage].fcn;
    AnalogClip[0].fval = AnalogTable[pval.x][pval.y][currentpage].fval;
    AnalogClip[0].tscale = AnalogTable[pval.x][pval.y][currentpage].tscale;
  }
}

/**
Replaces Highlighted Cell contents with the values copied to the clipboard using
Analog_Cell_Copy This function Handles copies and pastes of analog channel data.
*/
void CVICALLBACK Analog_Cell_Paste(int panelHandle, int controlID,
                                   int MenuItemID, void* callbackData) {
  Rect selection;
  Point pval = {0, 0};

  GetTableSelection(
      panelHandle, PANEL_ANALOGTABLE,
      &selection);  // returns a 0 to all values if only 1 cell selected

  // Paste made into multiple cells of analog channels
  if (selection.top <= NUMBERANALOGCHANNELS && selection.top > 0) {
    int row = selection.top;
    while ((row <= selection.top + (selection.height - 1)) &&
           (row <= NUMBERANALOGCHANNELS)) {
      for (int col = selection.left;
           col <= selection.left + (selection.width - 1); col++) {
        AnalogTable[col][row][currentpage].fcn = AnalogClip[0].fcn;
        AnalogTable[col][row][currentpage].fval = AnalogClip[0].fval;
        AnalogTable[col][row][currentpage].tscale = AnalogClip[0].tscale;
      }
      row++;
    }
  }

  // Paste Made into single Cell
  else if (selection.top == 0)
    ;
  {
    GetActiveTableCell(panelHandle, PANEL_ANALOGTABLE, &pval);
    if (pval.y <= NUMBERANALOGCHANNELS) {
      AnalogTable[pval.x][pval.y][currentpage].fcn = AnalogClip[0].fcn;
      AnalogTable[pval.x][pval.y][currentpage].fval = AnalogClip[0].fval;
      AnalogTable[pval.x][pval.y][currentpage].tscale = AnalogClip[0].tscale;
    }
  }
  ChangedVals = 1;
  DrawNewTable(0);
}

/**
@brief Callback triggered by selecting Exit from the menubar. Directly calls
PANEL_CALLBACK() with event EVENT_CLOSE.
@author Kerry Wang
*/
void CVICALLBACK EXIT(int menuBar, int menuItem, void* callbackData,
                      int panel) {
  PANEL_CALLBACK(panelHandle, EVENT_CLOSE, NULL, 0, 0);
}

/**
@brief Callback for the panel. Handles closing the program.
@author Kerry Wang
*/
int CVICALLBACK PANEL_CALLBACK(int panel, int event, void* callbackData,
                               int eventData1, int eventData2) {
  int status = 0;
  switch (event) {
    case EVENT_CLOSE:
      status = ConfirmPopup(
          "Exit", "Are you sure you want to quit?\nUnsaved data will be lost.");
      if (status == 1) QuitUserInterface(1);  // kills the GUI and ends program
      break;
  }
  return status;
}

/**
@brief Opens the Scan Loader panel.
*/
void CVICALLBACK Scan_Table_Load(int panel, int controlID, int MenuItemID,
                                 void* callbackData) {
  DisplayPanel(panelHandle8);
}
