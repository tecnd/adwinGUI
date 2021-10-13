#ifndef VAR_DEFS // Make sure this file is included only once
#define VAR_DEFS

/************************************************************************
Macro Definitions
*************************************************************************/

#define DefaultEventPeriod 0.100 // in milliseconds
#define AdwinTick 0.000025		 //Adwin clock cycle, in ms.
#define NUMBERANALOGCHANNELS 40	 // Number of analog Channels available for control
#define NUMBERDIGITALCHANNELS 64 // number of digital channels DISPLAYED!!!      \
								 // some are not user controlled, e.g. DDS lines \
								 // 32 in total.  5 used for DDS1                \
								 // 5 for DD2 (K40 evap)                         \
								 // reserved for DDS1                            \
								 // reserved for DDS2                            \
								 // reserved for DDS3
#define NUMBERDDS 3		   // Number of DDS's
#define NUMBEROFCOLUMNS 17
#define TRUE 1
#define FALSE 0
#define NUMBEROFPAGES 11

#define DDS2_CLOCK 983.04 // clock speed of DDS 2 in MHz
#define DDS3CLOCK 300.0	  // clock speed of DDS 2 in MHz

#define MAXANALOG 50  // Need 40 lines, leave room for 48
#define MAXDIGITAL 70 // need 64 lines, leave some leeway
/************************************************************************
Structure/Typedef Declarations
*************************************************************************/
// expand the array sizes to allow for further growth.  Need 32 analog, leave room for 48
// digital lines   leave room for 64 (2 digital io cards)
typedef int BOOL;

typedef struct ddsoptions_struct
{
	float start_frequency; /* in MHz*/
	float end_frequency;
	float amplitude;
	float delta_time;
	BOOL is_stop;
} ddsoptions_struct;

/************************************************************************
Global Variables
*************************************************************************/

int panelHandle, panelHandle2, panelHandle3, panelHandle4, panelHandle5, panelHandle6, panelHandle7, panelHandle8;
int panelHandle_sub1, panelHandle_sub2;
int menuHandle;
int currentx, currenty, currentpage;
int pic_off, pic_static, pic_change, pic_don;
int ischecked[NUMBEROFPAGES + 1], isdimmed;
BOOL ChangedVals;
BOOL UseSimpleTiming;

BOOL loop_active;

struct LoopPoints
{
	int startpage;
	int endpage;
	int startcol;
	int endcol;
} LoopPoints;

struct AnalogTableValues
{
	int fcn;																		 //fcn is an integer refering to a function to use.																					 // 1-step, 2-linear, 3- exp, 4- 'S' curve 5-sine 6-"same as last cell"
	double fval;																	 //the final value
	double tscale;																	 //the timescale to approach final value
} AnalogTable[NUMBEROFCOLUMNS + 1][NUMBERANALOGCHANNELS + NUMBERDDS][NUMBEROFPAGES + 1]; //+1 needed because all code done assumed base 1 arrays...
																					 // the structure is the values/elements contained at each point in the
																					 // analog panel.  The array aval, is set up as [x][y][page]
int DigTableValues[NUMBEROFCOLUMNS + 1][MAXDIGITAL][NUMBEROFPAGES + 1];
int ChMap[MAXANALOG]; // The channel mapping (for analog). i.e. if we program line 1 as channel
					  // 12, the ChMap[12]=1

double TimeArray[NUMBEROFCOLUMNS + 1][NUMBEROFPAGES + 1];

struct AnalogChannelProperties
{
	int chnum;		 // channel number 1-8 DAC1	9-16 DAC2
	char chname[50]; // name to appear on the panel
	char units[50];
	double tfcn; // Transfer function.  i.e. 10V out = t G/cm etc...
	double tbias;
	int resettozero;
	double maxvolts;
	double minvolts;
} AChName[MAXANALOG + NUMBERDDS];

struct DigitalChannelProperties
{
	int chnum;		 // digital line to control
	char chname[50]; // name of the channel on the panel
	int resettolow;
} DChName[MAXDIGITAL];

double EventPeriod; //The Update Period Defined by the pull down menu (in ms)
short processnum;

struct DDSClock
{
	double extclock;
	int PLLmult;
	double clock;
} DDSFreq;

ddsoptions_struct ddstable[NUMBEROFCOLUMNS + 1][NUMBEROFPAGES + 1];	 //17 columns (actually only 14, but in case we expand), 10 pages
ddsoptions_struct dds2table[NUMBEROFCOLUMNS + 1][NUMBEROFPAGES + 1]; //17 columns (actually only 14, but in case we expand), 10 pages
ddsoptions_struct dds3table[NUMBEROFCOLUMNS + 1][NUMBEROFPAGES + 1];
int Active_DDS_Panel; // 1 for Rb evap dds, 2 for K40 evap dds, 3 for HFS dds

/* Parameter Scan variables*/
typedef struct AnalogScanParameters
{
	double Start_Of_Scan;
	double End_Of_Scan;
	double Scan_Step_Size;
	int Iterations_Per_Step;
	int Analog_Channel;
	int Analog_Mode;
} AnalogScan;

typedef struct TimeScanParameters
{
	double Start_Of_Scan;
	double End_Of_Scan;
	double Scan_Step_Size;
	int Iterations_Per_Step;
} TimeScan;

typedef struct DDSScanParameters
{
	double Base_Freq;
	double Start_Of_Scan;
	double End_Of_Scan;
	double Scan_Step_Size;
	double Current;
	int Iterations_Per_Step;

} DDSScan;

typedef struct DDSFloorScanParameters
{
	double Floor_Start;
	double Floor_End;
	double Floor_Step;
	int Iterations_Per_Step;
} DDSFloorScan;

struct ScanParameters
{
	int Row;
	int Column;
	int Page;
	int ScanMode; // 0 for Analog, 1 for Time, 2 for DDS
	BOOL ScanDone;
	BOOL Scan_Active;
	BOOL Use_Scan_List;
	struct AnalogScanParameters Analog;
	struct TimeScanParameters Time;
	struct DDSScanParameters DDS;
	struct DDSFloorScanParameters DDSFloor;
} PScan;

struct ScanSet
{
	double Start;
	double End;
	double Step;
	int Iterations;
	int Current_Step;
	double Current_Value;
	int Current_Iteration;
} ScanVal;

struct ScanBuff
{
	int Iteration;
	int Step;
	double Value;
	char Time[100];
	int BufferSize;
} ScanBuffer[1000];
#endif
