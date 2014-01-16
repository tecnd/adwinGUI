#include <ansi_c.h>
#include <tcpsupp.h>
#include <cvirte.h>		
#include <userint.h>
#include "RabbitCom9910.h"
#include "RabbitCom9910_2.h" 

#define PORT 1111
#define RABBIT_IP "10.10.6.100"
#define TCP_BUFF 960  //960 = max # of bytes per data transmission

// Bytes sent to Rabbit to indicate certain functions
#define COMMANDWORD_SEND 0x00
#define COMMANDWORD_CLEAR 0x01
#define COMMANDWORD_SETUP_DDS 0x02
#define COMMANDWORD_FINALIZE 0xff

#define COMMAND_LIST_ROW_LENGTH 15

// Ramp rate will now vary with current version
// to prevent frequency step size from hitting zero
//#define RAMPRATE 1	// Assume a static ramp rate of 1 (fastest possible)

#define REFCLK 1000	// 1 GHz reference clock
//#define REFCLK 50		// 50 MHz reference clock (when PLL is not used)

#define NUMBITS 32	// FTW is 32 bits long

unsigned int tcp_handle;
static int panelHandle;

//int main (int argc, char *argv[])
//{
//	if (InitCVIRTE (0, argv, 0) == 0)
//		return -1;	/* out of memory */
//	if ((panelHandle = LoadPanel (0, "RabbitCom9910.uir", PANEL)) < 0)
//		return -1;
//	DisplayPanel (panelHandle);
//	RunUserInterface ();
//	DiscardPanel (panelHandle);
//	return 0;
//}

