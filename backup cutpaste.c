Guidesign.c

int menuHandle;
int currentx=0,currenty=0,currentpage=0;
int pic_off,pic_static,pic_change,pic_don;
int ischecked[10]={0},isdimmed=0;
int ChangedVals=1;
struct AnalogTableValues{
	int		fcn;		//fcn is an integer refering to a function to use.
						// 1-step, 2-linear, 3- exp, 4- 'S' curve
	double 	fval;		//the final value
	double	tscale;		//the timescale to approach final value
	} AnalogTable[17][NUMBERANALOGCHANNELS+1][10]={1,0,1};
	// the structure is the values/elements contained at each point in the 
	// analog panel.  The array aval, is set up as [x][y][page]
int DigTableValues[17][17][10]={0};
int ChMap[NUMBERANALOGCHANNELS+1]={0};	// The channel mapping (for analog). i.e. if we program line 1 as channel 
				// 12, the ChMap[12]=1

double TimeArray[17][10]={0};
struct AnalogChannelProperties{
	int		chnum;		// channel number 1-8 DAC1	9-16 DAC2
	char    chname[50]; // name to appear on the panel
	char	units[50];
	double  tfcn;		// Transfer function.  i.e. 10V out = t G/cm etc...
	double  tbias;
	}  AChName[NUMBERANALOGCHANNELS+1]={0,"","",0,1};

struct DigitalChannelProperties{
	int		chnum;		// digital line to control
	char 	chname[50];	// name of the channel on the panel
	}	DChName[33]={0,""};
	
	
struct DDSParameters{
	int		channel;  // "channel" to update	
	int		FreqWord1;	
	int		FreqWord2;
	int		FreqDeltaWord;
	int		Amplitude;
	}  DDSTable[17][10]  ;
double EventPeriod=DefaultEventPeriod;
int processnum;



DihitalSettings.c



AnalogSettings
// Calling globals from main.c
extern int panelHandle,panelHandle2;
	extern int ChangedVals;
extern  struct AnalogTableValues{
	int		fcn;		//fcn is an integer refering to a function to use.
						// 0-step, 1-linear, 2- exp, 3- 'S' curve
	double 	fval;		//the final value
	double	tscale;		//the timescale to approach final value
	} AnalogTable[17][NUMBERANALOGCHANNELS+1][10];
	
extern struct AnalogChannelProperties{
	int		chnum;		// channel number 1-8 DAC1	9-16 DAC2
	char    chname[50]; // name to appear on the panel
	char	units[50];
	double  tfcn;		// Transfer function.  i.e. 10V out = t G/cm etc...
	double  tbias;
	}  AChName[NUMBERANALOGCHANNELS+1];



AnalogControl
 extern int panelHandle4,panelHandle;
extern struct AnalogTableValues{
	int		fcn;		//fcn is an integer refering to a function to use.// 0-step, 1-linear, 2- exp, 3- 'S' curve
	double 	fval;		//the final value
	double	tscale;		//the timescale to approach final value
	} AnalogTable[17][NUMBERANALOGCHANNELS+1][10];  // read as x,y,page
	
extern struct AnalogChannelProperties{
	int		chnum;		// channel number 1-8 DAC1	9-16 DAC2
	char    chname[50]; // name to appear on the panel
	char	units[50];
	double  tfcn;		// Transfer function.  i.e. 10V out = t G/cm etc...
	double  tbias;
	}  AChName[NUMBERANALOGCHANNELS+1];	

extern double TimeArray[17][10];
extern int isdimmed;	
extern int currentpage,currentx,currenty;

