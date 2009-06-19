/*  RabbitCom9858.h
	Author: David Burns, July 2006
	Purpose: Function library for rabbit control of AD9858 DDS via TCP
	All functions return tcp commands and command length (by reference) */

#ifndef RB9858COMDEF
#define RB9858COMDEF

#define MAXRBERRS 10
#define RBREADY 0x7E

/* Command Headers and lengths for tcp commands for AD9858 control (can be added to an event sequence) */

#define POWER_DDS 0xA0		   // Powers up and down DDS Core
#define POWER_DDS_SIZE 2       // 2nd byte is control 1=on, 0=off

#define DDSRESET 0xA1		   // Hardware Reset of all DDS rgisters
#define DDSRESET_SIZE 1		   

#define FUD 0xA2			   // Latches Contents of I/O Buffer into Memory
#define FUD_SIZE 1

#define PROFILE_SELECT 0xA3	   // Switches between DDS profiles
#define PROFILE_SELECT_SIZE 2  // 2nd byte: 0-3 for Profiles 0-3

#define AWAIT_TRIG 0xA4        // For use in cmdBlock, execution is halted until trigger signal received
#define AWAIT_TRIG_SIZE 1	   // See documentation for accepted trigger signals

#define SETDDSFREQ 0xA5        // Sets DDS Frequency Tuning Word (FTW)
#define SETDDSFREQ_SIZE 6      // 2nd byte: selects profile to write to: last bytes 3-6 set the FTW

#define POWER_PFD 0xA6         // Powers up and down the DSS' phase frequency detector circuitry & charge pumps
#define POWER_PFD_SIZE 2       // 2nd byte set PFDs on or off, and toggles Fast Locking

#define SETCPUMPREF 0xA7       // Sets the Charge Pump Reference
#define SETCPUMPREF_SIZE 2     // 2nd bit: 0=GND REF, 1=Supply Ref 

#define SET_PFD_DIVIDER 0xA8   // Sets the divisor ratio on the PLL comp 2 (ROSA) input (FUD reqd)
#define SET_PFD_DIVIDER_SIZE 2 // 2nd byte holds divisor ratio (1,2,or 4)	

#define SET_CPCURRENT 0xA9     // Sets the CP current settings in the dds CFR
#define SET_CPCURRENT_SIZE 3   // 2nd byte: wrote directly into CFR3, 3rd byte: 1 enables offset current, 0 disables

#define DDSSCAN 0xAA		   // To initiate DDS Scanning (back and forth freq sweeps between two frequencies)            	
#define DDSSCAN_SIZE 12		   // 2nd bit toggle on/off, bit 3-6 DFTW, bit 7-8 DFTRR, bit 9-12 ScanTime

#define RAMPDDS_END 0xAB       // Ends DDS ramp (Prof Change reqd) 	
#define RAMPDDS_END_SIZE 1	   

#define RAMPDDS_START 0xAC     // Begins a DDS ramp (FUD reqd)
#define RAMPDDS_START_SIZE 7   // Byte 2-5 are DFTW, byte 6-7 are the DFRRW

#define SETDAC 0xAD
#define SETDAC_SIZE 3 			 // Byte 2 and 3 hold the 12 bit value to set the DAC to LSB first

#define DACSCAN 0xAE          // Begins a DAC scan. Last eight bytes hold four short ints for (in order)
#define DACSCAN_SIZE 9			// Step Size,hold time at each step,scan bottom,scan top

#define MODE_CFGS 0xAF		   // Sets values of linear/int, and feedback CMOS switches	
#define MODE_CFGS_SIZE 2	   // Byte 2 holds info for switch states


//----------------- Rabbit commands (these commands cannot be added to an event sequence) -------------------------------//

#define CLEAREVENTS 0xC0	   // Clears Current Event Table from RAM
#define CLEAREVENTS_SIZE 1

#define ADDEVENT 0xC1		   // Header Byte prefixing a commands to be added to the event table
#define ADDEVENT_SIZE 1 		

#define INIT_AUTOCONFIG 0xC2   // Saves the current Event Table to Flash (non volatile for future execution)
#define INIT_AUTOCONFIG_SIZE 2 // 2nd Byte identifies which autoconfig to save to, options 0-4

#define EXCT_AUTOCONFIG 0xC3   // executes the autoconfig identified by 2nd byte
#define EXCT_AUTOCONFIG_SIZE 2

