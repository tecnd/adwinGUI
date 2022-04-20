/**
@file vars.h
@brief Contains global macros, variables, typedefs, and struct definitions.

Some arrays have +1 added to their size because LabWindows is 1-indexed.
In those cases, the 0-th index is undefined and should not be used.

@todo Figure out what the scan structs do and write documentation
*/

#ifndef VAR_DEFS  // Make sure this file is included only once
#define VAR_DEFS

/************************************************************************
Macro Definitions
*************************************************************************/
/**
@brief Period of an event-in trigger signal, in milliseconds
*/
#define EVENTPERIOD 0.010
/**
@brief Number of analog channels available
*/
#define NUMBERANALOGCHANNELS 40
/**
@brief Number of digital channels available
*/
#define NUMBERDIGITALCHANNELS 64
/**
@brief Number of table columns to generate per page
*/
#define NUMBEROFCOLUMNS 20
#define TRUE 1
#define FALSE 0
/**
@brief Number of table pages to generate
*/
#define NUMBEROFPAGES 14

// Table layout defines
/**
@brief Height of a table cell, in pixels
*/
#define CELL_HEIGHT 17
/**
@brief Width of a table cell, in pixels
*/
#define CELL_WIDTH 40
/**
@brief Margin between the main tables and the units and scan lists, in pixels
*/
#define TABLE_MARGIN_RIGHT 20
/**
@brief (Legacy) Number of analog channels
@deprecated This originally reserved additional space when writing save files
so new channels could be added without breaking save compatibility, however now
it is no longer needed. Refactoring this out would probably require breaking
existing save compatibility, though.
*/
#define MAXANALOG 50
/**
@brief (Legacy) Number of digital channels
@deprecated This originally reserved additional space when writing save files
so new channels could be added without breaking save compatibility, however now
it is no longer needed. Refactoring this out would probably require breaking
existing save compatibility, though.
*/
#define MAXDIGITAL 70
/************************************************************************
Structure/Typedef Declarations
*************************************************************************/
typedef int BOOL;

/************************************************************************
Global Variables
*************************************************************************/
/**
@brief Panel handle for the main panel
@todo Rename to be more descriptive and prevent variable shadowing
*/
int panelHandle;
/**
@brief Panel handle for the analog settings window
@todo Rename to be more descriptive
*/
int panelHandle2;
/**
@brief Panel handle for the digital settings window
@todo Rename to be more descriptive
*/
int panelHandle3;
/**
@brief Panel handle for the analog cell editing window
@todo Rename to be more descriptive
*/
int panelHandle4;
/**
@brief Panel handle for the scan window
@todo Rename to be more descriptive
*/
int panelHandle7;
/**
@brief Panel handle for the scan table loader window
@todo Rename to be more descriptive
*/
int panelHandle8;
/**
@brief Panel handle for the comments box.
*/
int commentsHandle;
/**
@brief Panel handle for the scan status window.
@todo Rename to be more descriptive
*/
int panelHandle_sub2;
/**
@brief Handle for the menu bar.
*/
int menuHandle;
/**
@brief x-position of the last-clicked analog table cell
*/
int currentx;
/**
@brief y-position of the last-clicked analog table cell
*/
int currenty;
/**
@brief Page number of the currently active page
*/
int currentpage;
/**
@brief Whether or not values have changed since last run. Used to determine if
data needs to be resent to the ADwin.
*/
BOOL ChangedVals;

/**
@brief Struct that stores instructions for an analog cell.
*/
struct AnalogTableValues {
  /**
  @brief Which function to use

  An integer refering to a function to use with the following mapping:
  - 1: Step
  - 2: Linear ramp
  - 3: Exponential ramp
  - 4: 'S' curve
  - 5: Sine wave
  - 6: Extend previous
  */
  int fcn;
  /**
  @brief The final value of the cell
  */
  double fval;
  /**
  @brief The timescale to reach the final value
  */
  double tscale;
}
/**
@brief 3D array containing all analog cell values.
*/
AnalogTable[NUMBEROFCOLUMNS + 1][NUMBERANALOGCHANNELS + 1][NUMBEROFPAGES + 1];

/**
@brief 3D array containing all digital cell values.
*/
int DigTableValues[NUMBEROFCOLUMNS + 1][MAXDIGITAL][NUMBEROFPAGES + 1];

/**
@brief 2D array containing all time values.
*/
double TimeArray[NUMBEROFCOLUMNS + 1][NUMBEROFPAGES + 1];

/**
@brief Struct that stores configuration and metadata for an analog channel.
*/
struct AnalogChannelProperties {
  /**
  @brief Hardware channel number.
  */
  int chnum;
  /**
  @brief The name of the channel
  */
  char chname[50];
  /**
  @brief The name of the channel's units
  */
  char units[50];
  /**
  @brief Proportionality constant between the onscreen value and the output from the card.
  */
  double tfcn;
  /**
  @brief Voltage offset from 0.
  */
  double tbias;
  /**
  @brief Whether or not to reset the channel to 0 at the end
  */
  int resettozero;
  /**
  @brief Maximum allowed voltage generated by card.
  */
  double maxvolts;
  /**
  @brief Minimum allowed voltage generated by card.
  */
  double minvolts;
}
/**
@brief Array holding the channel properties for each analog channel
*/
AChName[MAXANALOG];

/**
@brief Struct that stores configuration and metadata for a digital channel.
*/
struct DigitalChannelProperties {
  /**
  @brief Hardware channel number.
  */
  int chnum;
  /**
  @brief The name of the channel.
  */
  char chname[50];
  /**
  @brief (Not implemented) Whether or not to reset the channel to off at the end.
  @todo Implement functionality
  */
  int resettolow;
}
/**
@brief Array holding the channel properties for each digital channel.
*/
DChName[MAXDIGITAL];

/* Parameter Scan variables */
struct AnalogScanParameters {
  double Start_Of_Scan;
  double End_Of_Scan;
  double Scan_Step_Size;
  int Iterations_Per_Step;
  int Analog_Channel;
  int Analog_Mode;
};

struct TimeScanParameters {
  double Start_Of_Scan;
  double End_Of_Scan;
  double Scan_Step_Size;
  int Iterations_Per_Step;
};

struct ScanParameters {
  int Row;
  int Column;
  int Page;
  int ScanMode;  // 0 for Analog, 1 for Time
  BOOL ScanDone;
  BOOL Scan_Active;
  BOOL Use_Scan_List;
  struct AnalogScanParameters Analog;
  struct TimeScanParameters Time;
} PScan;

struct ScanSet {
  double Start;
  double End;
  double Step;
  int Iterations;
  int Current_Step;
  double Current_Value;
  int Current_Iteration;
} ScanVal;

struct ScanBuff {
  int Iteration;
  int Step;
  double Value;
  char Time[100];
  int BufferSize;
} ScanBuffer[1000];
#endif
