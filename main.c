/**
@file main.c
@brief Contains code to build the GUI layout.
@todo Move update history and notes from comments to documentation
*/
/**
@mainpage adwinGUI Documentation

adwinGUI is built in LabWindows, with supplementary parts written in ADbasic and
Python.
*/
// IDEAS:
/*
Add a killswitch...to interupt the ADwin hardware.  Dangerous because it will
leave the outputs in whatever state they had at the time the system was 'killed.
Could be fixed by adding a bit to the ADBasic code that recognizes the kill
switch and sets channels low.

*/
/*
Caveats:
If making any changes to the program, be aware of several inconsistencies.
Adwin software is base 1 for arrays
C (LabWindows) is base 0 for arrays,
however some controls for LabWindows are base 1   : Tables
and some are base 0 : Listboxes

2010:
Aug 2:  change to the "ADbasic binary file: "TransferData_August02_2010.TB1"
                purpose: activate DIO 1 and DIO 2 outputs for Aubin Lab (W&M)
sequencer.

2006:
Mar 9:  Added the ability to loop between 2 columns of the panel, spanning pages
if necessary.  Could be used for repetitive outputs.. e.g. cycling between MOT
and an optical probe. (need to be careful with DDS programming) Mar 9: Force DDS
clock frequency to be set at run-time. Fixed bug that caused the DDS trap bottom
to set to 0 after doing a Scan.

Feb 9   Noticed a bug.. probably been there for a VERY long time.  The 'Debug
Window' has been slowly accumulating text, and not being cleared.  Change code
to clear the debug text window before saving the panel


2005
July 29 Added option to output a history of the Scan values when running a scan.
                Adding option to stream the panel to text files;
JULY 20 commands for DDS2 are now generated, also sent to ADwin now (see new
ADBasic code  TransferData_Jul20.BAS) Fixed ADBasic software bug.  Turns out
that DIG_WRITELATCH1 and DIG_WRITELATCH2 (for lower and upper 16 bits
respectively) are incompatible.  Use DIG_WRITELATCH32 instead. July 18 Added 3rd
DDS interface, simplified DDS control by reusing the existing DDS control panel
                DDS 2 and 3 clock settings displayed in DDSSettings.uir  Not
modifiable. Set using a #define in vars.h Save DDS 2,3 info to file. July4
Adding 2nd DDS (interface only, creating dual dds command structure still needs
to be done.) June7   Finalized scan programming.  Now scans in amplitude, time
or DDS frequency. (Only DDS1 so far) April 20 Changed the way that the table
cells are coloured.  Now all cells are coloured based on the information in the
cell.  No longer based on the history of that row. Sine wave output now relabels
amplitude and frequency on analog control panel. Colours Cyan on table. April 7
Fixed a bug where we didn't reach the final value on a ramp, but reached the
value before. Cause: in calculating ramps, i.e determine slope=amplitude/number
of steps but should be amplitude/number_steps-1 Mar 23  Added A Frequency OFfset
to the DDS...so the same ramp can be continually used while changing the trap
bottom. Fixed duration of ramps etc at column duration. Mar 10  Fixed a problem
where the frequency ramps generated by the DDS finished in half the expected
time Can't find a reason for this, except that the DDS manual might be wrong.
                Added a Sinewave option to the list of possible functions.  Only
accepts amplitude and frequency..no bias
                  -  bias could be 'worked around' using bias setting under
Analog Channel Setup Jan 18 Add menu option to turn off the DDS for all
cells. Avoids a string of warnings created by the DDS command routines if a DDS
command is written before the previous DDS command was done; Jan 5   Fix bug in
code where the timing isn't always copied into the DDS commands Fixed the last
panel mobile ability


//Update History
2004
Apr 15:  Run button flashes with ADwin is operating.
Apr 29:  Fixed Digital line 16.  Loop counted to 15, should have been 16
May 04:  Adding code to delete/insert columns.
May 06:  Added Copy/Paste fcns for column.  Doesn't work 100% yet.... test that
channels 16 work for dig. and analog. May 10:  Fixed a bug where the arrays
weren't properly initialized, causing strange values to be written to the adwin
                 Added flashing led's to notify when the ADwin is doing
something May 13:  Fixed 16th line of the panel, now is actually output.  Bug
was result of inconsistency with arrays. i.e. Base 0 vs base 1.
                 - fixed by increasing the internal array size to 17 and
starting from 1. (dont read element 0) May 18:  Improved performance.  Added a
variable (ChangedVals) that is 0 when no changes have been made since the last
run.  If ChangedVals==0, then don't recreate/resend the arrays, shortens delay
between runs. June24: Add support for more analog output cards.  Change the
#define NUMBEROFANALOGCHANNELS xx to reflect the number of channels. NOTE:  You
need to change this in every .c file. Still need to update the code to use a
different channel for digital.  (currently using 17, which will be overwritten
                if using more than 16 analog channels.
July26: Begin adding code to control the DDS (AD9854 from Analog Devices)
                Use an extra line on the analog table (17 or 25) as the DDS
control interface AUg   Include DDS control Nov   DDS control
re-written by Ian Leroux Dec7 Add a compression routine for the NumberUpdates
variable, to speed up communication with ADwin. Added Menu option to turn
compression on/off Dec16  Made the last panel mobile, such that it can be
inserted into other pages.
*/

