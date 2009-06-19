#include <utility.h>
/*  LaserTuning.c  
	Author: David Burns, August 2006 
	Purpose: Generates and programs event sequences for the Rabbits based on parameters entered into the GUI. (via tcp) 
	Additionally generates the trigger array required to program the adwin. */

#include "LaserTuning.h"
#include "GUIDesign.h"
#include "GUIDesign2.h" 
#include "RabbitCom9858.h"

#define TCP_BUFF 960   //960/2 = max # of bytes per data transmission
#define UNCHOKE_INT 25 // interval (# of commands) before the sequencer waits for the rabit to "unchoke" itself (process tcp commands)
#define UNCHOKE_TO 4.0 // timeout for the rabbit during an unchoke in seconds
static int comReady;  


void BuildLaserUpdates(struct LaserTableValues MetaLaserArray[NUMBERLASERS][500],double MetaTimeArray[500],unsigned int MetaTriggerArray[NUMBERLASERS][500],int numtimes)
// todo add overflow checking
/*  Generates,programs,and begins event sequence for rabbits controlling AD9858 DDSs for Laser Tuning
	Adwin Triggers inserted (reqd) at begining of frequency changes and begining and end of frequency ramps. */
{
	unsigned char* cmdList[MAXCMDNUM];
	int cmdLengthList[MAXCMDNUM];
	int laserNum,cmdCount,cmode,pfdUpdate,profileShadow,initCmdCount;
	double FreqShadow,rampHeight,newFreq;
	int i,j,tcpErr,CPRefShadow,destProfile;
	unsigned int tcp_handle,trigCount=0;
	char errorBuff[100];
	
	//printf("Building List...\n");
	
	for(laserNum=0;laserNum<NUMBERLASERS;laserNum++)
	{
		if(LaserProperties[laserNum].Active)
		{
			for(i=1;i<=numtimes;i++)
				MetaTriggerArray[laserNum][i]=0;
				
			DDSCLK=LaserProperties[laserNum].DDS_Clock;
			ICPREF=LaserProperties[laserNum].ICPREF;
			PFD_DivShadow=0;
			FreqShadow=0.0;													
			profileShadow=0;												//Profile Shadow variable (we only use 1 and 0)
			CPRefShadow=0;			                                        //Shadow variable for the CP Reference
			RB9858ErrorCount=0;
			comReady=0;
			
			cmdCount=1;
			initCmdCount=1;

			//Sets up commands to set intial settings
			cmdList[cmdCount]=cmd_ddsReset(&cmdLengthList[cmdCount]);  		//resets dds 
			cmdCount++;
			initCmdCount++;
			
			if(ChangedVals)
			{
				cmdList[cmdCount]=cmd_clearEvents(&cmdLengthList[cmdCount]);     //clears event table
				cmdCount++;
				initCmdCount++;
			
				cmdList[cmdCount]=cmd_powerDDS(1,&cmdLengthList[cmdCount]);      // powers up dds
				cmdCount++;
				initCmdCount++;
			
				cmdList[cmdCount]=cmd_setCPCurent(LaserProperties[laserNum].ICP_FD_Mult,LaserProperties[laserNum].ICP_WL_Mult,LaserProperties[laserNum].ICP_FL_Mult,&cmdLengthList[cmdCount]);
				cmdCount++;
				initCmdCount++;
			
				cmdList[cmdCount]=cmd_powerPFD(1,0,0,&cmdLengthList[cmdCount]);  // Powers up PFD (FL disnabled)
				cmdCount++;
				initCmdCount++;
			
				cmdList[cmdCount]=cmd_setSwitchStates(0,1,&cmdLengthList[cmdCount]); // sets to integral mode and enables feedback
				cmdCount++;
				initCmdCount++;
			
				//loop through event table 
				for(i=1;i<=numtimes;i++)
				{
					if(MetaLaserArray[laserNum][i].fval!=FreqShadow||cmdCount==initCmdCount) // Set freq if its changed or this is first runthrough
					{
					
						cmode=MetaLaserArray[laserNum][i].fcn;
						
						if(cmode==1)         
						{
						
							pfdUpdate=checkDividerSetting(fabs(MetaLaserArray[laserNum][i].fval));
						
							newProfile(&profileShadow);
							cmdList[cmdCount]=cmd_setFreq(profileShadow,calcFreq(MetaLaserArray[laserNum][i].fval),&cmdLengthList[cmdCount]);
							cmdCount++;
						
						
							//check if need to update pfd before setting freq
							if(pfdUpdate>0)
							{
								cmdList[cmdCount]=cmd_setPFD_Div(pfdUpdate,&cmdLengthList[cmdCount]);		
								cmdCount++;
							
								if(!MetaTriggerArray[laserNum][i]) 
								{
									cmdList[cmdCount]=cmd_waitForTrigger(&cmdLengthList[cmdCount]);		
									cmdCount++;
									trigCount++;  
								}
							
								cmdList[cmdCount]=cmd_FUD(&cmdLengthList[cmdCount]);		
								cmdCount++;
							}
							else if(!MetaTriggerArray[laserNum][i]) 
							{
								cmdList[cmdCount]=cmd_waitForTrigger(&cmdLengthList[cmdCount]);		
								cmdCount++;
								trigCount++;  
							}
						
							//check if CPRef needs to be changed
							if((MetaLaserArray[laserNum][i].fval>0&&CPRefShadow!=0)||(MetaLaserArray[laserNum][i].fval<0&&CPRefShadow!=1))
							{
								CPRefShadow=!CPRefShadow;
								cmdList[cmdCount]=cmd_setCPRef(CPRefShadow,&cmdLengthList[cmdCount]);
								cmdCount++; 
							
								cmdList[cmdCount]=cmd_FUD(&cmdLengthList[cmdCount]);		
								cmdCount++;
							}
						
							cmdList[cmdCount]=cmd_setProfile(profileShadow,&cmdLengthList[cmdCount]);		
							cmdCount++;
						
							MetaTriggerArray[laserNum][i]=1;
						}
						else if(cmode==2)			 //freq ramp
						{
						
							rampHeight=calcRamp(FreqShadow,MetaLaserArray[laserNum][i].fval,&pfdUpdate,&newFreq);  
						
							//Set Destination Freq in inactive profile
							destProfile=newProfile(&profileShadow);
							cmdList[cmdCount]=cmd_setFreq(destProfile,calcFreq(MetaLaserArray[laserNum][i].fval),&cmdLengthList[cmdCount]);
							cmdCount++;
						
						
							if(pfdUpdate>0)  // Set the new frequency under new profile and change pfd divders
							{
							
								newProfile(&profileShadow);
								cmdList[cmdCount]=cmd_setFreq(profileShadow,newFreq,&cmdLengthList[cmdCount]);
								cmdCount++;
							
								cmdList[cmdCount]=cmd_setPFD_Div(pfdUpdate,&cmdLengthList[cmdCount]);		;
								cmdCount++;
							
								if(!MetaTriggerArray[laserNum][i])
								{
									cmdList[cmdCount]=cmd_waitForTrigger(&cmdLengthList[cmdCount]);		;
									cmdCount++;
									trigCount++;  
								}
							
								cmdList[cmdCount]=cmd_FUD(&cmdLengthList[cmdCount]);		;
								cmdCount++;
							
								cmdList[cmdCount]=cmd_setProfile(profileShadow,&cmdLengthList[cmdCount]);		;
								cmdCount++;
							
								cmdList[cmdCount]=cmd_beginDDSRamp(rampHeight,MetaTimeArray[i],&cmdLengthList[cmdCount]);
								cmdCount++;
							
								cmdList[cmdCount]=cmd_FUD(&cmdLengthList[cmdCount]);		;
								cmdCount++;
							}
							else
							{
								cmdList[cmdCount]=cmd_beginDDSRamp(rampHeight,MetaTimeArray[i],&cmdLengthList[cmdCount]);
								cmdCount++;
						
								if(!MetaTriggerArray[laserNum][i]) 
								{
									cmdList[cmdCount]=cmd_waitForTrigger(&cmdLengthList[cmdCount]);		;
									cmdCount++;
									trigCount++;  
								}
						
								cmdList[cmdCount]=cmd_FUD(&cmdLengthList[cmdCount]);		;
								cmdCount++;
							}
						
							cmdList[cmdCount]=cmd_StopDDSRamp(&cmdLengthList[cmdCount]);
							cmdCount++; 
						
							cmdList[cmdCount]=cmd_waitForTrigger(&cmdLengthList[cmdCount]);		;
							cmdCount++;
							trigCount++;  
						
							profileShadow=destProfile;
							cmdList[cmdCount]=cmd_setProfile(profileShadow,&cmdLengthList[cmdCount]);		;
							cmdCount++;
						
							//put in triggers at start of this column and next column
							MetaTriggerArray[laserNum][i]=1;	
							MetaTriggerArray[laserNum][i+1]=1;
						}
					
						FreqShadow=MetaLaserArray[laserNum][i].fval;
				
					}
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
			}
			
			cmdList[cmdCount]=cmd_startSeq(0,&cmdLengthList[cmdCount]);		//Begin sequence execution (Adwin Triggered) 
			cmdCount++;
			
			if(RB9858ErrorCount==0) // Communicate and Execute sequence so long as all settings good
			{
				tcp_handle=tcpConnect(laserNum);
				tcpErr=tcpSendCmdList(tcp_handle,cmdList,cmdLengthList,cmdCount);
				
				//tcpTriggering(tcp_handle,trigCount);		// for testing with tcp troggering only 
				
				Delay (1);                                  // wait a second for the rabbit
				tcpDisconnect(tcp_handle);
				
				SetTableCellAttribute (panelHandle, PANEL_TBL_ANAMES,MakePoint(1,laserNum+NUMBERANALOGCHANNELS+NUMBERDDS+1), ATTR_TEXT_BGCOLOR,VAL_RED); 
				
				if(tcpErr>0)
				{
					sprintf(errorBuff,"%d Error[s] sending seq to ",tcpErr);
					strcat(errorBuff,LaserProperties[laserNum].Name);		
					SeqError(errorBuff);
				}
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
					}
				
				}
				sprintf(errorBuff,"Error[s] in seq for ");
				strcat(errorBuff,LaserProperties[laserNum].Name);		
				SeqError(errorBuff);

			}
				
			//Free Mem
			for(i=1;i<cmdCount;i++)
				free(cmdList[i]);
					
		
		}
	}
	

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
		SetTableCellAttribute (panelHandle, PANEL_TBL_ANAMES,MakePoint(1,laserNum+NUMBERANALOGCHANNELS+NUMBERDDS+1), ATTR_TEXT_BGCOLOR,VAL_GREEN); 
		//printf("Connected!\n");
	}
	else if (connected<0)
	{
		//printf("TCP Connection Error: %d\n",connected);
		//error=tcp_errorlookup(connected);
		//printf(error);  
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
	
	if (tcpErr=DisconnectFromTCPServer (tcp_handle)<0)
	{
		//printf("Error Closing Socket\n");
		//printf(tcp_errorlookup(tcpErr));
	}
	return tcpErr;
	
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
			ClientTCPRead (handle, buffer, TCP_BUFF-1, 0);
		comReady=1;
		
		break;
	}
	return 0;
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
int tcpSendCmdList(unsigned int tcp_handle,unsigned char* cmdList[MAXCMDNUM],int* cmdLengthList,int cmdCount)
/*  Sends cmdList over tcp to the socket identified by tcp_handle. */
{
	int i,j,sendErr,totalErrs;
	double ucStart;
	totalErrs=0;

	
	for(i=1;i<cmdCount;i++)
	{
		sendErr=ClientTCPWrite(tcp_handle, cmdList[i],cmdLengthList[i],0);
		if(sendErr<0)
			totalErrs++;
			
		if(i%UNCHOKE_INT==0||i==cmdCount-2)	// allow rabbit to unchoke at interval and before last cmd (start seq)
		{
			ucStart=Timer();
			comReady=0;  					// @@ this was moved from after loop test 
			while(!comReady)				// com ready set to 1 when console receives HB from rabbit
			{
				if(Timer()-ucStart>UNCHOKE_TO)
				{
					totalErrs++;
					return totalErrs;
				}
				ProcessTCPEvents ();
			}
		}
		
		/*printf("\nCommand Sent: ");
		for (j=0;j<cmdLengthList[i];j++)
			printf("%d ",cmdList[i][j]);*/
	}
	return totalErrs;

}
/*************************************************************************************************************************/
double calcRamp(double laserStartFreq,double laserEndFreq,int *dividerUpdate,double *newDDSFreq)
/*  Calculates the required ramp parameters for the DDS in order to achieve the desired ramp in laser freq
	laserStartFreq and laserEndFreq are in MHz
	return value is the DDS frequency rampHeight
	dividerUpdate gives the required pfd divder update val (0 if not required) 
	newDDSFreq if non zero indicates a DDSFreq update is required prior to ramp starting
*/
{
	double rampHeight;
	double largerFreq;
	*newDDSFreq=0;
	
	
	if(laserStartFreq*laserEndFreq>0)		//ensure not ramping acorss zero (currently not supported)
	{
		if(fabs(laserStartFreq)>fabs(laserEndFreq))
			largerFreq=laserStartFreq;
		else
			largerFreq=fabs(laserEndFreq);
		
		*dividerUpdate=checkDividerSetting(largerFreq);
	
		if(*dividerUpdate>0)
		{
			*newDDSFreq=calcFreq(laserStartFreq);
			
		}
		rampHeight=calcFreq(fabs(laserEndFreq)-fabs(laserStartFreq));
	}
	else
		RB9858LibErr("Error cannot ramp across 0");
	
	return rampHeight;


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
		RB9858LibErr("FREQ Set to low or high ERROR\n");
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
void tcpTriggering(unsigned int tcp_handle,unsigned trigCount)
//this function sends a tcp trigg when the enter key is pressed, sends up to trigCount triggers, for testing only
{
	unsigned char HBcmd = HEARTCMD;
	unsigned char TCPtrig[2];
	char c;
	
	TCPtrig[0]=TCP_TRIG;
	TCPtrig[1]=0x00;
	
	printf("Press enter to send a trigger...\n");
	
	while(trigCount>0)
	{
		c=getchar();
		ClientTCPWrite(tcp_handle, TCPtrig,2,0);	
		printf("Trig Sent\n");
		trigCount--;
	}
}
/*************************************************************************************************************************/
int newProfile(int *profile)
/*  This function is meant to cycle through the numbers 0-3, corresponding to the 4 available profile setting. 
	In future, this algorithm could be optimized to return a profile which currently stores a frequency with the max
	number of bytes identical to those of the update frequency. This would minimize the number Register updates.
	(The rabbit holds shadow vairables for the FTW registers and only updates necessary bytes */
	
{
	if(*profile<3)
		*profile+=1;
	else
		*profile=0;
	
	return *profile;
}
	
	
	













