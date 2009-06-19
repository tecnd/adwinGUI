

#ifndef VAR_DEFS          // Make sure this file is included only once
#define VAR_DEFS

/************************************************************************
Macro Definitions
*************************************************************************/

#define DefaultEventPeriod 0.100   // in milliseconds
#define AdwinTick	0.000025       //Adwin clock cycle, in ms.
#define NUMBERANALOGCHANNELS 32	   // Number of analog Channels available for control
#define NUMBERDIGITALCHANNELS 36  	// number of digital channels DISPLAYED!!!
									// some are not user controlled, e.g. DDS lines
									// 32 in total.  5 used for DDS1
									// 5 for DD2 (K40 evap)
									// reserved for DDS1
									// reserved for DDS2
									// reserved for DDS3				  NU
									
#define NUMBERDDS 3					// Number of DDS's
#define NUMBERLASERS 4				// Number of Lasers
#define NUMBEROFCOLUMNS 17			//
#define TRUE 1
#define FALSE 0
#define NUMBEROFPAGES 11				//currently hardwired up to 10
									   // to be quick & dirty about it, just change 
									   //numberofpages to 1 more than actual
									   
#define DDS2_CLOCK 983.04				  // clock speed of DDS 2 in MHz
#define DDS3CLOCK 300.0				  // clock speed of DDS 2 in MHz  

#define MAXANALOG 50				 // Need 40 lines, leave room for 48
#define MAXDIGITAL 70				 // need 64 lines, leave some leeway													 

#define SEQUENCER_VERSION "ADwin Sequencer V13.0 - "
#define MAXERRS 10
/************************************************************************
Structure/Typedef Declarations
*************************************************************************/
// expand the array sizes to allow for further growth.  Need 32 analog, leave room for 48
// digital lines   leave room for 64 (2 digital io cards)
typedef int  BOOL;

typedef struct ddsoptions_struct {
	float start_frequency; /* in MHz*/
	float end_frequency;
	float amplitude;
	float delta_time;
	BOOL is_stop;
} ddsoptions_struct;

typedef struct dds2options_struct {
	float start_frequency; /* in MHz*/
	float end_frequency;
	float amplitude;
	float delta_time;
	BOOL is_stop;
} dds2options_struct;

typedef struct dds3options_struct {
	float start_frequency; /* in MHz*/
	float end_frequency;
	float amplitude;
	float delta_time;
	BOOL is_stop;
} dds3options_struct;
/************************************************************************
Global Variables
*************************************************************************/

//panelHandles: 8: ScanTableLoader  9:NumSet  10:LaserSettings  11:LaserControl
int panelHandle,panelHandle2,panelHandle3,panelHandle4,panelHandle5,panelHandle6,panelHandle7,panelHandle8,panelHandle9,panelHandle10,panelHandle11;         
int panelHandle_sub1,panelHandle_sub2;
int  menuHandle;
int currentx,currenty,currentpage;
int pic_off,pic_static,pic_change,pic_don;
int ischecked[NUMBEROFPAGES],isdimmed;
BOOL ChangedVals;
BOOL UseSimpleTiming;

static char* SeqErrorBuffer[MAXERRS];	   // Holds error messages
static int SeqErrorCount;

struct Switches{
	BOOL loop_active;
} Switches;

struct LoopPoints{
	int startpage;
	int endpage;
	int startcol;
	int endcol;
} LoopPoints;

struct AnalogTableValues{
	int		fcn;		//fcn is an integer refering to a function to use.
						// 1-step, 2-linear, 3- exp, 4- 'S' curve 5-sine 6-"same as last cell"
	double 	fval;		//the final value
	double	tscale;		//the timescale to approach final value
	} AnalogTable[NUMBEROFCOLUMNS+1][NUMBERANALOGCHANNELS+NUMBERDDS][NUMBEROFPAGES]; //+1 needed because all code done assumed base 1 arrays...
	// the structure is the values/elements contained at each point in the 
	// analog panel.  The array aval, is set up as [x][y][page]
	
int DigTableValues[NUMBEROFCOLUMNS+1][MAXDIGITAL][NUMBEROFPAGES];
int ChMap[MAXANALOG];	// The channel mapping (for analog). i.e. if we program line 1 as channel 
				// 12, the ChMap[12]=1

double TimeArray[NUMBEROFCOLUMNS+1][NUMBEROFPAGES];

struct AnalogChannelProperties{
	int		chnum;		// channel number 1-8 DAC1	9-16 DAC2
	char    chname[50]; // name to appear on the panel
	char	units[50];
	double  tfcn;		// Transfer function.  i.e. 10V out = t G/cm etc...
	double  tbias;
	int		resettozero;
	double  maxvolts;
	double  minvolts;
	}  AChName[MAXANALOG+NUMBERDDS];

struct DigitalChannelProperties{
	int		chnum;		// digital line to control
	char 	chname[50];	// name of the channel on the panel
	int 	resettolow;
	}	DChName[MAXDIGITAL];

double EventPeriod;  //The Update Period Defined by the pull down menu (in ms)
int processnum;