#include "main.h"

#include <ansi_c.h>
#include <cvirte.h>
#include <userint.h>

#include "AnalogSettings.h"
#include "AnalogSettings2.h"
#include "DigitalSettings2.h"
#include "GUIDesign.h"
#include "GUIDesign2.h"
#include "vars.h"

// Forward declaration
void Initialization(void);

/**
@brief Entry point. Handles initializing data arrays and GUI elements.
*/
int main(int argc, char* argv[]) {
  if (InitCVIRTE(0, argv, 0) == 0) return -1; /* out of memory */
  if ((panelHandle = LoadPanel(0, "GUIDesign.uir", PANEL)) < 0) return -1;
  if ((panelHandle_sub2 = LoadPanel(0, "GUIDesign.uir", SUBPANEL2)) < 0)
    return -1;
  if ((commentsHandle = LoadPanel(0, "Comments.uir", PANEL)) < 0) return -1;
  if ((panelHandle2 = LoadPanel(0, "AnalogSettings.uir", PANEL)) < 0) return -1;
  if ((panelHandle3 = LoadPanel(0, "DigitalSettings.uir", PANEL)) < 0)
    return -1;
  if ((panelHandle4 = LoadPanel(0, "AnalogControl.uir", PANEL)) < 0) return -1;
  if ((panelHandle7 = LoadPanel(0, "Scan.uir", PANEL)) < 0) return -1;
  if ((panelHandle8 = LoadPanel(0, "ScanTableLoader.uir", PANEL)) < 0)
    return -1;

  // Initialize arrays (to avoid undefined elements causing -99 to be written)
  // ramp over # of analog chanels
  for (int j = 0; j <= NUMBERANALOGCHANNELS; j++) {
    AChName[j].tfcn = 1;
    AChName[j].tbias = 0;
    AChName[j].resettozero = 1;
    // ramp over # of cells per page
    for (int i = 0; i <= NUMBEROFCOLUMNS; i++) {
      // ramp over pages
      for (int page = 0; page <= NUMBEROFPAGES; page++) {
        AnalogTable[i][j][page].fcn = 1;
        AnalogTable[i][j][page].fval = 0.0;
        AnalogTable[i][j][page].tscale = 1;
      }
    }
  }

  for (int j = 0; j <= NUMBERDIGITALCHANNELS; j++) {
    // ramp over # of cells per page
    for (int i = 0; i <= NUMBEROFCOLUMNS; i++) {
      // ramp over pages
      for (int page = 0; page <= NUMBEROFPAGES; page++) {
        DigTableValues[i][j][page] = 0;
      }
    }
  }

  // done initializing
  menuHandle = GetPanelMenuBar(panelHandle);

  currentpage = 1;

  Initialization();

  DisplayPanel(panelHandle);
  RunUserInterface();         // start the GUI
  DiscardPanel(panelHandle);  // returns here after the GUI shutsdown

  return 0;
}

