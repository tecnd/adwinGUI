// Modified by Seth Aubin on August 2, 2010
//  change to the "ADbasic binary file: "TransferData_August02_2010.TB1"
//  purpose: activate DIO 1 and DIO 2 outputs.

#include "ScanTableLoader.h"

//To DO:  add more DDS 'clips' for copy/paste routines

//2006
//March 9:  Reorder the routines to more closely match the order in which they are executed.
//          Applies to the 'engine' but not the cosmetic/table handling routnes

/*********************** Sandro Gvakharia October 2010 **********************/
//Added DDS vars starting at line 21, added DDS code starting at line 462.
//Works with DDS1, Rb Evaporation
/*********************** Sandro Gvakharia October 2010 **********************/

#include <utility.h>
#include "Scan.h"
#include <userint.h>
#include "GUIDesign.h"
#include "GUIDesign2.h"
#include "AnalogSettings2.h"
#include "DigitalSettings2.h"
/***** Sandro Gvakharia, October 2010 *****/
#include <tcpsupp.h>
#define PORT 1111
#define TCP_BUFF 960 //960 = max # of bytes per data transmission

// Ramp rate will now vary with current version
// to prevent frequency step size from hitting zero
//#define RAMPRATE 1	// Assume a static ramp rate of 1 (fastest possible)

#define REFCLK 1000 // 1 GHz reference clock
//#define REFCLK 50		// 50 MHz reference clock (when PLL is not used)

#define NUMBITS 32 // FTW is 32 bits long

/*****************************************/

//Clipboard for copy/paste functions
double TimeClip;
int ClipColumn = -1;
struct AnalogTableClip
{
	int fcn;	   //fcn is an integer refering to a function to use.
				   // 0-step, 1-linear, 2- exp, 3- 'S' curve, 4 sine wave
	double fval;   //the final value
	double tscale; //the timescale to approach final value
} AnalogClip[NUMBERANALOGCHANNELS + 1];
int DigClip[NUMBERDIGITALCHANNELS + 1];
ddsoptions_struct ddsclip, dds2clip, dds3clip;

extern int Active_DDS_PANEL;

//***************************************************************************************************
//Executed when the "RUN" button is pressed.
//Disables scanning capability, saves the GUI  in C:/LastGui.pan    Just in case of a crash
//Activates the TIMER if necessary.
//Starts the routine to generate new data for ADwin
int CVICALLBACK CMD_RUN_CALLBACK(int panel, int control, int event,
								 void *callbackData, int eventData1, int eventData2)
{
	int repeat = 0;
	switch (event)
	{
	case EVENT_COMMIT:
		PScan.Scan_Active = FALSE;
		SaveLastGuiSettings();
		ChangedVals = TRUE; // Forces the BuildUpdateList() routine to generate new data for the ADwin
		scancount = 0;
		GetCtrlVal(panelHandle, PANEL_TOGGLEREPEAT, &repeat); // reads the state of the "repeat" switch
		if (repeat == TRUE)
		{
			SetCtrlAttribute(panelHandle, PANEL_TIMER, ATTR_ENABLED, 1);
			//activate timer:  calls TIMER_CALLBACK to restart the RunOnce commands after a set time.
		}
		else
		{
			SetCtrlAttribute(panelHandle, PANEL_TIMER, ATTR_ENABLED, 0);
			//deactivate timer
		}
		RunOnce(); // starts the routine to build the ADwin data.
		break;
	}

	return 0;
}
//*********************************************************************************
//pretty much a copy of the CMD_RUN routine, but activates the SCAN flag and resets the scan counter
// could be integrated into the CMD_RUN routine.... but this works
int CVICALLBACK CMD_SCAN_CALLBACK(int panel, int control, int event,
								  void *callbackData, int eventData1, int eventData2)
{
	int repeat = 0;
	switch (event)
	{
	case EVENT_COMMIT:
		UpdateScanValue(TRUE); // sending value of 1 resets the scan counter.
		PScan.Scan_Active = TRUE;
		SaveLastGuiSettings();
		ChangedVals = TRUE;
		repeat = TRUE;
		SetCtrlVal(panelHandle, PANEL_TOGGLEREPEAT, repeat);		 //sets "repeat" button to active
		SetCtrlAttribute(panelHandle, PANEL_TIMER, ATTR_ENABLED, 1); //Turn on the timer
		RunOnce();													 // starts the routine to build the ADwin data.
		break;
	}

	return 0;
}

//*********************************************************************************************************
//TIMER is a CVI object.  Whenever its countdown reaches 0, it executes the following code.
//TIMER is activated in the CMD_RUN, CMD_SCAN and BUILDUPDATELIST routines.
//it gets de-activated in this routine, and when we hit CMD_STOP
int CVICALLBACK TIMER_CALLBACK(int panel, int control, int event,
							   void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
	case EVENT_TIMER_TICK:
		SetCtrlAttribute(panelHandle, PANEL_TIMER, ATTR_ENABLED, FALSE);
		//disable timer and re-enable in the runonce (or update list) loop, if the repeat butn is pressed.
		//reset the timer too and set a timer time of 50ms?
		if (PScan.Scan_Active == TRUE)
		{
			UpdateScanValue(FALSE);
		}
		RunOnce();

		break;
	}
	return 0;
}
//*********************************************************************************************************
//By: Stefan Myrskog
//Turns off the TIMER object, and turns off the "repeat" button
//Lets the ADwin finish its current program.  Interrupting the program partway can be bad for the
//equipment as the variables are not cleared in memory and updates get out of sync.
//Edited: Aug8, 2005
//Change:  Force all panel controls related to scanning to hide.
int CVICALLBACK CMDSTOP_CALLBACK(int panel, int control, int event,
								 void *callbackData, int eventData1, int eventData2)
{
	SetCtrlAttribute(panelHandle_sub2, SUBPANEL2, ATTR_VISIBLE, 0); // hide the SCAN display panel
	switch (event)
	{
	case EVENT_COMMIT:
		SetCtrlAttribute(panelHandle, PANEL_TIMER, ATTR_ENABLED, 0);
		SetCtrlVal(panelHandle, PANEL_TOGGLEREPEAT, 0);
		//check to see if we need to export a Scan history
		if (PScan.Scan_Active == TRUE)
		{
			ExportScanBuffer();
		}
		break;
	}
	return 0;
}
//**************************************************************************************************
void RunOnce(void)
//Converts the 10 'pages' (3D array) into single 2D array 'metatables'
//Ignores dimmed out columns and pages
{

	//could/should change these following defs and use malloc instead, but they should never exceed.. 170 or so.
	double MetaTimeArray[500];
	int MetaDigitalArray[NUMBERDIGITALCHANNELS + 1][500];
	struct AnVals MetaAnalogArray[NUMBERANALOGCHANNELS + 1][500];
	ddsoptions_struct MetaDDSArray[500];
	ddsoptions_struct MetaDDS2Array[500];
	dds3options_struct MetaDDS3Array[500];
	int i, j, k, mindex, tsize;
	int insertpage, insertcolumn, x, y, lastpagenum, FinalPageToUse;
	BOOL nozerofound_2;

	isdimmed = TRUE;
	lastpagenum = 10;
	// Find the position of the imaging panel (last panel on the GUI), this panel can be positioned inside other panels
	GetCtrlVal(panelHandle, PANEL_NUM_INSERTIONPAGE, &insertpage);
	GetCtrlVal(panelHandle, PANEL_NUM_INSERTIONCOL, &insertcolumn);

	//Lets build the times list first...so we know how long it will be.
	//check each page...find columns in use and dim out unused....(with 0 or negative time values)
	SetCtrlAttribute(panelHandle, PANEL_ANALOGTABLE, ATTR_TABLE_MODE, VAL_COLUMN);
	mindex = 0;
	FinalPageToUse = NUMBEROFPAGES - 2; //issues with '0 or 1 indexing'.  If there are 10 pages, we end at 9.
	//Page 10(imaging) handled separately.

	if (insertpage == NUMBEROFPAGES - 1) //if the imaging page is set for 10, then set the final page as 10.
	{
		FinalPageToUse++;
	}
	//go through for each page
	for (k = 1; k <= FinalPageToUse; k++) // numberofpages-1 because last page is 'mobile'
	{
		if (ischecked[k] == 1) //if the page is selected (checkbox is checked)
		{
			BOOL nozerofound = TRUE;
			//go through for each time column
			for (i = 1; i <= NUMBEROFCOLUMNS; i++)
			{
				if ((i == insertcolumn) && (k == insertpage) && (k != NUMBEROFPAGES - 1))
				{
					nozerofound_2 = TRUE;
					if (ischecked[lastpagenum] == 1)
					{
						for (x = 1; x <= NUMBEROFCOLUMNS; x++)
						{
							if ((nozerofound == TRUE) && (nozerofound_2 == TRUE) && (TimeArray[x][lastpagenum] > 0))
							{
								mindex++;
								MetaTimeArray[mindex] = TimeArray[x][lastpagenum];
								for (y = 1; y <= NUMBERANALOGCHANNELS; y++)
								{
									//Sets MetaArray with appropriate fcn val when "same" selected
									if (AnalogTable[x][y][lastpagenum].fcn == 6)
									{
										MetaAnalogArray[y][mindex].fcn = 1; //on the right for repeat function, currently set to hold val //MetaAnalogArray[y][mindex].fcn=MetaAnalogArray[y][mindex-1].fcn;
										MetaAnalogArray[y][mindex].fval = MetaAnalogArray[y][mindex - 1].fval;
										MetaAnalogArray[y][mindex].tscale = MetaAnalogArray[y][mindex - 1].tscale;
									}
									else
									{
										MetaAnalogArray[y][mindex].fcn = AnalogTable[x][y][lastpagenum].fcn;
										MetaAnalogArray[y][mindex].fval = AnalogTable[x][y][lastpagenum].fval;
										MetaAnalogArray[y][mindex].tscale = AnalogTable[x][y][lastpagenum].tscale;
									}
								}
								for (y = 1; y <= NUMBERDIGITALCHANNELS; y++)
								{
									MetaDigitalArray[y][mindex] = DigTableValues[x][y][lastpagenum];
								}
								MetaDDSArray[mindex] = ddstable[x][lastpagenum];
								MetaDDS2Array[mindex] = dds2table[x][lastpagenum];
								MetaDDS3Array[mindex] = dds3table[x][lastpagenum];
							}
							else if (TimeArray[x][lastpagenum] == 0)
							{
								nozerofound_2 = 0;
							}
						}
					}
				}
				/*end A */

				if ((nozerofound == 1) && (TimeArray[i][k] > 0))
				//ignore all columns after the first
				// time 0 (for that page)
				{
					mindex++; //increase the number of columns counter
					MetaTimeArray[mindex] = TimeArray[i][k];

					//go through for each analog channel
					for (j = 1; j <= NUMBERANALOGCHANNELS; j++)
					{
						//Sets MetaArray with appropriate fcn val when "same" selected
						if (AnalogTable[i][j][k].fcn == 6)
						{
							MetaAnalogArray[j][mindex].fcn = 1; //on the right for repeat function, currently set to hold val ////MetaAnalogArray[j][mindex].fcn=MetaAnalogArray[j][mindex-1].fcn;
							MetaAnalogArray[j][mindex].fval = MetaAnalogArray[j][mindex - 1].fval;
							MetaAnalogArray[j][mindex].tscale = MetaAnalogArray[j][mindex - 1].tscale;
						}
						else
						{
							MetaAnalogArray[j][mindex].fcn = AnalogTable[i][j][k].fcn;
							MetaAnalogArray[j][mindex].fval = AnalogTable[i][j][k].fval;
							MetaAnalogArray[j][mindex].tscale = AnalogTable[i][j][k].tscale;
						}
					}
					for (j = 1; j <= NUMBERDIGITALCHANNELS; j++)
					{
						MetaDigitalArray[j][mindex] = DigTableValues[i][j][k];
					}
					/* ddsoptions_struct contains floats and ints, so shallow copy is ok */
					MetaDDSArray[mindex] = ddstable[i][k];
					MetaDDS2Array[mindex] = dds2table[i][k];
					MetaDDS3Array[mindex] = dds3table[i][k];
					MetaDDSArray[mindex].delta_time = TimeArray[i][k] / 1000;
					MetaDDS2Array[mindex].delta_time = TimeArray[i][k] / 1000;
					MetaDDS3Array[mindex].delta_time = TimeArray[i][k] / 1000;
					// Don't know why the preceding line is necessary...
					// But the time information wasn't always copied in... or it was erased elsewhere
				}
				else if (TimeArray[i][k] == 0)
				{
					nozerofound = FALSE;
				}
			}
		}
	}
	isdimmed = TRUE;

	DrawNewTable(TRUE);
	tsize = mindex; //tsize is the number of columns

	//Prints MetaIndexes for testting only

	/*	printf("UPDATE TIMES\t");
	for(i=1;i<=mindex;i++)
		printf("%0.1f\t",MetaTimeArray[i]);
	printf("\nANALOG CHANNEL LINE\n");
	for(j=1;j<=NUMBERANALOGCHANNELS;j++)
	{
		printf("\nCHANNEL LINE #%d\t",j);
		for (i=1;i<=mindex;i++)
			printf("%0.0f\t",MetaAnalogArray[j][i].fval);
		printf("\n\t\tFNC#\t");
		for (i=1;i<=mindex;i++)
			printf("%d\t",MetaAnalogArray[j][i].fcn);
	}

	printf("\n\nDIG CHANNEL LINEs\n");
	for(j=1;j<=NUMBERDIGITALCHANNELS;j++)
	{
		printf("\nCHANNEL LINE #%d\t",j);
		for (i=1;i<=mindex;i++)
			printf("%0.0f\t",MetaDigitalArray[j][i]);

	}

	printf("\n\nDDS1 LINE\nEndFreq:\t");
	for (i=1;i<=mindex;i++)
			printf("%0.0f\t",MetaDDSArray[i].end_frequency);
	printf("\nStartFreq:\t");
	for (i=1;i<=mindex;i++)
			printf("%0.0f\t",MetaDDSArray[i].start_frequency);
	printf("\nis_stop:\t");
	for (i=1;i<=mindex;i++)
			printf("%d\t",MetaDDSArray[i].is_stop);
	printf("\nAmplitude:\t");
	for (i=1;i<=mindex;i++)
			printf("%0.0f\t",MetaDDSArray[i].amplitude);
	printf("\nDelta Time:\t");								  //At this Point all the delta time vals are zero (all conditions)
	for (i=1;i<=mindex;i++)									  //Not being used???
			printf("%0.0f\t",MetaDDSArray[i].delta_time);*/

	// Send the new arrays to BuildUpdateList()
	BuildUpdateList(MetaTimeArray, MetaAnalogArray, MetaDigitalArray, MetaDDSArray, MetaDDS2Array, MetaDDS3Array, tsize);
}
//*****************************************************************************************
void BuildUpdateList(double TMatrix[], struct AnVals AMat[NUMBERANALOGCHANNELS + 1][500], int DMat[NUMBERDIGITALCHANNELS + 1][500],
					 ddsoptions_struct DDSArray[500], ddsoptions_struct DDS2Array[500], dds3options_struct DDS3Array[500], int numtimes)
{
	/*

	TMatrix[update period#] -- stores the interval time of each column
	AMat[channel#][update period#] -- stores info located in the analog table
	DMat[channel#][update period#] --
	DDS -- note is_stop=1 means DDS OFF

	all the above have 500 update period elements note that valid elements are base1

	numtimes = the actual number of valid update period elements.


	Generate the data that is sent to the ADwin and sends the data.
	From the meta-lists, we generate 3 arrays.
	UpdateNum - each entry is the number of channel updates that we perform during the ADwin EVENT, where an
				ADwin event is an update cycle, i.e. 10 microseconds, 100 microseconds... etc.  We advance  through this
				array once per ADwin Event.  UpdateNum controls how fast we scan through ChNum and ChVal
	ChNum - 	An array that contains the channel number to be updated. Synchronous with ChVal.  	   Channels listed below
	ChVal -		An array that contains the value to be written to a channel. Synchronous with ChVal.

	ChNum -     Value 1-32:  Analog lines, 4 cards with 8 lines each.  ChVal is -10V to 10V
				Value 51:	 DDS1 line.   ChVal is either a 2-bit value (0-3) to write, or (4-7) a reset signal
				Value 52:	 DDS2 line.   ChVal is either a 2-bit value (0-3) to write, or (4-7) a reset signal
				Value 101, 102  First 16 and last 16 lines on the first DIO card.  ChVal is a 16 bit integer
				Value 103, 104	First 16 and last 16 lines on the second DIO card. ChVal is a 16 bit integer

    Mar 09_2006:Added ChNum 201,202   These  are codes to enable/disable looping.
                Corresponding ChVal is the number of loops.
	dds_cmd_seq List of dds commands, parsed into 2-bit sections, or reset lines to be written
				Commands are listed along with the time they should occur at.
	*/
	BOOL UseCompression, ArraysToDebug, StreamSettings;
	int *UpdateNum;
	int *ChNum;
	float *ChVal;
	int NewTimeMat[500];
	int nupcurrent, nuptotal = 0, checkresettozero = 0;
	int usefcn, digchannel; //Bool
	unsigned int digval, digval2, digval3, digval4, LastDVal = 0, LastDVal2 = 0;
	int UsingFcns[NUMBERANALOGCHANNELS + 1] = {0}, count = 0;
	double LastAval[NUMBERANALOGCHANNELS + 1] = {0}, NewAval, TempChVal, TempChVal2;
	long ResetToZeroAtEnd[NUMBERANALOGCHANNELS + 6]; //1-24 for analog, ...but for now, if [1]=1 then all zero, else no change
	int timemult, t, c;
	double cycletime = 0;
	int GlobalDelay = 300000; //Updated July 14 2009 - Ben Sofka (303030 corresponds to 1ms on T11 processor)
	char buff[100];
	int repeat = 0, timesum = 0;
	static int didboot = 0;
	static int didprocess = 0;
	int memused;
	int timeused;
	int tmp_dds;
	dds_cmds_ptr dds_cmd_seq = NULL;
	double DDSoffset = 0.0, DDS2offset = 0.0, DDS3offset = 0.0;
	int digchannelsum;
	int newcount;
	// variables for timechannel optimization
	int start_offset = 0;
	time_t tstart, tstop;

	//Change run button appearance while operating
	SetCtrlAttribute(panelHandle, PANEL_CMD_RUN, ATTR_CMD_BUTTON_COLOR, VAL_GREEN);
	tstart = clock();																												   // Timing information for debugging purposes
	timemult = (int)(1 / EventPeriod);																								   //number of adwin upates per ms
	GlobalDelay = (int)(EventPeriod / 0.00000333333333333333333333333333333333333333333333333333333333333333333333333333333333333333); // AdwintTick=0.0000033ms=AW clock cycle (Gives #of clock cycles/update) Updated July 2 2009 - Ben Sofka

	//make a new time list...converting the TimeTable from milliseconds to number of events (numtimes=total #of columns)
	for (int i = 1; i <= numtimes; i++)
	{
		NewTimeMat[i] = (int)(TMatrix[i] * timemult); //number of Adwin events in column i
		timesum = timesum + NewTimeMat[i];			  //total number of Adwin events
	}

	cycletime = (double)timesum / (double)timemult / 1000; // Total duration of the cycle, in seconds

	sprintf(buff, "timesum %d", timesum); // Print more debug info
	InsertListItem(panelHandle, PANEL_DEBUG, -1, buff, 1);

	if (ChangedVals == TRUE) //reupdate the ADWIN array if the user values have changed
	{
		/* Update the array of DDS commands
		EventPeriod is in ms, create_command_array in s, so convert units */
		GetMenuBarAttribute(menuHandle, MENU_PREFS_SIMPLETIMING, ATTR_CHECKED, &UseSimpleTiming);
		GetCtrlVal(panelHandle, PANEL_NUM_DDS_OFFSET, &DDSoffset);
		GetCtrlVal(panelHandle, PANEL_NUM_DDS2_OFFSET, &DDS2offset);
		GetCtrlVal(panelHandle, PANEL_NUM_DDS3_OFFSET, &DDS3offset);
		// read offsets to add to DDSArray

		dds_cmd_seq = create_ad9852_cmd_sequence(DDSArray, numtimes, DDSFreq.PLLmult,
												 DDSFreq.extclock, EventPeriod / 1000);
		// again, uncomment as needed
		/*  dds_cmd_seq_AD9858 = create_ad9858_cmd_sequence(DDS2Array, numtimes,DDS2_CLOCK,
		EventPeriod/1000,0);	   // assume frequency offset of 0 MHz
	    */
		/*dds_cmd_seq = create_ad9852_cmd_sequence(DDS3Array, numtimes,DDSFreq.PLLmult,
		DDSFreq.extclock,EventPeriod/1000);
   	    */
		//dynamically allocate the memory for the time array (instead of using a static array:UpdateNum)
		//We are making an assumption about how many programmable points we may need to use.
		//For now assume that number of channel updates <= 4* #of events, serious overestimate

		UpdateNum = calloc((int)((double)timesum * 1.2), sizeof(int));
		if (!UpdateNum)
		{
			exit(1);
		}
		ChNum = calloc((int)((double)timesum * 4), sizeof(int));
		if (!ChNum)
		{
			exit(1);
		}
		ChVal = calloc((int)((double)timesum * 4), sizeof(double));
		if (!ChVal)
		{
			exit(1);
		}

		//Go through for each column that needs to be updated

		//Important Variables:
		//count: Number of Adwin events until the current position
		//nupcurrent: number of updates for the current Adwin event
		//nuptotal: current position in the channel/value column
		for (int i = 1; i <= numtimes; i++)
		{
			// find out how many channels need updating this round...
			// if it's a non-step fcn, then keep a list of UsingFcns, and change it now
			nupcurrent = 0;
			usefcn = 0;

			// scan over the analog channel..find updated values by comparing to old values.
			for (int j = 1; j <= NUMBERANALOGCHANNELS; j++)
			{
				LastAval[j] = -99;
				if (AMat[j][i].fval != AMat[j][i - 1].fval)
				{
					nupcurrent++;
					nuptotal++;
					ChNum[nuptotal] = AChName[j].chnum;
					NewAval = CalcFcnValue(AMat[j][i].fcn, AMat[j][i - 1].fval, AMat[j][i].fval, AMat[j][i].tscale, 0.0, TMatrix[i]);

					TempChVal = AChName[j].tbias + NewAval * AChName[j].tfcn;
					ChVal[nuptotal] = CheckIfWithinLimits(TempChVal, j);

					if (AMat[j][i].fcn != 1)
					{
						usefcn++;
						UsingFcns[usefcn] = j; // mark these lines for special attention..more complex
					}
				}
			} //done scanning the analog values.
			//*********now the digital value
			digval = 0;
			digval2 = 0;
			digval3 = 0;
			digval4 = 0;
			for (int k = 1; k <= NUMBERDIGITALCHANNELS; k++)
			{
				digchannel = DChName[k].chnum;

				if (digchannel <= 32)
				{
					digval = digval + DMat[k][i] * pow(2, DChName[k].chnum - 1);
				}
				if ((digchannel >= 101) && (digchannel <= 132))
				{
					digval2 = digval2 + DMat[k][i] * pow(2, (DChName[k].chnum - 100) - 1);
				}
				/*
				if(digchannel<=16)
				{
					digval=digval+DMat[k][i]*pow(2,DChName[k].chnum-1);
				}
				if((digchannel>=17)&&(digchannel<=32))
				{
					digval2=digval2+DMat[k][i]*pow(2,(DChName[k].chnum-16)-1);
				}
				if((digchannel>=101)&&(digchannel<=116))// starting on card 2.  Channels run from 101-132 (1-32)
				{
					digval3=digval3+DMat[k][i]*pow(2,(DChName[k].chnum-100)-1);
				}
				if((digchannel>=117)&&(digchannel<=132))
				{
					digval4=digval4+DMat[k][i]*pow(2,(DChName[k].chnum-116)-1);
				}
*/
			} // finished computing current digital data

			if (digval != LastDVal)
			{
				nupcurrent++;
				nuptotal++;
				ChNum[nuptotal] = 101;
				ChVal[nuptotal] = digval;
			}
			LastDVal = digval;
			if (digval2 != LastDVal2)
			{
				nupcurrent++;
				nuptotal++;
				ChNum[nuptotal] = 102;
				ChVal[nuptotal] = digval2;
			}
			LastDVal2 = digval2;

			/*			if(!(digval3==LastDVal3))
			{
				nupcurrent++;
				nuptotal++;
				ChNum[nuptotal]=103;
				ChVal[nuptotal]=digval3;
			}
			LastDVal3=digval3;
			if(!(digval4==LastDVal4))
			{
				nupcurrent++;
				nuptotal++;
				ChNum[nuptotal]=104;
				ChVal[nuptotal]=digval4;
			}
			LastDVal4=digval4;
*/
			count++;
			UpdateNum[count] = nupcurrent;

			//end of first scan
			//now do the remainder of the loop...but just the complicated fcns, i.e. ramps, sine wave

			t = 0;
			while (t < NewTimeMat[i] - 1)
			{
				t++;
				k = 0;
				nupcurrent = 0;
				//look for a new DDS command, start_offset=0
				tmp_dds = get_dds_cmd(dds_cmd_seq, count - 1 - start_offset); //dds translator(zero base) runs 1 behind this counter
				if (tmp_dds >= 0)
				{
					nupcurrent++;
					nuptotal++;
					ChNum[nuptotal] = 51; //DDS1 dummy channel
					ChVal[nuptotal] = tmp_dds;

				} //done the DDS1

				/*		tmp_dds = get_dds_cmd(dds_cmd_seq_AD9858, count-1-start_offset);  //dds translator(zero base) runs 1 behind this counter
				if (tmp_dds>=0)
				{
					nupcurrent++;
					nuptotal++;
					ChNum[nuptotal] = 52; //dummy channel
					ChVal[nuptotal] = tmp_dds;

				} //done the DDS2
		*/

				while (k < usefcn)
				{
					k++;
					c = UsingFcns[k];
					NewAval = CalcFcnValue(AMat[c][i].fcn, AMat[c][i - 1].fval, AMat[c][i].fval, AMat[c][i].tscale, t, TMatrix[i]);
					TempChVal = AChName[c].tbias + NewAval * AChName[c].tfcn;
					TempChVal2 = CheckIfWithinLimits(TempChVal, c);
					if (fabs(TempChVal2 - LastAval[k]) > 0.0003) // only update if the ADwin will output a new value.
					// ADwin is 16 bit, +/-10 V, to 1 bit resolution implies dV=20V/2^16 ~=  0.3mV
					{
						nupcurrent++;
						nuptotal++;
						ChNum[nuptotal] = AChName[c].chnum;
						ChVal[nuptotal] = AChName[c].tbias + NewAval * AChName[c].tfcn;
						LastAval[k] = TempChVal2;
					}
				}
				count++;
				UpdateNum[count] = nupcurrent;
			} //Done this element of the TMatrix
		}	  //done scanning over times array

		//Find the largest of the arrays
		//		bigger=count;
		//		if(nuptotal>bigger) {bigger=nuptotal;}

		// read some menu options
		GetMenuBarAttribute(menuHandle, MENU_PREFS_COMPRESSION, ATTR_CHECKED, &UseCompression);
		GetMenuBarAttribute(menuHandle, MENU_PREFS_SHOWARRAY, ATTR_CHECKED, &ArraysToDebug);
		newcount = 0;

		if (UseCompression)
		{
			OptimizeTimeLoop(UpdateNum, count, &newcount);
		}

		if (ArraysToDebug) // only used if we suspect the ADwin code of being really faulty.
		{				   // ArraysToDebug is a declare in vars.h
			//imax=newcount;
			//if(newcount==0) {imax=count;}
			//if(imax>1000){imax=1000;}
			//for(i=1;i<imax+1;i++)

			// this loop just writes out the first 1000 updates to the stdio window in the format
			// i  UpdateNum    ChNum   Chval
			int k = 1;
			for (int i = 1; i < 1000; i++)
			{
				printf("%-5d%d\t", i, UpdateNum[i]);
				if (UpdateNum[i] > 0)
				{
					printf("%d\t%f\n", ChNum[k], ChVal[k]);
					k++;
					for (int j = 1; j < UpdateNum[i]; j++)
					{
						printf("\t\t%d\t%f\n", ChNum[k], ChVal[k]);
						k++;
					}
				}
				else
				{
					printf("\n");
				}
			}
		}

		tstop = clock();

#ifdef PRINT_TO_DEBUG
		sprintf(buff, "count %d", count);
		InsertListItem(panelHandle, PANEL_DEBUG, -1, buff, 1);
		sprintf(buff, "compressed count %d", newcount);
		InsertListItem(panelHandle, PANEL_DEBUG, -1, buff, 1);
		sprintf(buff, "updates %d", nuptotal);
		InsertListItem(panelHandle, PANEL_DEBUG, -1, buff, 1);
		sprintf(buff, "time to generate arrays:   %d", tstop - tstart);
		InsertListItem(panelHandle, PANEL_DEBUG, -1, buff, 1);

#endif

		if (didboot == FALSE) // is the ADwin booted?  if not, then boot
		{
			Boot("C:\\ADWIN\\ADWIN11.BTL", 0); //Updated July 14 2009 - Ben Sofka
			didboot = 1;
		}
		if (didprocess == FALSE) // is the ADwin process already loaded?
		{
			processnum = Load_Process("TransferData_May25_2012.TB1"); //Updated May 25, 2012 - Seth Aubin
			didprocess = 1;
		}

		//Commented out by Dave June 6, 2006. Reason: repeated few lines down? (access of -1 index?)
		//for(p-1;p<=NUMBERANALOGCHANNELS;p++) {ResetToZeroAtEnd[p]=AChName[p].resettozero;}

		if (UseCompression)
		{
			SetPar(1, newcount); //Let ADwin know how many counts (read as Events) we will be using.
			SetData_Long(1, UpdateNum, 1, newcount + 1);
		}
		else
		{
			SetPar(1, count); //Let ADwin know how many counts (read as Events) we will be using.
			SetData_Long(1, UpdateNum, 1, count + 1);
		}

		// Send the Array to the AdWin Sequencer
		// ------------------------------------------------------------------------------------------------------------------
		SetPar(2, GlobalDelay);
		SetData_Long(2, ChNum, 1, nuptotal + 1);
		SetData_Float(3, ChVal, 1, nuptotal + 1);
		// ------------------------------------------------------------------------------------------------------------------

		// determine if we should reset values to zero after a cycle
		GetMenuBarAttribute(menuHandle, MENU_SETTINGS_RESETZERO, ATTR_CHECKED, &checkresettozero);
		for (i = 1; i <= NUMBERANALOGCHANNELS; i++)
		{
			ResetToZeroAtEnd[i - 1] = AChName[i].resettozero;
			//			printf("Analog Chn = %d, Reset-to-zero = %d \n", i, ResetToZeroAtEnd[i-1]);
		}
		digchannelsum = 0;
		for (i = 1; i <= 16; i++)
		{
			if (DChName[i].resettolow == TRUE)
			{
				digchannelsum += 2 ^ (i - 1);
			}
		}
		//		ResetToZeroAtEnd[101]=digchannelsum;// lower 16 digital channels
		for (i = 17; i <= NUMBERDIGITALCHANNELS; i++)
			digchannelsum = 0;
		{
			if (DChName[i].resettolow == TRUE)
			{
				digchannelsum += 2 ^ (i - 17);
			}
		}
		//		ResetToZeroAtEnd[102]=digchannelsum;// digital channels 17-24
		//ResetToZeroAtEnd[25]=0;// lower 16 digital channels
		//ResetToZeroAtEnd[26]=0;// digital channels 17-24
		//		ResetToZeroAtEnd[27]=0;// master override....if ==1 then reset none
		//		if(checkresettozero==0) { ResetToZeroAtEnd[27]=1;}

		SetData_Long(4, ResetToZeroAtEnd, 1, NUMBERANALOGCHANNELS);
		// done evaluating channels that are reset to  zero (low)
		ChangedVals = 0;
		free(UpdateNum);
		free(ChNum);
		free(ChVal);
	}
	// more debug info
	tstop = clock();
	timeused = tstop - tstart;
	t = Start_Process(processnum);
	tstop = clock();
	sprintf(buff, "Time to transfer and start ADwin:   %d", timeused);

	GetMenuBarAttribute(menuHandle, MENU_PREFS_STREAM_SETTINGS, ATTR_CHECKED, &StreamSettings);
	if (StreamSettings == TRUE)
	{
		//Fill IN
	}

	// even more debug info
	InsertListItem(panelHandle, PANEL_DEBUG, -1, buff, 1);
	memused = (count + 2 * nuptotal) * 4; //in bytes
	sprintf(buff, "MB of data sent:   %f", (double)memused / 1000000);
	InsertListItem(panelHandle, PANEL_DEBUG, -1, buff, 1);
	sprintf(buff, "Transfer Rate:   %f   MB/s", (double)memused / (double)timeused / 1000);
	InsertListItem(panelHandle, PANEL_DEBUG, -1, buff, 1);

	SetCtrlAttribute(panelHandle, PANEL_CMD_RUN, ATTR_CMD_BUTTON_COLOR, 0x00B0B0B0);
	//re-enable the timer if necessary
	GetCtrlVal(panelHandle, PANEL_TOGGLEREPEAT, &repeat);
	if ((PScan.Scan_Active == TRUE) && (PScan.ScanDone == TRUE))
	{
		repeat = FALSE; //remember to reset the front panel repeat button
		SetCtrlVal(panelHandle, PANEL_TOGGLEREPEAT, repeat);
	}
	if (repeat == TRUE)
	{
		SetCtrlAttribute(panelHandle, PANEL_TIMER, ATTR_ENABLED, 1);
		SetCtrlAttribute(panelHandle, PANEL_TIMER, ATTR_INTERVAL, cycletime);
		ResetTimer(panelHandle, PANEL_TIMER);
	}

}