#define EXCT_SEQ 0xC4    	   // Begins event table execution of 
#define EXCT_SEQ_SIZE 2        //2nd byte: 0 for Adwin Triggering 1 for TCP/IP triggering

#define TCP_TRIG 0xC5          // TCP trigger signal when executing in TCP triggered mode
#define TCP_TRIG_SIZE 2        // 2nd byte indicates trig type 0 is short (continue execution), 1 is long (break)

#define FORCE_MONCON 0xC6	   // Tells the rabbit to attemp to connect to the monitor console
#define FORCE_MONCON_SIZE 1

static double DDSCLK;          //the clock speed of the DDS in MHz
static double ICPREF;          // Charge Pump reference current in uA    
static int PFD_DivShadow;      //Takes values 0,1,2 for divider settings 1,2,4 respectively

static char* RB9858ErrorBuffer[MAXRBERRS];	   // Holds error messages
static int RB9858ErrorCount;

/*************************************************************************************************************************/
void RB9858LibErr(char * msg)
{
	if(RB9858ErrorCount<MAXRBERRS)
	{
		RB9858ErrorBuffer[RB9858ErrorCount]=msg;
		RB9858ErrorCount++;
	}
}
/*************************************************************************************************************************/
int int_pow(int base, int power)
/*  Returns base^power */
{
	int i,num=1;
	for (i=0;i<power;i++)
		num*=base;
	return num;
}

/*************************************************************************************************************************/
void zero(unsigned char arr[],int arrsize)
/* Sets all the values in the arr to 0x00 */
{
	int i=0;
	for(i=0;i<arrsize;i++)
		arr[i]=0x00;
		
}

/*************************************************************************************************************************/
unsigned char * cmd_powerDDS(unsigned int dds_on,int *cmdLength)
/*  Returns the command required to turn the DDS core on or off
	Parameters: dds_on=1 core powered up 
				dds_on=0 shut down
*/
{
	unsigned char *cmd;
	if((cmd = (unsigned char *)malloc(POWER_DDS_SIZE*sizeof(char)))==(unsigned char *) NULL)
	{
		MessagePopup ("Memory Error", "Insufficient Memory, Exiting");
		exit(1);
	}
	else
	{
		*cmdLength=POWER_DDS_SIZE;
		cmd[0]=POWER_DDS;
		if ((dds_on==1)||(dds_on==0))
			cmd[1]=dds_on;
		else
		{
			RB9858LibErr("INVALID powerDDS Command\n"); 
			*cmdLength=0;
		}
	}
	return cmd;
}

/*************************************************************************************************************************/
unsigned char * cmd_ddsReset(int *cmdLength)
/*  Returns the command required to reset the dds to its default settings */
{
	unsigned char *cmd = (unsigned char *)malloc(DDSRESET_SIZE*sizeof(char));
	*cmdLength=DDSRESET_SIZE; 
	cmd[0]=DDSRESET;
	return cmd;
}
/*************************************************************************************************************************/	
unsigned char * cmd_FUD(int *cmdLength)
/*  Returns the command required to trigger a FUD on the DDS 
	A FUD is required to latch the contents of the AD9858's IO buffer into system memory 
	This command is required following commands which list (req FUD) in order for that command to take effect */
{
	unsigned char *cmd = (unsigned char *)malloc(FUD_SIZE*sizeof(char));	
	*cmdLength=FUD_SIZE;
	cmd[0]=FUD;
	return cmd;
}
/*************************************************************************************************************************/	
unsigned char* cmd_setFreq(int profile, double frequency,int *cmdLength)
/* Returns the command required to set the FTW in order produce the frequency (parameter) in the selected profile 
   2nd Byte is the profile to write to
   last 4 bytes are the FTW LSB first
   frequency in MHz */

