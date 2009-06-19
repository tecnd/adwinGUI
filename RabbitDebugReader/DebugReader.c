#include <utility.h>
#include <tcpsupp.h>
#include <ansi_c.h>
#include <cvirte.h>		
#include <userint.h>
#include "DebugReader.h"

#include "DebugReader2.h"

#define NUMBERRABBITS 4

#define SERVPORT0 6595	    // not rabbit 0 must connect to the server on port 0, etc
#define SERVPORT1 6596
#define SERVPORT2 6597 
#define SERVPORT3 6598

#define TCP_BUFF  100
#define IP_BUFF_SIZE 20

#define HEARTBEATTIME 30.0       // The interval in s between sending the command which maintains the socket to the rabbit
#define TIMEOUT 600.0			   // The interval as an integral multiple of HEARTBEATTIME which the socket will be kept open without receiving anything
								

//Outgoing Commands 
#define HEARTCMD 0x7E


static int mainpanel;
static int configLoaded;
static unsigned int rabbitHandles[NUMBERRABBITS];
static int rabbitCBD[NUMBERRABBITS];
static int connectionStatus[NUMBERRABBITS];
static int heartCounts[NUMBERRABBITS];    
static int servStatus[NUMBERRABBITS];
static int servPorts[NUMBERRABBITS];


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
}LaserProperties[NUMBERRABBITS];





int main (int argc, char *argv[])
{
	int i;
	
	if (InitCVIRTE (0, argv, 0) == 0)
		return -1;	/* out of memory */
	if ((mainpanel = LoadPanel (0, "DebugReader.uir", MAINPANEL)) < 0)
		return -1;
	
	
	/* Initialization */
	
	servPorts[0]=SERVPORT0;
	servPorts[1]=SERVPORT1;
	servPorts[2]=SERVPORT2;
	servPorts[3]=SERVPORT3;
	
	for(i=0;i<NUMBERRABBITS;i++)
	{
		rabbitCBD[i]=i;
		connectionStatus[i]=0;
		heartCounts[i]=0;
		if((servStatus[i]=RegisterTCPServer (servPorts[i],&TCP_Comm_Callback, &rabbitCBD[i]))<0)     // Load TCP serevrs for each rabbit 
			writeText(i,"Error Opening Server\n",0);
		else
			writeText(i,"Server Loaded\n",1);
	}
	
	configLoaded=0;
	SetCtrlAttribute (mainpanel,MAINPANEL_HBTTIMER,ATTR_INTERVAL,HEARTBEATTIME); 
	
	DisplayPanel (mainpanel);
	RunUserInterface ();
	UnregisterTCPServer (SERVPORT0);
	UnregisterTCPServer (SERVPORT1);
	DiscardPanel (mainpanel);
	return 0;
}
/*************************************************************************************************************************/
int CVICALLBACK Exit_Callback (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
		{
		case EVENT_COMMIT:
			QuitUserInterface (0);
			break;
		}
	return 0;
}
/*************************************************************************************************************************/
int TCP_Comm_Callback(unsigned handle, int xType, int errCode, void *callbackData)
/*  Callback function called when a TCP traffic is received for any sockets opened by NI CVI
	xType is TCP transaction type */