//*****************************************************************************************
double CalcFcnValue(int fcn, double Vinit, double Vfinal, double timescale, double telapsed, double celltime)
{
	double value = -99, amplitude, slope, aconst, bconst, tms, frequency, newtime;
	frequency = timescale;
	if (UseSimpleTiming == TRUE)
	{
		timescale = celltime - EventPeriod;
	}
	if (timescale <= 0)
	{
		timescale = 1;
	}
	tms = telapsed * EventPeriod;
	// add commands here to select 'simple timing'

	switch (fcn)
	{
	case 1: //step function
		value = Vfinal;
		break;
	case 2: //linear ramp
		amplitude = Vfinal - Vinit;
		slope = amplitude / timescale;
		if (tms > timescale)
		{
			value = Vfinal;
		}
		else
		{
			value = Vinit + slope * tms;
		}
		break;
	case 3: //exponential
		amplitude = Vfinal - Vinit;
		newtime = timescale;
		if (UseSimpleTiming == TRUE)
		{
			newtime = timescale / fabs(log(fabs(amplitude)) - log(0.001));
		}
		value = Vfinal - amplitude * exp(-tms / newtime);
		break;
	case 4:
		amplitude = Vfinal - Vinit;
		aconst = 3 * amplitude / pow(timescale, 2);
		bconst = -2 * amplitude / pow(timescale, 3);
		if (tms > timescale)
		{
			value = Vfinal;
		}
		else
		{
			value = Vinit + (aconst * pow(tms, 2) + bconst * pow(tms, 3));
		}
		break;
	case 5: // generate a sinewave.  Use Vfinal as the amplitude and timescale as the frequency
		// ignore the 'Simple Timing' option...use the user entered value.

		amplitude = Vfinal;
		//	frequency=timescale; //consider it to be Hertz (tms is time in milliseconds)
		value = amplitude * sin(2 * 3.14159 * frequency * tms / 1000);
	}
	// Check if the value exceeds the allowed voltage limits.
	return value;
}
//*****************************************************************************************
void OptimizeTimeLoop(int *UpdateNum, int count, int *newcount)
{
	// This routine compresses the updatenum list by replacing long strings of 0 with a single line.
	// i.e.  if we see 2000 zero's in a row, just write -2000 instead.

	int i = 0, k = 0; // i is the counter through the original UpdateNum list
	int j = 0;		  // t is the counter through the NewUpdateNum list
	int t = 0;
	int LowZeroThreshold, HighZeroThreshold;
	int numberofzeros;
	i = 1;
	t = 1;
	LowZeroThreshold = 0;		// minimum number of consecutive zero's to encounter before optimizing
	HighZeroThreshold = 100000; // maximum number of consecutive zero's to optimize
								//  We do not want to exceed the counter on the ADwin
								//  ADwin uses a 40MHz clock, 1 ms implies counter set to 40,000
	while (i < count + 1)
	{
		if (UpdateNum[i] != 0)
		{
			UpdateNum[t] = UpdateNum[i];
			i++;
			t++;
		}
		else // found a 0
		{	 // now we need to scan to find the # of zeros
			j = 1;
			while (((i + j) < (count + 1)) && (UpdateNum[i + j] == 0))
			{
				j++;
			} //if this fails, then AA[i+j]!=0
			if ((i + j) < (count + 1))
			{
				numberofzeros = j;
				if (numberofzeros <= LowZeroThreshold)
				{
					for (k = 1; k <= numberofzeros; k++)
					{
						UpdateNum[t] = 0;
						t++;
						i++;
					}
				}
				else
				{
					while (numberofzeros > HighZeroThreshold)
					{
						numberofzeros = numberofzeros - HighZeroThreshold;
						UpdateNum[t] = -HighZeroThreshold;
						t++;
					}
					UpdateNum[t] = -numberofzeros;
					t++;
					UpdateNum[t] = UpdateNum[i + j];
					t++;
					i = i + j + 1;
				}
			}
			else
			{
				UpdateNum[t] = -(count + 1 - i - j);
				i = i + j + 1;
			}
		}
	}
	*newcount = t;
}
//*****************************************************************************************
//June 7, 2005 :  Completed Scan capability, added on-screen display of scan progress.
// May 11, 2005:  added capability to change time and DDS settings too.  Redesigned Scan structure
// May 03, 2005
// existing problem: if the final value isn't exactly reached by the steps, then the last stage is skipped and the
// cycle doesn't end
// has to do with numsteps.  Should be programmed with ceiling(), not abs

