/* Rabbit Control Program GUI VIA TCP/IP */
/* Author: David Burns, 06 July 2006 */
/* An annoying note about LabWindows C, comparisons between characters>0x7F cannot bet made. Contacted NI to report the bug
   This means that currently we will be unable to read any commands using the ANSI C codes. However, they can be sent at least.
   Solution: enter switched like this switch(buffer[0]&0x0FF), resolves ambiguity between  hex and char comps, can also use unsigned char*/

#include <tcpsupp.h>
#include <ansi_c.h>
#include <stdio.h>
#include <cvirte.h>		
#include <userint.h>
#include "TCPcontrolGUI.h"
#include "TCPcontrolGUI2.h"
#include "RabbitCom9858.h"

#define HEARTBEATTIME 0.5      // The interval in s between sending the command which maintains the socket to the rabbit
#define TIMEOUT 3			   // The interval as an integral multiple of HEARTBEATTIME which the socket will be kept open without receiving anything
								

#define PORT 1111
#define RABBIT_IP "192.168.1.104"
#define TCP_BUFF 960  //960/2 = max # of bytes per data transmission


//Outgoing Commands 

#define HEARTCMD 0x7F
#define HEARTCMD_SIZE 1


// GLOBALS

unsigned int tcp_handle;
static int mainPanel;
int heartSent=0;
int heartReceived=0;
unsigned totalRead;


/*************************************************************************************************************************/
int main (int argc, char *argv[])
{
	if (InitCVIRTE (0, argv, 0) == 0)
		return -1;	/* out of memory */
	if ((mainPanel = LoadPanel (0, "TCPcontrolGUI.uir", MAIN_PANEL)) < 0)
		return -1;
	DisplayPanel (mainPanel);
	
	totalRead=0;
	
	//Initialization Stuff
	SetCtrlAttribute (mainPanel,MAIN_PANEL_CONNECTION_LED, ATTR_ON_COLOR, VAL_GREEN);
	
	//Timer Initialization
	SetCtrlAttribute (mainPanel,MAIN_PANEL_TCP_HEART_TIMER,ATTR_INTERVAL,HEARTBEATTIME);
	SuspendTimerCallbacks();
	
	//Initialize DDS settings
	DDSCLK=983.04;
	ICPREF=0.517;
	PFD_DivShadow=1;
	

	RunUserInterface ();
	DiscardPanel (mainPanel);
	return 0;
}

/*************************************************************************************************************************/
//CallBack Function to the Connect Button, Initiates TCP comm with Rabbit
int CVICALLBACK CONNECT (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	int connected;
	char* error;
	
	switch (event)
		{
		case EVENT_COMMIT:
			connected = ConnectToTCPServer (&tcp_handle,PORT,RABBIT_IP,&TCP_Comm_Callback, 0,1000);
			error=GetTCPSystemErrorString ();
			if (connected==0)
			{
				SetCtrlVal (mainPanel,MAIN_PANEL_CONNECTION_LED,1);
				ResumeTimerCallbacks();

			}
			else if (connected<0)
			{
				printf("TCP Connection Error: %d\n",connected);
				error=tcp_errorlookup(connected);
				printf(error);
			}
			
			break;
		}
	return 0;
}


/*************************************************************************************************************************/
//Callback Function for the Disconnect Button. Disconnects from TCP comm with Rabbit
int CVICALLBACK DISCONNECT (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	int tcpErr;
	char * error;
	
	switch (event)
		{
		case EVENT_COMMIT:
			if (tcpErr=DisconnectFromTCPServer (tcp_handle)<0)
			{
				printf("Error Closing Socket\n");
				printf(tcp_errorlookup(tcpErr));
			}
			else
			{
				//printf("Socket Closed\n");
				SuspendTimerCallbacks();
				SetCtrlVal (mainPanel,MAIN_PANEL_CONNECTION_LED,0);
			}	
			break;
		}
	return 0;
}