/**
@brief Resizes and moves analog table, analog channel names table, and unit name table.
@param top Pixels from the top edge
@param left Pixels from the left edge
@param height Total pixel height of analog table
@param width Total pixel width of analog table
@author Stefan, Kerry Wang
*/
void ReshapeAnalogTable(int top, int left, int height, int width) {
  int cellHeight = height / (NUMBERANALOGCHANNELS);
  int cellWidth = width / NUMBEROFCOLUMNS;
  int unitTableHeight = cellHeight * NUMBERANALOGCHANNELS;

  // resize the analog table and all its related list boxes
  SetCtrlAttribute(panelHandle, PANEL_ANALOGTABLE, ATTR_TOP, top);
  SetCtrlAttribute(panelHandle, PANEL_ANALOGTABLE, ATTR_LEFT, left);
  SetCtrlAttribute(panelHandle, PANEL_ANALOGTABLE, ATTR_HEIGHT, height);

  SetCtrlAttribute(panelHandle, PANEL_TBL_ANAMES, ATTR_TOP, top);
  SetCtrlAttribute(panelHandle, PANEL_TBL_ANAMES, ATTR_LEFT, left - 165);
  SetCtrlAttribute(panelHandle, PANEL_TBL_ANAMES, ATTR_HEIGHT, height);

  SetCtrlAttribute(panelHandle, PANEL_TBL_ANALOGUNITS, ATTR_TOP, top);
  SetCtrlAttribute(panelHandle, PANEL_TBL_ANALOGUNITS, ATTR_LEFT,
                   left + CELL_WIDTH * NUMBEROFCOLUMNS + TABLE_MARGIN_RIGHT);
  SetCtrlAttribute(panelHandle, PANEL_TBL_ANALOGUNITS, ATTR_HEIGHT,
                   unitTableHeight);

  SetTableRowAttribute(panelHandle, PANEL_ANALOGTABLE, -1, ATTR_SIZE_MODE,
                       VAL_USE_EXPLICIT_SIZE);
  SetTableRowAttribute(panelHandle, PANEL_ANALOGTABLE, -1, ATTR_ROW_HEIGHT,
                       cellHeight);
  SetTableColumnAttribute(panelHandle, PANEL_ANALOGTABLE, -1, ATTR_SIZE_MODE,
                          VAL_USE_EXPLICIT_SIZE);
  SetTableColumnAttribute(panelHandle, PANEL_ANALOGTABLE, -1, ATTR_COLUMN_WIDTH,
                          cellWidth);
  SetTableRowAttribute(panelHandle, PANEL_TBL_ANAMES, -1, ATTR_SIZE_MODE,
                       VAL_USE_EXPLICIT_SIZE);
  SetTableRowAttribute(panelHandle, PANEL_TBL_ANAMES, -1, ATTR_ROW_HEIGHT,
                       cellHeight);
  SetTableRowAttribute(panelHandle, PANEL_TBL_ANALOGUNITS, -1, ATTR_SIZE_MODE,
                       VAL_USE_EXPLICIT_SIZE);
  SetTableRowAttribute(panelHandle, PANEL_TBL_ANALOGUNITS, -1, ATTR_ROW_HEIGHT,
                       cellHeight);

  SetCtrlAttribute(panelHandle, PANEL_ANALOGTABLE, ATTR_NUM_VISIBLE_COLUMNS,
                   NUMBEROFCOLUMNS);
  SetCtrlAttribute(panelHandle, PANEL_ANALOGTABLE, ATTR_NUM_VISIBLE_ROWS,
                   NUMBERANALOGCHANNELS);
}