{
	unsigned char *cmd = (unsigned char *)malloc(SETDDSFREQ_SIZE*sizeof(char));
	unsigned char *ftw_hex; 
	unsigned int ftw_int;
	int i;
	
	if((profile>=0)&&(profile<=3)&&(frequency<DDSCLK/(double)2)&&(frequency>0))
	{
		*cmdLength=SETDDSFREQ_SIZE; 
		cmd[0]=SETDDSFREQ;
		ftw_int=(unsigned int)(frequency*pow(2,32)/(DDSCLK));
		ftw_hex=(unsigned char *)&ftw_int; 	
		cmd[1]=profile;
		for (i=0;i<4;i++)										  // @@ this algoriothm can be improved
		{
			cmd[i+2]=ftw_hex[i];
		}
	}
	else
	{
		RB9858LibErr("Unnaceptable Frequency or Profile Setting\n");  
		*cmdLength=0;
	}
	
	return cmd;
}
/*************************************************************************************************************************/			
unsigned char* cmd_setProfile(int profile, int *cmdLength)
/*  Returns the command to change the Frequency profile of the DDS immediately
	4 profiles: 0-3, 2nd byte indicates which one to change to */
{
	unsigned char *cmd = (unsigned char *)malloc(PROFILE_SELECT_SIZE*sizeof(char));
	*cmdLength=PROFILE_SELECT_SIZE;
	if((profile>=0)&&(profile<=3))
	{
		cmd[0]=PROFILE_SELECT;
		cmd[1]=profile;
	}
	else
	{
		RB9858LibErr("Unnaceptable Profile\n"); 
		*cmdLength=0;
	}
	return cmd;
}
/*************************************************************************************************************************/
unsigned char* cmd_setPFD_Div(int divisor, int *cmdLength)		
/*  Returns the command required to load I/O buffer with command to change the divider ratio on the COMP 1 (ROSA) input
	Options: 1,2,4: 
	(FUD reqd)
	
	Note: The Rosa signal is divided by 16 in hardware, this divider acts after that producing total division of 16,32,64.
	This is required to achieve high freq offsets becasue the COMP inputs are limited to 150MHz, 400Mhz (with 1/4 enabled) */
{
	unsigned char *cmd = (unsigned char *)malloc(SET_PFD_DIVIDER_SIZE*sizeof(char));
	if((divisor==1)||(divisor==2)||(divisor==4))
	{
		*cmdLength=SET_PFD_DIVIDER_SIZE;
		cmd[0]=SET_PFD_DIVIDER;
		cmd[1]=divisor;
	}
	else
	{
		RB9858LibErr("Bad PFD Divider Setting\n"); 
		*cmdLength=0;
	}
		
	return cmd;
}