{
	char buffer[TCP_BUFF]; //hard limits max message size
	char ipBuff[IP_BUFF_SIZE];
	int error;
	int bytesRead;
	int maxData;
	int i,j,checkCount,*rabbitNum;
	
	
	rabbitNum=(int*)callbackData;
	
	
	switch (xType)
	{
		case TCP_CONNECT:
		if(GetTCPPeerAddr (handle, ipBuff,20)<0);
			//printf("Error Getting IP\n");
		else 
		{
			j=0;
			checkCount=0;
			while(ipBuff[j]!=NULL&&j<IP_BUFF_SIZE)
			{
				if(LaserProperties[*rabbitNum].IP[j]==ipBuff[j])
					checkCount++;
				j++;
			}
			if(checkCount==j)
			{
				connectionStatus[*rabbitNum]=1;
				rabbitHandles[*rabbitNum]=handle;
				writeText(*rabbitNum,"Connection Established\n",1);
				setLed(*rabbitNum,1);		//turn on connection indicator
			}
			else
			{
				DisconnectTCPClient (handle);	// client disconnected if the IP does not match that expected
			}
		}
		
		break;
		
		
		case TCP_DISCONNECT:
		//tcpDisconnectMode is auto by default
			setLed(*rabbitNum,0);							// Led switched off on disconnect
			connectionStatus[*rabbitNum]=0;		
		break;
		
		case TCP_DATAREADY:

			
			bytesRead=ServerTCPRead (handle, buffer,TCP_BUFF-1,0);
			buffer[bytesRead]='\0';						 	// append null byte
			writeText(*rabbitNum,buffer,1);
			heartCounts[*rabbitNum]=1;
				
		break;
		
		
	}
	
	return 0;
}
/*************************************************************************************************************************/
void setLed(int rabbitNum,int status)
/*  Sets the link led for rabbitNum to on status=1, or off status =0 */
{	
	if (status==1||status==0)
	{
		switch(rabbitNum)
		{
			case 0:
				SetCtrlVal (mainpanel, MAINPANEL_LED0, status);
				break;
			case 1:
				SetCtrlVal (mainpanel, MAINPANEL_LED1, status);
				break;
			case 2:
				SetCtrlVal (mainpanel, MAINPANEL_LED2, status);
				break;
			case 3:
				SetCtrlVal (mainpanel, MAINPANEL_LED3, status);
				break;
		}
	
	}
}
/*************************************************************************************************************************/
void writeText(int rabbitNum,char *txt,int logtext)
/*  Writes the null terminated string txt to one of the 4 txt boxes denoted by rabbitNum 
	Text is logged in logfile if logtext==1 
	HEARTCMD or '~' are parsed out of strings so we dont print them */
{
	char outTxt[TCP_BUFF];
	int i,j;
	int txtLen=strlen(txt);
	
	if(txtLen>0)
	{
		j=0;
		for(i=0;(i<txtLen)&&(i<TCP_BUFF);i++)
		{
			if(txt[i]!=HEARTCMD)
			{
				outTxt[j]=txt[i];
				j++;	
			}
		}
		outTxt[j]='\0';
	
	
		switch(rabbitNum)
		{
			case 0:
				SetCtrlVal (mainpanel, MAINPANEL_LAS0BOX0, outTxt);
				break;
			case 1:
				SetCtrlVal (mainpanel, MAINPANEL_LAS0BOX1, outTxt);
				break;
			case 2:
				SetCtrlVal (mainpanel, MAINPANEL_LAS0BOX2, outTxt);
				break;
			case 3:
				SetCtrlVal (mainpanel, MAINPANEL_LAS0BOX3, outTxt);
				break;
		}
		if(logtext&&strlen(outTxt)>0)
			saveText(rabbitNum,outTxt);	
	}

}
/*************************************************************************************************************************/
void saveText(int rabbitNum,char *txt)  
/*  Writes the null terminated string txt to File MonitorDat.txt */
{
	FILE *sdata;
	char buff1[20]="";
	char extBuff[5]=".txt";
	int i,newfile;
	
	newfile=0;
	
	strcpy (buff1, DateStr ());
	for(i=3;i<7;i++)
		buff1[i]=buff1[i+3];	
	buff1[i]='\0';
	strncat(buff1,extBuff,strlen(extBuff));
	
	if((sdata=fopen(buff1,"r"))==NULL)  	// this means the file does not yet exist so we write header in
	{
		newfile=1;
	}
	else
	{
		fclose (sdata);
	}
	
	if((sdata=fopen(buff1,"a"))==NULL)
	{
		writeText(rabbitNum,"Failed to save text\n",0);
	}
	else
	{
		if(newfile)
		{
			fprintf(sdata,"WOLF Rabbit Monitor Log File - ");
			fprintf(sdata,buff1);
			fprintf(sdata,"\n\nMM-DD-YYYY:HH:MM:SS: LNum. Laser Name: Message Log\n\n");
		
		}
		fprintf (sdata,DateStr());
		fprintf(sdata,":");
		fprintf(sdata,TimeStr());
		fprintf(sdata,": %d. ",rabbitNum);
		fprintf(sdata,LaserProperties[rabbitNum].Name);
		fprintf(sdata,": ");
		fprintf(sdata,txt);
		fclose (sdata);

		
	}
}
/*************************************************************************************************************************/
int CVICALLBACK HBTimerTick (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
/*	Callback function for Timer ticks. Sends heartbeats at the correct interval and moniters rabbit connection to watch 
	for timeouts */

{
	int i,tcpErr;
	char outCMD = HEARTCMD;
	
	switch (event)
		{
		case EVENT_TIMER_TICK:
			
			for(i=0;i<NUMBERRABBITS;i++)
			{
				if(connectionStatus[i])
				{
					//Send heartbeats out
					if(tcpErr=ServerTCPWrite(rabbitHandles[i], &outCMD, sizeof(outCMD),0)<0)   // @@ need to test to see what happens when
					{																	 // rabbit executing (ie a timeout on a write)	
						writeText(i,"TCP Write Error\n",0);
						writeText(i,tcp_errorlookup(tcpErr),0);
					}
				
					if(heartCounts[i]==1) // Heartbeat received, resseting counter,
						heartCounts[i]=0;
					else if ((heartCounts[i]<=0)&&(heartCounts[i]>-TIMEOUT))  //No heartbeat received, TIMEOUT not reached
						heartCounts[i]--;   
					else if (heartCounts[i]<=-TIMEOUT) //No heartbeats resulting in TIMEOUT, closing socket
					{
						writeText(i,"Rabbit Processor not detected.\nClosing Socket...\n",1);
						if (tcpErr=DisconnectTCPClient(rabbitHandles[i])<0)
						{
							writeText(i,"Error Closing Socket\n",0);
							writeText(i,tcp_errorlookup(tcpErr),0);
						}
						else
						{
							//printf("Socket Closed\n");
						}
						connectionStatus[i]=0;
						setLed(i,0);
					}
				}	
			
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
int CVICALLBACK ForceCon (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
/*  Callback for the force connections button
	Connects to the rabbit tcp sock server and sends command to request the rabbit opens a connection with the 
	console's monitoring socket. This is required if the console was off when the rabbits were powered up.*/

{
	unsigned char FORCECMD[1];
	char *error;
	int i,conErr;
	unsigned int tcp_handle;
	
	FORCECMD[0]=0xC6;
	
	switch (event)
		{
		case EVENT_COMMIT:
			if(configLoaded)
			{
				for(i=0;i<NUMBERRABBITS;i++)
				{
					conErr = ConnectToTCPServer (&tcp_handle,LaserProperties[i].Port,LaserProperties[i].IP,&ForceComm_Callback, 0,1000);
					if (conErr==0)
					{
						ClientTCPWrite (tcp_handle, FORCECMD, 1, 0);
						DisconnectFromTCPServer (tcp_handle);
						writeText(i,"Force Command Sent\n",0);
					}
					else if (conErr<0)
					{
						writeText(i,"TCP Connection Error\n",0);
						error=tcp_errorlookup(conErr);
						writeText(i,error,0);
					}
					
				}
			}
			else
				MessagePopup ("Error", "Load Settings Before Forcing a Connection");
			break;
		}
	return 0;
}
/*************************************************************************************************************************/
int ForceComm_Callback(unsigned handle, int xType, int errCode, void *callbackData)                         
/*  Callback function for the ForceCon socket. */
{
	return 0;
}
/*************************************************************************************************************************/
int CVICALLBACK ClearTextBoxes (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	switch (event)
		{
		case EVENT_COMMIT:
				ResetTextBox  (mainpanel, MAINPANEL_LAS0BOX0, "");
				ResetTextBox  (mainpanel, MAINPANEL_LAS0BOX1, "");
				ResetTextBox  (mainpanel, MAINPANEL_LAS0BOX2, "");
				ResetTextBox  (mainpanel, MAINPANEL_LAS0BOX3, "");
			break;
		}
	return 0;
}
/*************************************************************************************************************************/
void CVICALLBACK Load_Settings(int menubar, int menuItem, void *callbackData, int panel)
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
/*************************************************************************************************************************/
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
		MessagePopup("Load Error",buff2);
	}
	else
	{
		fread(LaserProperties,sizeof LaserProperties, 1,ldata);	
		
		
		SetCtrlAttribute (mainpanel, MAINPANEL_LAS0BOX0, ATTR_LABEL_TEXT, LaserProperties[0].Name);
		SetCtrlAttribute (mainpanel, MAINPANEL_LAS0BOX1, ATTR_LABEL_TEXT, LaserProperties[1].Name);
		SetCtrlAttribute (mainpanel, MAINPANEL_LAS0BOX2, ATTR_LABEL_TEXT, LaserProperties[2].Name);
		SetCtrlAttribute (mainpanel, MAINPANEL_LAS0BOX3, ATTR_LABEL_TEXT, LaserProperties[3].Name);
		
		configLoaded=1;
		
	}
}
