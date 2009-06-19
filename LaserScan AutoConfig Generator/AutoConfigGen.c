/*  AutoConfigGen.c
	Author: David Burns, 09 August 2006
	Purpose: Scanning AD9858 DDS, and saving scan configurations into Rabbit Flash memory for automatic execution
			 using user switches. */

#include <tcpsupp.h>
#include <ansi_c.h>
#include <cvirte.h>		
#include <userint.h>
#include "AutoConfigGen.h"
#include "AutoConfigGen2.h"
#include "vars.h"
#include "RabbitCom9858.h"

#define TCP_BUFF 960

// Globals
static int PFD_DivShadow;
static int configLoaded;		   // Keeps track of Rabbit state
static int FreqMode;			   // 0 for const, 1 for DDS Scan, 2 for Dac Scan

int main (int argc, char *argv[])
{
	int i,j;
	
	if (InitCVIRTE (0, argv, 0) == 0)
		return -1;	/* out of memory */
	if ((mainpanel = LoadPanel (0, "AutoConfigGen.uir", MainPanel)) < 0)
		return -1;
	
	//Initialization
	configLoaded=-1;
	SetCtrlAttribute (mainpanel, MainPanel_RAM_LED, ATTR_ON_COLOR, VAL_GREEN);
	SetCtrlVal(mainpanel,MainPanel_CONSTFREQRAD,1);	// set default FreqMode to ConstFreq
	FreqMode=0;
	
	LoadScanData();
	
	DisplayPanel (mainpanel);
	RunUserInterface ();
	DiscardPanel (mainpanel);
	SaveScanData();
	return 0;
}
/*******************************************************************************************************************/
int CVICALLBACK LASERSELECT_CALLBACK (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
/* Callback function for selecting a laser on the ring control */
{
	switch (event)
		{
		case EVENT_COMMIT:
			updateDisplay();
			if (configLoaded>0)
				configLoaded=0;
			break;
		}
	return 0;
}
/*******************************************************************************************************************/
int CVICALLBACK CONFIG_NUM_CALLBACK (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
/* Callback function for selecting a configuration number on the Autoconfig Index numeric */
{
	switch (event)
		{
		case EVENT_COMMIT:
			updateDisplay();
			if (configLoaded>0)
				configLoaded=0;
			break;
		}
	return 0;
}

/*******************************************************************************************************************/
int CVICALLBACK RAM_LOAD_CALLBACK (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
/*  Callback function for selecting Load Config to Ram and Test button
	Programs Rabbit (RAM) with scanning parameters and executes sequence */
{
	switch (event)
		{
		case EVENT_COMMIT:
			if (configLoaded>=0) 
			{
				sendEventList();
			}
			break;
		}
	return 0;
}
/*******************************************************************************************************************/
int CVICALLBACK FLASH_LOAD_CALLBACK (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
/*  Callback function for selecting Save RAM to FLASH button
	Writes loaded sequence into Rabbit Flash and saves scan parameters in ConfigSaves.cfg  */

{
	int laserNum,configNum,comErr=0;
	unsigned char* cmdList[5];
	int cmdLengthList[5];
	int cmdCount=1,i;
	unsigned int tcp_handle;
	
	switch (event)
		{
		case EVENT_COMMIT:
			
			if(configLoaded==1)
			{
				
				GetCtrlVal (mainpanel, MainPanel_LASER_RING, &laserNum);
				GetCtrlVal (mainpanel, MainPanel_CONFIG_NUM, &configNum);
				
				cmdList[cmdCount]=cmd_setAutoConfig(configNum,&cmdLengthList[cmdCount]);  	
				cmdCount++;
				
				tcp_handle=tcpConnect(laserNum);
				comErr+=tcpSendCmdList(tcp_handle,cmdList,cmdLengthList,cmdCount);
				comErr+=tcpDisconnect(tcp_handle);
				
				for(i=1;i<cmdCount;i++)
					free(cmdList[i]);
					
				if(comErr==0)
				{
					GetCtrlVal(mainpanel,MainPanel_SCANSTART,&LaserScans[laserNum][configNum].startFreq);
					GetCtrlVal(mainpanel,MainPanel_SCANEND,&LaserScans[laserNum][configNum].endFreq);
					GetCtrlVal(mainpanel,MainPanel_NUM_SCANTIME,&LaserScans[laserNum][configNum].scanTime);
					LaserScans[laserNum][configNum].mode=FreqMode;
			
					configLoaded=2; 
				}
				else
					printf("Error saving config to flash.\n");
			}
			
			break;
		}
	return 0;
}
/*******************************************************************************************************************/
int CVICALLBACK RESET_CALLBACK (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
/*  Callback function for selecting Reset DDS */ 
{
	int laserNum,comErr=0;
	unsigned char* cmdList[5];
	int cmdLengthList[5];
	int cmdCount=1,i;
	unsigned int tcp_handle;
	
	
	switch (event)
		{
		case EVENT_COMMIT:
			if (configLoaded>=0) 
			{
				GetCtrlVal (mainpanel, MainPanel_LASER_RING, &laserNum);
				
				cmdList[cmdCount]=cmd_ddsReset(&cmdLengthList[cmdCount]);  	
				cmdCount++;
				
				tcp_handle=tcpConnect(laserNum);
				comErr+=tcpSendCmdList(tcp_handle,cmdList,cmdLengthList,cmdCount);
				comErr+=tcpDisconnect(tcp_handle);
				
				for(i=1;i<cmdCount;i++)
					free(cmdList[i]);
					
				if(comErr==0)
				{
					configLoaded=0; 
				}
				else
					printf("Error Resetting DDS.\n");
				

			}
			break;
		}
	return 0;
}
/*******************************************************************************************************************/
int CVICALLBACK FLASH_TEST_CALLBACK (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
/*  Callback for Test config from FLASH button. Executes the sequence currently loaded on the current config */
{
	int laserNum,configNum,comErr=0;
	unsigned char* cmdList[5];
	int cmdLengthList[5];
	int cmdCount=1,i;
	unsigned int tcp_handle;
	
	switch (event)
		{
		case EVENT_COMMIT:
			if (configLoaded>=0) 
			{
				GetCtrlVal (mainpanel, MainPanel_LASER_RING, &laserNum);
				GetCtrlVal (mainpanel, MainPanel_CONFIG_NUM, &configNum);
				
				cmdList[cmdCount]=cmd_exctAutoConfig(configNum,&cmdLengthList[cmdCount]);  	
				cmdCount++;
				
				tcp_handle=tcpConnect(laserNum);
				comErr+=tcpSendCmdList(tcp_handle,cmdList,cmdLengthList,cmdCount);
				comErr+=tcpDisconnect(tcp_handle);
				
				for(i=1;i<cmdCount;i++)
					free(cmdList[i]);
					
				if(comErr==0)
				{
					configLoaded=0;
				}
				else
					printf("Error excuting config.\n");
			}
			break;
		}
	return 0;
}
/*******************************************************************************************************************/
int CVICALLBACK TIMER_CALLBACK (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
/*  Handles Idiot Light Flashing */
{
	int tempStatus;
	
	switch (event)
		{
		case EVENT_TIMER_TICK:
			switch(configLoaded)
			{
				case -1:		// laser settings not loaded
					SetCtrlAttribute (mainpanel, MainPanel_RAM_LED, ATTR_ON_COLOR, VAL_RED);
					SetCtrlAttribute (mainpanel, MainPanel_FLASH_LED, ATTR_ON_COLOR, VAL_RED);
					SetCtrlVal (mainpanel,MainPanel_RAM_LED,1);  
					SetCtrlVal (mainpanel,MainPanel_FLASH_LED,1);
					break;
				case 0:			// no config loaded //laser settings loaded
					
					// flash ram led green
					SetCtrlAttribute (mainpanel, MainPanel_RAM_LED, ATTR_ON_COLOR, VAL_GREEN);
					GetCtrlVal (mainpanel,MainPanel_RAM_LED, &tempStatus);
					SetCtrlVal (mainpanel,MainPanel_RAM_LED,!tempStatus);
					
					// hold flash led red
					SetCtrlAttribute (mainpanel, MainPanel_FLASH_LED, ATTR_ON_COLOR, VAL_RED);
					SetCtrlVal (mainpanel,MainPanel_FLASH_LED,1);
					break;
				
				case 1:		    // config loaded to RAM
					// hold ram led green
					SetCtrlVal (mainpanel,MainPanel_RAM_LED,1);
					
					// flash flash led green
					SetCtrlAttribute (mainpanel, MainPanel_FLASH_LED, ATTR_ON_COLOR, VAL_GREEN);
					GetCtrlVal (mainpanel,MainPanel_FLASH_LED, &tempStatus);
					SetCtrlVal (mainpanel,MainPanel_FLASH_LED,!tempStatus);
					break;
				case 2:		    // config loaded to FLASH
					SetCtrlVal (mainpanel,MainPanel_RAM_LED,1);  
					SetCtrlVal (mainpanel,MainPanel_FLASH_LED,1);
					break;
			
			}
			break;
		}
	return 0;
}
/*******************************************************************************************************************/
int CVICALLBACK EXIT_CALLBACK (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
/*  Callback function for the Exit Button */
{
	switch (event)
		{
		case EVENT_COMMIT:
			QuitUserInterface (0);
			break;
		}
	return 0;
}
/*******************************************************************************************************************/
void CVICALLBACK LSET_SAVE_CALLBACK (int menuBar, int menuItem, void *callbackData,
		int panel)
/*  Settings Menu -Save Button Callback - feature not programmed */
{
}
/*******************************************************************************************************************/
void CVICALLBACK LSET_LOAD_CALLBACK (int menuBar, int menuItem, void *callbackData,
		int panel)
/*  Settings Menu -Load Button Callback 
	Allows you to load laser parameters (saved in Sequencer Panel) from .laser file 
	This is a required step before and communication with Rabbits */
{
	int status;
	char fsavename[500];
	
	status=FileSelectPopup ("E:\\User Data\\Burns\\National Instruments\\Test Panels\\LaserTests", "*.laser", "", "Load Settings", VAL_LOAD_BUTTON, 0, 0, 1, 1,fsavename );
	if(!(status==0))
	{
		LoadLaserData(fsavename,strlen(fsavename));
	}
	else
	{
		MessagePopup ("File Error", "No file was selected");
	}

}
/*******************************************************************************************************************/
void CVICALLBACK LSET_EDIT_CALLBACK (int menuBar, int menuItem, void *callbackData,
		int panel)
/*  Settings Menu -Edit Button Callback - feature not programmed */
{
}
/*******************************************************************************************************************/
void LoadLaserData(char savedname[500],int nameSize)
{
/*  Loads all Panel attributes and values relevant to the laser control which was saved in a
	.laser file by SaveLaserData
	   
    Note that if the lengths of any of the data arrays are changed previous saves will not be able to be loaded. 
    If necessary See AdwinGUI Panel Converter V11-V12 (created June 01, 2006) */

	
	FILE *ldata;
	char buff2[600]="";
	int i;
	
	if((ldata=fopen(savedname,"r"))==NULL)
	{
		strcpy(buff2,"Failed to load laser data arrays. \n Filename: \n");
		strcat(buff2,savedname);
		MessagePopup("Save Error",buff2);
	}
	else
	{
		fread(LaserProperties,sizeof LaserProperties, 1,ldata);	
		for(i=0;i<NUMBERLASERS;i++)
			ReplaceListItem (mainpanel,MainPanel_LASER_RING,i,LaserProperties[i].Name,i);
		configLoaded=0;
		
	}
}
/*******************************************************************************************************************/  
void SaveScanData()
/*	Saves the status of all autoconfigs programmed using this tool */
{
	FILE *ldata;  
	char buff[100]="ConfigSaves.cfg";
	char buff2[200];
	
	if((ldata=fopen(buff,"w"))==NULL)
	{
		strcpy(buff2,"Failed to save configuration. \n Filename: \n");
		strcat(buff2,buff);
		MessagePopup("Save Error",buff2);
	}
	else
	{
		fwrite(LaserScans,sizeof LaserScans, 1,ldata);	
	}

}
/*******************************************************************************************************************/
void LoadScanData()
/*  Loads autoconfig statuses */
{
	FILE *ldata;  
	char buff[100]="ConfigSaves.cfg";
	char buff2[200];
	
	if((ldata=fopen(buff,"r"))==NULL)
	{
		strcpy(buff2,"Failed to load configuration. \n Filename: \n");
		strcat(buff2,buff);
		MessagePopup("Load Error",buff2);
	}
	else
	{
		fread(LaserScans,sizeof LaserScans, 1,ldata);	
	}

}

/*******************************************************************************************************************/
void updateDisplay()
/*  Displays saved scan paramters */
{
	int laserNum,configNum;
	
	GetCtrlVal (mainpanel, MainPanel_LASER_RING, &laserNum);
	GetCtrlVal (mainpanel, MainPanel_CONFIG_NUM, &configNum);
	
	SetCtrlVal(mainpanel,MainPanel_SCANSTART,LaserScans[laserNum][configNum].startFreq);
	SetCtrlVal(mainpanel,MainPanel_SCANEND,LaserScans[laserNum][configNum].endFreq);
	SetCtrlVal(mainpanel,MainPanel_NUM_SCANTIME,LaserScans[laserNum][configNum].scanTime); 
	FreqMode=LaserScans[laserNum][configNum].mode;
	
	SetCtrlVal(MainPanel, MainPanel_DACSCANRAD,0);
	SetCtrlVal(MainPanel, MainPanel_DDSSCANRAD,0);
	SetCtrlVal(MainPanel, MainPanel_CONSTFREQRAD,0);
	
	switch(FreqMode)
	{
		case 0:
			SetCtrlVal(MainPanel, MainPanel_CONSTFREQRAD,1); 
			break;
		case 1:
			SetCtrlVal(MainPanel, MainPanel_DDSSCANRAD,1);      
			break;
		case 2:
			SetCtrlVal(MainPanel, MainPanel_DACSCANRAD,0);
			break;									  
	}
	
	
	
}
/*******************************************************************************************************************/ 
void sendEventList()
/*  The guts of this program. Builds an event sequence based on the Scan Parameters and sends them to rabbit followed 
	By an execute command. */
{
	int laserNum,i,j,comErr=0;
	unsigned char* cmdList[MAXCMDNUM*10];
	int cmdLengthList[MAXCMDNUM*10];
	int cmdCount,initCmdCount,pfdUpdate;
	unsigned int tcp_handle;
	double largerFreq,startFreq,endFreq,scanTime; 
	char errorBuff[200];
	
	GetCtrlVal (mainpanel, MainPanel_LASER_RING, &laserNum);
	GetCtrlVal(mainpanel,MainPanel_SCANSTART,&startFreq);
	GetCtrlVal(mainpanel,MainPanel_SCANEND,&endFreq);
	GetCtrlVal(mainpanel,MainPanel_NUM_SCANTIME,&scanTime);
	RB9858ErrorCount=0;
	
	
	DDSCLK=LaserProperties[laserNum].DDS_Clock;
	ICPREF=LaserProperties[laserNum].ICPREF;
			
	cmdCount=1;
	initCmdCount=1;
	
	cmdList[cmdCount]=cmd_clearEvents(&cmdLengthList[cmdCount]);     //clears event table
	cmdCount++;
	initCmdCount++;
	
	//Sets up commands to set intial settings
	cmdList[cmdCount]=cmd_ddsReset(&cmdLengthList[cmdCount]);  		//resets dds 
	cmdCount++;
		
	cmdList[cmdCount]=cmd_powerDDS(1,&cmdLengthList[cmdCount]);      // powers up dds
	cmdCount++;
	
	cmdList[cmdCount]=cmd_setCPCurent(LaserProperties[laserNum].ICP_FD_Mult,LaserProperties[laserNum].ICP_WL_Mult,LaserProperties[laserNum].ICP_FL_Mult,&cmdLengthList[cmdCount]);
	cmdCount++;
	
	cmdList[cmdCount]=cmd_powerPFD(1,1,0,&cmdLengthList[cmdCount]);  // Powers up PFD (FL enabled)
	cmdCount++;
		
	PFD_DivShadow=0;
		
	switch(FreqMode)
	{
		case 0:	// const freq
			cmdList[cmdCount]=cmd_setSwitchStates(0,1,&cmdLengthList[cmdCount]); // sets to integral mode and enables feedback
			cmdCount++;
			
			pfdUpdate=checkDividerSetting(fabs(startFreq));   
			
			cmdList[cmdCount]=cmd_setFreq(0,calcFreq(startFreq),&cmdLengthList[cmdCount]);
			cmdCount++;
				
			//check if need to update pfd before setting freq
			if(pfdUpdate>0)
			{
				cmdList[cmdCount]=cmd_setPFD_Div(pfdUpdate,&cmdLengthList[cmdCount]);		
				cmdCount++;
							
				cmdList[cmdCount]=cmd_FUD(&cmdLengthList[cmdCount]);		
				cmdCount++;
			}
			
			//check if CPRef needs to be changed
			if(startFreq<0)
			{
				cmdList[cmdCount]=cmd_setCPRef(1,&cmdLengthList[cmdCount]);
				cmdCount++; 
									
				cmdList[cmdCount]=cmd_FUD(&cmdLengthList[cmdCount]);		
				cmdCount++;
			}
			break;
		
		
		case 1: // DDS Scan
			if(startFreq*endFreq<0)  
			{
				RB9858LibErr("Fatal Error cannot ramp across zero\n");
				comErr--;
			}
			else
			{
				cmdList[cmdCount]=cmd_setSwitchStates(0,1,&cmdLengthList[cmdCount]); // sets to integral mode and enables feedback
				cmdCount++;
	
				//Begin Setting start Freq
				if(fabs(startFreq)>fabs(endFreq))
					largerFreq=startFreq;
				else
					largerFreq=fabs(endFreq);
	
				pfdUpdate=checkDividerSetting(largerFreq);
				
				cmdList[cmdCount]=cmd_setFreq(0,calcFreq(startFreq),&cmdLengthList[cmdCount]);
				cmdCount++;
						
				//check if need to update pfd before setting freq
				if(pfdUpdate>0)
				{
					cmdList[cmdCount]=cmd_setPFD_Div(pfdUpdate,&cmdLengthList[cmdCount]);		
					cmdCount++;
							
					cmdList[cmdCount]=cmd_FUD(&cmdLengthList[cmdCount]);		
					cmdCount++;
				}
			
				//check if CPRef needs to be changed
				if(startFreq<0)
				{
					cmdList[cmdCount]=cmd_setCPRef(1,&cmdLengthList[cmdCount]);
					cmdCount++; 
										
					cmdList[cmdCount]=cmd_FUD(&cmdLengthList[cmdCount]);		
					cmdCount++;
				}
	
				cmdList[cmdCount]=cmd_ScanDDS(calcFreq(fabs(endFreq)-fabs(startFreq)),scanTime,&cmdLengthList[cmdCount]);
				cmdCount++;
			}				
			break;
				
		case 2:	//DAC Scan
			MessagePopup ("Under Construction", "DAC Scanning not yet complete");
			return;
			break;
		
	}
		
						
	//Add ADDEVENT header to sequence updates (not the initializing commands)
	for(i=initCmdCount;i<cmdCount;i++)
	{
		cmdLengthList[i]++;
		cmdList[i]=(unsigned char *)realloc(cmdList[i],cmdLengthList[i]*sizeof(char));
		for(j=cmdLengthList[i]-1;j>0;j--)
			cmdList[i][j]=cmdList[i][j-1];
		cmdList[i][0]=ADDEVENT;
	}
			
	
	cmdList[cmdCount]=cmd_startSeq(1,&cmdLengthList[cmdCount]);		//Begin sequence execution (Adwin Triggered) set to zero (now at 1 for testing)
	cmdCount++;

	if(cmdCount>=MAXCMDNUM)
		RB9858LibErr("MAX CMD Number Exceeded");
		
	if(RB9858ErrorCount==0)
	{
		tcp_handle=tcpConnect(laserNum);
		comErr+=tcpSendCmdList(tcp_handle,cmdList,cmdLengthList,cmdCount);
		comErr+=tcpDisconnect(tcp_handle);
	}
	else
	{
		sprintf(errorBuff,"Bad Setting in Sequence. Rabbit not programmed.\nView %d Error[s]?",RB9858ErrorCount);
		if(ConfirmPopup (LaserProperties[laserNum].Name,errorBuff))
		{
			for(j=0;j<RB9858ErrorCount;j++)
			{
				sprintf(errorBuff,"Error %d",j);
				MessagePopup (errorBuff, RB9858ErrorBuffer[j]);
				comErr=-1;
			}
		}
	}

	//Free Mem
	for(i=1;i<cmdCount;i++)
		free(cmdList[i]);

	
	if(comErr==0)
	{
		configLoaded=1;   // RAM sucessfully laoded	
	}
	
			
}
/*************************************************************************************************************************/
int checkDividerSetting(double newLaserFreq)
/*  Checks to ensure that the current PFD divider setting is suitable for the newLaserFreq setting.
	Returns a 0 if no divider change is needed, otherwise returns the new required divider val and updates the divider 
	shadow 
	newLaserFreq in MHz*/
{
	
	int pfdDivVals[3]={1,2,4};
	int goodSetting,dividerUpdate;
	
	
	goodSetting=0;
	dividerUpdate=0;

	
	goodSetting=0;
	
	if(newLaserFreq<MIN_ROSA_FREQ||newLaserFreq>ONBOARD_ROSA_DIV*400)
		RB9858LibErr("FREQ Set to low or high ERROR\n"); //throw @@
	else
	{
		// Determines whether the PFD dividers need to be set to a different value, these formulae are derived to
		// comply with the AD9858 PFD input specs
		while(goodSetting==0)
		{
			if(newLaserFreq<pfdDivVals[PFD_DivShadow]*MIN_ROSA_FREQ)
			{
				PFD_DivShadow--;
				dividerUpdate=pfdDivVals[PFD_DivShadow];
			}
			else if(newLaserFreq>pfdDivVals[PFD_DivShadow]*ONBOARD_ROSA_DIV*(MAX_PFD_INPUT-25.0*PFD_DivShadow))	 
			{
				PFD_DivShadow++;
				dividerUpdate=pfdDivVals[PFD_DivShadow];
			}
			else
				goodSetting++;
		}
	
	}
	return dividerUpdate;
}
/*************************************************************************************************************************/
double calcFreq(double LaserFreq)
/*  Calculates and returns the required DDS frequency setting to achieve the desired LaserFreq (offset) (both in MHz)
	The laser offset frequency is divided down by 16 in hardware, and is divided further by the programmable PFD divider
	by a value of 1,2 or 4. */
{
	int pfdDivVals[3]={1,2,4};
	
	return fabs(LaserFreq)/(double)(ONBOARD_ROSA_DIV*pfdDivVals[PFD_DivShadow]);
}
/*************************************************************************************************************************/
int tcpSendCmdList(unsigned int tcp_handle,unsigned char* cmdList[MAXCMDNUM],int* cmdLengthList,int cmdCount)
/*  Sends cmdList over tcp to the socket identified by tcp_handle. Returns a negative number if a communication error
	occured. */
{
	int i,j,comErr=0;
	for(i=1;i<cmdCount;i++)
	{
		comErr+=ClientTCPWrite(tcp_handle, cmdList[i],cmdLengthList[i],0);
		/*printf("\nCommand Sent: ");
		for (j=0;j<cmdLengthList[i];j++)
			printf("%d ",cmdList[i][j]);*/
		
	}
	if(comErr<0)
		printf("Error sending Data\n");
	else
		comErr=0;	
	return comErr;

}
/*************************************************************************************************************************/
unsigned int tcpConnect(int laserNum)
{
/*  Opens a tcp socket with the Rabbit controlling the laser indexed by laserNum.
	Returns: tcp_handle used as an identifier for the socket */ 
	
	int connected;
	char* error;
	unsigned int tcp_handle;
	
	tcp_handle=0;
	connected = ConnectToTCPServer (&tcp_handle,LaserProperties[laserNum].Port,LaserProperties[laserNum].IP,&TCP_Comm_Callback, 0,1000);
	
	//printf("Rabbit Ip: ");
	//printf(LaserProperties[laserNum].IP);
	//printf("\nPort: %d", LaserProperties[laserNum].Port);
	
	if (connected==0)
	{
		//printf("Connected!\n");
	}
	else if (connected<0)
	{
		printf("TCP Connection Error: %d\n",connected);
		error=tcp_errorlookup(connected);
		printf(error);  
	}
	
	return tcp_handle;  
	
}
/*************************************************************************************************************************/
int tcpDisconnect(unsigned int tcp_handle)
/*  Closes tcp socket identified by tcp_handle. 
	Returns: 0 for successful completion, negative number if unsuccesful */
{
	int tcpErr;
	char * error;
	
	tcpErr=DisconnectFromTCPServer (tcp_handle);		
	if (tcpErr<0)
	{
		printf("Error Closing Socket\n");
		printf(tcp_errorlookup(tcpErr));
	}
	else
		tcpErr=0;		
	return tcpErr;
	
}
/*************************************************************************************************************************/
char* tcp_errorlookup(int tcp_error_code)
/*  Given the tcp_error_code - returns the explanation of the error as a string */
{
	char* error;
	
	switch(tcp_error_code)
	{
		case 0: 
		error="No Error\n";
		break;
		
		case -1:
		error="Unable To Register Service\n";
		break;
		
		case -2:
		error="Unable To Establish Connection\n";
		break;
		
		case -3:
		error="Existing Server\n";
		break;
		
		case -4:
		error="Failed To Connect\n";
		break;
		
		case -5:
		error="Server Not Registered\n";
		break;
		
		case -6:
		error="Too Many Connections\n";
		break;
		
		case -7:
		error="Read Failed\n";
		break;
		
		case -8:
		error="Write Failed\n";
		break;
		
		case -9:
		error="Invalid Parameter\n";
		break;
		
		case -10:
		error="Out Of Memory\n";
		break;
		
		case -11:
		error="Time Out Err\n";
		break;
		
		case -12:
		error="No Connection Established\n";
		break;
		
		case -13:
		error="General IO Err\n";
		break;
		
		case -14:
		error="Connection Closed\n";
		break;
		
		case -15:
		error="Unable To Load WinsockDLL\n";
		break;
		
		case -16:
		error="Incorrect WinsockDLL Version\n";
		break;
		
		case -17:
		error="Network Subsystem Not Ready\n";
		break;
		
		case -18:
		error="Connections Still Open\n";
		break;
		
		case -19:
		error="Disconnect Pending\n";
		break;
		
		case -20:
		error="Info Not Available\n";
		break;
		
		case -21:
		error="Host Address Not Found\n";
		break;
		
		default:
		error="Unknown Error\n";
	}
	return error;
}
/*************************************************************************************************************************/
int TCP_Comm_Callback(unsigned handle, int xType, int errCode, void *callbackData)
/*  Callback function called when a TCP traffic is received for any sockets opened by NI CVI
	xType is TCP transaction type */

{
	unsigned char buffer[TCP_BUFF]; //hard limits max message size
	char * error;
	int bytesRead;
	int maxData;
	int i;
	
	
	switch (xType)
	{
		case TCP_DISCONNECT:
		tcpDisconnect(handle);		
		break;
		
		case TCP_DATAREADY:
		break;
		
		
	}
	
	return 0;
}
/*************************************************************************************************************************/
int CVICALLBACK ConstFreqCallback (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
		{
		case EVENT_COMMIT:
			SetCtrlVal(panel, MainPanel_DACSCANRAD,0);
			SetCtrlVal(panel, MainPanel_DDSSCANRAD,0);
			SetCtrlVal(panel, MainPanel_CONSTFREQRAD,1);
			FreqMode=0;
			
			break;
		}
	return 0;
}
/*************************************************************************************************************************/
int CVICALLBACK DDSScanCallback (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
		{
		case EVENT_COMMIT:
			SetCtrlVal(panel, MainPanel_DACSCANRAD,0);
			SetCtrlVal(panel, MainPanel_CONSTFREQRAD,0);
			SetCtrlVal(panel, MainPanel_DDSSCANRAD,1);   
			FreqMode=1;
			
			break;
		}
	return 0;
}
/*************************************************************************************************************************/
int CVICALLBACK DACScanCallback (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
		{
		case EVENT_COMMIT:
			SetCtrlVal(panel, MainPanel_CONSTFREQRAD,0);
			SetCtrlVal(panel, MainPanel_DDSSCANRAD,0);
			SetCtrlVal(panel, MainPanel_DACSCANRAD,1);     
			FreqMode=2;
			
			break;
		}
	return 0;
}