/*************************************************************************************************************************/
unsigned char* cmd_powerPFD(int power_toggle,int FastLock_enable,int useFTWforFL,int *cmdLength)
/*  Returns the command to power on or off the DDS PFD and CP cores
	cmd[1] contains the data to set the following settings
	
	Parameters: power_toggle 1 = power up
	
 	bit0  use FTW for Fast Locking 0=enable 1=disable
  	bit1  Fast lock 1=enable 0=disable
  	bit3: Power PFD: 0=on 1=off	 */
{
	unsigned char *cmd = (unsigned char *)malloc(POWER_PFD_SIZE*sizeof(char));
	if ((power_toggle==0|power_toggle==1)&&(FastLock_enable==0|FastLock_enable==1)&&(useFTWforFL==0|useFTWforFL==1))
	{
		*cmdLength=POWER_PFD_SIZE;
		cmd[0]=POWER_PFD;
		cmd[1]=!useFTWforFL*int_pow(2,0)+FastLock_enable*int_pow(2,1)+!power_toggle*int_pow(2,3);
	}
	else
	{
		RB9858LibErr("Bad PFD Setting\n"); 
		*cmdLength=0;
	}
	return cmd;
}
/*************************************************************************************************************************/
unsigned char* cmd_setCPRef(int ref, int *cmdLength)
/*  Sets the charge pump to either GND (0) or Supply (1) reference mode 
	(FUD reqd) */
{
	unsigned char *cmd = (unsigned char *)malloc(SETCPUMPREF_SIZE*sizeof(char));
	if(ref==0||ref==1)
	{
		*cmdLength=SETCPUMPREF_SIZE;
		cmd[0]=SETCPUMPREF;
		cmd[1]=ref;
	}
	else
	{
		RB9858LibErr("Bad CP Ref Setting\n"); 
		*cmdLength=0;
	}
	return cmd;
}
/*************************************************************************************************************************/
unsigned char* cmd_setCPCurent(int freqDetect,int wideClosedLoop,int finalClosedLoop,int *cmdLength)
/* Sets the charge pump current scaling factors for the three lock modes modes, the current is scalefactor*ICPREF
   Options are given in AD9858 man */
{   
   int errors;
   unsigned char *cmd = (unsigned char *)malloc(SET_CPCURRENT_SIZE*sizeof(char));
   
   errors=0;
   cmd[0]=SET_CPCURRENT;
   cmd[1]=0x00;
   cmd[2]=0x00;
   
   switch(freqDetect)
   {
   		case 0:
   			cmd[1]+=0x00*0x40;
   		break;
   		case 20:
   			cmd[1]+=0x01*0x40;
   		break;
   		case 40:
   			cmd[1]+=0x02*0x40;
   		break;
   		case 60:
   			cmd[1]+=0x03*0x40;
   		break;
   		default:
   			errors++;
   		break;
   	}
   	
   	switch(wideClosedLoop)
   	{
   		case 0:
   			cmd[1]+=0x00;
   		break;
   		case 2:
   			cmd[1]+=0x01;
   		break;
   		case 4:
   			cmd[1]+=0x02;
   		break;
   		case 6:
   			cmd[1]+=0x03;
   		break;
   		case 8:
   			cmd[1]+=0x04;
   		break;
   		case 10:
   			cmd[1]+=0x05;
   		break;
   		case 12:
   			cmd[1]+=0x06;
   		break;
   		case 14:
   			cmd[1]+=0x07;
   		break;
   		default:
   			errors++;
   		break;
   	}
   	
   	switch(finalClosedLoop)
   	{
   		case 0:
   			cmd[1]+=0x00*0x08;
   		break;
   		case 1:
   			cmd[1]+=0x04*0x08;
   		break;
   		case 2:
   			cmd[1]+=0x05*0x08;
   		break;
   		case 3:
   			cmd[1]+=0x06*0x08;
   		break;
   		case 4:
   			cmd[1]+=0x07*0x08;
   		break;
   		case 5:
   			cmd[1]+=0x04*0x08;
   			cmd[2]=1;
   		break;
   		case 6:
   			cmd[1]+=0x05*0x08;
   			cmd[2]=1;
   		break;
   		case 7:
   			cmd[1]+=0x06*0x08;
   			cmd[2]=1;
   		break;
   		case 8:
   			cmd[1]+=0x07*0x08;
   			cmd[2]=1;
   		break;
   		
   		default:
   			errors++;
   		break;
   	}
   	
	if (errors==0)
		*cmdLength=SET_CPCURRENT_SIZE;
	else 
	{	
		RB9858LibErr("Bad CP Current Setting\n"); 
		*cmdLength=0;
	}
	
	return cmd;
		
}
/*************************************************************************************************************************/
unsigned char* cmd_StopDDSRamp(int *cmdLength)
/*  This function causes the Rabbit to load stop freq sweeping command into the DDS I/O buffer.
	Follow this function with a FUD or profile change command triggered at the desired execution time */
{
	unsigned char *cmd = (unsigned char *)malloc(RAMPDDS_END_SIZE*sizeof(char));
	*cmdLength=RAMPDDS_END_SIZE; 
	cmd[0]=RAMPDDS_END;
	return cmd;			  
}
/*************************************************************************************************************************/
unsigned char* cmd_beginDDSRamp(double rampHeight,double rampTime, int *cmdLength)
/*  This command ramps the dds from the current frequency to current+rampHeight (rampHeight can be negative) in 
	time rampTime. To use this command properly, first set the desired end frequency to an inactive profile. Use
	a FUD to trigger beginning of ramp, then load StopDDSRamp, followed by a profile change triggeres rampTime after
	the FUD. See example command list below
	
	setFreq(destination freq,destination profile)
	beginDDSRamp()
	WaitForTrigger()
	FUD()
	WaitForTrigger()
	setProfile(destination profile)
	
	(FUD reqd)
	rampHeight in MHz (can be negative),rampTime in ms
    cmd[1]-cmd[4] are DFTW, cmd[5]-cmd[6] are the DFRRW */
{
	unsigned char *cmd = (unsigned char *)malloc(RAMPDDS_START_SIZE*sizeof(char));
	unsigned short dfrrw=0;
	int dftw=0,i;
	
	unsigned char * dftw_hex;
	unsigned char * dfrrw_hex;
	double dftw_over_dfrrw;
	
	if ((fabs(rampHeight)>0.0)&&(fabs(rampHeight)<400.0)&&(rampTime>0.0)&&(rampTime<pow(10,9)))
	{
		// Ensure that max scan rate not exceeded
		if(fabs(rampHeight)/rampTime>MAX_DDS_SCANRATE)
		{
			RB9858LibErr("Max DDS Ramp rate exceeded\n"); 
			*cmdLength=0;
		}
		else
		{
			*cmdLength=RAMPDDS_START_SIZE;
			cmd[0]=RAMPDDS_START;
		
			dftw_over_dfrrw=rampHeight*pow(2.0,35.0)/rampTime/pow(DDSCLK,2.0)/pow(10,3);
		
			//Algorithm to minimize stepsize, (ensuring it is non-zero)
			if (fabs(dftw_over_dfrrw)>1.0)
			{
				dfrrw=1;
				dftw=(int)(dftw_over_dfrrw);
			
			}
			//otherwise set dftw to 1 and set ramp rate slower 
			else
			{
				if(dftw_over_dfrrw>0)
					dftw=1;
				else
					dftw=-1;
			
				dfrrw=(short)(1.0/fabs(dftw_over_dfrrw));
			}
			//printf("dfrrw: %d\tdftw: %d\n",dfrrw,dftw);
			
			dfrrw_hex=(unsigned char *)&dfrrw;  
			dftw_hex=(unsigned char *)&dftw;  
		
			cmd[5]=dfrrw_hex[0];
			cmd[6]=dfrrw_hex[1];
		
			for(i=0;i<4;i++)
			{
				cmd[i+1]=dftw_hex[i];
			}
			//printf("Message: ");
   		   	//for (i=0;i<8;i++)
   		   	//	printf("%d ",cmd[i]);
   		    //printf("\n");
		}
	}
	else
	{
		RB9858LibErr("Fatal Ramp Error\n"); 
		*cmdLength=0;
	}
	return cmd;

}

   		
/*************************************************************************************************************************/
unsigned char* cmd_ScanDDS(double scanAmplitude,double scanTime, int *cmdLength)
/*  2nd byt toggle on/off, byte 3-6 DFTW, byte 7-8 DFTRR, byte 9-12 ScanTime 
	Sends the command to start DDS scanning from the current freq  
	DDS freq oscillates linearly from current setting to current setting +/- scanAmplitude (MHz) with a period of 2scanTime (in ms)
	to scan downwards from current freq set scanDown to 1, otherwise, will scan up
	Scan Amplitude in MHz, scanTime in ms
	Note: There is an error in the AD9858 Documentation, the freq step size is DFTW*SYSCLK/2^32 (not2^31)
	*/
	
