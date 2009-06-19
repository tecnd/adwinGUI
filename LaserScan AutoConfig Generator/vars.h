/* vars.h */

#ifndef VARSDEF
#define VARSDEF

static int mainpanel;

/* LaserTable Data and Settings Arrays */

//The digital channels which the Adwin triggers the rabbits for the respective lasers on
#define LASCHAN0 117							
#define LASCHAN1 118
#define LASCHAN2 119
#define LASCHAN3 120

#define NUMBERLASERS 4
#define NUMBERCONFIGS 5 

#define MINRAMP_LEADOUT 0.0   //This much time in (ms) is given to the rabbit to begin preprocessing of events following a ramp.
#define MAX_DDS_SCANRATE 100.0 //in MHz/ms (this is actually currently limited by the max laser scan response rate
#define MIN_ROSA_FREQ 200.0 //MHz - this is a result of the minimum freq which is accepted into Hittite prescalers 
#define MAX_PFD_INPUT 150.0 //MHz - limitation of AD9858 detector
#define ONBOARD_ROSA_DIV 16 //On board integer divders ratio cutting down rosa signal (16 on eval boad)
#define MAXCMDNUM 40

struct SavedLaserScans{
	double startFreq;
	double endFreq;
	double scanTime;
	int mode;
}LaserScans[NUMBERLASERS][NUMBERCONFIGS];
	


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