// Sends information to Rabbit to prepare a frequency sweep
// This programs in a row in the Rabbit's command list.
// The format of a row is as follows:
// Index 0: Direction of sweep, 1 for sweep from high to low, 0 for sweep from low to high
// Index 1 through 4: Frequency tuning word of the higher of the two frequencies
// Index 5 through 8: Frequency tuning word of the lower of the two frequencies
// Index 9 through 12: Delta frequency word of sweep, set to match time frame of sweep
// Index 13 through 14: Ramp rate of sweeps, set to make sure delta frequency is large enough
int CVICALLBACK Sendfreq (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	double *freqinput1 = malloc(sizeof(double));
	double *freqinput2 = malloc(sizeof(double));
	double *timelength = malloc(sizeof(double));
	double freqword;
	double ramprate;
	int c; 
	unsigned char *commandword = malloc(sizeof(unsigned char));
	unsigned char *commandlistrow = malloc(sizeof(unsigned char)*COMMAND_LIST_ROW_LENGTH);
	unsigned char sweepdir;

	
	int connected;
	char* error;
	
	int tcpErr;
	
	switch (event)
	{
		case EVENT_COMMIT:
			// Get first input frequency
			GetCtrlVal(panel, PANEL_FREQUENCY1, freqinput1);
			// Get second input frequency
			GetCtrlVal(panel, PANEL_FREQUENCY2, freqinput2);
			// Get sweep time
			GetCtrlVal(panel, PANEL_SWEEPTIME, timelength);
			
			// Only set sweep if frequencies are below 40% of the system clock frequency.
			// Time length must be set greater than 0
			if((freqinput1[0] <= (0.4 * REFCLK)) && (freqinput2[0] <= (0.4 * REFCLK)) && timelength[0] > 0)
			{
				// Determine sweep direction (sweep from higher to lower frequency or vice versa)
				sweepdir = freqinput1[0] > freqinput2[0];  // sweepdir = 1 is sweep from high to low
			
				//printf("%d\n", sweepdir);
			
				// Convert first input frequency to FTW
				freqword = freqinput1[0] * pow(2, NUMBITS) / REFCLK;

				// Tuning word list will also indicate to the Rabbit the direction of the sweep
				commandlistrow[0] = sweepdir;
			
			
				// Tuning word list will have higher frequency in indices 1 through 4,
				// and lower frequency in indices 5 through 8.
				
				// Place first input FTW in command list row,
				// breaking the tuning word up into 4 bytes
				for(c = 3; c >= 0; c--)
				{
					commandlistrow[(4 + 4 * (!sweepdir)) - c] = (char)(freqword / pow(2, 8 * c));
					freqword -= (commandlistrow[(4 + 4 *(!sweepdir)) - c] * pow(2, 8 * c));
				}
			
				printf("\n");
			
				// Convert second input frequency to FTW
				freqword = freqinput2[0] * pow(2, NUMBITS) / REFCLK;
				
				// Place second input FTW in command list row,
				// breaking the tuning word up into 4 bytes
				for(c = 3; c >= 0; c--)
				{
					commandlistrow[(4 + 4 * sweepdir) - c] = (char)(freqword / pow(2, 8 * c));
					freqword -= (commandlistrow[(4 + 4 * sweepdir) - c] * pow(2, 8 * c));
				}
				
				// In setting frequency in time step, start with a ramp rate
				// of 1 and work up
				
				ramprate = 0.0;
				freqword = 0.0;

				while(freqword < 10.0)
				{
					// Increase ramp rate until freqword is large enough
					ramprate = ramprate + 1.0;	

					
					// From time length of sweep, calculate an appropriate delta frequency (in MHz)
					// Need to multiply ramp rate by four because the DDS clock operates
					// at a quarter of the frequency of the reference clock.
					freqword = (fabs(freqinput1[0] - freqinput2[0]) * (4 * ramprate)) / (1000 * timelength[0] * REFCLK);
				
					// Convert delta frequency t ramp step size tuning word
					freqword = freqword * pow(2, NUMBITS) / REFCLK;
				}
				
				//printf("\nThe ramp rate is:%f\n\n", ramprate);
				
				// Place ramp step size tuning word into indices 9 through 12  
				// of the command list row, breaking the tuning word up into 4 bytes.
				for(c = 3; c >= 0; c--)
				{	
					commandlistrow[12 - c] = (char)(freqword / pow(2, 8 * c));
					freqword -= (commandlistrow[12 - c] * pow(2, 8 * c));
				}
				
				// Place ramp rate into indices 14 through 13
				// of the command list row, breaking the tuning word up into 2 bytes.
				for(c = 1; c >= 0; c--)
				{	
					commandlistrow[14 - c] = (char)(ramprate / pow(2, 8 * c));
					ramprate -= (commandlistrow[14 - c] * pow(2, 8 * c));
				}
				
				// Print out command list row to be sent, for debugging purposes
				
				for(c = 0; c < COMMAND_LIST_ROW_LENGTH; c++)
				{
					if(c == 0)
					{
						if(commandlistrow[c])
						{
							printf("Sweep from high to low");
						}
						else
						{
							printf("Sweep from low to high");
						}
						printf("\nFrom %f to %f", freqinput1[0], freqinput2[0]);
					}
					else
					{
						if(c == 1)
						{
							printf("\nUpper Limit FTW\n");
						}
						if(c == 5)
						{
							printf("\nLower Limit FTW\n");	
						}
						if(c == 9)
						{
							printf("\nRamp Step Size Word\n");
						}
						if(c == 13)
						{
							printf("\nRamp Rate\n");	
						}
						printf("%x ", commandlistrow[c]);

					}
				}
				printf("\n\n");
				
				
				// Connect to Rabbit
				printf("Connecting...\n");
			
				connected = ConnectToTCPServer (&tcp_handle,PORT,RABBIT_IP,&TCP_Comm_Callback, 0,5000);
				error=GetTCPSystemErrorString ();
				if (connected==0)
				{
					printf("Success!\n");
					ResumeTimerCallbacks();
				}
				else if (connected<0)
				{
					printf("TCP Connection Error: %d\n",connected);
					error=tcp_errorlookup(connected);
					printf(error);
					break;
				
				}

				// Send command word to Rabbit
				// Indicates sending of tuning words
				commandword[0] = COMMANDWORD_SEND;
				ClientTCPWrite(tcp_handle, commandword, 1, 0);
			
				//Send tuning words to Rabbit
				ClientTCPWrite(tcp_handle, commandlistrow,COMMAND_LIST_ROW_LENGTH,0);
			
				//Disconnect from Rabbit
				if (tcpErr=DisconnectFromTCPServer (tcp_handle)<0)
				{
					printf("Error Closing Socket\n");
					printf(tcp_errorlookup(tcpErr));
				}
				else
				{
					printf("Connection Closed\n");
					SuspendTimerCallbacks();
				}
			}
			else if(timelength[0] <= 0)
			{
				printf("Invalid sweep time input\n");					
			}
			else
			{
				printf("Frequency input must be below 0.4 of the frequency of the DDS system clock\n");	
			}
			
			break;
	}
	return 0;
}