{
	unsigned char *cmd = (unsigned char *)malloc(DDSSCAN_SIZE*sizeof(char));
	unsigned int scanTime_int = (unsigned int)scanTime;
	int dftw=0,i;
	unsigned short dfrrw=0;
	unsigned char * scanTime_hex;
	unsigned char * dftw_hex;
	unsigned char * dfrrw_hex;
	double dftw_over_dfrrw;
	
	if ((fabs(scanAmplitude)>0.0)&&(fabs(scanAmplitude)<400.0)&&(scanTime>0.0)&&(scanTime<pow(10,9)))
	{
		// Ensure that max scan rate not exceeded
		if(fabs(scanAmplitude)/scanTime>MAX_DDS_SCANRATE)
		{
			RB9858LibErr("Max DDS Scan rate exceeded\n"); 
			*cmdLength=0;
		}
		else
		{
			*cmdLength=DDSSCAN_SIZE;
			cmd[0]=DDSSCAN;
			cmd[1]=1;
			scanTime_hex=(unsigned char *)&scanTime_int;
		
			dftw_over_dfrrw=scanAmplitude*pow(2.0,35.0)/scanTime/pow(DDSCLK,2.0)/pow(10,3);
		
			//Algorithm to minimize stepsize, (ensuring it is non-zero)
			if (fabs(dftw_over_dfrrw)>1.0)
			{
				dfrrw=1;
				dftw=(int)(dftw_over_dfrrw);
				
			}
			//otherwise set dftw to 1 and set ramp rate slower 
			else
			{
				if(dftw_over_dfrrw>0)
					dftw=1;
				else
					dftw=-1;
				
				dfrrw=(short)(1.0/fabs(dftw_over_dfrrw));
			}
			//printf("ScanTime: %dms\tdfrrw: %d\tdftw: %d\n",scanTime_int,dfrrw,dftw);
			
			dfrrw_hex=(unsigned char *)&dfrrw;  
			dftw_hex=(unsigned char *)&dftw;  
		
			cmd[6]=dfrrw_hex[0];
			cmd[7]=dfrrw_hex[1];
		
			for(i=0;i<4;i++)
			{
				cmd[i+8]=scanTime_hex[i];
				cmd[i+2]=dftw_hex[i];
			}
			//printf("Message: ");
    	  	//for (i=0;i<12;i++)
    	  	//	printf("%d ",cmd[i]);
   		    // printf("\n");
		}	
	}
	else
	{
		RB9858LibErr("Bad Scan Setting");
		*cmdLength=0;
	}
	return cmd;

}
/*************************************************************************************************************************/
unsigned char* cmd_StopDDSScan(int *cmdLength)
// not currently used, instead I have used the reset command
{
 	*cmdLength=0;
 	return 0;
}
/*************************************************************************************************************************/
unsigned char* cmd_clearEvents(int *cmdLength)
/*  Clears the event tabled from Rabbit Memory.
	This command should be executed prior to loading a new event sequence */
{
	unsigned char *cmd = (unsigned char *)malloc(CLEAREVENTS_SIZE*sizeof(char));
	*cmdLength=CLEAREVENTS_SIZE;
	cmd[0]=CLEAREVENTS;
	return cmd;
}
/*************************************************************************************************************************/
unsigned char* cmd_startSeq(int triggerMode, int *cmdLength)
/*  Begins execution of sequence loaded into cmdBlock 
	triggerMode 0 for Adwin, 1 for TCP triggering */	
{
	unsigned char *cmd = (unsigned char *)malloc(EXCT_SEQ_SIZE*sizeof(char));
	
	if(triggerMode==0||triggerMode==1)
	{
		*cmdLength=EXCT_SEQ_SIZE;
		cmd[0]=EXCT_SEQ;
		cmd[1]=triggerMode;
	}
	else
	{
		RB9858LibErr("Invalid startSeq parameter\n");
		*cmdLength=0;
	}
	return cmd;
}
/*************************************************************************************************************************/
unsigned char* cmd_setAutoConfig(int configNum,int *cmdLength)
/*  Saves the current sequence to flash (non volatile) and can be executed later with exct command 
	Possible to set 5 autoconfigs, indexed 0-4, max 500 bytes each (~45 Commands) 
	As a note, you can test the current sequence by executing normally before storing it in flash, this is recommended */
{
	unsigned char *cmd = (unsigned char *)malloc(INIT_AUTOCONFIG_SIZE*sizeof(char));
	if(configNum>=0&&configNum<=4)
	{
		*cmdLength=INIT_AUTOCONFIG_SIZE;	
		cmd[0]=INIT_AUTOCONFIG;
		cmd[1]=configNum;
	}
	else
	{
		RB9858LibErr("Invalid Config Num\n");
		*cmdLength=0;
	}
	return cmd;
}
/*************************************************************************************************************************/
unsigned char* cmd_exctAutoConfig(int configNum,int *cmdLength)
/* Executes autoconfig in flash designated by configNum */
{
	unsigned char *cmd = (unsigned char *)malloc(EXCT_AUTOCONFIG_SIZE*sizeof(char));
	if(configNum>=0&&configNum<=4)
	{
		*cmdLength=EXCT_AUTOCONFIG_SIZE;	
		cmd[0]=EXCT_AUTOCONFIG;
		cmd[1]=configNum;
	}
	else
	{
		RB9858LibErr("Invalid Config Num\n");  
		*cmdLength=0;
	}
	return cmd;
}
/*************************************************************************************************************************/
unsigned char* cmd_tcpTrig(int trigLength, int *cmdLength)
/* Triggers resume of execution or break of sequence in TCP trigger mode 
   trigLength: 0 for short (resume)
   			   1 for long (break) */
{
	unsigned char *cmd = (unsigned char *)malloc(TCP_TRIG_SIZE*sizeof(char));
	if (trigLength==0||trigLength==1)
	{
		*cmdLength=TCP_TRIG_SIZE;
		cmd[0]=TCP_TRIG;
		cmd[1]=trigLength;
	}
	else
	{
		RB9858LibErr("Bad TCP Trigger"); 
		*cmdLength=0;
	}
	return cmd;
}
/*************************************************************************************************************************/
unsigned char* cmd_waitForTrigger(int *cmdLength)
/*  This command pauses execution of the sequence until a trigger signal is received (either type based on triggering mode
	set when beginning sequence execution) */
{
	unsigned char *cmd = (unsigned char *)malloc(AWAIT_TRIG_SIZE*sizeof(char));
	*cmdLength=AWAIT_TRIG_SIZE;
	cmd[0]=AWAIT_TRIG;
	return cmd;
}
/*************************************************************************************************************************/
unsigned char* cmd_setSwitchStates(int lin_or_int,int CPfb_or_DAC,int *cmdLength)
/*  Sets the position of the CMOS switches. 
	lin_or_int: 1 for linear 0 for integral CP feedback
	CPfb_or_DAC: 1 enables CP feedback 0 enables DAC control*/
{
	unsigned char *cmd = (unsigned char *)malloc(MODE_CFGS_SIZE*sizeof(char));
	if((lin_or_int==1||lin_or_int==0)&&(CPfb_or_DAC==1||CPfb_or_DAC==0))
	{
		*cmdLength=MODE_CFGS_SIZE;
		cmd[0]=MODE_CFGS;
		cmd[1]=CPfb_or_DAC+2*lin_or_int;
	}
	else
	{
		RB9858LibErr("Bad CMOS Switch Setting\n");
		*cmdLength=0;
	}
	return cmd; 

}
/*************************************************************************************************************************/
unsigned char* cmd_forceMonCon(int *cmdLength)
{
	unsigned char *cmd = (unsigned char *)malloc(FORCE_MONCON_SIZE*sizeof(char));
	*cmdLength=FORCE_MONCON_SIZE;
	cmd[0]=FORCE_MONCON;
	return cmd;
}
/*************************************************************************************************************************/
unsigned char* cmd_setDac(double voltage,int *cmdLength)
/* DAC set to voltage, 0V-4.095V */
{
	short* vbin;
	
	unsigned char *cmd =(unsigned char *)malloc(SETDAC_SIZE*sizeof(char));
	if(voltage>=0.0&&voltage<=4.095)
	{
		*cmdLength=SETDAC_SIZE;
		cmd[0]=SETDAC;
		vbin=(short*)&cmd[1];
		*vbin=voltage/4.095*4095;
	}
	else
	{
		RB9858LibErr("DAC Setting Error: Bad Voltage\n");
		*cmdLength=0;
	}
	return cmd; 
}
/*************************************************************************************************************************/
unsigned char* cmd_dacScan(double interval,double VBottom,double VTop,int *cmdLength)
/*  Returns the Command to begin a DAC Scan from VBottom to VTop (Volts) within interval (ms) 
	The LTC1451IC8 DAC full scale is 4.095V */
{
	short *step,*htime,*bottom,*top;
	unsigned char *cmd =(unsigned char *)malloc(DACSCAN_SIZE*sizeof(char));   
	
	//check bounds
	if((VBottom<VTop)&&(VBottom>=0.0)&&(VTop<=4.095))
	{
		*cmdLength=DACSCAN_SIZE;	
		cmd[0]=DACSCAN;
		step=(short*)&cmd[1];
		htime=(short*)&cmd[3];
		bottom=(short*)&cmd[5];
		top=(short*)&cmd[7];
		
		*bottom=(short)(4095.0*VBottom/4.095);
		*top=(short)(4095.0*VTop/4.095);
		
		// @@ using min step time of 1 ms, test this to ensure the rabbit can keep up fast enough
		*htime=1;
		*step=(short)((*top-*bottom)/(interval/(double)*htime));
		
		if(*step>0)
			*cmdLength=DACSCAN_SIZE;
		else
		{
			RB9858LibErr("DAC Scan Interval too short\n"); 
			*cmdLength=0; 
		}
		
	}
	else
	{
		RB9858LibErr("Invalid DAC Scan Voltage Bounds, use 0-4.095V\n"); 
		*cmdLength=0; 
	}
	
	return cmd;
}
	
#endif