/**
@brief Resizes and moves digital table, digital channel names table, and scan table.
@param top Pixels from the top edge
@param left Pixels from the left edge
@param height Total pixel height of digital table
@param width Total pixel width of digital table
@author Stefan, Kerry Wang
*/
void ReshapeDigitalTable(int top, int left, int height, int width) {
  int cellHeight = height / NUMBERDIGITALCHANNELS;
  int cellWidth = width / NUMBEROFCOLUMNS;

  SetCtrlAttribute(panelHandle, PANEL_DIGTABLE, ATTR_HEIGHT, height);
  SetCtrlAttribute(panelHandle, PANEL_DIGTABLE, ATTR_LEFT, left);
  SetCtrlAttribute(panelHandle, PANEL_DIGTABLE, ATTR_TOP, top);
  SetCtrlAttribute(panelHandle, PANEL_TBL_DIGNAMES, ATTR_TOP, top);
  SetCtrlAttribute(panelHandle, PANEL_TBL_DIGNAMES, ATTR_LEFT, left - 165);
  SetCtrlAttribute(panelHandle, PANEL_TBL_DIGNAMES, ATTR_HEIGHT, height);

  SetTableRowAttribute(panelHandle, PANEL_DIGTABLE, -1, ATTR_SIZE_MODE,
                       VAL_USE_EXPLICIT_SIZE);
  SetTableRowAttribute(panelHandle, PANEL_DIGTABLE, -1, ATTR_ROW_HEIGHT,
                       cellHeight);
  SetTableColumnAttribute(panelHandle, PANEL_DIGTABLE, -1, ATTR_SIZE_MODE,
                          VAL_USE_EXPLICIT_SIZE);
  SetTableColumnAttribute(panelHandle, PANEL_DIGTABLE, -1, ATTR_COLUMN_WIDTH,
                          cellWidth);

  SetTableRowAttribute(panelHandle, PANEL_TBL_DIGNAMES, -1, ATTR_SIZE_MODE,
                       VAL_USE_EXPLICIT_SIZE);
  SetTableRowAttribute(panelHandle, PANEL_TBL_DIGNAMES, -1, ATTR_ROW_HEIGHT,
                       cellHeight);

  // Move the scan table
  SetCtrlAttribute(panelHandle, PANEL_SCAN_TABLE, ATTR_TOP, top);
  SetCtrlAttribute(panelHandle, PANEL_SCAN_TABLE, ATTR_LEFT,
                   left + CELL_WIDTH * NUMBEROFCOLUMNS + TABLE_MARGIN_RIGHT);

  SetCtrlAttribute(panelHandle, PANEL_DIGTABLE, ATTR_NUM_VISIBLE_COLUMNS,
                   NUMBEROFCOLUMNS);
  SetCtrlAttribute(panelHandle, PANEL_DIGTABLE, ATTR_NUM_VISIBLE_ROWS,
                   NUMBERDIGITALCHANNELS);
}