struct DDSClock{
	double 	extclock;
	int 	PLLmult;
	double	clock;	
}	DDSFreq;


ddsoptions_struct ddstable[NUMBEROFCOLUMNS+1][NUMBEROFPAGES]; //17 columns (actually only 14, but in case we expand), 10 pages
ddsoptions_struct dds2table[NUMBEROFCOLUMNS+1][NUMBEROFPAGES]; //17 columns (actually only 14, but in case we expand), 10 pages
dds3options_struct dds3table[NUMBEROFCOLUMNS+1][NUMBEROFPAGES];
int Active_DDS_Panel; // 1 for Rb evap dds, 2 for K40 evap dds, 3 for HFS dds

// Mark and Interval vars for Synchronous Scan and Repeat wait periods
static double beginProcMark;
static double CycleTime;			// Min time allocated for sequence calculations and execution


/* Parameter Scan variables*/
typedef struct AnalogScanParameters{
	double Start_Of_Scan;
	double End_Of_Scan;
	double Scan_Step_Size;
	int	   Iterations_Per_Step;
	int	   Analog_Channel;
	int	   Analog_Mode;
} AnalogScan;

typedef struct TimeScanParameters{
	double 	Start_Of_Scan;
	double 	End_Of_Scan;
	double 	Scan_Step_Size;
	int	   	Iterations_Per_Step;
}	TimeScan;

typedef struct DDSScanParameters{
	double  Base_Freq;
	double  Start_Of_Scan;
	double 	End_Of_Scan;
	double 	Scan_Step_Size;
	double  Current;
	int	   	Iterations_Per_Step;

} DDSScan;

typedef struct DDSFloorScanParameters{
	double 	Floor_Start;
	double 	Floor_End;
	double 	Floor_Step;
	int		Iterations_Per_Step;
} DDSFloorScan;

typedef struct LaserScanParameters{
	double 	Start_Of_Scan;
	double 	End_Of_Scan;
	double 	Scan_Step_Size;
	int		Iterations_Per_Step;
} LaserScan;


struct ScanParameters{
	int    Row;
	int    Column;
	int    Page;
	int    ScanMode;// 0 for Analog, 1 for Time, 2 for DDS
	BOOL   ScanDone;
	BOOL   Scan_Active;
	BOOL   Use_Scan_List;
	struct AnalogScanParameters	Analog;
	struct TimeScanParameters	Time;
	struct DDSScanParameters   	DDS;
	struct DDSFloorScanParameters DDSFloor;
	struct LaserScanParameters Laser;
}  PScan;

struct ScanSet{
	double Start;
	double End;
	double Step;
	int	   Iterations;
	int    Current_Step;
	double Current_Value;
	int    Current_Iteration;	
}	ScanVal;

struct ScanBuff{
	int Iteration;
	int Step;
	double Value;
	char Time[100];
	int BufferSize;
} ScanBuffer[1000];

/* LaserTable Data and Settings Arrays */

//The digital channels which the Adwin triggers the rabbits for the respective lasers on
//Colums array done in base 1  in accordance with the rest of this program
//Note that NUMBEROFPAGES was set to 11 even though there is only 10 pages, hence NUMBEROFPAGES array is also base 1
#define LASCHAN0 117							
#define LASCHAN1 118
#define LASCHAN2 119
#define LASCHAN3 120

#define MINRAMP_LEADOUT 0.0   //This much time in (ms) is given to the rabbit to begin preprocessing of events following a ramp.
#define MAX_DDS_SCANRATE 100.0 //in MHz/ms (this is actually currently limited by the max laser scan response rate
#define MIN_ROSA_FREQ 200.0 //MHz - this is a result of the minimum freq which is accepted into Hittite prescalers 
#define MAX_ROSA_FREQ 6400.0 //MHz
#define MAX_PFD_INPUT 150.0 //MHz - limitation of AD9858 detector
#define ONBOARD_ROSA_DIV 16 //On board integer divders ratio cutting down rosa signal (16 on eval boad)



struct LaserTableValues{
	int fcn;			   // 0 for hold,1 for step,2 for ramp
	double fval;
}LaserTable[NUMBERLASERS][NUMBEROFCOLUMNS+1][NUMBEROFPAGES];

struct LaserProps{
	int Active;				 	  // 1 if that laser is being used, 0 otherwise
	char Name[20];			 	  // Laser name
	char IP[20];			 	  // IP addresses for Rabbit controller TCP Socket
	unsigned int Port;		 	  // Port for Rabbit controller TCP Socket  
	int DigitalChannel;		      // An array built from the LASCHAN values
	double DDS_Clock;		      // Laser DDS Clock Frequency
	double ICPREF;			      // Charge Pump Ref Current(mA) Note: ICP=1.24V/CPISET --> CPISET resistor is 2.4kOhm on eval board
	int ICP_FD_Mult;			  // Charge pump current mode multipliers: FD -> Freq Detect
	int ICP_WL_Mult;			  // Wide Loop
	int ICP_FL_Mult;			  // Final Loop
}LaserProperties[NUMBERLASERS];





#endif