/*************************************************************************************************************************/
// Callback function for the Send Data Button. Utilizes RabbitCom9858 library to generate tcp commands based on 
// CMD and data numerics.
int CVICALLBACK SEND_DATA (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	unsigned char* outCMD;
	
	int outdata[4];
	int ledstatus[3]={0};
	int i,outsize,dummyVar;
	int cmdLen=0;
	int tableAddition;
	
	tableAddition=0;
	
	
	
	switch (event)
		{
		case EVENT_COMMIT:
			GetCtrlVal (mainPanel, MAIN_PANEL_OUT_CMD, &outdata[0]);
			GetCtrlVal (mainPanel, MAIN_PANEL_OUT_DATA1, &outdata[1]);
			GetCtrlVal (mainPanel, MAIN_PANEL_OUT_DATA2, &outdata[2]);
			GetCtrlVal (mainPanel, MAIN_PANEL_OUT_DATA3, &outdata[3]);
			
			switch(outdata[0])
			{
				case SETLEDS:
					for(i=0;i<3;i++)
						ledstatus[i]=outdata[i+1];
					outCMD=cmd_SetLED(ledstatus,&cmdLen);
					break;
				
				case DDSRESET:
					outCMD=cmd_ddsReset(&cmdLen);
					break;
					
				case FUD:
					outCMD=cmd_FUD(&cmdLen);
					break;
				case SETDDSFREQ:
					outCMD=cmd_setFreq(outdata[1],(double)outdata[2],&cmdLen);
					break;
				case PROFILE_SELECT:
					outCMD=cmd_setProfile(outdata[1],&cmdLen);
					break;
				case SET_PFD_DIVIDER:
					outCMD=cmd_setPFD_Div(outdata[1],&cmdLen);
				case POWER_DDS:
					outCMD=cmd_powerDDS(outdata[1],&cmdLen);
					break;
				case POWER_PFD:
					outCMD=cmd_powerPFD(outdata[1],outdata[2],outdata[3],&cmdLen);
					break;
				case DDSSCAN:
					outCMD=cmd_ScanDDS((double) outdata[1],(double)outdata[2],&cmdLen);  
					break;
				case CLEAREVENTS:
					outCMD=cmd_clearEvents(&cmdLen);
					break;
				case EXCT_SEQ:
					outCMD=cmd_startSeq(outdata[1],&cmdLen);
					break;
				case INIT_AUTOCONFIG:
					outCMD=cmd_setAutoConfig(outdata[1],&cmdLen);
					break;					
				case EXCT_AUTOCONFIG:
					outCMD=cmd_exctAutoConfig(outdata[1],&cmdLen);
					break;
				case SETCPUMPREF:
					outCMD=cmd_setCPRef(outdata[1],&cmdLen);
					break;
				case SET_CPCURRENT:
					outCMD=cmd_setCPCurent(outdata[1],outdata[2],outdata[3],&cmdLen);
					break;
				case TCP_TRIG:
					outCMD=cmd_tcpTrig(outdata[1],&cmdLen);
					break;
				case AWAIT_TRIG:
					outCMD=cmd_waitForTrigger(&cmdLen);
					break;
				
				//testing sets F register with od[1]
				case 0x01:
					outCMD=(unsigned char *)malloc(2*sizeof(char));
					outCMD[0]=0x01;
					outCMD[1]=outdata[1];
					cmdLen=2;
					break;
				//testing sets G register with od[1]
				case 0x02:
					outCMD=(unsigned char *)malloc(2*sizeof(char));
					outCMD[0]=0x02;
					outCMD[1]=outdata[1];
					cmdLen=2;
					break;
				case 0x03:
					outCMD=(unsigned char *)malloc(2*sizeof(char));
					outCMD[0]=0x03;
					outCMD[1]=outdata[1];
					cmdLen=2;
					break;
				case 0x04:
					outCMD=(unsigned char *)malloc(2*sizeof(char));
					outCMD[0]=0x04;
					outCMD[1]=outdata[1];
					cmdLen=2;
					break;	
				default:
					return 0;
					break;
			}
				
			//printf("outcmd[0]: %d\n",outCMD[0]);
			
			/* For Debugging
			for (i=0;i<cmdLen;i++)
				printf("%d ",outCMD[i]);
			printf("\n");*/
			
			//Check to see if the command should be added to the table or not							
			GetCtrlVal (mainPanel,MAIN_PANEL_CMDBLOCKTICK,&tableAddition); 
			if(tableAddition)
			{
				cmdLen++;
				
				outCMD=(unsigned char *)realloc(outCMD,cmdLen*sizeof(char));
				
				for(i=cmdLen-1;i>0;i--)
					outCMD[i]=outCMD[i-1];
				outCMD[0]=ADDEVENT;
				
			}
			
				
				
			
			ClientTCPWrite(tcp_handle, outCMD,cmdLen,0);
			free(outCMD);
			break;
	}
	return 0;
}
/*************************************************************************************************************************/
// The HeartTimer "ticks" every 500ms, calling this function. The purpose is to ensure that a heartbeat signals are being sent
// and recieved. This is used in to detect connnection timeouts and time stay alive communiques.