/**
@brief Resizes and moves tables into place.
@author David McKay, Kerry Wang
*/
void BuildTables(void) {
  // Build Time Table
  SetCtrlAttribute(panelHandle, PANEL_TIMETABLE, ATTR_TABLE_MODE, VAL_GRID);
  SetCtrlAttribute(panelHandle, PANEL_TIMETABLE, ATTR_ENABLE_COLUMN_SIZING, 0);
  SetCtrlAttribute(panelHandle, PANEL_TIMETABLE, ATTR_ENABLE_ROW_SIZING, 0);
  SetTableRowAttribute(panelHandle, PANEL_TIMETABLE, -1, ATTR_PRECISION, 3);

  InsertTableColumns(panelHandle, PANEL_TIMETABLE, -1, NUMBEROFCOLUMNS - 1,
                     VAL_CELL_NUMERIC);
  SetTableColumnAttribute(panelHandle, PANEL_TIMETABLE, -1, ATTR_COLUMN_WIDTH,
                          CELL_WIDTH);
  SetCtrlAttribute(panelHandle, PANEL_TIMETABLE, ATTR_NUM_VISIBLE_COLUMNS,
                   NUMBEROFCOLUMNS);

  // Build Analog Table
  SetTableColumnAttribute(panelHandle, PANEL_TBL_ANAMES, 1, ATTR_CELL_TYPE,
                          VAL_CELL_STRING);
  SetTableColumnAttribute(panelHandle, PANEL_TBL_ANAMES, 2, ATTR_CELL_TYPE,
                          VAL_CELL_NUMERIC);
  SetTableColumnAttribute(panelHandle, PANEL_TBL_ANAMES, 2, ATTR_DATA_TYPE,
                          VAL_UNSIGNED_INTEGER);
  SetCtrlAttribute(panelHandle, PANEL_TBL_ANAMES, ATTR_TABLE_MODE, VAL_COLUMN);

  SetCtrlAttribute(panelHandle, PANEL_ANALOGTABLE, ATTR_TABLE_MODE, VAL_GRID);
  SetCtrlAttribute(panelHandle, PANEL_ANALOGTABLE, ATTR_ENABLE_COLUMN_SIZING,
                   0);
  SetCtrlAttribute(panelHandle, PANEL_ANALOGTABLE, ATTR_ENABLE_ROW_SIZING, 0);
  SetTableRowAttribute(panelHandle, PANEL_ANALOGTABLE, -1, ATTR_PRECISION, 3);

  InsertTableColumns(panelHandle, PANEL_ANALOGTABLE, -1, NUMBEROFCOLUMNS - 1,
                     VAL_CELL_NUMERIC);
  InsertTableRows(panelHandle, PANEL_ANALOGTABLE, -1, NUMBERANALOGCHANNELS - 1,
                  VAL_CELL_NUMERIC);
  InsertTableRows(panelHandle, PANEL_TBL_ANAMES, -1, NUMBERANALOGCHANNELS - 1,
                  VAL_USE_MASTER_CELL_TYPE);
  InsertTableRows(panelHandle, PANEL_TBL_ANALOGUNITS, -1,
                  NUMBERANALOGCHANNELS - 1, VAL_USE_MASTER_CELL_TYPE);
  SetAnalogChannels();

  for (int i = 1; i <= NUMBERANALOGCHANNELS; i++) {
    SetTableCellAttribute(panelHandle, PANEL_TBL_ANAMES, MakePoint(2, i),
                          ATTR_CTRL_VAL, i);
  }

  // Build Digital Table
  SetTableColumnAttribute(panelHandle, PANEL_TBL_DIGNAMES, 1, ATTR_CELL_TYPE,
                          VAL_CELL_STRING);
  SetTableColumnAttribute(panelHandle, PANEL_TBL_DIGNAMES, 2, ATTR_CELL_TYPE,
                          VAL_CELL_NUMERIC);
  SetTableColumnAttribute(panelHandle, PANEL_TBL_DIGNAMES, 2, ATTR_DATA_TYPE,
                          VAL_UNSIGNED_INTEGER);
  SetCtrlAttribute(panelHandle, PANEL_TBL_DIGNAMES, ATTR_TABLE_MODE,
                   VAL_COLUMN);

  SetCtrlAttribute(panelHandle, PANEL_DIGTABLE, ATTR_TABLE_MODE, VAL_GRID);
  SetCtrlAttribute(panelHandle, PANEL_DIGTABLE, ATTR_ENABLE_COLUMN_SIZING, 0);
  SetCtrlAttribute(panelHandle, PANEL_DIGTABLE, ATTR_ENABLE_ROW_SIZING, 0);
  SetCtrlAttribute(panelHandle, PANEL_DIGTABLE,
                   ATTR_HILITE_ONLY_WHEN_PANEL_ACTIVE, 1);
  InsertTableColumns(panelHandle, PANEL_DIGTABLE, -1, NUMBEROFCOLUMNS - 1,
                     VAL_USE_MASTER_CELL_TYPE);
  InsertTableRows(panelHandle, PANEL_DIGTABLE, -1, NUMBERDIGITALCHANNELS - 1,
                  VAL_USE_MASTER_CELL_TYPE);
  InsertTableRows(panelHandle, PANEL_TBL_DIGNAMES, -1,
                  NUMBERDIGITALCHANNELS - 1, VAL_USE_MASTER_CELL_TYPE);

  for (int i = 1; i <= NUMBERDIGITALCHANNELS; i++) {
    SetTableCellAttribute(panelHandle, PANEL_TBL_DIGNAMES, MakePoint(2, i),
                          ATTR_CTRL_VAL, i);
  }

  // Move and resize tables
  int analogHeight = CELL_HEIGHT * (NUMBERANALOGCHANNELS) + 6; // +6 to stop table from scrolling
  int analogTop = 155;
  int leftpos = 165;
  int digitalTop = analogTop + analogHeight + 50;

  ReshapeAnalogTable(analogTop, leftpos,
                     CELL_HEIGHT * (NUMBERANALOGCHANNELS) + 6,
                     CELL_WIDTH * NUMBEROFCOLUMNS);
  ReshapeDigitalTable(digitalTop, leftpos,
                      CELL_HEIGHT * NUMBERDIGITALCHANNELS + 6,
                      CELL_WIDTH * NUMBEROFCOLUMNS);

  // Right-click menus
  NewCtrlMenuItem(panelHandle, PANEL_ANALOGTABLE, "Copy", -1, Analog_Cell_Copy,
                  0);
  NewCtrlMenuItem(panelHandle, PANEL_ANALOGTABLE, "Paste", -1,
                  Analog_Cell_Paste, 0);
  HideBuiltInCtrlMenuItem(panelHandle, PANEL_ANALOGTABLE,
                          -4); // Hides Sort Command

  NewCtrlMenuItem(panelHandle, PANEL_DIGTABLE, "Copy", -1, Dig_Cell_Copy,
                  0); // Adds Popup Menu Item "Copy"
  NewCtrlMenuItem(panelHandle, PANEL_DIGTABLE, "Paste", -1, Dig_Cell_Paste, 0);
  HideBuiltInCtrlMenuItem(panelHandle, PANEL_DIGTABLE,
                          -4); // Hides Sort Command

  NewCtrlMenuItem(panelHandle, PANEL_SCAN_TABLE, "Load Values", -1,
                  Scan_Table_Load, 0);
  HideBuiltInCtrlMenuItem(panelHandle, PANEL_SCAN_TABLE, -4);
  return;
}