void UpdateScanValue(int Reset)
{
	int cx, cy, cz;
	double cellval, nextcell;
	BOOL UseList;
	int hour, minute, second;
	static BOOL ScanUp;
	static int timesdid, counter;

	cx = PScan.Column;
	cy = PScan.Row;
	cz = PScan.Page;

	SetCtrlAttribute(panelHandle_sub2, SUBPANEL2, ATTR_VISIBLE, 1);

	//if Use_List is checked, then read values off of the SCAN_TABLE on the main panel.
	GetCtrlVal(panelHandle7, SCANPANEL_CHECK_USE_LIST, &UseList);
	// Initialization on first iteration
	if (Reset == TRUE)
	{
		counter = 0;
		for (int i = 0; i < 1000; i++)
		{
			ScanBuffer[i].Step = 0;
			ScanBuffer[i].Iteration = 0;
			ScanBuffer[i].Value = 0;
		}

		//Copy information from the appropriate scan mode to the variables.
		switch (PScan.ScanMode)
		{
		case 0: // set to analog
			ScanVal.End = PScan.Analog.End_Of_Scan;
			ScanVal.Start = PScan.Analog.Start_Of_Scan;
			ScanVal.Step = PScan.Analog.Scan_Step_Size;
			ScanVal.Iterations = PScan.Analog.Iterations_Per_Step;
			break;
		case 1: // time scan
			ScanVal.End = PScan.Time.End_Of_Scan;
			ScanVal.Start = PScan.Time.Start_Of_Scan;
			ScanVal.Step = PScan.Time.Scan_Step_Size;
			ScanVal.Iterations = PScan.Time.Iterations_Per_Step;
			break;
		case 2:
			ScanVal.End = PScan.DDS.End_Of_Scan;
			ScanVal.Start = PScan.DDS.Start_Of_Scan;
			ScanVal.Step = PScan.DDS.Scan_Step_Size;
			ScanVal.Iterations = PScan.DDS.Iterations_Per_Step;
			break;
		case 3:
			ScanVal.End = PScan.DDSFloor.Floor_End;
			ScanVal.Start = PScan.DDSFloor.Floor_Start;
			ScanVal.Step = PScan.DDSFloor.Floor_Step;
			ScanVal.Iterations = PScan.DDSFloor.Iterations_Per_Step;
		}

		if (UseList) // if we are set to use the scan list instead of a linear scan, then read first value
		{
			GetTableCellVal(panelHandle, PANEL_SCAN_TABLE, MakePoint(1, 1), &cellval);
			ScanVal.Start = cellval;
		}
		timesdid = 0;
		ScanVal.Current_Step = 0;
		ScanVal.Current_Iteration = -1;
		ScanVal.Current_Value = ScanVal.Start;

		// determine the sign of the step and correct if necessary
		if (ScanVal.End >= ScanVal.Start)
		{
			ScanUp = TRUE;
			if (ScanVal.Step < 0)
			{
				ScanVal.Step = -ScanVal.Step;
			}
		}
		else
		{
			ScanUp = FALSE; // ie. we scan downwards
			if (ScanVal.Step > 0)
			{
				ScanVal.Step = -ScanVal.Step;
			}
		}
	} //Done setting/resetting values

	// numsteps to depend on mode
	if (UseList) // UseList=TRUE .... therefore using table of Scan Values
	{
		ScanVal.Current_Iteration++;
		if (ScanVal.Current_Iteration >= ScanVal.Iterations)
		{
			ScanVal.Current_Iteration = 0;
			ScanVal.Current_Step++;
			// read next element of scan list
			GetTableCellVal(panelHandle, PANEL_SCAN_TABLE, MakePoint(1, ScanVal.Current_Step + 1), &cellval);
			// indicate which element of the list we are currently using
			SetTableCellAttribute(panelHandle, PANEL_SCAN_TABLE, MakePoint(1, ScanVal.Current_Step + 1), ATTR_TEXT_BGCOLOR,
								  VAL_LT_GRAY);
			SetTableCellAttribute(panelHandle, PANEL_SCAN_TABLE, MakePoint(1, ScanVal.Current_Step), ATTR_TEXT_BGCOLOR,
								  VAL_WHITE);
			ScanVal.Current_Value = cellval;
			ChangedVals = TRUE;
		}
	}

	else // UseList=FALSE.... therefor assume linear scanning
	{
		// calculate number of steps in the ramp
		int numsteps = ceil(abs(((double)ScanVal.Start - (double)ScanVal.End) / (double)ScanVal.Step));
		PScan.ScanDone = FALSE;
		timesdid++;
		ScanVal.Current_Iteration++;

		if ((ScanVal.Current_Iteration >= ScanVal.Iterations) && (ScanVal.Current_Step < numsteps)) // update the step at correct time
		{
			ScanVal.Current_Iteration = 0;
			ScanVal.Current_Step++;
			ScanVal.Current_Value = ScanVal.Current_Value + ScanVal.Step;
			ChangedVals = TRUE;
		}
		//if we are at the last step, then set the scan value to the last value (in case the step size causes the scan to go too far
		if ((ScanVal.Current_Value >= ScanVal.End) && (ScanUp == TRUE))
		{
			ScanVal.Current_Value = ScanVal.End;
		}

		if ((ScanVal.Current_Value <= ScanVal.End) && (ScanUp == FALSE))
		{
			ScanVal.Current_Value = ScanVal.End;
		}
	}

	//insert current scan values into the tables , so they are included in the next BuildUpdateList
	switch (PScan.ScanMode)
	{
	case 0: // Analog value
		AnalogTable[cx][cy][cz].fval = ScanVal.Current_Value;
		AnalogTable[cx][cy][cz].fcn = PScan.Analog.Analog_Mode;
		break;
	case 1: // Time duration
		TimeArray[cx][cz] = ScanVal.Current_Value;
		break;
	case 2: // DDS frequency
		ddstable[cx][cz].amplitude = PScan.DDS.Current;
		ddstable[cx][cz].start_frequency = PScan.DDS.Base_Freq;
		ddstable[cx][cz].end_frequency = ScanVal.Current_Value;
		break;
	case 3: // DDS offset/floor
		SetCtrlVal(panelHandle, PANEL_NUM_DDS_OFFSET, ScanVal.Current_Value);
		break;
	}

	// Record current scan information into a string buffer, so we can write it to disk later.
	GetSystemTime(&hour, &minute, &second);
	ScanBuffer[counter].Step = ScanVal.Current_Step;
	ScanBuffer[counter].Iteration = ScanVal.Current_Iteration;
	ScanBuffer[counter].Value = ScanVal.Current_Value;
	ScanBuffer[0].BufferSize = counter;
	sprintf(ScanBuffer[counter].Time, "%d:%d:%d", hour, minute, second);
	counter++;

	// display current scan parameters on screen
	SetCtrlVal(panelHandle_sub2, SUBPANEL2_NUM_SCANVAL, ScanVal.Current_Value);
	SetCtrlVal(panelHandle_sub2, SUBPANEL2_NUM_SCANSTEP, ScanVal.Current_Step);
	SetCtrlVal(panelHandle_sub2, SUBPANEL2_NUM_SCANITER, ScanVal.Current_Iteration);

	//check for end condition
	if (UseList) // Scanning ends if we program -999 into a cell of the Scan List
	{
		GetTableCellVal(panelHandle, PANEL_SCAN_TABLE, MakePoint(1, ScanVal.Current_Step + 2), &nextcell);
		if ((nextcell <= -999) && (ScanVal.Current_Iteration >= ScanVal.Iterations - 1))
		{
			PScan.ScanDone = TRUE;
		}
	}

	else //not using the ScanTable
	{
		if (ScanUp)
		{
			if ((ScanVal.Current_Value >= ScanVal.End) && (ScanVal.Current_Iteration >= ScanVal.Iterations - 1))
			{						   //Done Scan
				PScan.ScanDone = TRUE; // Flag used in RunOnce() to initiate a stop
			}
		}
		else
		{
			if ((ScanVal.Current_Value <= ScanVal.End) && (ScanVal.Current_Iteration >= ScanVal.Iterations - 1))
			{						   //Done Scan
				PScan.ScanDone = TRUE; // Flag used in RunOnce() to initiate a stop
			}
		}
	} //done checking the scan is done

	// if the scan is done, then cleanup and write the starting values back into the tables
	if (PScan.ScanDone == TRUE)
	{ // reset initial values in the tables
		AnalogTable[cx][cy][cz].fval = PScan.Analog.Start_Of_Scan;
		TimeArray[cx][cz] = PScan.Time.Start_Of_Scan;
		SetCtrlVal(panelHandle, PANEL_NUM_DDS_OFFSET, PScan.DDSFloor.Floor_Start);
		ddstable[cx][cz].end_frequency = PScan.DDS.Start_Of_Scan;

		//hide the information panel
		SetCtrlAttribute(panelHandle_sub2, SUBPANEL2, ATTR_VISIBLE, 0);
		ExportScanBuffer(); // prompt to write out information
	}
}
//*****************************************************************************************
void LoadSettings(void)
{
	int status;
	//If .ini exists, then open the file dialog.  Else just open the file dialog.  Add a method to load
	//last used settings on program startup.
	FILE *fpini;
	char c, fsavename[500] = "";
	static char defaultdir[200] = "C:\\UserDate\\Data";

	//Check if .ini file exists.  Load it if it does.
	if (!(fpini = fopen("gui.ini", "r")) == NULL)
	{
		char fname[100] = "";
		int inisize = 0;
		while (fscanf(fpini, "%c", &c) != -1)
			fname[inisize++] = c; //Read the contained name
	}
	fclose(fpini);

	// prompt for a file, if selected then load the Panel and Arrays
	status = FileSelectPopup(defaultdir, "*.pan", "", "Load Settings", VAL_LOAD_BUTTON, 0, 0, 1, 1, fsavename);
	if (!(status == 0))
	{
		RecallPanelState(PANEL, fsavename, 1);
		LoadArrays(fsavename, strlen(fsavename));
	}
	else
	{
		MessagePopup("File Error", "No file was selected");
	}

	DrawNewTable(0);
	strcpy(defaultdir, "");
}
//*****************************************************************************************
void LoadLastSettings(int check)
{
	//If .ini exists, then open the file dialog.  Else just open the file dialog.  Add a method to load
	//last used settings on program startup.
	FILE *fpini;
	char fname[100] = "", c, loadname[100] = "";
	int inisize = 0;
	//Check if .ini file exists.  Load it if it does.
	if (!(fpini = fopen("gui.ini", "r")) == NULL)
	{
		while (fscanf(fpini, "%c", &c) != -1)
			fname[inisize++] = c; //Read the contained name
	}
	fclose(fpini);

	c = fname[inisize - 1];
	strncpy(loadname, fname, inisize - 2);
	if ((check == 0) || (c == 49)) // 49 is the ascii code for "1"
	{
		RecallPanelState(PANEL, loadname, 1);
		LoadArrays(fname, strlen(loadname));
	}
	DrawNewTable(0);
}
//*****************************************************************************************

void SaveSettings(void)
/* Modified:
Feb 09, 2006   Clear the Debug box before saving. (was causing insanely large save files, and slowed down loading of old panels.)
*/
{
	// Save settings:  First look for file .ini  This will be a simple 1 line file staating the name of the last file
	//saved.  Load this up and use as the starting name in the file dialog.
	FILE *fpini;
	char c, fsavename[500] = "";
	static char defaultdir[200] = "C:\\UserDate\\Data";
	int status, loadonboot = 0;
	//Check if .ini file exists.  Load it if it does.
	if (!(fpini = fopen("gui.ini", "r")) == NULL) // if "gui.ini" exists, then read it  Just contains a filename.
	{											  //If not, do nothing
		char fname[100] = "";
		int inisize = 0;
		while (fscanf(fpini, "%c", &c) != -1)
			fname[inisize++] = c; //Read the contained name
	}
	fclose(fpini);

	status = FileSelectPopup(defaultdir, "*.pan", "", "Save Settings", VAL_SAVE_BUTTON, 0, 0, 1, 1, fsavename);
	GetMenuBarAttribute(menuHandle, MENU_FILE_BOOTLOAD, ATTR_CHECKED, &loadonboot);
	if (!(status == 0))
	{
		ClearListCtrl(panelHandle, PANEL_DEBUG); // added Feb 09, 2006
		SavePanelState(PANEL, fsavename, 1);
		if (!(fpini = fopen("gui.ini", "w")) == NULL)
		{
			fprintf(fpini, fsavename);
			fprintf(fpini, "\n%d", loadonboot);
		}
		fclose(fpini);
		SaveArrays(fsavename, strlen(fsavename));
	}
	else
	{
		MessagePopup("File Error", "No file was selected");
	}
	strcpy(defaultdir, "");
}
//*********************************************************************
void DrawNewTable(int isdimmed)
//if isdimmed=0/FALSE  Draw everything, editing mode
//if isdimmed=1/TRUE   Dim out appropriate columns....
// Make isdimmed a global I guess...
// Jan 24,2006:   Now dim/invisible appropriate 'arrows' that indicate loop points.
// May 12, 2005:  Updated to change color of cell that is active by a parameter scan.  This cell (ANalog, Time or DDS) now
//                turns dark yellow. might pick a better color.

