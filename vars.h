

#ifndef VAR_DEFS          // Make sure this file is included only once
#define VAR_DEFS

/************************************************************************
Macro Definitions
*************************************************************************/

#define DefaultEventPeriod 0.010   // in milliseconds
#define AdwinTick	0.000025       //Adwin clock cycle, in ms.
#define NUMBERANALOGCHANNELS 24	   // Number of analog Channels available for control
#define NUMBERDIGITALCHANNELS 24  	// number of digital channels that may 
									// be user controlled.  Some are reserved
									// for DDS etc.
#define TRUE 1
#define FALSE 0
#define NUMBEROFPAGES 11				//currently hardwired up to 10
									   // to be quick & dirty about it, just change 
									   //numberofpages to 1 more than actual
/************************************************************************
Structure/Typedef Declarations
*************************************************************************/

typedef enum transtype_enum {LINEAR,EXPONENTIAL} TRANSTYPE;
typedef int  BOOL;

typedef struct ddsoptions_struct {
	float frequency1; /*these frequencies are in units of mega hertz*/
	float frequency2;
	float current;
    TRANSTYPE transition_type;
	BOOL is_update; /*if false, ignore this element*/
	float delta_time;
	BOOL is_stop;
	float start_time;
	BOOL new_settings; /*if true, reset all frequency values, if false, only frequency 2 is changing*/ 
} DDSOPTIONS;

/************************************************************************
Global Variables
*************************************************************************/

int panelHandle,panelHandle2,panelHandle3,panelHandle4,panelHandle5,panelHandle6;         
int  menuHandle;
int currentx,currenty,currentpage;
int pic_off,pic_static,pic_change,pic_don;
int ischecked[NUMBEROFPAGES],isdimmed;
int ChangedVals;
struct AnalogTableValues{
	int		fcn;		//fcn is an integer refering to a function to use.
						// 1-step, 2-linear, 3- exp, 4- 'S' curve
	double 	fval;		//the final value
	double	tscale;		//the timescale to approach final value
	} AnalogTable[17][NUMBERANALOGCHANNELS+1][NUMBEROFPAGES];
	// the structure is the values/elements contained at each point in the 
	// analog panel.  The array aval, is set up as [x][y][page]
int DigTableValues[17][NUMBERDIGITALCHANNELS+1][NUMBEROFPAGES];
int ChMap[NUMBERANALOGCHANNELS+1];	// The channel mapping (for analog). i.e. if we program line 1 as channel 
				// 12, the ChMap[12]=1

double TimeArray[17][NUMBEROFPAGES];
struct AnalogChannelProperties{
	int		chnum;		// channel number 1-8 DAC1	9-16 DAC2
	char    chname[50]; // name to appear on the panel
	char	units[50];
	double  tfcn;		// Transfer function.  i.e. 10V out = t G/cm etc...
	double  tbias;
	int		resettozero;
	double  maxvolts;
	double  minvolts;
	}  AChName[NUMBERANALOGCHANNELS+1];

struct DigitalChannelProperties{
	int		chnum;		// digital line to control
	char 	chname[50];	// name of the channel on the panel
	int 	resettolow;
	}	DChName[33];
	
	
struct DDSParameters{
	int		channel;  // "channel" to update	
	int		FreqWord1;	
	int		FreqWord2;
	int		FreqDeltaWord;
	int		Amplitude;
	}  DDSTable[17][NUMBEROFPAGES]  ;
double EventPeriod;
int processnum;

struct DDSClock{
	double 	extclock;
	int 	PLLmult;
	double	clock;	
}	DDSFreq;

DDSOPTIONS *ddstable[17]; //17 columns (actually only 14, but in case we expand), 10 pages


	
#endif