/**
@brief Initializes the GUI.
*/
void Initialization(void) {
  PScan.Scan_Active = FALSE;
  PScan.Use_Scan_List = FALSE;

  BuildTables();

  // Change Analog Settings window
  SetCtrlAttribute(panelHandle2, ANLGPANEL_NUM_ACH_LINE, ATTR_MAX_VALUE,
                   NUMBERANALOGCHANNELS);
  SetCtrlAttribute(panelHandle2, ANLGPANEL_NUM_ACHANNEL, ATTR_MAX_VALUE,
                   NUMBERANALOGCHANNELS);

  int button_spacing = 60;
  for (int i = 1; i <= NUMBEROFPAGES; i++) {
    // Generate labels
    int newTable = NewCtrl(panelHandle, CTRL_TABLE, "Desc.", 88, 165);
    SetCtrlAttribute(panelHandle, newTable, ATTR_HEIGHT, 25);
    SetCtrlAttribute(panelHandle, newTable, ATTR_WIDTH, 686);
    SetCtrlAttribute(panelHandle, newTable, ATTR_LABEL_TOP, 93);
    SetCtrlAttribute(panelHandle, newTable, ATTR_LABEL_LEFT, 130);
    SetCtrlAttribute(panelHandle, newTable, ATTR_TABLE_MODE, VAL_GRID);
    SetCtrlAttribute(panelHandle, newTable, ATTR_CELL_TYPE, VAL_CELL_STRING);
    SetCtrlAttribute(panelHandle, newTable, ATTR_COLUMN_LABELS_VISIBLE, 0);
    SetCtrlAttribute(panelHandle, newTable, ATTR_ROW_LABELS_VISIBLE, 0);
    SetCtrlAttribute(panelHandle, newTable, ATTR_SCROLL_BARS,
                     VAL_NO_SCROLL_BARS);
    SetCtrlAttribute(panelHandle, newTable, ATTR_HORIZONTAL_GRID_COLOR,
                     VAL_BLACK);
    SetCtrlAttribute(panelHandle, newTable, ATTR_VERTICAL_GRID_COLOR,
                     VAL_BLACK);
    InsertTableRows(panelHandle, newTable, -1, 1, VAL_USE_MASTER_CELL_TYPE);
    InsertTableColumns(panelHandle, newTable, -1, NUMBEROFCOLUMNS,
                       VAL_USE_MASTER_CELL_TYPE);
    SetTableColumnAttribute(panelHandle, newTable, -1, ATTR_COLUMN_WIDTH, 40);
    SetCtrlAttribute(panelHandle, newTable, ATTR_NUM_VISIBLE_COLUMNS,
                     NUMBEROFCOLUMNS);
    LabelArray[i] = newTable;

    // Generate buttons
    int newTB = NewCtrl(panelHandle, CTRL_SQUARE_TEXT_BUTTON, "", 30,
                        165 + (i - 1) * button_spacing);
    SetCtrlAttribute(panelHandle, newTB, ATTR_HEIGHT, 25);
    SetCtrlAttribute(panelHandle, newTB, ATTR_WIDTH, 45);
    char buttonName[8];
    sprintf(buttonName, "Page %d", i);
    SetCtrlAttribute(panelHandle, newTB, ATTR_ON_TEXT, buttonName);
    SetCtrlAttribute(panelHandle, newTB, ATTR_OFF_TEXT, buttonName);
    SetCtrlAttribute(panelHandle, newTB, ATTR_ON_COLOR, 0x66CC99);
    InstallCtrlCallback(panelHandle, newTB, TOGGLE_CALLBACK, NULL);
    ButtonArray[i] = newTB;

    // Generate checkboxes
    int newCB = NewCtrl(panelHandle, CTRL_CHECK_BOX, "On/Off", 60,
                        165 + (i - 1) * button_spacing);
    InstallCtrlCallback(panelHandle, newCB, CHECKBOX_CALLBACK, NULL);
    CheckboxArray[i] = newCB;
  }

  // Set default button names
  SetCtrlAttribute(panelHandle, ButtonArray[1], ATTR_ON_TEXT, "MOT");
  SetCtrlAttribute(panelHandle, ButtonArray[1], ATTR_OFF_TEXT, "MOT");
  SetCtrlAttribute(panelHandle, ButtonArray[2], ATTR_ON_TEXT, "B-Trap");
  SetCtrlAttribute(panelHandle, ButtonArray[2], ATTR_OFF_TEXT, "B-Trap");
  SetCtrlAttribute(panelHandle, ButtonArray[3], ATTR_ON_TEXT, "Xfer");
  SetCtrlAttribute(panelHandle, ButtonArray[3], ATTR_OFF_TEXT, "Xfer");
  SetCtrlAttribute(panelHandle, ButtonArray[4], ATTR_ON_TEXT, "Evap");
  SetCtrlAttribute(panelHandle, ButtonArray[4], ATTR_OFF_TEXT, "Evap");
  SetCtrlAttribute(panelHandle, ButtonArray[NUMBEROFPAGES], ATTR_ON_TEXT,
                   "Imaging");
  SetCtrlAttribute(panelHandle, ButtonArray[NUMBEROFPAGES], ATTR_OFF_TEXT,
                   "Imaging");

  SetCtrlVal(panelHandle, ButtonArray[1], 1);
  SetCtrlVal(panelHandle, CheckboxArray[1], 1);
  setVisibleLabel(1);

  SetCtrlAttribute(panelHandle_sub2, SUBPANEL2, ATTR_VISIBLE, 0);

  PScan.Analog.Scan_Step_Size = 1.0;
  PScan.Analog.Iterations_Per_Step = 1;
  PScan.Scan_Active = FALSE;
  DrawNewTable(0);
  return;
}

/**
@brief Sets which label is visible.
@param labelNum 1-based index of label to display
@author Kerry Wang
*/
void setVisibleLabel(int labelNum) {
  for (int i = 1; i <= NUMBEROFPAGES; i++) {
    SetCtrlAttribute(panelHandle, LabelArray[i], ATTR_VISIBLE, i == labelNum);
  }
}