{
	int i, j, m, picmode, page, cmode;
	int analogtable_visible = 0;
	int digtable_visible = 0;
	double vnow = 0;

	int ispicture = 1, celltype = 0; //celtype has 3 values.  0=Numeric, 1=String, 2=Picture
	// list of colours used for different rows, alternate after every 3 rows
	int ColourTable[37] = {VAL_BLACK, VAL_GRAY, VAL_GRAY, VAL_GRAY, 0x00B0B0B0, 0x00B0B0B0, 0x00B0B0B0, VAL_GRAY, VAL_GRAY, VAL_GRAY, 0x00B0B0B0, 0x00B0B0B0, 0x00B0B0B0,
						   VAL_GRAY, VAL_GRAY, VAL_GRAY, 0x00B0B0B0, 0x00B0B0B0, 0x00B0B0B0, VAL_GRAY, VAL_GRAY, VAL_GRAY, 0x00B0B0B0, 0x00B0B0B0, 0x00B0B0B0, VAL_GRAY, VAL_GRAY, VAL_GRAY, 0x00B0B0B0, 0x00B0B0B0, 0x00B0B0B0, VAL_GRAY, VAL_GRAY, VAL_GRAY, 0x00B0B0B0, 0x00B0B0B0, 0x00B0B0B0};

	GetCtrlAttribute(panelHandle, PANEL_ANALOGTABLE, ATTR_VISIBLE, &analogtable_visible);
	GetCtrlAttribute(panelHandle, PANEL_DIGTABLE, ATTR_VISIBLE, &digtable_visible);
	SetCtrlAttribute(panelHandle, PANEL_ANALOGTABLE, ATTR_VISIBLE, 0);
	SetCtrlAttribute(panelHandle, PANEL_DIGTABLE, ATTR_VISIBLE, 0);
	SetCtrlAttribute(panelHandle, PANEL_TIMETABLE, ATTR_VISIBLE, 0);
	page = GetPage(); // Check which page is active.

	GetCtrlVal(panelHandle, PANEL_TGL_NUMERICTABLE, &celltype);
	if (celltype == 2)
	{
		ispicture = TRUE;
	}
	else
	{
		ispicture = FALSE;
	}

	CheckActivePages();
	if (ischecked[page] == FALSE)
	{ // dim the tables
		SetCtrlAttribute(panelHandle, PANEL_ANALOGTABLE, ATTR_DIMMED, 1);
		SetCtrlAttribute(panelHandle, PANEL_DIGTABLE, ATTR_DIMMED, 1);
		SetCtrlAttribute(panelHandle, PANEL_TIMETABLE, ATTR_DIMMED, 1);
	}
	else
	{ //undim the tables
		SetCtrlAttribute(panelHandle, PANEL_ANALOGTABLE, ATTR_DIMMED, 0);
		SetCtrlAttribute(panelHandle, PANEL_DIGTABLE, ATTR_DIMMED, 0);
		SetCtrlAttribute(panelHandle, PANEL_TIMETABLE, ATTR_DIMMED, 0);
	}
	picmode = 0;

	for (i = 1; i <= NUMBEROFCOLUMNS; i++) // scan over the columns
	{

		SetTableCellAttribute(panelHandle, PANEL_TIMETABLE, MakePoint(i, 1), ATTR_CELL_DIMMED, 0);
		for (j = 1; j <= NUMBERANALOGCHANNELS; j++) // scan over analog channels
		{
			cmode = AnalogTable[i][j][page].fcn;
			vnow = AnalogTable[i][j][page].fval;

			SetTableCellAttribute(panelHandle, PANEL_ANALOGTABLE, MakePoint(i, j),
								  ATTR_CELL_TYPE, VAL_CELL_NUMERIC);

			if (cmode != 6)
				// write the ending value into the cell
				SetTableCellAttribute(panelHandle, PANEL_ANALOGTABLE, MakePoint(i, j), ATTR_CTRL_VAL, vnow);
			else if (i == 1 && page == 1)
			{
				ConfirmPopup("User Error", "Do not use \"Same as Previous\" Setting for Column 1 Page 1.\nThis results in unstable behaviour.\nResetting Cell Function to default: step");
				SetTableCellAttribute(panelHandle, PANEL_ANALOGTABLE, MakePoint(i, j), ATTR_CTRL_VAL, 0.0);
				cmode = 1;
				AnalogTable[1][j][1].fcn = cmode;
				AnalogTable[1][j][1].fval = 0.0;
				AnalogTable[1][j][1].tscale = 0.0;
			}
			else
				//888 indicates cell will take value of previous cell
				SetTableCellAttribute(panelHandle, PANEL_ANALOGTABLE, MakePoint(i, j), ATTR_CTRL_VAL, 888.0);

			SetTableCellAttribute(panelHandle, PANEL_ANALOGTABLE, MakePoint(i, j), ATTR_CELL_DIMMED, 0);
			SetTableCellAttribute(panelHandle, PANEL_DIGTABLE, MakePoint(i, j), ATTR_CELL_DIMMED, 0);

			// get Analog table parameters for that cell... e.g. start/end values, function to get there

			SetTableCellAttribute(panelHandle, PANEL_ANALOGTABLE, MakePoint(i, j), ATTR_TEXT_BGCOLOR, ColourTable[j]);
			// Change the cell color depending on the function type
			if (cmode == 1)
			{
				if (vnow == 0)
				{
					SetTableCellAttribute(panelHandle, PANEL_ANALOGTABLE, MakePoint(i, j), ATTR_TEXT_BGCOLOR, ColourTable[j]);
				}
				else
				{
					SetTableCellAttribute(panelHandle, PANEL_ANALOGTABLE, MakePoint(i, j), ATTR_TEXT_BGCOLOR, VAL_RED);
				}
			}
			if (cmode == 2) // linear ramp
			{
				SetTableCellAttribute(panelHandle, PANEL_ANALOGTABLE, MakePoint(i, j), ATTR_TEXT_BGCOLOR, VAL_GREEN);
			}
			if (cmode == 3) // exponential ramp
			{
				SetTableCellAttribute(panelHandle, PANEL_ANALOGTABLE, MakePoint(i, j), ATTR_TEXT_BGCOLOR, VAL_BLUE);
			}
			if (cmode == 4) // constant jerk function
			{
				SetTableCellAttribute(panelHandle, PANEL_ANALOGTABLE, MakePoint(i, j), ATTR_TEXT_BGCOLOR, VAL_MAGENTA);
			}
			if (cmode == 5) // Sine wave output
			{
				SetTableCellAttribute(panelHandle, PANEL_ANALOGTABLE, MakePoint(i, j), ATTR_TEXT_BGCOLOR, VAL_CYAN);
			}
			if (cmode == 6) //Same as Previous Output
			{
				SetTableCellAttribute(panelHandle, PANEL_ANALOGTABLE, MakePoint(i, j), ATTR_TEXT_BGCOLOR, VAL_YELLOW);
			}
		}
		for (j = 1; j <= NUMBERDIGITALCHANNELS; j++) // scan over analog channels
		{
			// if a digital value is high, colour the cell red
			if (DigTableValues[i][j][page] == 1)
			{
				SetTableCellAttribute(panelHandle, PANEL_DIGTABLE, MakePoint(i, j), ATTR_TEXT_BGCOLOR, VAL_RED);
			}
			else
			{
				SetTableCellVal(panelHandle, PANEL_DIGTABLE, MakePoint(i, j), 0);
				SetTableCellAttribute(panelHandle, PANEL_DIGTABLE, MakePoint(i, j), ATTR_TEXT_BGCOLOR, ColourTable[j]);
			}
		} //Done digital drawing.

		//***************update DDS row********************

		int DDSChannel1 = NUMBERANALOGCHANNELS + 1;
		int DDSChannel2 = NUMBERANALOGCHANNELS + 2;
		int DDSChannel3 = NUMBERANALOGCHANNELS + 3;
		/*DDS1*/ SetTableCellVal(panelHandle, PANEL_ANALOGTABLE, MakePoint(i, DDSChannel1), 0.);
		//if(ispicture==0)
		{
			SetTableCellVal(panelHandle, PANEL_ANALOGTABLE, MakePoint(i, DDSChannel1), ddstable[i][page].start_frequency);
		}
		SetTableCellAttribute(panelHandle, PANEL_ANALOGTABLE, MakePoint(i, DDSChannel1), ATTR_CELL_DIMMED, 0);
		SetTableCellAttribute(panelHandle, PANEL_DIGTABLE, MakePoint(i, DDSChannel1), ATTR_CELL_DIMMED, 0);

		if (ddstable[i][page].amplitude == 0.0 || ddstable[i][page].is_stop)
		{
			//make grey
			SetTableCellAttribute(panelHandle, PANEL_ANALOGTABLE, MakePoint(i, DDSChannel1), ATTR_TEXT_BGCOLOR, VAL_WHITE);
		}
		else
		{
			if (ddstable[i][page].start_frequency > ddstable[i][page].end_frequency)
			{
				//ramping down
				SetTableCellAttribute(panelHandle, PANEL_ANALOGTABLE, MakePoint(i, DDSChannel1), ATTR_TEXT_BGCOLOR, VAL_BLUE);
			}
			else
			{
				//ramping up
				SetTableCellAttribute(panelHandle, PANEL_ANALOGTABLE, MakePoint(i, DDSChannel1), ATTR_TEXT_BGCOLOR, VAL_GREEN);
			}
		}
		/*DDS2*/ SetTableCellVal(panelHandle, PANEL_ANALOGTABLE, MakePoint(i, DDSChannel2), 0.);
		//if(ispicture==0)
		{
			SetTableCellVal(panelHandle, PANEL_ANALOGTABLE, MakePoint(i, DDSChannel2), dds2table[i][page].start_frequency);
		}
		SetTableCellAttribute(panelHandle, PANEL_ANALOGTABLE, MakePoint(i, DDSChannel2), ATTR_CELL_DIMMED, 0);
		SetTableCellAttribute(panelHandle, PANEL_DIGTABLE, MakePoint(i, DDSChannel2), ATTR_CELL_DIMMED, 0);

		if (dds2table[i][page].amplitude == 0.0 || dds2table[i][page].is_stop)
		{
			//make grey
			SetTableCellAttribute(panelHandle, PANEL_ANALOGTABLE, MakePoint(i, DDSChannel2), ATTR_TEXT_BGCOLOR, VAL_WHITE);
		}
		else
		{
			if (dds2table[i][page].start_frequency > dds2table[i][page].end_frequency)
			{
				//ramping down
				SetTableCellAttribute(panelHandle, PANEL_ANALOGTABLE, MakePoint(i, DDSChannel2), ATTR_TEXT_BGCOLOR, VAL_BLUE);
			}
			else
			{
				//ramping up
				SetTableCellAttribute(panelHandle, PANEL_ANALOGTABLE, MakePoint(i, DDSChannel2), ATTR_TEXT_BGCOLOR, VAL_GREEN);
			}
		}
		/*DDS3*/ SetTableCellVal(panelHandle, PANEL_ANALOGTABLE, MakePoint(i, DDSChannel3), 0.);
		//if(ispicture==0)
		{
			SetTableCellVal(panelHandle, PANEL_ANALOGTABLE, MakePoint(i, DDSChannel3), dds3table[i][page].start_frequency);
		}
		SetTableCellAttribute(panelHandle, PANEL_ANALOGTABLE, MakePoint(i, DDSChannel3), ATTR_CELL_DIMMED, 0);
		SetTableCellAttribute(panelHandle, PANEL_DIGTABLE, MakePoint(i, DDSChannel3), ATTR_CELL_DIMMED, 0);

		if (dds3table[i][page].amplitude == 0.0 || dds3table[i][page].is_stop)
		{
			//make grey
			SetTableCellAttribute(panelHandle, PANEL_ANALOGTABLE, MakePoint(i, DDSChannel3), ATTR_TEXT_BGCOLOR, VAL_WHITE);
		}
		else
		{
			if (dds3table[i][page].start_frequency > dds3table[i][page].end_frequency)
			{
				//ramping down
				SetTableCellAttribute(panelHandle, PANEL_ANALOGTABLE, MakePoint(i, DDSChannel3), ATTR_TEXT_BGCOLOR, VAL_BLUE);
			}
			else
			{
				//ramping up
				SetTableCellAttribute(panelHandle, PANEL_ANALOGTABLE, MakePoint(i, DDSChannel3), ATTR_TEXT_BGCOLOR, VAL_GREEN);
			}
		}
		//***************update DDS row********************

		// update the times row
		SetTableCellVal(panelHandle, PANEL_TIMETABLE, MakePoint(i, 1), TimeArray[i][page]);
	}
	// So far, all columns are undimmed
	//now check if we need to dim out any columns(of timetable,AnalogTable and DigTable
	if (isdimmed == TRUE)
	{
		int nozerofound = TRUE; // haven't encountered a zero yet... so keep going
		for (i = 1; i <= NUMBEROFCOLUMNS; i++)
		{
			BOOL dimset = FALSE;
			if ((nozerofound == FALSE) || (TimeArray[i][page] == 0)) // if we have seen a zero before, or we see one now, then
			{
				nozerofound = FALSE; // Flag that tells us to dim out all remaining columns
				dimset = TRUE;		 // Flag to dim out this column
			}
			else if ((nozerofound == TRUE) && (TimeArray[i][page] < 0)) // if we haven't seen a zero, but this column has a negative time then...
			{
				dimset = TRUE;
			}
			SetTableCellAttribute(panelHandle, PANEL_TIMETABLE, MakePoint(i, 1), ATTR_CELL_DIMMED, dimset);
			if (ispicture == 0)
			{
				picmode = 0;
			}
			if (ispicture == 1)
			{
				picmode = 2;
			}
			if (dimset == TRUE)
			{
				picmode = 2;
			}

			for (j = 1; j <= NUMBERANALOGCHANNELS; j++)
			{
				SetTableCellAttribute(panelHandle, PANEL_ANALOGTABLE, MakePoint(i, j), ATTR_CELL_DIMMED, dimset);
				SetTableCellAttribute(panelHandle, PANEL_DIGTABLE, MakePoint(i, j), ATTR_CELL_DIMMED, dimset);
				SetTableCellAttribute(panelHandle, PANEL_ANALOGTABLE, MakePoint(i, j), ATTR_CELL_TYPE, picmode);
			}
		}
	}
	SetCtrlAttribute(panelHandle, PANEL_ANALOGTABLE, ATTR_VISIBLE, analogtable_visible);
	SetCtrlAttribute(panelHandle, PANEL_DIGTABLE, ATTR_VISIBLE, digtable_visible);
	SetCtrlAttribute(panelHandle, PANEL_TIMETABLE, ATTR_VISIBLE, 1);

	for (m = 1; m <= NUMBEROFCOLUMNS; m++)
	{
		SetTableCellAttribute(panelHandle, PANEL_TIMETABLE, MakePoint(m, 1), ATTR_TEXT_BGCOLOR, VAL_WHITE);
	}

	if ((currentpage == PScan.Page) && (PScan.Scan_Active == TRUE)) //display the cell active for a parameter scan
	{
		switch (PScan.ScanMode)
		{
		case 0:
			SetTableCellAttribute(panelHandle, PANEL_ANALOGTABLE, MakePoint(PScan.Column, PScan.Row), ATTR_TEXT_BGCOLOR, VAL_DK_YELLOW);
			break;
		case 1:
			SetTableCellAttribute(panelHandle, PANEL_TIMETABLE, MakePoint(PScan.Column, 1), ATTR_TEXT_BGCOLOR, VAL_DK_YELLOW);
			break;
		case 2:
			SetTableCellAttribute(panelHandle, PANEL_ANALOGTABLE, MakePoint(PScan.Column, 25), ATTR_TEXT_BGCOLOR, VAL_DK_YELLOW);
			break;
		}
	}

	if (currentpage == 10)
	{
		SetCtrlAttribute(panelHandle, PANEL_NUM_INSERTIONPAGE, ATTR_DIMMED, 0);
		SetCtrlAttribute(panelHandle, PANEL_NUM_INSERTIONCOL, ATTR_DIMMED, 0);
	}
	else
	{
		SetCtrlAttribute(panelHandle, PANEL_NUM_INSERTIONPAGE, ATTR_DIMMED, 1);
		SetCtrlAttribute(panelHandle, PANEL_NUM_INSERTIONCOL, ATTR_DIMMED, 1);
	}
	DrawLoopIndicators(); // Draw the arrows indicating loop position
}

//*****************************************************************************************
// MENU:Analog Settings  option chosen
//
void CVICALLBACK ANALOGSET_CALLBACK(int menuBar, int menuItem, void *callbackData,
									int panel)
{
	ChangedVals = TRUE;
	DisplayPanel(panelHandle2);
}
//***********************************************************************************

void CVICALLBACK DIGITALSET_CALLBACK(int menuBar, int menuItem, void *callbackData,
									 int panel)
{
	ChangedVals = TRUE;
	DisplayPanel(panelHandle3);
}
//***********************************************************************************

void CVICALLBACK MENU_CALLBACK(int menuBar, int menuItem, void *callbackData,
							   int panel)
{
	switch (menuItem)
	{
	case MENU_FILE_SAVESET:
		SaveSettings();
		break;

	case MENU_FILE_LOADSET:
		LoadSettings();
		break;
	}
}
//*********************************************************************************************

int CVICALLBACK ANALOGTABLE_CALLBACK(int panel, int control, int event,
									 void *callbackData, int eventData1, int eventData2)
{
	int pixleft = 0, pixtop = 0;
	Point cpoint = {0, 0};
	char buff[80] = "";
	int leftbuttondown, rightbuttondown, keymod;
	int panel_to_display;

	ChangedVals = TRUE;
	switch (event)
	{
	case EVENT_LEFT_DOUBLE_CLICK:
		cpoint.x = 0;
		cpoint.y = 0;
		GetActiveTableCell(panelHandle, PANEL_ANALOGTABLE, &cpoint);
		//now set the indicators in the control panel title..ie units
		currentx = cpoint.x;
		currenty = cpoint.y;
		currentpage = GetPage();

		if ((currenty > NUMBERANALOGCHANNELS) && (currenty <= NUMBERANALOGCHANNELS + NUMBERDDS))
		{
			Active_DDS_Panel = currenty - NUMBERANALOGCHANNELS;

			SetDDSControlPanel(Active_DDS_Panel);
			panel_to_display = panelHandle6; // Program DDS
		}
		else
		{

			SetControlPanel();
			panel_to_display = panelHandle4; // Program Analog Channel
		}

		sprintf(buff, "x:%d   y:%d    z:%d", currentx, currenty, currentpage);
		SetPanelAttribute(panel_to_display, ATTR_TITLE, buff);
		//Read the mouse position and open window there.
		GetGlobalMouseState(&panelHandle, &pixleft, &pixtop, &leftbuttondown,
							&rightbuttondown, &keymod);
		SetPanelAttribute(panel_to_display, ATTR_LEFT, pixleft);
		SetPanelAttribute(panel_to_display, ATTR_TOP, pixtop);

		DisplayPanel(panel_to_display);

		break;
	}
	return 0;
}

//********************************************************************************************
int GetPage(void)
{
	return currentpage;
	//Scan and find out what page we are on....
}

//*************************************************************************************
void DrawLoopIndicators()
{
	int page, dx, xendoffset, xstartoffset;
	int canvaswidth;

	GetCtrlAttribute(panelHandle, PANEL_CANVAS_LOOPLINE, ATTR_WIDTH, &canvaswidth);
	page = GetPage();
	CanvasClear(panelHandle, PANEL_CANVAS_LOOPLINE, VAL_ENTIRE_OBJECT);

	if (Switches.loop_active)
	{
		if (page == LoopPoints.startpage)
		{
			MoveCanvasStart(LoopPoints.startcol, TRUE);
		}
		else
		{
			MoveCanvasStart(LoopPoints.startcol, FALSE);
		}
		if (page == LoopPoints.endpage)
		{
			MoveCanvasEnd(LoopPoints.endcol, TRUE);
		}
		else
		{
			MoveCanvasEnd(LoopPoints.endcol, FALSE);
		}
	}
	else
	{
		MoveCanvasStart(LoopPoints.startcol, FALSE);
		MoveCanvasEnd(LoopPoints.endcol, FALSE);
	}

	// If good, then draw a connecting line
	dx = 40;
	xstartoffset = 10;
	xendoffset = 10;

	if (Switches.loop_active)
	{
		SetCtrlAttribute(panelHandle, PANEL_CANVAS_LOOPLINE, ATTR_PEN_WIDTH, 2);
		SetCtrlAttribute(panelHandle, PANEL_CANVAS_LOOPLINE, ATTR_PEN_COLOR, 0x00FF00L);

		int xposstart = (LoopPoints.startcol - 1) * dx + xstartoffset;
		int xposend = (LoopPoints.endcol) * dx - xstartoffset;

		if ((page == LoopPoints.startpage) && !(page == LoopPoints.endpage))
		{
			xposend = canvaswidth;
		}
		if (!(page == LoopPoints.startpage) && (page == LoopPoints.endpage))
		{
			xposstart = 0 + xstartoffset;
		}
		if ((page > LoopPoints.startpage) && (page < LoopPoints.endpage))
		{
			xposstart = 0 + xstartoffset;
			xposend = canvaswidth;
		}
		CanvasDrawLine(panelHandle, PANEL_CANVAS_LOOPLINE, MakePoint(xposstart, 8), MakePoint(xposend, 8));
		if ((page < LoopPoints.startpage) || (page > LoopPoints.endpage))
		{
			CanvasClear(panelHandle, PANEL_CANVAS_LOOPLINE, VAL_ENTIRE_OBJECT);
		}
	}
}

//*************************************************************************************
int CVICALLBACK TOGGLE1_CALLBACK(int panel, int control, int event,
								 void *callbackData, int eventData1, int eventData2)
{

	switch (event)
	{
	case EVENT_COMMIT:
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE1, 1);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE2, 0);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE3, 0);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE4, 0);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE5, 0);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE6, 0);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE7, 0);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE8, 0);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE9, 0);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE10, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_1, ATTR_VISIBLE, 1);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_2, ATTR_VISIBLE, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_3, ATTR_VISIBLE, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_4, ATTR_VISIBLE, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_5, ATTR_VISIBLE, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_6, ATTR_VISIBLE, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_7, ATTR_VISIBLE, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_8, ATTR_VISIBLE, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_9, ATTR_VISIBLE, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_10, ATTR_VISIBLE, 0);
		currentpage = 1;
		ChangedVals = TRUE;
		DrawNewTable(isdimmed);

		break;
	}
	return 0;
}

int CVICALLBACK TOGGLE2_CALLBACK(int panel, int control, int event,
								 void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
	case EVENT_COMMIT:
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE1, 0);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE2, 1);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE3, 0);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE4, 0);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE5, 0);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE6, 0);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE7, 0);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE8, 0);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE9, 0);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE10, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_1, ATTR_VISIBLE, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_2, ATTR_VISIBLE, 1);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_3, ATTR_VISIBLE, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_4, ATTR_VISIBLE, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_5, ATTR_VISIBLE, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_6, ATTR_VISIBLE, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_7, ATTR_VISIBLE, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_8, ATTR_VISIBLE, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_9, ATTR_VISIBLE, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_10, ATTR_VISIBLE, 0);
		ChangedVals = TRUE;
		currentpage = 2;
		DrawNewTable(isdimmed);
		break;
	}
	return 0;
}

int CVICALLBACK TOGGLE3_CALLBACK(int panel, int control, int event,
								 void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
	case EVENT_COMMIT:
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE1, 0);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE2, 0);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE3, 1);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE4, 0);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE5, 0);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE6, 0);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE7, 0);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE8, 0);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE9, 0);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE10, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_1, ATTR_VISIBLE, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_2, ATTR_VISIBLE, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_3, ATTR_VISIBLE, 1);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_4, ATTR_VISIBLE, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_5, ATTR_VISIBLE, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_6, ATTR_VISIBLE, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_7, ATTR_VISIBLE, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_8, ATTR_VISIBLE, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_9, ATTR_VISIBLE, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_10, ATTR_VISIBLE, 0);
		ChangedVals = TRUE;
		currentpage = 3;
		DrawNewTable(isdimmed);
		break;
	}
	return 0;
}

int CVICALLBACK TOGGLE4_CALLBACK(int panel, int control, int event,
								 void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
	case EVENT_COMMIT:
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE1, 0);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE2, 0);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE3, 0);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE4, 1);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE5, 0);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE6, 0);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE7, 0);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE8, 0);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE9, 0);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE10, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_1, ATTR_VISIBLE, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_2, ATTR_VISIBLE, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_3, ATTR_VISIBLE, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_4, ATTR_VISIBLE, 1);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_5, ATTR_VISIBLE, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_6, ATTR_VISIBLE, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_7, ATTR_VISIBLE, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_8, ATTR_VISIBLE, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_9, ATTR_VISIBLE, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_10, ATTR_VISIBLE, 0);
		ChangedVals = TRUE;
		currentpage = 4;
		DrawNewTable(isdimmed);
		break;
	}
	return 0;
}

int CVICALLBACK TOGGLE5_CALLBACK(int panel, int control, int event,
								 void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
	case EVENT_COMMIT:
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE1, 0);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE2, 0);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE3, 0);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE4, 0);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE5, 1);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE6, 0);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE7, 0);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE8, 0);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE9, 0);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE10, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_1, ATTR_VISIBLE, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_2, ATTR_VISIBLE, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_3, ATTR_VISIBLE, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_4, ATTR_VISIBLE, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_5, ATTR_VISIBLE, 1);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_6, ATTR_VISIBLE, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_7, ATTR_VISIBLE, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_8, ATTR_VISIBLE, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_9, ATTR_VISIBLE, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_10, ATTR_VISIBLE, 0);
		ChangedVals = TRUE;
		currentpage = 5;
		DrawNewTable(isdimmed);
		break;
	}
	return 0;
}

int CVICALLBACK TOGGLE6_CALLBACK(int panel, int control, int event,
								 void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
	case EVENT_COMMIT:
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE1, 0);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE2, 0);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE3, 0);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE4, 0);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE5, 0);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE6, 1);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE7, 0);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE8, 0);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE9, 0);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE10, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_1, ATTR_VISIBLE, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_2, ATTR_VISIBLE, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_3, ATTR_VISIBLE, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_4, ATTR_VISIBLE, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_5, ATTR_VISIBLE, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_6, ATTR_VISIBLE, 1);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_7, ATTR_VISIBLE, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_8, ATTR_VISIBLE, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_9, ATTR_VISIBLE, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_10, ATTR_VISIBLE, 0);
		ChangedVals = TRUE;
		currentpage = 6;
		DrawNewTable(isdimmed);
		break;
	}
	return 0;
}