int CVICALLBACK TCP_HEART_TIME_CALLBACK (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	int heart_led_status;
	int tcpErr;
	char * error;
	unsigned char outCMD = HEARTCMD;
	
	switch (event)
		{
		case EVENT_TIMER_TICK:
			
			if(heartSent==0)          //No comms made to rabbit in past HEARTBEATTIME, sending heartbeat
			{
				if(tcpErr=ClientTCPWrite(tcp_handle, &outCMD, sizeof(outCMD),0)<0)
				{
					printf("TCP Write Error\n");
					printf(tcp_errorlookup(tcpErr));
				}
			}
			heartSent=0;
			
			
			if(heartReceived==1) // Heartbeat received, resseting counter, flicker indicator
			{
				heartReceived = 0;
				GetCtrlVal (mainPanel,MAIN_PANEL_HEART_LED, &heart_led_status);
				SetCtrlVal (mainPanel,MAIN_PANEL_HEART_LED,!heart_led_status);
			}
			else if ((heartReceived<=0)&&(heartReceived>-TIMEOUT))  //No heartbeat received, TIMEOUT not reached
				heartReceived--;   
			else if (heartReceived<=-TIMEOUT) //No heartbeats resulting in TIMEOUT, closing socket
			{
				printf("Rabbit Processor not detected.Closing Socket...\n");
				if (tcpErr=DisconnectFromTCPServer (tcp_handle)<0)
				{
					printf("Error Closing Socket\n");
					printf(tcp_errorlookup(tcpErr));
				}
				else
				{
					//printf("Socket Closed\n");
				}
				SuspendTimerCallbacks();
				SetCtrlVal (mainPanel,MAIN_PANEL_CONNECTION_LED,0);
			}
			
		break;	
		}

	return 0;
}
/*************************************************************************************************************************/
//Callback for Quit Button. Exits GUI
int CVICALLBACK QUIT (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
		{
		case EVENT_COMMIT:
			DisconnectFromTCPServer (tcp_handle);
			SetCtrlVal (mainPanel,MAIN_PANEL_CONNECTION_LED,0);
			QuitUserInterface (0);
			break;
		}
	return 0;
}

/*************************************************************************************************************************/
//Callback function called when a TCP traffic is received.
//xType is TCP transaction type

int TCP_Comm_Callback(unsigned handle, int xType, int errCode, void *callbackData)
{
	unsigned char buffer[TCP_BUFF]; //hard limits max message size
	char * error;
	int bytesRead;
	int maxData;
	int i;
	
	
	switch (xType)
	{
		case TCP_DISCONNECT:
		
		break;
		
		case TCP_DATAREADY:
			maxData = sizeof (buffer) - 1;
			bytesRead = ClientTCPRead (tcp_handle,buffer,maxData, 0);
			if (bytesRead>=0)
			{
				buffer[bytesRead]='\0';
				
				switch(buffer[0])
				{
					case HEARTCMD:
					break;
					
					case 0xD0:
					
					//print 10 byte message to stdio
					for(i=1;i<11;i++)
						printf("%d ",buffer[i]);
					printf("\n");
					break;
					
					default:
					printf("Unknown CMD: %c ",buffer[0]);
				}
				heartReceived=1;
				
			}
			else 
			{
				error = tcp_errorlookup(bytesRead);
				printf(error);
			}
				
		break;
		
		
	}
	
	return 0;
}

/*************************************************************************************************************************/
//Given the tcp_error_code - returns the explanation of the error as a string
char* tcp_errorlookup(int tcp_error_code)
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
	

