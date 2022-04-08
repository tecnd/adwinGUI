#ifndef VAR_DEFS // Make sure this file is included only once
#define VAR_DEFS

/************************************************************************
Macro Definitions
*************************************************************************/

#define NUMBERANALOGCHANNELS 32	 // Number of analog Channels available for control
#define NUMBERDIGITALCHANNELS 36 // number of digital channels DISPLAYED!!!      \
								 // some are not user controlled, e.g. DDS lines \
								 // 32 in total.  5 used for DDS1                \
								 // 5 for DD2 (K40 evap)                         \
								 // reserved for DDS1                            \
								 // reserved for DDS2                            \
								 // reserved for DDS3
#define NUMBERDDS 3		   // Number of DDS's
#define NUMBEROFCOLUMNS 17 //
#define TRUE 1
#define FALSE 0
#define NUMBEROFPAGES 11 //currently hardwired up to 10               \
						 // to be quick & dirty about it, just change \
						 //numberofpages to 1 more than actual

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

int panelHandle;
int menuHandle;
int currentx, currenty, currentpage;
int ischecked[NUMBEROFPAGES], isdimmed;

struct AnalogTableValues
{
	int fcn;																		 //fcn is an integer refering to a function to use.
																					 // 1-step, 2-linear, 3- exp, 4- 'S' curve 5-sine 6-"same as last cell"
	double fval;																	 //the final value
	double tscale;																	 //the timescale to approach final value
} AnalogTable[NUMBEROFCOLUMNS + 1][NUMBERANALOGCHANNELS + NUMBERDDS][NUMBEROFPAGES]; //+1 needed because all code done assumed base 1 arrays...
																					 // the structure is the values/elements contained at each point in the
																					 // analog panel.  The array aval, is set up as [x][y][page]
int DigTableValues[NUMBEROFCOLUMNS + 1][MAXDIGITAL][NUMBEROFPAGES];
int ChMap[MAXANALOG]; // The channel mapping (for analog). i.e. if we program line 1 as channel
					  // 12, the ChMap[12]=1

double TimeArray[NUMBEROFCOLUMNS + 1][NUMBEROFPAGES];

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

struct DDSClock
{
	double extclock;
	int PLLmult;
	double clock;
} DDSFreq;

ddsoptions_struct ddstable[NUMBEROFCOLUMNS + 1][NUMBEROFPAGES];	 //17 columns (actually only 14, but in case we expand), 10 pages
ddsoptions_struct dds2table[NUMBEROFCOLUMNS + 1][NUMBEROFPAGES]; //17 columns (actually only 14, but in case we expand), 10 pages
ddsoptions_struct dds3table[NUMBEROFCOLUMNS + 1][NUMBEROFPAGES];

/* Parameter Scan variables*/
struct AnalogScanParameters
{
	double Start_Of_Scan;
	double End_Of_Scan;
	double Scan_Step_Size;
	int Iterations_Per_Step;
	int Analog_Channel;
	int Analog_Mode;
};

struct TimeScanParameters
{
	double Start_Of_Scan;
	double End_Of_Scan;
	double Scan_Step_Size;
	int Iterations_Per_Step;
};

struct DDSScanParameters
{
	double Base_Freq;
	double Start_Of_Scan;
	double End_Of_Scan;
	double Scan_Step_Size;
	double Current;
	int Iterations_Per_Step;

};

struct DDSFloorScanParameters
{
	double Floor_Start;
	double Floor_End;
	double Floor_Step;
	int Iterations_Per_Step;
};

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


#endif