int CVICALLBACK TOGGLE7_CALLBACK(int panel, int control, int event,
								 void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
	case EVENT_COMMIT:
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE1, 0);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE2, 0);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE3, 0);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE4, 0);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE5, 0);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE6, 0);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE7, 1);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE8, 0);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE9, 0);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE10, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_1, ATTR_VISIBLE, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_2, ATTR_VISIBLE, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_3, ATTR_VISIBLE, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_4, ATTR_VISIBLE, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_5, ATTR_VISIBLE, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_6, ATTR_VISIBLE, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_7, ATTR_VISIBLE, 1);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_8, ATTR_VISIBLE, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_9, ATTR_VISIBLE, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_10, ATTR_VISIBLE, 0);
		ChangedVals = TRUE;
		currentpage = 7;
		DrawNewTable(isdimmed);
		break;
	}
	return 0;
}

int CVICALLBACK TOGGLE8_CALLBACK(int panel, int control, int event,
								 void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
	case EVENT_COMMIT:
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE1, 0);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE2, 0);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE3, 0);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE4, 0);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE5, 0);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE6, 0);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE7, 0);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE8, 1);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE9, 0);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE10, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_1, ATTR_VISIBLE, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_2, ATTR_VISIBLE, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_3, ATTR_VISIBLE, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_4, ATTR_VISIBLE, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_5, ATTR_VISIBLE, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_6, ATTR_VISIBLE, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_7, ATTR_VISIBLE, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_8, ATTR_VISIBLE, 1);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_9, ATTR_VISIBLE, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_10, ATTR_VISIBLE, 0);
		ChangedVals = TRUE;
		currentpage = 8;
		DrawNewTable(isdimmed);
		break;
	}
	return 0;
}

int CVICALLBACK TOGGLE9_CALLBACK(int panel, int control, int event,
								 void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
	case EVENT_COMMIT:
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE1, 0);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE2, 0);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE3, 0);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE4, 0);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE5, 0);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE6, 0);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE7, 0);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE8, 0);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE9, 1);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE10, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_1, ATTR_VISIBLE, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_2, ATTR_VISIBLE, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_3, ATTR_VISIBLE, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_4, ATTR_VISIBLE, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_5, ATTR_VISIBLE, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_6, ATTR_VISIBLE, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_7, ATTR_VISIBLE, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_8, ATTR_VISIBLE, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_9, ATTR_VISIBLE, 1);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_10, ATTR_VISIBLE, 0);
		ChangedVals = TRUE;
		currentpage = 9;
		DrawNewTable(isdimmed);
		break;
	}
	return 0;
}

int CVICALLBACK TOGGLE10_CALLBACK(int panel, int control, int event,
								  void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
	case EVENT_COMMIT:
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE1, 0);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE2, 0);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE3, 0);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE4, 0);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE5, 0);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE6, 0);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE7, 0);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE8, 0);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE9, 0);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE10, 1);

		SetCtrlAttribute(panelHandle, PANEL_LABEL_1, ATTR_VISIBLE, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_2, ATTR_VISIBLE, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_3, ATTR_VISIBLE, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_4, ATTR_VISIBLE, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_5, ATTR_VISIBLE, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_6, ATTR_VISIBLE, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_7, ATTR_VISIBLE, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_8, ATTR_VISIBLE, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_9, ATTR_VISIBLE, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_10, ATTR_VISIBLE, 1);

		ChangedVals = TRUE;
		currentpage = 10;
		DrawNewTable(isdimmed);
		break;
	}
	return 0;
}

void CheckActivePages(void)
{
	int bool;
	GetCtrlVal(panelHandle, PANEL_CHECKBOX, &bool);
	ischecked[1] = bool;
	GetCtrlVal(panelHandle, PANEL_CHECKBOX_2, &bool);
	ischecked[2] = bool;
	GetCtrlVal(panelHandle, PANEL_CHECKBOX_3, &bool);
	ischecked[3] = bool;
	GetCtrlVal(panelHandle, PANEL_CHECKBOX_4, &bool);
	ischecked[4] = bool;
	GetCtrlVal(panelHandle, PANEL_CHECKBOX_5, &bool);
	ischecked[5] = bool;
	GetCtrlVal(panelHandle, PANEL_CHECKBOX_6, &bool);
	ischecked[6] = bool;
	GetCtrlVal(panelHandle, PANEL_CHECKBOX_7, &bool);
	ischecked[7] = bool;
	GetCtrlVal(panelHandle, PANEL_CHECKBOX_8, &bool);
	ischecked[8] = bool;
	GetCtrlVal(panelHandle, PANEL_CHECKBOX_9, &bool);
	ischecked[9] = bool;
	GetCtrlVal(panelHandle, PANEL_CHECKBOX_10, &bool);
	ischecked[10] = bool;
}
//***************************************************************************************************

int CVICALLBACK DIGTABLE_CALLBACK(int panel, int control, int event,
								  void *callbackData, int eventData1, int eventData2)
{
	int page, digval;
	Point pval = {0, 0};

	ChangedVals = TRUE;
	page = GetPage();
	switch (event)
	{
	case EVENT_LEFT_DOUBLE_CLICK:
		GetActiveTableCell(PANEL, PANEL_DIGTABLE, &pval);
		digval = DigTableValues[pval.x][pval.y][page];
		if (digval == 0)
		{
			SetTableCellAttribute(panelHandle, PANEL_DIGTABLE, pval, ATTR_TEXT_BGCOLOR, VAL_RED);
			DigTableValues[pval.x][pval.y][page] = 1;
		}
		else
		{
			SetTableCellAttribute(panelHandle, PANEL_DIGTABLE, pval, ATTR_TEXT_BGCOLOR, VAL_GRAY);
			DigTableValues[pval.x][pval.y][page] = 0;
		}
		break;
	}
	return 0;
}

//***************************************************************************************************
int CVICALLBACK TIMETABLE_CALLBACK(int panel, int control, int event,
								   void *callbackData, int eventData1, int eventData2)
{
	double dval, tscaleold;
	int page, i, j;
	ChangedVals = TRUE;
	switch (event)
	{
	case EVENT_COMMIT: //EVENT_VAL_CHANGED:
					   //EVENT_VAL_CHANGED only seems to pick up the last changed values, or values changed using the
					   // May 31,2006 -- TimeArray is updated just fine (Dave Burns)
					   //controls (i.e. increment arrows).  Similar problems with other events...reading all times(for the page)
					   // at a change event seems to work.
		page = GetPage();

		for (i = 1; i <= NUMBEROFCOLUMNS; i++)
		{
			double oldtime = TimeArray[i][page];
			GetTableCellVal(PANEL, PANEL_TIMETABLE, MakePoint(i, 1), &dval);

			TimeArray[i][page] = dval; //double TimeArray[NUMBEROFCOLUMNS][NUMBEROFPAGES];
			//now rescale the time scale for waveform fcn. Go over all 16 analog channels
			double ratio = dval / oldtime;
			if (oldtime == 0)
			{
				ratio = 1;
			}

			for (j = 1; j <= NUMBERANALOGCHANNELS; j++)
			{
				tscaleold = AnalogTable[i][j][page].tscale; // use this  and next line to auto-scale the time
				AnalogTable[i][j][page].tscale = tscaleold * ratio;
				//		AnalogTable[i][j][page].tscale=dval;	  //use this line to force timescale to period
			}

			//set the delta time value for the dds array
			//	ddstable[i][page].delta_time = dval/1000;
		}

		/* for testing prints TimeArray to STDIO
			for(i=1;i<=NUMBEROFCOLUMNS;i++)
				printf("%0.1f\t",TimeArray[i][page]);*/

		break;
	case EVENT_LEFT_DOUBLE_CLICK:
		isdimmed = 0;
		DrawNewTable(isdimmed);
		break;
	}
	return 0;
}

//*********************************************************************************

void CVICALLBACK MENU_DEBUG_CALLBACK(int menuBar, int menuItem, void *callbackData,
									 int panel)
{
	int status;
	GetCtrlAttribute(panelHandle, PANEL_DEBUG, ATTR_VISIBLE, &status);
	if (status == 0)
	{
		status = 1;
	}
	else
	{
		status = 0;
	}
	SetCtrlAttribute(panelHandle, PANEL_DEBUG, ATTR_VISIBLE, status);
}
// Callbacks for changing control button labels. Found under Settings > Control Text > Stage n
void CVICALLBACK TITLE1_CALLBACK(int menuBar, int menuItem, void *callbackData,
								 int panel)
{
	char buff[80];
	int status = PromptPopup("Enter control button label", "Enter a new label for Phase 1 control button", buff, sizeof buff - 2);
	if (status == 0)
	{
		SetCtrlAttribute(panelHandle, PANEL_TB_SHOWPHASE1, ATTR_ON_TEXT, buff);
		SetCtrlAttribute(panelHandle, PANEL_TB_SHOWPHASE1, ATTR_OFF_TEXT, buff);
	}
}

void CVICALLBACK TITLE2_CALLBACK(int menuBar, int menuItem, void *callbackData,
								 int panel)
{
	char buff[80];
	int status = PromptPopup("Enter control button label", "Enter a new label for Phase 2 control button", buff, sizeof buff - 2);
	if (status == 0)
	{
		SetCtrlAttribute(panelHandle, PANEL_TB_SHOWPHASE2, ATTR_OFF_TEXT, buff);
		SetCtrlAttribute(panelHandle, PANEL_TB_SHOWPHASE2, ATTR_ON_TEXT, buff);
	}
}

void CVICALLBACK TITLE3_CALLBACK(int menuBar, int menuItem, void *callbackData,
								 int panel)
{
	char buff[80];
	int status = PromptPopup("Enter control button label", "Enter a new label for Phase 3 control button", buff, sizeof buff - 2);
	if (status == 0)
	{
		SetCtrlAttribute(panelHandle, PANEL_TB_SHOWPHASE3, ATTR_OFF_TEXT, buff);
		SetCtrlAttribute(panelHandle, PANEL_TB_SHOWPHASE3, ATTR_ON_TEXT, buff);
	}
}

void CVICALLBACK TITLE4_CALLBACK(int menuBar, int menuItem, void *callbackData,
								 int panel)
{
	char buff[80];
	int status = PromptPopup("Enter control button label", "Enter a new label for Phase 4 control button", buff, sizeof buff - 2);
	if (status == 0)
	{
		SetCtrlAttribute(panelHandle, PANEL_TB_SHOWPHASE4, ATTR_OFF_TEXT, buff);
		SetCtrlAttribute(panelHandle, PANEL_TB_SHOWPHASE4, ATTR_ON_TEXT, buff);
	}
}

void CVICALLBACK TITLE5_CALLBACK(int menuBar, int menuItem, void *callbackData,
								 int panel)
{
	char buff[80];
	int status = PromptPopup("Enter control button label", "Enter a new label for Phase 1 control button", buff, sizeof buff - 2);
	if (status == 0)
	{
		SetCtrlAttribute(panelHandle, PANEL_TB_SHOWPHASE5, ATTR_OFF_TEXT, buff);
		SetCtrlAttribute(panelHandle, PANEL_TB_SHOWPHASE5, ATTR_ON_TEXT, buff);
	}
}

void CVICALLBACK TITLE6_CALLBACK(int menuBar, int menuItem, void *callbackData,
								 int panel)
{
	char buff[80];
	int status = PromptPopup("Enter control button label", "Enter a new label for Phase 6 control button", buff, sizeof buff - 2);
	if (status == 0)
	{
		SetCtrlAttribute(panelHandle, PANEL_TB_SHOWPHASE6, ATTR_OFF_TEXT, buff);
		SetCtrlAttribute(panelHandle, PANEL_TB_SHOWPHASE6, ATTR_ON_TEXT, buff);
	}
}

void CVICALLBACK TITLE7_CALLBACK(int menuBar, int menuItem, void *callbackData,
								 int panel)
{
	char buff[80];
	int status = PromptPopup("Enter control button label", "Enter a new label for Phase 7 control button", buff, sizeof buff - 2);
	if (status == 0)
	{
		SetCtrlAttribute(panelHandle, PANEL_TB_SHOWPHASE7, ATTR_OFF_TEXT, buff);
		SetCtrlAttribute(panelHandle, PANEL_TB_SHOWPHASE7, ATTR_ON_TEXT, buff);
	}
}

void CVICALLBACK TITLE8_CALLBACK(int menuBar, int menuItem, void *callbackData,
								 int panel)
{
	char buff[80];
	int status = PromptPopup("Enter control button label", "Enter a new label for Phase 8 control button", buff, sizeof buff - 2);
	if (status == 0)
	{
		SetCtrlAttribute(panelHandle, PANEL_TB_SHOWPHASE8, ATTR_OFF_TEXT, buff);
		SetCtrlAttribute(panelHandle, PANEL_TB_SHOWPHASE8, ATTR_ON_TEXT, buff);
	}
}

void CVICALLBACK TITLE9_CALLBACK(int menuBar, int menuItem, void *callbackData,
								 int panel)
{
	char buff[80];
	int status = PromptPopup("Enter control button label", "Enter a new label for Phase 9 control button", buff, sizeof buff - 2);
	if (status == 0)
	{
		SetCtrlAttribute(panelHandle, PANEL_TB_SHOWPHASE9, ATTR_OFF_TEXT, buff);
		SetCtrlAttribute(panelHandle, PANEL_TB_SHOWPHASE9, ATTR_ON_TEXT, buff);
	}
}

void CVICALLBACK TITLEX_CALLBACK(int menuBar, int menuItem, void *callbackData,
								 int panel)
{

	char buff[80];
	int status = PromptPopup("Enter control button label", "Enter a new label for Phase 10 control button", buff, sizeof buff - 2);
	if (status == 0)
	{
		SetCtrlAttribute(panelHandle, PANEL_TB_SHOWPHASE10, ATTR_OFF_TEXT, buff);
		SetCtrlAttribute(panelHandle, PANEL_TB_SHOWPHASE10, ATTR_ON_TEXT, buff);
	}
}

void CVICALLBACK SETGD5_CALLBACK(int menuBar, int menuItem, void *callbackData,
								 int panel)
{
	SetMenuBarAttribute(menuHandle, MENU_UPDATEPERIOD_SETGD5, ATTR_CHECKED, 1);
	SetMenuBarAttribute(menuHandle, MENU_UPDATEPERIOD_SETGD10, ATTR_CHECKED, 0);
	SetMenuBarAttribute(menuHandle, MENU_UPDATEPERIOD_SETGD100, ATTR_CHECKED, 0);
	SetMenuBarAttribute(menuHandle, MENU_UPDATEPERIOD_SETGD1000, ATTR_CHECKED, 0);
	EventPeriod = 0.005;
}

void CVICALLBACK SETGD10_CALLBACK(int menuBar, int menuItem, void *callbackData,
								  int panel)
{
	SetMenuBarAttribute(menuHandle, MENU_UPDATEPERIOD_SETGD5, ATTR_CHECKED, 0);
	SetMenuBarAttribute(menuHandle, MENU_UPDATEPERIOD_SETGD10, ATTR_CHECKED, 1);
	SetMenuBarAttribute(menuHandle, MENU_UPDATEPERIOD_SETGD100, ATTR_CHECKED, 0);
	SetMenuBarAttribute(menuHandle, MENU_UPDATEPERIOD_SETGD1000, ATTR_CHECKED, 0);
	EventPeriod = 0.010;
}

void CVICALLBACK SETGD100_CALLBACK(int menuBar, int menuItem, void *callbackData,
								   int panel)
{
	SetMenuBarAttribute(menuHandle, MENU_UPDATEPERIOD_SETGD5, ATTR_CHECKED, 0);
	SetMenuBarAttribute(menuHandle, MENU_UPDATEPERIOD_SETGD10, ATTR_CHECKED, 0);
	SetMenuBarAttribute(menuHandle, MENU_UPDATEPERIOD_SETGD100, ATTR_CHECKED, 1);
	SetMenuBarAttribute(menuHandle, MENU_UPDATEPERIOD_SETGD1000, ATTR_CHECKED, 0);
	EventPeriod = 0.100;
}

void CVICALLBACK SETGD1000_CALLBACK(int menuBar, int menuItem, void *callbackData,
									int panel)
{
	SetMenuBarAttribute(menuHandle, MENU_UPDATEPERIOD_SETGD5, ATTR_CHECKED, 0);
	SetMenuBarAttribute(menuHandle, MENU_UPDATEPERIOD_SETGD10, ATTR_CHECKED, 0);
	SetMenuBarAttribute(menuHandle, MENU_UPDATEPERIOD_SETGD100, ATTR_CHECKED, 0);
	SetMenuBarAttribute(menuHandle, MENU_UPDATEPERIOD_SETGD1000, ATTR_CHECKED, 1);
	EventPeriod = 1.00;
}

//*********************************************************************************************************

void CVICALLBACK BOOTADWIN_CALLBACK(int menuBar, int menuItem, void *callbackData,
									int panel)
{

	Boot("C:\\ADWIN\\ADWIN11.BTL", 0);
	processnum = Load_Process("TransferData_May25_2012.TB1"); //Updated May 25, 2012 - Seth Aubin
}
//*********************************************************************************************************

void CVICALLBACK BOOTLOAD_CALLBACK(int menuBar, int menuItem, void *callbackData,
								   int panel)
{
	int ischecked = 0;
	GetMenuBarAttribute(menuHandle, MENU_FILE_BOOTLOAD, ATTR_CHECKED, &ischecked);
	SetMenuBarAttribute(menuHandle, MENU_FILE_BOOTLOAD, ATTR_CHECKED, abs(ischecked - 1));
}
//**********************************************************************************************
void CVICALLBACK CLEARPANEL_CALLBACK(int menuBar, int menuItem, void *callbackData,
									 int panel)
{
	// add code to clear all the analog and digital information.....
	int status = ConfirmPopup("Clear Panel", "Do you really want to clear the panel?");
	if (status == 1)
	{
		ChangedVals = 1;
		for (int i = 1; i <= NUMBEROFCOLUMNS; i++)
		{
			for (int j = 1; j <= NUMBERANALOGCHANNELS; j++)
			{
				for (int k = 0; k < NUMBEROFPAGES; k++)
				{
					AnalogTable[i][j][k].fcn = 1;
					AnalogTable[i][j][k].fval = 0;
					AnalogTable[i][j][k].tscale = 0;
					DigTableValues[i][j][k] = 0;
					TimeArray[i][k] = 0;
					ddstable[i][k].start_frequency = 0;
					ddstable[i][k].end_frequency = 0;
					ddstable[i][k].amplitude = 0;
					ddstable[i][k].is_stop = 0;
					dds2table[i][k].start_frequency = 0;
					dds2table[i][k].end_frequency = 0;
					dds2table[i][k].amplitude = 0;
					dds2table[i][k].is_stop = 0;
					dds3table[i][k].start_frequency = 0;
					dds3table[i][k].end_frequency = 0;
					dds3table[i][k].amplitude = 0;
					dds3table[i][k].is_stop = 0;
				}
			}
		}
		DrawNewTable(0);
	}
}
//**********************************************************************************************