// Function that will send a command to the Rabbitcore that tells it 
// to clear out its command list.
// All programmed sweeps will be removed from the Rabbitcore
// and must be reprogrammed.
int CVICALLBACK Clear (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	unsigned char *commandword = malloc(sizeof(unsigned char));
	
	int connected;
	char* error;
	
	int tcpErr;
	
	switch (event)
	{
		case EVENT_COMMIT:
						//Connect to Rabbit
			printf("Connecting...\n");
			
			connected = ConnectToTCPServer (&tcp_handle,PORT,RABBIT_IP,&TCP_Comm_Callback, 0,5000);
			error=GetTCPSystemErrorString ();
			if (connected==0)
			{
				printf("Success!\n");
				ResumeTimerCallbacks();
			}
			else if (connected<0)
			{
				printf("TCP Connection Error: %d\n",connected);
				error=tcp_errorlookup(connected);
				printf(error);
				break;
			}

			// Send command word to Rabbit
			// Causes Rabbit to clear its command list
			commandword[0] = COMMANDWORD_CLEAR;
			ClientTCPWrite(tcp_handle, commandword, 1, 0);
			
			//Disconnect from Rabbit
			if (tcpErr=DisconnectFromTCPServer (tcp_handle)<0)
			{
				printf("Error Closing Socket\n");
				printf(tcp_errorlookup(tcpErr));
			}
			else
			{
				printf("Connection Closed\n");
				SuspendTimerCallbacks();
			}
			break;
	}
	return 0;
}

// Function that will send a command to the Rabbitcore that tells it 
// to send DDS initial setup commands.
int CVICALLBACK SetupDDS (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	unsigned char *commandword = malloc(sizeof(unsigned char));
	
	int connected;
	char* error;
	
	int tcpErr;
	
	switch (event)
	{
		case EVENT_COMMIT:
			//Connect to Rabbit
			printf("Connecting...\n");
			
			connected = ConnectToTCPServer (&tcp_handle,PORT,RABBIT_IP,&TCP_Comm_Callback, 0,5000);
			error=GetTCPSystemErrorString ();
			if (connected==0)
			{
				printf("Success!\n");
				ResumeTimerCallbacks();
			}
			else if (connected<0)
			{
				printf("TCP Connection Error: %d\n",connected);
				error=tcp_errorlookup(connected);
				printf(error);
				break;
			}

			// Send command word to Rabbit
			// Causes Rabbit to stop listening for a connection
			commandword[0] = COMMANDWORD_SETUP_DDS;
			ClientTCPWrite(tcp_handle, commandword, 1, 0);
			
			//Disconnect from Rabbit
			if (tcpErr=DisconnectFromTCPServer (tcp_handle)<0)
			{
				printf("Error Closing Socket\n");
				printf(tcp_errorlookup(tcpErr));
			}
			else
			{
				printf("Connection Closed\n");
				SuspendTimerCallbacks();
			}
			break;
	}
	return 0;
}

// Function that will send a command to the Rabbitcore that tells it 
// to stop listening for a connection.
int CVICALLBACK Finalize (int panel, int control, int event,
		void *callbackData, int eventData1, int eventData2)
{
	unsigned char *commandword = malloc(sizeof(unsigned char));
	
	int connected;
	char* error;
	
	int tcpErr;
	
	switch (event)
	{
		case EVENT_COMMIT:
			//Connect to Rabbit
			printf("Connecting...\n");
			
			connected = ConnectToTCPServer (&tcp_handle,PORT,RABBIT_IP,&TCP_Comm_Callback, 0,5000);
			error=GetTCPSystemErrorString ();
			if (connected==0)
			{
				printf("Success!\n");
				ResumeTimerCallbacks();
			}
			else if (connected<0)
			{
				printf("TCP Connection Error: %d\n",connected);
				error=tcp_errorlookup(connected);
				printf(error);
				break;
				
			}

			// Send command word to Rabbit
			// Causes Rabbit to stop listening for a connection
			commandword[0] = COMMANDWORD_FINALIZE;
			ClientTCPWrite(tcp_handle, commandword, 1, 0);
			
			//Disconnect from Rabbit
			if (tcpErr=DisconnectFromTCPServer (tcp_handle)<0)
			{
				printf("Error Closing Socket\n");
				printf(tcp_errorlookup(tcpErr));
			}
			else
			{
				printf("Connection Closed\n");
				SuspendTimerCallbacks();
			}
			break;
	}
	return 0;
}

// Quits the interface
int CVICALLBACK Shutdown (int panel, int control, int event,
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

// I have no interest in reading data from the Rabbit for now,
// which, as I understand things, is the purpose of this required callback function.
// Thus, the function does nothing.
int TCP_Comm_Callback(unsigned handle, int xType, int errCode, void *callbackData)
{
	return 0;
}

// Function to look up the meaning of returned error codes.
// Returns a string describing the error.
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