void CVICALLBACK INSERTCOLUMN_CALLBACK(int menuBar, int menuItem, void *callbackData,
									   int panel)
{
	char buff[20] = "", buff2[100] = "";
	int page, status;
	Point cpoint = {0, 0};
	page = GetPage();

	GetActiveTableCell(panelHandle, PANEL_TIMETABLE, &cpoint);
	sprintf(buff, "%d", cpoint.x);
	strcat(buff2, "Really insert column at ");
	strcat(buff2, buff);
	status = ConfirmPopup("insert Array", buff2); //User Confirmation of Column Insert
	if (status == 1)
		ShiftColumn3(cpoint.x, page, -1); //-1 for shifting columns right
}

//**********************************************************************************************

void CVICALLBACK DELETECOLUMN_CALLBACK(int menuBar, int menuItem, void *callbackData,
									   int panel)
{
	char buff[20] = "", buff2[100] = "";
	int page, status;
	Point cpoint = {0, 0};
	page = GetPage();
	//PromptPopup ("Array Manipulation:Delete", "Delete which column?", buff, 3);
	GetActiveTableCell(panelHandle, PANEL_TIMETABLE, &cpoint);
	sprintf(buff, "%d", cpoint.x);

	strcat(buff2, "Really delete column ");
	strcat(buff2, buff);
	status = ConfirmPopup("Delete Array", buff2);

	if (status == 1)
		ShiftColumn3(cpoint.x, page, 1); //1 for shifting columns left
}
//**********************************************************************************************
void ShiftColumn3(int col, int page, int dir)
//Takes all columns starting at col on page and shifts them to the left or right depending on the value of dir
//dir==1 for a deletion: all the columns starting at col+1 are shifted one to the left, the page's last column is new
//dir==-1 for an insertion: all the columns starting at col are shifted one to the right, the column at col is new and the
//		  last column is lost
//The new column can be set with all values at 0 or with the attributes previously in place

//Replaced Malfunctioning ShiftColumn and ShiftColumn2(see previous AdwinGUI releases)
{

	int i, j, status, start, zerocol;
	printf("col %d", col);

	if (dir == -1) //shifts cols right (insertion)
		start = NUMBEROFCOLUMNS;
	else if (dir == 1) //shifts cols left  (deletion)
		start = col;

	//shift columns left or right depending on dir
	for (i = 0; i < NUMBEROFCOLUMNS - col; i++)
	{
		TimeArray[start + dir * i][page] = TimeArray[start + dir * (i + 1)][page];
		for (j = 1; j <= NUMBERANALOGCHANNELS; j++)
		{
			AnalogTable[start + dir * i][j][page].fcn = AnalogTable[start + dir * (i + 1)][j][page].fcn;
			AnalogTable[start + dir * i][j][page].fval = AnalogTable[start + dir * (i + 1)][j][page].fval;
			AnalogTable[start + dir * i][j][page].tscale = AnalogTable[start + dir * (i + 1)][j][page].tscale;
		}

		for (j = 1; j <= NUMBERDIGITALCHANNELS; j++)
			DigTableValues[start + dir * i][j][page] = DigTableValues[start + dir * (i + 1)][j][page];

		ddstable[start + dir * i][page].start_frequency = ddstable[start + dir * (i + 1)][page].start_frequency;
		ddstable[start + dir * i][page].end_frequency = ddstable[start + dir * (i + 1)][page].end_frequency;
		ddstable[start + dir * i][page].amplitude = ddstable[start + dir * (i + 1)][page].amplitude;
		ddstable[start + dir * i][page].is_stop = ddstable[start + dir * (i + 1)][page].is_stop;

		dds2table[start + dir * i][page].start_frequency = dds2table[start + dir * (i + 1)][page].start_frequency;
		dds2table[start + dir * i][page].end_frequency = dds2table[start + dir * (i + 1)][page].end_frequency;
		dds2table[start + dir * i][page].amplitude = dds2table[start + dir * (i + 1)][page].amplitude;
		dds2table[start + dir * i][page].is_stop = dds2table[start + dir * (i + 1)][page].is_stop;

		dds3table[start + dir * i][page].start_frequency = dds3table[start + dir * (i + 1)][page].start_frequency;
		dds3table[start + dir * i][page].end_frequency = dds3table[start + dir * (i + 1)][page].end_frequency;
		dds3table[start + dir * i][page].amplitude = dds3table[start + dir * (i + 1)][page].amplitude;
		dds3table[start + dir * i][page].is_stop = dds3table[start + dir * (i + 1)][page].is_stop;
	}

	//if we inserted a column, then set all values to zero
	// prompt and ask if we want to duplicate the selected column

	if (dir == 1)
	{
		zerocol = NUMBEROFCOLUMNS;
		status = ConfirmPopup("Duplicate", "Do you want the last column to be duplicated?");
	}
	else if (dir == -1)
	{
		zerocol = col;
		status = ConfirmPopup("Duplicate", "Do you want to duplicate the selected column?");
	}

	//Sets all values to zero
	if (status != 1)
	{
		if (dir == 1)
			TimeArray[zerocol][page] = 0;

		for (j = 1; j <= NUMBERANALOGCHANNELS; j++)
		{
			AnalogTable[zerocol][j][page].fcn = 1;
			AnalogTable[zerocol][j][page].fval = 0;
			AnalogTable[zerocol][j][page].tscale = 1;
		}

		for (j = 1; j <= NUMBERDIGITALCHANNELS; j++)
			DigTableValues[start + dir * i][j][page] = DigTableValues[start + dir * (i + 1)][j][page];

		ddstable[zerocol][page].start_frequency = 0;
		ddstable[zerocol][page].end_frequency = 0;
		ddstable[zerocol][page].amplitude = 0;
		ddstable[zerocol][page].is_stop = FALSE;

		dds2table[zerocol][page].start_frequency = 0;
		dds2table[zerocol][page].end_frequency = 0;
		dds2table[zerocol][page].amplitude = 0;
		dds2table[zerocol][page].is_stop = FALSE;

		dds3table[zerocol][page].start_frequency = 0;
		dds3table[zerocol][page].end_frequency = 0;
		dds3table[zerocol][page].amplitude = 0;
		dds3table[zerocol][page].is_stop = FALSE;
	}
	ChangedVals = 1;
	DrawNewTable(0);
}
//***************************************************************************************************************

void CVICALLBACK COPYCOLUMN_CALLBACK(int menuBar, int menuItem, void *callbackData,
									 int panel)
{
	//All attributes of active column are replaced with those of the global "clip" variables (from ClipColumn)

	char buff[20] = "", buff2[100] = "";
	Point cpoint = {0, 0};

	int page = GetPage();
	GetActiveTableCell(panelHandle, PANEL_TIMETABLE, &cpoint);

	//User Confirmation of Selected Column
	sprintf(buff, "%d", cpoint.x);
	strcat(buff2, "Confirm Copy of column ");
	strcat(buff2, buff);
	int status = ConfirmPopup("Array Manipulation:Copy", buff2);

	if (status == 1)
	{
		ClipColumn = cpoint.x;
		TimeClip = TimeArray[cpoint.x][page];
		for (int j = 1; j <= NUMBERANALOGCHANNELS; j++)
		{
			AnalogClip[j].fcn = AnalogTable[cpoint.x][j][page].fcn;
			AnalogClip[j].fval = AnalogTable[cpoint.x][j][page].fval;
			AnalogClip[j].tscale = AnalogTable[cpoint.x][j][page].tscale;
		}

		for (j = 1; j <= NUMBERDIGITALCHANNELS; j++)
			DigClip[j] = DigTableValues[cpoint.x][j][page];

		ddsclip.start_frequency = ddstable[cpoint.x][page].start_frequency;
		ddsclip.end_frequency = ddstable[cpoint.x][page].end_frequency;
		ddsclip.amplitude = ddstable[cpoint.x][page].amplitude;
		ddsclip.is_stop = ddstable[cpoint.x][page].is_stop;

		dds2clip.start_frequency = dds2table[cpoint.x][page].start_frequency;
		dds2clip.end_frequency = dds2table[cpoint.x][page].end_frequency;
		dds2clip.amplitude = dds2table[cpoint.x][page].amplitude;
		dds2clip.is_stop = dds2table[cpoint.x][page].is_stop;

		dds3clip.start_frequency = dds3table[cpoint.x][page].start_frequency;
		dds3clip.end_frequency = dds3table[cpoint.x][page].end_frequency;
		dds3clip.amplitude = dds3table[cpoint.x][page].amplitude;
		dds3clip.is_stop = dds3table[cpoint.x][page].is_stop;

		DrawNewTable(0); //draws undimmed table if already dimmed
	}
}
//**********************************************************************************

void CVICALLBACK PASTECOLUMN_CALLBACK(int menuBar, int menuItem, void *callbackData,
									  int panel)

// Replaces all the values in the selected column with the global "clip" values

{
	int page;
	Point cpoint = {0, 0};
	ChangedVals = 1;
	page = GetPage();

	//Ensures a column has been copied to the clipboard
	if (ClipColumn > 0)
	{
		char buff[100] = "";
		// User Confirmation of Copy Function
		GetActiveTableCell(panelHandle, PANEL_TIMETABLE, &cpoint);
		sprintf(buff, "Confirm Copy of Column %d to %d?\nContents of Column %d will be lost.", ClipColumn, cpoint.x, cpoint.x);
		int status = ConfirmPopup("Paste Column", buff);

		if (status == 1)
		{
			TimeArray[cpoint.x][page] = TimeClip;
			for (int j = 1; j <= NUMBERANALOGCHANNELS; j++)
			{
				AnalogTable[cpoint.x][j][page].fcn = AnalogClip[j].fcn;
				AnalogTable[cpoint.x][j][page].fval = AnalogClip[j].fval;
				AnalogTable[cpoint.x][j][page].tscale = AnalogClip[j].tscale;
				DigTableValues[cpoint.x][j][page] = DigClip[j];
			}
			ddstable[cpoint.x][page].start_frequency = ddsclip.start_frequency;
			ddstable[cpoint.x][page].end_frequency = ddsclip.end_frequency;
			ddstable[cpoint.x][page].amplitude = ddsclip.amplitude;
			ddstable[cpoint.x][page].is_stop = ddsclip.is_stop;

			dds2table[cpoint.x][page].start_frequency = dds2clip.start_frequency;
			dds2table[cpoint.x][page].end_frequency = dds2clip.end_frequency;
			dds2table[cpoint.x][page].amplitude = dds2clip.amplitude;
			dds2table[cpoint.x][page].is_stop = dds2clip.is_stop;

			dds3table[cpoint.x][page].start_frequency = dds3clip.start_frequency;
			dds3table[cpoint.x][page].end_frequency = dds3clip.end_frequency;
			dds3table[cpoint.x][page].amplitude = dds3clip.amplitude;
			dds3table[cpoint.x][page].is_stop = dds3clip.is_stop;

			DrawNewTable(0);
		}
	}
	else
		ConfirmPopup("Copy Column", "No Column Selected");
}
//**********************************************************************************

int CVICALLBACK TGLNUMERIC_CALLBACK(int panel, int control, int event,
									void *callbackData, int eventData1, int eventData2)
{
	int val = 0;
	switch (event)
	{
	case EVENT_COMMIT:
		// find out if the button is pressed or not
		// change buttons to appropriate mode and draw new table
		GetCtrlVal(panelHandle, PANEL_TGL_NUMERICTABLE, &val);
		if (val == 0)
		{
			SetDisplayType(VAL_CELL_NUMERIC);
		}
		else
		{
			SetDisplayType(VAL_CELL_PICTURE);
		}

		break;
	}
	return 0;
}
//**********************************************************************************
void CVICALLBACK DDSSETUP_CALLBACK(int menuBar, int menuItem, void *callbackData,
								   int panel)
{
	LoadDDSSettings();
	DisplayPanel(panelHandle5);
}

//**********************************************************************************
void LoadArrays(char savedname[500], int csize)
{
	/* Laods all Panel attributes and values which are not saved automatically by the NI function SavePanelState.
	   the values are stored in the .arr file by SaveArrays
	   x,y, and zval give the 3D table dimensions but are not critical to the saving/loading operation

	   Note that if the lengths of any of the data arrays are changed previous saves will not be able to be laoded.
	   If necessary See AdwinGUI Panel Converter V11-V12 (created June 01, 2006)
	*/

	FILE *fdata;
	int xval = 16, yval = 16, zval = 10, updatePer;
	char buff[500] = "", buff2[100] = "";
	strncat(buff, savedname, csize - 4);
	strcat(buff, ".arr");
	if ((fdata = fopen(buff, "r")) == NULL)
	{
		MessagePopup("Load Error", "Failed to load data arrays");
		//	exit(1);
	}

	fread(&xval, sizeof xval, 1, fdata);
	fread(&yval, sizeof yval, 1, fdata);
	fread(&zval, sizeof zval, 1, fdata);
	//now for the times.
	fread(&TimeArray, (sizeof TimeArray), 1, fdata);
	//and the analog data
	fread(&AnalogTable, (sizeof AnalogTable), 1, fdata);
	fread(&DigTableValues, (sizeof DigTableValues), 1, fdata);
	fread(&AChName, (sizeof AChName), 1, fdata);
	fread(&DChName, sizeof DChName, 1, fdata);
	fread(&ddstable, (sizeof ddstable), 1, fdata);
	fread(&buff2, sizeof buff2, 1, fdata);

	SetCtrlAttribute(panelHandle, PANEL_TB_SHOWPHASE1, ATTR_OFF_TEXT, buff2);
	SetCtrlAttribute(panelHandle, PANEL_TB_SHOWPHASE1, ATTR_ON_TEXT, buff2);
	fread(&buff2, sizeof buff2, 1, fdata);

	SetCtrlAttribute(panelHandle, PANEL_TB_SHOWPHASE2, ATTR_OFF_TEXT, buff2);
	SetCtrlAttribute(panelHandle, PANEL_TB_SHOWPHASE2, ATTR_ON_TEXT, buff2);
	fread(&buff2, sizeof buff2, 1, fdata);
	SetCtrlAttribute(panelHandle, PANEL_TB_SHOWPHASE3, ATTR_OFF_TEXT, buff2);
	SetCtrlAttribute(panelHandle, PANEL_TB_SHOWPHASE3, ATTR_ON_TEXT, buff2);
	fread(&buff2, sizeof buff2, 1, fdata);
	SetCtrlAttribute(panelHandle, PANEL_TB_SHOWPHASE4, ATTR_OFF_TEXT, buff2);
	SetCtrlAttribute(panelHandle, PANEL_TB_SHOWPHASE4, ATTR_ON_TEXT, buff2);
	fread(&buff2, sizeof buff2, 1, fdata);
	SetCtrlAttribute(panelHandle, PANEL_TB_SHOWPHASE5, ATTR_OFF_TEXT, buff2);
	SetCtrlAttribute(panelHandle, PANEL_TB_SHOWPHASE5, ATTR_ON_TEXT, buff2);
	fread(&buff2, sizeof buff2, 1, fdata);
	SetCtrlAttribute(panelHandle, PANEL_TB_SHOWPHASE6, ATTR_OFF_TEXT, buff2);
	SetCtrlAttribute(panelHandle, PANEL_TB_SHOWPHASE6, ATTR_ON_TEXT, buff2);
	fread(&buff2, sizeof buff2, 1, fdata);
	SetCtrlAttribute(panelHandle, PANEL_TB_SHOWPHASE7, ATTR_OFF_TEXT, buff2);
	SetCtrlAttribute(panelHandle, PANEL_TB_SHOWPHASE7, ATTR_ON_TEXT, buff2);
	fread(&dds2table, (sizeof dds2table), 1, fdata);
	fread(&dds3table, (sizeof dds3table), 1, fdata);

	//Update Period Retrieved and Set
	fread(&updatePer, sizeof updatePer, 1, fdata);
	if (updatePer == 5)
	{
		SetMenuBarAttribute(menuHandle, MENU_UPDATEPERIOD_SETGD5, ATTR_CHECKED, 1);
		EventPeriod = 0.005;
	}
	else
		SetMenuBarAttribute(menuHandle, MENU_UPDATEPERIOD_SETGD5, ATTR_CHECKED, 0);
	if (updatePer == 10)
	{
		SetMenuBarAttribute(menuHandle, MENU_UPDATEPERIOD_SETGD10, ATTR_CHECKED, 1);
		EventPeriod = 0.01;
	}
	else
		SetMenuBarAttribute(menuHandle, MENU_UPDATEPERIOD_SETGD10, ATTR_CHECKED, 0);
	if (updatePer == 100)
	{
		SetMenuBarAttribute(menuHandle, MENU_UPDATEPERIOD_SETGD100, ATTR_CHECKED, 1);
		EventPeriod = 0.1;
	}
	else
		SetMenuBarAttribute(menuHandle, MENU_UPDATEPERIOD_SETGD100, ATTR_CHECKED, 0);
	if (updatePer == 1000)
	{
		SetMenuBarAttribute(menuHandle, MENU_UPDATEPERIOD_SETGD1000, ATTR_CHECKED, 1);
		EventPeriod = 1;
	}
	else
		SetMenuBarAttribute(menuHandle, MENU_UPDATEPERIOD_SETGD1000, ATTR_CHECKED, 0);

	fread(&DDSFreq.extclock, sizeof DDSFreq.extclock, 1, fdata);
	fread(&DDSFreq.PLLmult, sizeof DDSFreq.PLLmult, 1, fdata);
	DDSFreq.clock = DDSFreq.extclock * DDSFreq.PLLmult;

	fclose(fdata);

	LoadDDSSettings(); //Enters the DDSsettings into the GUI Panel
	SetAnalogChannels();
	SetDigitalChannels();
}

//*****************************************************************************************
void SaveArrays(char savedname[500], int csize)
{
	/* Saves all Panel attributes and values which are not saved automatically by the NI function SavePanelState
	   The values are stored in the .arr file
	   x,y, and zval give the 3D table dimensions but are not critical to the saving/loading operation

	   Note that if the lengths of any of the data arrays are changed previous saves will not be able to be laoded.
	   If necessary See AdwinGUI Panel Converter V11-V12 (created June 01, 2006)
	*/

	FILE *fdata;
	int xval = NUMBEROFCOLUMNS, yval = NUMBERANALOGCHANNELS + NUMBERDIGITALCHANNELS + NUMBERDDS, zval = NUMBEROFPAGES - 1;
	int usupd5, usupd10, usupd100, usupd1000, updatePer; //Update Period Check
	char buff[500] = "", buff2[100];
	strncpy(buff, savedname, csize - 4);
	strcat(buff, ".arr");
	if ((fdata = fopen(buff, "w")) == NULL)
	{
		//	InsertListItem(panelHandle,PANEL_DEBUG,-1,buff,1);
		strcpy(buff2, "Failed to save data arrays. \n Panel Filename received\n");
		strcat(buff2, buff);
		//	strcat(buff2,"\n Array File name attempted \n");
		//	strcat(buff2,buff);

		MessagePopup("Save Error", buff2);
	}

	fwrite(&xval, sizeof xval, 1, fdata);
	fwrite(&yval, sizeof yval, 1, fdata);
	fwrite(&zval, sizeof zval, 1, fdata);
	//now for the times.
	fwrite(&TimeArray, sizeof TimeArray, 1, fdata);
	//and the analog data
	fwrite(&AnalogTable, sizeof AnalogTable, 1, fdata);
	fwrite(&DigTableValues, sizeof DigTableValues, 1, fdata);

	fwrite(&AChName, sizeof AChName, 1, fdata);
	fwrite(&DChName, sizeof DChName, 1, fdata);
	fwrite(&ddstable, sizeof ddstable, 1, fdata);
	GetCtrlAttribute(panelHandle, PANEL_TB_SHOWPHASE1, ATTR_OFF_TEXT, buff2);
	fwrite(&buff2, sizeof buff2, 1, fdata);
	GetCtrlAttribute(panelHandle, PANEL_TB_SHOWPHASE2, ATTR_OFF_TEXT, buff2);
	fwrite(&buff2, sizeof buff2, 1, fdata);
	GetCtrlAttribute(panelHandle, PANEL_TB_SHOWPHASE3, ATTR_OFF_TEXT, buff2);
	fwrite(&buff2, sizeof buff2, 1, fdata);
	GetCtrlAttribute(panelHandle, PANEL_TB_SHOWPHASE4, ATTR_OFF_TEXT, buff2);
	fwrite(&buff2, sizeof buff2, 1, fdata);
	GetCtrlAttribute(panelHandle, PANEL_TB_SHOWPHASE5, ATTR_OFF_TEXT, buff2);
	fwrite(&buff2, sizeof buff2, 1, fdata);
	GetCtrlAttribute(panelHandle, PANEL_TB_SHOWPHASE6, ATTR_OFF_TEXT, buff2);
	fwrite(&buff2, sizeof buff2, 1, fdata);
	GetCtrlAttribute(panelHandle, PANEL_TB_SHOWPHASE7, ATTR_OFF_TEXT, buff2);
	fwrite(&buff2, sizeof buff2, 1, fdata);
	fwrite(&dds2table, sizeof dds2table, 1, fdata);
	fwrite(&dds3table, sizeof dds3table, 1, fdata);

	GetMenuBarAttribute(menuHandle, MENU_UPDATEPERIOD_SETGD5, ATTR_CHECKED, &usupd5);
	GetMenuBarAttribute(menuHandle, MENU_UPDATEPERIOD_SETGD10, ATTR_CHECKED, &usupd10);
	GetMenuBarAttribute(menuHandle, MENU_UPDATEPERIOD_SETGD100, ATTR_CHECKED, &usupd100);
	GetMenuBarAttribute(menuHandle, MENU_UPDATEPERIOD_SETGD1000, ATTR_CHECKED, &usupd1000);

	if (usupd5 == 1)
		updatePer = 5;
	else if (usupd10 == 1)
		updatePer = 10;
	else if (usupd100 == 1)
		updatePer = 100;
	else if (usupd1000 == 1)
		updatePer = 1000;
	fwrite(&updatePer, sizeof updatePer, 1, fdata);
	fwrite(&DDSFreq.extclock, sizeof DDSFreq.extclock, 1, fdata);
	fwrite(&DDSFreq.PLLmult, sizeof DDSFreq.PLLmult, 1, fdata);

	fclose(fdata);
}
//********************************************************************************************
int CVICALLBACK DISPLAYDIAL_CALLBACK(int panel, int control, int event,
									 void *callbackData, int eventData1, int eventData2)
{
	int dialval = 0;
	switch (event)
	{
	case EVENT_COMMIT:
		GetCtrlVal(panelHandle, PANEL_DISPLAYDIAL, &dialval);
		// Now change the size of the tables depending on the dial value.

		SetChannelDisplayed(dialval);

		break;
	}
	return 0;
}
/************************************************************************
Author: Stefan
-------
Date Created: August 2004
-------
Description: Resizes the analog table on the gui
-------
Return Value: void
-------
Parameters: new top, left and height values for the list box
*************************************************************************/
void ReshapeAnalogTable(int top, int left, int height)
{
	int tempint;
	int modheight;

	SetTableRowAttribute(panelHandle, PANEL_ANALOGTABLE, -1, ATTR_SIZE_MODE, VAL_USE_EXPLICIT_SIZE);
	SetTableRowAttribute(panelHandle, PANEL_ANALOGTABLE, -1, ATTR_ROW_HEIGHT, (height) / (NUMBERANALOGCHANNELS + NUMBERDDS));
	SetTableRowAttribute(panelHandle, PANEL_TBL_ANAMES, -1, ATTR_SIZE_MODE, VAL_USE_EXPLICIT_SIZE);
	SetTableRowAttribute(panelHandle, PANEL_TBL_ANAMES, -1, ATTR_ROW_HEIGHT, (height) / (NUMBERANALOGCHANNELS + NUMBERDDS));
	SetTableRowAttribute(panelHandle, PANEL_TBL_ANALOGUNITS, -1, ATTR_SIZE_MODE, VAL_USE_EXPLICIT_SIZE);
	SetTableRowAttribute(panelHandle, PANEL_TBL_ANALOGUNITS, -1, ATTR_ROW_HEIGHT, (height) / (NUMBERANALOGCHANNELS + NUMBERDDS));
	modheight = (NUMBERANALOGCHANNELS + NUMBERDDS) * (int)((height) / (NUMBERANALOGCHANNELS + NUMBERDDS)) + 3;

	//resize the analog table and all it's related list boxes
	SetCtrlAttribute(panelHandle, PANEL_ANALOGTABLE, ATTR_HEIGHT, modheight);
	SetCtrlAttribute(panelHandle, PANEL_ANALOGTABLE, ATTR_LEFT, left);
	SetCtrlAttribute(panelHandle, PANEL_ANALOGTABLE, ATTR_TOP, top);

	SetCtrlAttribute(panelHandle, PANEL_TBL_ANAMES, ATTR_LEFT, left - 165);
	SetCtrlAttribute(panelHandle, PANEL_TBL_ANAMES, ATTR_TOP, top);
	SetCtrlAttribute(panelHandle, PANEL_TBL_ANAMES, ATTR_HEIGHT, modheight);

	SetCtrlAttribute(panelHandle, PANEL_TBL_ANALOGUNITS, ATTR_HEIGHT, modheight);
	SetCtrlAttribute(panelHandle, PANEL_TBL_ANALOGUNITS, ATTR_TOP, top);
	SetCtrlAttribute(panelHandle, PANEL_TBL_ANALOGUNITS, ATTR_LEFT, left + 705);

	// move the DDS offsets
	tempint = height / 27;
	SetCtrlAttribute(panelHandle, PANEL_NUM_DDS_OFFSET, ATTR_TOP, top + height - 3 * tempint - 6);
	SetCtrlAttribute(panelHandle, PANEL_NUM_DDS_OFFSET, ATTR_LEFT, 880);
	SetCtrlAttribute(panelHandle, PANEL_NUM_DDS2_OFFSET, ATTR_TOP, top + height - 2 * tempint - 5);
	SetCtrlAttribute(panelHandle, PANEL_NUM_DDS2_OFFSET, ATTR_LEFT, 880);
	SetCtrlAttribute(panelHandle, PANEL_NUM_DDS3_OFFSET, ATTR_TOP, top + height - 1 * tempint - 4);
	SetCtrlAttribute(panelHandle, PANEL_NUM_DDS3_OFFSET, ATTR_LEFT, 880);
}
/************************************************************************
Author: Stefan
-------
Date Created: August 2004
-------
Description: Resizes the digital table on the gui
-------
Return Value: void
-------
Parameters: new top, left and height values for the list box
*************************************************************************/
void ReshapeDigitalTable(int top, int left, int height)
{
	int j;

	SetCtrlAttribute(panelHandle, PANEL_DIGTABLE, ATTR_HEIGHT, height);
	SetCtrlAttribute(panelHandle, PANEL_DIGTABLE, ATTR_LEFT, left);
	SetCtrlAttribute(panelHandle, PANEL_DIGTABLE, ATTR_TOP, top);
	SetCtrlAttribute(panelHandle, PANEL_TBL_DIGNAMES, ATTR_TOP, top);
	SetCtrlAttribute(panelHandle, PANEL_TBL_DIGNAMES, ATTR_LEFT, left - 165);
	SetCtrlAttribute(panelHandle, PANEL_TBL_DIGNAMES, ATTR_HEIGHT, height);

	for (j = 1; j <= NUMBERDIGITALCHANNELS; j++)
	{
		SetTableRowAttribute(panelHandle, PANEL_DIGTABLE, j, ATTR_SIZE_MODE, VAL_USE_EXPLICIT_SIZE);
		SetTableRowAttribute(panelHandle, PANEL_DIGTABLE, j, ATTR_ROW_HEIGHT, (height) / (NUMBERDIGITALCHANNELS));
		SetTableRowAttribute(panelHandle, PANEL_TBL_DIGNAMES, j, ATTR_SIZE_MODE, VAL_USE_EXPLICIT_SIZE);

		SetTableRowAttribute(panelHandle, PANEL_TBL_DIGNAMES, j, ATTR_ROW_HEIGHT, (height) / (NUMBERDIGITALCHANNELS));
	}
}
/************************************************************************
Author: David McKay
-------
Date Created: August 23, 2004
-------
Date Modified: August 23, 2004
-------
Description: Sets how to display the data, graphically or numerically
-------
Return Value: void
-------
Parameter1:
int display_setting: type of display
VAL_CELL_NUMERIC : graphic
VAL_CELL_PICTURE : numeric
*************************************************************************/
void SetDisplayType(int display_setting)
{

	int i, j;

	//set button status
	if (display_setting == VAL_CELL_NUMERIC)
	{
		SetCtrlVal(panelHandle, PANEL_TGL_NUMERICTABLE, 0);
	}
	else if (display_setting == VAL_CELL_PICTURE)
	{
		SetCtrlVal(panelHandle, PANEL_TGL_NUMERICTABLE, 1);
	}

	//	printf("called Display Type with value:   %d \n",display_setting);
	for (i = 1; i <= NUMBEROFCOLUMNS; i++)
	{
		for (j = 1; j <= NUMBERANALOGCHANNELS; j++)
		{
			SetTableCellAttribute(panelHandle, PANEL_ANALOGTABLE, MakePoint(i, j),
								  ATTR_CELL_TYPE, display_setting);
		}
	}
}

/************************************************************************
Author: David McKay
-------
Date Created: August 23, 2004
-------
Date Modified: August 23, 2004
-------
Description: Sets which channel to display: analog, digital or both
-------
Return Value: void
-------
Parameter1:
int display_setting: channel to display
1: both
2: analog
3: digital
*************************************************************************/
void SetChannelDisplayed(int display_setting)
{
	int toppos1, toppos2, leftpos, heightpos;
	//hide everything
	SetCtrlAttribute(panelHandle, PANEL_DIGTABLE, ATTR_VISIBLE, 0);
	SetCtrlAttribute(panelHandle, PANEL_ANALOGTABLE, ATTR_VISIBLE, 0);
	SetCtrlAttribute(panelHandle, PANEL_TBL_ANAMES, ATTR_VISIBLE, 0);
	SetCtrlAttribute(panelHandle, PANEL_TBL_ANALOGUNITS, ATTR_VISIBLE, 0);
	SetCtrlAttribute(panelHandle, PANEL_TBL_DIGNAMES, ATTR_VISIBLE, 0);
	//ATTR_LABEL_VISIBLE

	switch (display_setting)
	{
	case 1:				 //both
		heightpos = 550; // 380 for 25 rows works... use 410 for 27 rows
		toppos1 = 155;
		leftpos = 170;
		toppos2 = toppos1 + heightpos;
		//toppos2=1280-100-heightpos;
		//toppos2=155+380+50;

		ReshapeAnalogTable(toppos1, 170, heightpos); //passed top, left and height
		ReshapeDigitalTable(toppos2, 170, heightpos - 60);

		SetCtrlAttribute(panelHandle, PANEL_DIGTABLE, ATTR_VISIBLE, 1);
		SetCtrlAttribute(panelHandle, PANEL_ANALOGTABLE, ATTR_VISIBLE, 1);
		SetCtrlAttribute(panelHandle, PANEL_TBL_ANAMES, ATTR_VISIBLE, 1);
		SetCtrlAttribute(panelHandle, PANEL_TBL_ANALOGUNITS, ATTR_VISIBLE, 1);
		SetCtrlAttribute(panelHandle, PANEL_TBL_DIGNAMES, ATTR_VISIBLE, 1);

		break;

	case 2: //analog table

		ReshapeAnalogTable(155, 170, 600); //passed top, left and height

		SetCtrlAttribute(panelHandle, PANEL_ANALOGTABLE, ATTR_VISIBLE, 1);
		SetCtrlAttribute(panelHandle, PANEL_TBL_ANAMES, ATTR_VISIBLE, 1);
		SetCtrlAttribute(panelHandle, PANEL_TBL_ANALOGUNITS, ATTR_VISIBLE, 1);
		break;

	case 3: // digital table

		//passed top, left and height
		ReshapeDigitalTable(155, 170, 500);

		SetCtrlAttribute(panelHandle, PANEL_DIGTABLE, ATTR_VISIBLE, 1);
		SetCtrlAttribute(panelHandle, PANEL_TBL_DIGNAMES, ATTR_VISIBLE, 1);
		break;
	}

	SetCtrlVal(panelHandle, PANEL_DISPLAYDIAL, display_setting);
	DrawNewTable(0);
	return;
}
//***********************************************************************************************
double CheckIfWithinLimits(double OutputVoltage, int linenumber)
{
	double NewOutput;
	NewOutput = OutputVoltage;
	if (OutputVoltage > AChName[linenumber].maxvolts)
		NewOutput = AChName[linenumber].maxvolts;
	if (OutputVoltage < AChName[linenumber].minvolts)
		NewOutput = AChName[linenumber].minvolts;
	return NewOutput;
}
//***********************************************************************************************
void CVICALLBACK RESETZERO_CALLBACK(int
										menuBar,
									int menuItem, void *callbackData,
									int panel)
{
	int ischecked = 0;
	GetMenuBarAttribute(menuHandle, MENU_SETTINGS_RESETZERO, ATTR_CHECKED, &ischecked);
	SetMenuBarAttribute(menuHandle, MENU_SETTINGS_RESETZERO, ATTR_CHECKED, abs(ischecked - 1));
}
//***********************************************************************************************
void SaveLastGuiSettings(void)
{
	// Save settings:  First look for file .ini  This will be a simple 1 line file stating the name of the last file
	//saved.  Load this up and use as the starting name in the file dialog.
	char fname[100] = "c:\\LastGui.pan";
	SavePanelState(PANEL, fname, 1);
	SaveArrays(fname, strlen(fname));
}
//********************************************************************************************************************

void CVICALLBACK EXPORT_PANEL_CALLBACK(int menuBar, int menuItem, void *callbackData,
									   int panel)
{
	char fexportname[200];

	FileSelectPopup("", "*.export", "", "Export Panel?", VAL_SAVE_BUTTON, 0, 0, 1, 1, fexportname);
	ExportPanel(fexportname, strlen(fexportname));
}
//******************************************************************************************************
void ExportPanel(char fexportname[200], int fnamesize)
{

	FILE *fexport;
	int i = 0, j = 0, k = 0;
	char buff[500], bigbuff[2000];
	char fcnmode[6] = " LEJ"; // step, linear, exponential, const-jerk:  Step is assumed if blank
	double MetaTimeArray[500];
	int MetaDigitalArray[NUMBERDIGITALCHANNELS + 1][500];
	struct AnVals MetaAnalogArray[NUMBERANALOGCHANNELS + 1][500];
	double DDSfreq1[500], DDSfreq2[500], DDScurrent[500], DDSstop[500];
	int mindex, tsize;
	fcnmode[0] = ' ';
	fcnmode[1] = 'L';
	fcnmode[2] = 'E';
	fcnmode[3] = 'J';
	isdimmed = 1;

	if ((fexport = fopen(fexportname, "w")) == NULL)
	{
		MessagePopup("Save Error", "Failed to save configuration file");
	}

	//Lets build the times list first...so we know how long it will be.
	//check each page...find used columns and dim out unused....(with 0 or negative values)
	SetCtrlAttribute(panelHandle, PANEL_ANALOGTABLE, ATTR_TABLE_MODE, VAL_COLUMN);
	mindex = 0;
	for (k = 1; k <= NUMBEROFPAGES; k++) //go through for each page
	{
		if (ischecked[k] == 1) //if the page is selected
		{
			int nozerofound = 1;
			//go through for each time column
			for (i = 1; i < NUMBEROFCOLUMNS; i++)
			{
				if ((nozerofound == 1) && (TimeArray[i][k] > 0))
				//ignore all columns after the first time=0
				{
					mindex++; //increase the number of columns counter
					MetaTimeArray[mindex] = TimeArray[i][k];

					//go through for each analog channel
					for (j = 1; j <= NUMBERANALOGCHANNELS; j++)
					{
						MetaAnalogArray[j][mindex].fcn = AnalogTable[i][j][k].fcn;
						MetaAnalogArray[j][mindex].fval = AnalogTable[i][j][k].fval;
						MetaAnalogArray[j][mindex].tscale = AnalogTable[i][j][k].tscale;
						MetaDigitalArray[j][mindex] = DigTableValues[i][j][k];
						DDScurrent[mindex] = ddstable[i][k].amplitude;
						DDSfreq1[mindex] = ddstable[i][k].start_frequency;
						DDSfreq2[mindex] = ddstable[i][k].end_frequency;
						DDSstop[mindex] = ddstable[i][k].is_stop;
					}
				}
				else if (TimeArray[i][k] == 0)
				{
					nozerofound = 0;
				}
			}
		}
	}
	tsize = mindex; //tsize is the number of columns
	// now write to file
	// write header
	sprintf(bigbuff, "Time(ms)");
	for (i = 1; i <= tsize; i++)
	{
		strcat(bigbuff, ",");
		sprintf(buff, "%f", MetaTimeArray[i]);
		strcat(bigbuff, buff);
	}
	strcat(bigbuff, "\n");
	fprintf(fexport, bigbuff);
	//done header, now write analog lines
	for (j = 1; j <= NUMBERANALOGCHANNELS; j++)
	{
		sprintf(bigbuff, AChName[j].chname);
		for (i = 1; i <= tsize; i++)
		{
			strcat(bigbuff, ",");
			strncat(bigbuff, fcnmode + MetaAnalogArray[j][i].fcn - 1, 1);
			sprintf(buff, "%3.2f", MetaAnalogArray[j][i].fval);
			strcat(bigbuff, buff);
		}
		strcat(bigbuff, "\n");
		fprintf(fexport, bigbuff);
	}
	//done analog lines, now write DDS
	sprintf(bigbuff, "DDS");
	for (i = 1; i <= tsize; i++)
	{
		strcat(bigbuff, ",");
		if (DDSstop[i] == TRUE)
		{
			strcat(bigbuff, "OFF");
		}
		else
		{
			strcat(bigbuff, "ma");
			sprintf(buff, "%4.2f", DDScurrent[i]);
			strcat(bigbuff, buff);
			strcat(bigbuff, "_FA");
			sprintf(buff, "%4.2f", DDSfreq1[i]);
			strcat(bigbuff, buff);
			strcat(bigbuff, "_FB");
			sprintf(buff, "%4.2f", DDSfreq2[i]);
			strcat(bigbuff, buff);
		}
	}
	fprintf(fexport, bigbuff);
	//done DDS, now do digital
	for (j = 1; j <= NUMBERDIGITALCHANNELS; j++)
	{
		sprintf(bigbuff, DChName[j].chname);
		for (i = 1; i <= tsize; i++)
		{
			strcat(bigbuff, ",");
			sprintf(buff, "%d", MetaDigitalArray[j][i]);

			strcat(bigbuff, buff);
		}
		strcat(bigbuff, "\n");
		fprintf(fexport, bigbuff);
	}
	fclose(fexport);
}

//***********************************************************************************************

void CVICALLBACK CONFIG_EXPORT_CALLBACK(int menuBar, int menuItem, void *callbackData,
										int panel)
{

	FILE *fconfig;
	int i = 0;
	char buff[500], fconfigname[200], buff3[31];

	FileSelectPopup("", "*.config", "", "Save Configuration", VAL_SAVE_BUTTON, 0, 0, 1, 1, fconfigname);

	if ((fconfig = fopen(fconfigname, "w")) == NULL)
	{
		//	InsertListItem(panelHandle,PANEL_DEBUG,-1,buff,1);
		MessagePopup("Save Error", "Failed to save configuration file");
	}
	// write out analog channel info
	sprintf(buff, "Analog Channels\n");
	fprintf(fconfig, buff);
	sprintf(buff, "Row,Channel,Name,tbias,tfcn,MaxVolts,MinVolts,Units\n");
	fprintf(fconfig, buff);
	for (i = 1; i <= NUMBERANALOGCHANNELS; i++)
	{
		strncpy(buff3, AChName[i].chname, 30);
		sprintf(buff, "%d,%d,%s,%f,%f,%f,%f,%s\n", i, AChName[i].chnum, buff3, AChName[i].tbias, AChName[i].tfcn, AChName[i].maxvolts, AChName[i].minvolts, AChName[i].units);
		fprintf(fconfig, buff);
	}
	// Write out digital Channel info
	sprintf(buff, "Digital Channels\n");
	fprintf(fconfig, buff);
	sprintf(buff, "Row,Channel,Name\n");
	fprintf(fconfig, buff);

	for (i = 1; i <= NUMBERDIGITALCHANNELS; i++)
	{
		sprintf(buff, "%d,%d,%s\n", i, DChName[i].chnum, DChName[i].chname);
		fprintf(fconfig, buff);
	}

	fclose(fconfig);
}

void CVICALLBACK MENU_ALLLOW_CALLBACK(int menuBar, int menuItem, void *callbackData,
									  int panel)
{
}

void CVICALLBACK MENU_HOLD_CALLBACK(int menuBar, int menuItem, void *callbackData,
									int panel)
{
}

void CVICALLBACK MENU_BYCHANNEL_CALLBACK(int menuBar, int menuItem, void *callbackData,
										 int panel)
{
}

//**************************************************************************************************************

int CVICALLBACK NUM_INSERTIONPAGE_CALLBACK(int panel, int control, int event,
										   void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
	case EVENT_COMMIT:
		ChangedVals = TRUE;
	}
	return 0;
}
//**************************************************************************************************************
int CVICALLBACK NUM_INSERTIONCOL_CALLBACK(int panel, int control, int event,
										  void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
	case EVENT_COMMIT:
		ChangedVals = TRUE;
		break;
	}
	return 0;
}
//**************************************************************************************************************
void CVICALLBACK COMPRESSION_CALLBACK(int menuBar, int menuItem, void *callbackData,
									  int panel)
{
	BOOL UseCompression;
	GetMenuBarAttribute(menuHandle, MENU_PREFS_COMPRESSION, ATTR_CHECKED, &UseCompression);
	if (UseCompression)
	{
		SetMenuBarAttribute(menuHandle, MENU_PREFS_COMPRESSION, ATTR_CHECKED, FALSE);
	}
	else
	{
		SetMenuBarAttribute(menuHandle, MENU_PREFS_COMPRESSION, ATTR_CHECKED, TRUE);
	}
}

//**************************************************************************************************************
void CVICALLBACK SHOWARRAY_CALLBACK(int menuBar, int menuItem, void *callbackData,
									int panel)
{
	BOOL ShowArray;
	GetMenuBarAttribute(menuHandle, MENU_PREFS_SHOWARRAY, ATTR_CHECKED, &ShowArray);
	if (ShowArray)
	{
		SetMenuBarAttribute(menuHandle, MENU_PREFS_SHOWARRAY, ATTR_CHECKED, FALSE);
	}
	else
	{
		SetMenuBarAttribute(menuHandle, MENU_PREFS_SHOWARRAY, ATTR_CHECKED, TRUE);
	}
}
//**************************************************************************************************************
void CVICALLBACK DDS_OFF_CALLBACK(int menuBar, int menuItem, void *callbackData,
								  int panel)
{
	int i = 0, j = 0;
	for (j = 0; j < NUMBEROFPAGES; j++)
	{
		for (i = 0; i < NUMBEROFCOLUMNS; i++)
		{
			ddstable[i][j].is_stop = TRUE;
		}
	}
}

//**************************************************************************************************************

void CVICALLBACK SIMPLETIMING_CALLBACK(int menuBar, int menuItem, void *callbackData,
									   int panel)
{
	BOOL Simple_Timing;
	GetMenuBarAttribute(menuHandle, MENU_PREFS_SIMPLETIMING, ATTR_CHECKED, &Simple_Timing);
	if (Simple_Timing)
	{
		SetMenuBarAttribute(menuHandle, MENU_PREFS_SIMPLETIMING, ATTR_CHECKED, FALSE);
	}
	else
	{
		SetMenuBarAttribute(menuHandle, MENU_PREFS_SIMPLETIMING, ATTR_CHECKED, TRUE);
	}
}
//**************************************************************************************************************

//**************************************************************************************************************

void CVICALLBACK SCANSETTING_CALLBACK(int menuBar, int menuItem, void *callbackData,
									  int panel)
{
	InitializeScanPanel();
}

//**************************************************************************************************************

void CVICALLBACK NOTECHECK_CALLBACK(int menuBar, int menuItem, void *callbackData,
									int panel)
{
	BOOL status;
	GetMenuBarAttribute(menuHandle, MENU_SETTINGS_NOTECHECK, ATTR_CHECKED, &status);
	SetMenuBarAttribute(menuHandle, MENU_SETTINGS_NOTECHECK, ATTR_CHECKED, !status);
	//SetCtrlAttribute (panelHandle_sib1, PANEL_LABNOTE_TXT, ATTR_VISIBLE, !status);
	//SetCtrlAttribute (panelHandle, MENU_NOTECHECK, ATTR_VISIBLE, !status);
	if (status == 1)
	{
		DisplayPanel(panelHandle_sub1);
	}
	else
	{
		HidePanel(panelHandle_sub1);
	}
}

void CVICALLBACK STREAM_CALLBACK(int menuBar, int menuItem, void *callbackData,
								 int panel)
{
	BOOL status;
	GetMenuBarAttribute(menuHandle, MENU_PREFS_STREAM_SETTINGS, ATTR_CHECKED, &status);
	SetMenuBarAttribute(menuHandle, MENU_PREFS_STREAM_SETTINGS, ATTR_CHECKED, !status);
	if (!status == TRUE)
	{
		char fstreamdir[250];
		DirSelectPopup("", "Select Stream Folder", 1, 1, fstreamdir);
	}
}
//**************************************************************************************************************
void ExportScanBuffer(void)
{
	char fbuffername[250];

	int status = FileSelectPopup("", "*.scan", "", "Save Scan Buffer", VAL_SAVE_BUTTON, 0, 0, 1, 1, fbuffername);
	if (status > 0)
	{
		FILE *fbuffer;
		if ((fbuffer = fopen(fbuffername, "w")) == NULL)
		{
			MessagePopup("Save Error", "Failed to save configuration file");
		}
		char buff[500];
		switch (PScan.ScanMode)
		{
		default:
		case 0:
			sprintf(buff, "Analog Scan\nRow,%d,Column,%d,Page,%d\n", PScan.Row, PScan.Column, PScan.Page);
			break;
		case 1:
			sprintf(buff, "Time Scan\nColumn,%d,Page,%d\n", PScan.Column, PScan.Page);
			break;
		case 2:
			sprintf(buff, "DDS Scan\nColumn,%d,Page,%d\n", PScan.Column, PScan.Page);
			break;
		}
		fprintf(fbuffer, buff);
		sprintf(buff, "Cycle,Value,Step,Iteration,Time\n");
		fprintf(fbuffer, buff);
		for (int i = 0; i <= ScanBuffer[0].BufferSize; i++)
		{

			double val = ScanBuffer[i].Value;
			int step = ScanBuffer[i].Step;
			int iter = ScanBuffer[i].Iteration;
			char date[100];
			sprintf(date, ScanBuffer[i].Time);
			sprintf(buff, "%d,%f,%d,%d,%s\n", i, val, step, iter, date);
			fprintf(fbuffer, buff);
		}
		fclose(fbuffer);
	}
}
//**************************************************************************************************************
void MoveCanvasStart(int XPos, int IsVisible)
{
	int x0 = 170, dx = 40;

	SetCtrlAttribute(panelHandle, PANEL_CANVAS_START, ATTR_LEFT, x0 + (XPos - 1) * dx);
	SetCtrlAttribute(panelHandle, PANEL_CANVAS_START, ATTR_TOP, 141);
	SetCtrlAttribute(panelHandle, PANEL_CANVAS_START, ATTR_VISIBLE, IsVisible);
}
//**************************************************************************************************************
void MoveCanvasEnd(int XPos, int IsVisible)
{
	int x0 = 170, dx = 40, xwidth = 25;

	SetCtrlAttribute(panelHandle, PANEL_CANVAS_END, ATTR_LEFT, x0 + (XPos - 1) * dx + xwidth);
	SetCtrlAttribute(panelHandle, PANEL_CANVAS_END, ATTR_TOP, 141);
	SetCtrlAttribute(panelHandle, PANEL_CANVAS_END, ATTR_VISIBLE, IsVisible);
}

//**************************************************************************************************************
//Jan24, 2006
// ensure start/stop ordering is correct, if not, then swap them
// Move Loop start and stop indicators, and refresh display
int CVICALLBACK SWITCH_LOOP_CALLBACK(int panel, int control, int event,
									 void *callbackData, int eventData1, int eventData2)
{
	int pagestart, pageend, colstart, colend;
	BOOL ValuesGood, LoopSwitchOn;
	switch (event)
	{
	case EVENT_COMMIT:
		ValuesGood = FALSE;
		LoopSwitchOn = FALSE;
		Switches.loop_active = FALSE; // set default loop condition to false

		GetCtrlVal(panelHandle, PANEL_SWITCH_LOOP, &LoopSwitchOn);

		GetCtrlVal(panelHandle, PANEL_NUM_LOOPPAGE1, &pagestart);
		GetCtrlVal(panelHandle, PANEL_NUM_LOOPPAGE2, &pageend);
		GetCtrlVal(panelHandle, PANEL_NUM_LOOPCOL1, &colstart);
		GetCtrlVal(panelHandle, PANEL_NUM_LOOPCOL2, &colend);

		if ((pagestart > pageend) || ((pagestart == pageend) && (colstart > colend)))
		{
			ValuesGood = FALSE;
			MessagePopup("Bad Conditions", "Start and end cells do not compute");
			SetCtrlVal(panelHandle, PANEL_SWITCH_LOOP, FALSE);

			MoveCanvasStart(colstart, FALSE);
			MoveCanvasEnd(colend, FALSE);
		}
		else
		{
			ValuesGood = TRUE;
			LoopPoints.startpage = pagestart;
			LoopPoints.endpage = pageend;
			LoopPoints.startcol = colstart;
			LoopPoints.endcol = colend;
		}

		if (ValuesGood && LoopSwitchOn)
		{
			Switches.loop_active = TRUE;
			MoveCanvasStart(colstart, TRUE);
			MoveCanvasEnd(colend, TRUE);
		}
		else
		{
			Switches.loop_active = FALSE;
			MoveCanvasStart(colstart, FALSE);
			MoveCanvasEnd(colend, FALSE);
		}
		DrawNewTable(1);
		break;
	}
	return 0;
}

void CVICALLBACK Dig_Cell_Copy(int panelHandle, int controlID, int MenuItemID, void *callbackData)
{
	//Copies the value of the active DigitalTabel value into the clipboard

	int page;
	Point pval = {0, 0};

	page = GetPage();
	GetActiveTableCell(PANEL, PANEL_DIGTABLE, &pval);
	DigClip[0] = DigTableValues[pval.x][pval.y][page];
}

void CVICALLBACK Dig_Cell_Paste(int panelHandle, int controlID, int MenuItemID, void *callbackData)
{
	//Pastes the value store in DigClip[0] by Dig_Cell_Copy into the selected Digital Table Cells

	int page;
	Rect selection;
	Point pval = {0, 0};

	page = GetPage();
	GetTableSelection(panelHandle, PANEL_DIGTABLE, &selection); //note: returns a 0 to all values if only 1 cell selected

	//Pasting into multiple cells
	if (selection.top > 0)
	{
		for (pval.y = selection.top; pval.y <= selection.top + (selection.height - 1); pval.y++)
		{
			for (pval.x = selection.left; pval.x <= selection.left + (selection.width - 1); pval.x++)
			{

				DigTableValues[pval.x][pval.y][page] = DigClip[0];
				if (DigClip[0] == 1)
					SetTableCellAttribute(panelHandle, PANEL_DIGTABLE, pval, ATTR_TEXT_BGCOLOR, VAL_RED);
				else if (DigClip[0] == 0)
					SetTableCellAttribute(panelHandle, PANEL_DIGTABLE, pval, ATTR_TEXT_BGCOLOR, VAL_GRAY);
			}
		}
	}
	//Pasting into single cell
	else if (selection.top == 0)
	{
		GetActiveTableCell(PANEL, PANEL_DIGTABLE, &pval);
		DigTableValues[pval.x][pval.y][page] = DigClip[0];
		if (DigClip[0] == 1)
			SetTableCellAttribute(panelHandle, PANEL_DIGTABLE, pval, ATTR_TEXT_BGCOLOR, VAL_RED);
		else if (DigClip[0] == 0)
			SetTableCellAttribute(panelHandle, PANEL_DIGTABLE, pval, ATTR_TEXT_BGCOLOR, VAL_GRAY);
	}
}

void CVICALLBACK Analog_Cell_Copy(int panelHandle, int controlID, int MenuItemID, void *callbackData)
{
	//This function copies the contents of the active AnalogTable Cell to the Clipboard Globals
	//Handles Analog Channels DDS1,DDS2,DDS3, it is called by right clicking on the Analog Table and Selecing "Copy"

	int page;
	Point pval = {0, 0};

	page = GetPage();
	GetActiveTableCell(PANEL, PANEL_ANALOGTABLE, &pval);

	if (pval.y <= NUMBERANALOGCHANNELS)
	{
		AnalogClip[0].fcn = AnalogTable[pval.x][pval.y][page].fcn;
		AnalogClip[0].fval = AnalogTable[pval.x][pval.y][page].fval;
		AnalogClip[0].tscale = AnalogTable[pval.x][pval.y][page].tscale;
	}
	else if (pval.y <= NUMBERANALOGCHANNELS + NUMBERDDS)
		;
	{
		ddsclip.start_frequency = ddstable[pval.x][page].start_frequency;
		ddsclip.end_frequency = ddstable[pval.x][page].end_frequency;
		ddsclip.amplitude = ddstable[pval.x][page].amplitude;
		ddsclip.is_stop = ddstable[pval.x][page].is_stop;
	}
}

void CVICALLBACK Analog_Cell_Paste(int panelHandle, int controlID, int MenuItemID, void *callbackData)
{
	//Replaces Highlighted Cell contents with the values copied to the clipboard using Analog_Cell_Copy
	//This function Handles copies and pastes of analog channel and dds data.

	int page, col, row;
	Rect selection;
	Point pval = {0, 0};

	page = GetPage();
	GetTableSelection(panelHandle, PANEL_ANALOGTABLE, &selection); //returns a 0 to all values if only 1 cell selected

	//Paste made into multiple cells of analog channels
	if (selection.top <= NUMBERANALOGCHANNELS && selection.top > 0)
	{
		row = selection.top;
		while ((row <= selection.top + (selection.height - 1)) && (row <= NUMBERANALOGCHANNELS))
		{
			for (col = selection.left; col <= selection.left + (selection.width - 1); col++)
			{
				AnalogTable[col][row][page].fcn = AnalogClip[0].fcn;
				AnalogTable[col][row][page].fval = AnalogClip[0].fval;
				AnalogTable[col][row][page].tscale = AnalogClip[0].tscale;
			}
			row++;
		}
	}

	//Paste made into multiple cells of DDS channels
	else if (selection.top > NUMBERANALOGCHANNELS && selection.top > 0)
	{
		for (row = selection.top; row <= selection.top + (selection.height - 1); row++)
		{

			for (col = selection.left; col <= selection.left + (selection.width - 1); col++)
			{
				if (row == NUMBERANALOGCHANNELS + 1)
				{
					ddstable[col][page].start_frequency = ddsclip.start_frequency;
					ddstable[col][page].end_frequency = ddsclip.end_frequency;
					ddstable[col][page].amplitude = ddsclip.amplitude;
					ddstable[col][page].is_stop = ddsclip.is_stop;
				}
				else if (row == NUMBERANALOGCHANNELS + 2)
				{
					dds2table[col][page].start_frequency = ddsclip.start_frequency;
					dds2table[col][page].end_frequency = ddsclip.end_frequency;
					dds2table[col][page].amplitude = ddsclip.amplitude;
					dds2table[col][page].is_stop = ddsclip.is_stop;
				}
				else if (row == NUMBERANALOGCHANNELS + 3)
				{
					dds3table[col][page].start_frequency = ddsclip.start_frequency;
					dds3table[col][page].end_frequency = ddsclip.end_frequency;
					dds3table[col][page].amplitude = ddsclip.amplitude;
					dds3table[col][page].is_stop = ddsclip.is_stop;
				}
			}
		}
	}

	//Paste Made into single Cell
	else if (selection.top == 0)
		;
	{
		GetActiveTableCell(panelHandle, PANEL_ANALOGTABLE, &pval);
		if (pval.y <= NUMBERANALOGCHANNELS)
		{
			AnalogTable[pval.x][pval.y][page].fcn = AnalogClip[0].fcn;
			AnalogTable[pval.x][pval.y][page].fval = AnalogClip[0].fval;
			AnalogTable[pval.x][pval.y][page].tscale = AnalogClip[0].tscale;
		}
		else if (pval.y == NUMBERANALOGCHANNELS + 1)
		{
			ddstable[pval.x][page].start_frequency = ddsclip.start_frequency;
			ddstable[pval.x][page].end_frequency = ddsclip.end_frequency;
			ddstable[pval.x][page].amplitude = ddsclip.amplitude;
			ddstable[pval.x][page].is_stop = ddsclip.is_stop;
		}
		else if (pval.y == NUMBERANALOGCHANNELS + 2)
		{
			dds2table[pval.x][page].start_frequency = ddsclip.start_frequency;
			dds2table[pval.x][page].end_frequency = ddsclip.end_frequency;
			dds2table[pval.x][page].amplitude = ddsclip.amplitude;
			dds2table[pval.x][page].is_stop = ddsclip.is_stop;
		}
		else if (pval.y == NUMBERANALOGCHANNELS + 3)
		{
			dds3table[pval.x][page].start_frequency = ddsclip.start_frequency;
			dds3table[pval.x][page].end_frequency = ddsclip.end_frequency;
			dds3table[pval.x][page].amplitude = ddsclip.amplitude;
			dds3table[pval.x][page].is_stop = ddsclip.is_stop;
		}
	}
	ChangedVals = 1;
	DrawNewTable(0);
}

void CVICALLBACK EXIT(int menuBar, int menuItem, void *callbackData, int panel)
{
	int status;
	status = ConfirmPopup("Exit", "Are you sure you want to quit?\nUnsaved data will be lost.");
	if (status == 1)
		QuitUserInterface(1); // kills the GUI and ends program
}

//Open Scan Table Loader Panel
void CVICALLBACK Scan_Table_Load(int panelHandle, int controlID, int MenuItemID, void *callbackData)
{
	DisplayPanel(panelHandle8);
}
