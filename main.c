//Things to do:
//		in analog control	If timescale too long then prompt for overwrite of following cell
//		overall				-	Verify that channel values aren't used twice..
//							-	Let choose negative channel to turn off.
//							-	Rewrite Analog table to be textual..use tect or BG color to indicate mode
//							-	Use proportionality constants
//	
//		fix bug where the 16th row of the analog (and probably digital) panel arent being updated.
//      guess of what needs to be fixed:  Drawpanel, make update list
//		load, save arrays ?  Analog/digital setup


/*
Caveats:
If making any changes to the program, be aware of several inconsistencies.
Adwin software is base 1 for arrays
C (LabWindows) is base 0 for arrays,
however some controls for LabWindows are base 1   : Tables
and some are base 0								  : Listboxes



//Update History
Apr 15:  Run button flashes with ADwin is operating.
Apr 29:  Fixed Digital line 16.  Loop counted to 15, should have been 16
May 04:  Adding code to delete/insert columns.
May 06:  Added Copy/Paste fcns for column.  Doesn't work 100% yet.... test that channels 16 work for dig. and analog.
May 10:  Fixed a bug where the arrays weren't properly initialized, causing strange values to be written to the adwin
		 Added flashing led's to notify when the ADwin is doing something
May 13:  Fixed 16th line of the panel, now is actually output.  Bug was result of inconsistency with arrays.
		 i.e. Base 0 vs base 1.  
		 - fixed by increasing the internal array size to 17 and starting from 1. (dont read element 0)		 
May 18:  Improved performance.  Added a variable (ChangedVals) that is 0 when no changes have been made since
 		the last run.  If ChangedVals==0, then don't resend the arrays, shortens delay between runs.


*/
#define ALLOC_GLOBALS  
#include "main.h"
#include "Adwin.h"
#include <time.h>
//#include <userint.h>
//#include <stdio.h>
//#include  <Windows.h>
#define DefaultEventPeriod 0.010   // in milliseconds
#define AdwinTick	0.000025       //Adwin clock cycle, in ms.
//**************DECLARE GLOBALS***********************************************************************
int panelHandle,panelHandle2,panelHandle3,panelHandle4;
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
	} AnalogTable[17][17][10]={1,0,1};
	// the structure is the values/elements contained at each point in the 
	// analog panel.  The array aval, is set up as [x][y][page]
int DigTableValues[17][17][10]={0};
int ChMap[16]={0};	// The channel mapping (for analog). i.e. if we program line 1 as channel 
				// 12, the ChMap[12]=1

double TimeArray[17][10]={0};
struct AnalogChannelProperties{
	int		chnum;		// channel number 1-8 DAC1	9-16 DAC2
	char    chname[50]; // name to appear on the panel
	char	units[50];
	double  tfcn;		// Transfer function.  i.e. 10V out = t G/cm etc...
	}  AChName[16]={0,"","",1};

struct DigitalChannelProperties{
	int		chnum;		// digital line to control
	char 	chname[50];	// name of the channel on the panel
	}	DChName[33]={0,""};
	
double EventPeriod=DefaultEventPeriod;
int processnum;
//************DONE GLOBALS******************************************************************
int main (int argc, char *argv[])
{
	int i,j,k;
	if (InitCVIRTE (0, argv, 0) == 0)
		return -1;	/* out of memory */
	if ((panelHandle = LoadPanel (0, "GUIDesign.uir", PANEL)) < 0)
		return -1;
	if ((panelHandle2 = LoadPanel (0, "AnalogSettings.uir", PANEL)) < 0)
		return -1;
	if ((panelHandle3 = LoadPanel (0, "DigitalSettings.uir", PANEL)) < 0)
		return -1;
	if ((panelHandle4 = LoadPanel (0, "AnalogControl.uir", PANEL)) < 0)
		return -1;
	//Load bitmaps
	SetCtrlAttribute (panelHandle, PANEL_DEBUG, ATTR_VISIBLE, 0);
	// Initialize arrays (to avoid undefined elements causing -99 to be written)
	for (i=0;i<=16;i++) {
		for(j=0;j<=16;j++){
			for(k=0;k<=9;k++){
				AnalogTable[i][j][k].fcn=1;
				AnalogTable[i][j][k].fval=0.0;
				AnalogTable[i][j][k].tscale=1;
				DigTableValues[i][j][k]=0;
			}
		}
	}
	// done initializing
	
	
	LoadLastSettings(1);
	
	SetCtrlVal (panelHandle, PANEL_TB_SHOWPHASE1, 1);
	SetCtrlVal (panelHandle, PANEL_TB_SHOWPHASE2, 0);
	SetCtrlVal (panelHandle, PANEL_TB_SHOWPHASE3, 0);
	SetCtrlVal (panelHandle, PANEL_TB_SHOWPHASE4, 0);
	SetCtrlVal (panelHandle, PANEL_TB_SHOWPHASE5, 0);
	SetCtrlVal (panelHandle, PANEL_TB_SHOWPHASE6, 0);
	SetCtrlVal (panelHandle, PANEL_TB_SHOWPHASE7, 0);

	EventPeriod=1.00;
	ClearListCtrl(panelHandle,PANEL_DEBUG);
	menuHandle=GetPanelMenuBar(panelHandle);
	SetMenuBarAttribute (menuHandle, MENU_UPDATEPERIOD_SETGD5, ATTR_CHECKED, 0);
	SetMenuBarAttribute (menuHandle, MENU_UPDATEPERIOD_SETGD10, ATTR_CHECKED, 0);
	SetMenuBarAttribute (menuHandle, MENU_UPDATEPERIOD_SETGD100, ATTR_CHECKED, 0);
	SetMenuBarAttribute (menuHandle, MENU_UPDATEPERIOD_SETGD1000, ATTR_CHECKED, 1);
	currentpage=1;
	DrawNewTable(0);
	ReadAnalogChannels();//from the panel
	ReadDigitalChannels();
	
	DisplayPanel (panelHandle); 
	RunUserInterface ();
	DiscardPanel (panelHandle);
	
	return 0;
}
//*****************************************************************************************
void SaveArrays(char savedname[500],int csize)
{
	FILE *fdata;
	int i=0,j=0,k=0;
	int xval=16,yval=16,zval=10;
	char buff[500],buff2[100];
	strncpy(buff,savedname,csize-4);
	strcat(buff,".arr");
	if((fdata=fopen(buff,"w"))==NULL)
	{
		InsertListItem(panelHandle,PANEL_DEBUG,-1,buff,1);
		//exit(1);
	}
	//The Plan:
	//write the dimensionality of the array....
	//szve as x,y,z...
	//scan order is Page,X,Y..
	//So, scanning by channel at each step....
	//Need time info tooo...
	// So...after dim...save appropriate time array. (page ,x)
	// save structures.
	//locked dimensions...16,16,10	
	//The dimensions
	fwrite(&xval,sizeof xval, 1,fdata);
	fwrite(&yval,sizeof yval, 1,fdata);
	fwrite(&zval,sizeof zval, 1,fdata);
	//now for the times.	
	fwrite(&TimeArray,sizeof TimeArray,1,fdata);
	//and the analog data
	fwrite(&AnalogTable,sizeof AnalogTable,1,fdata);
	fwrite(&DigTableValues,sizeof DigTableValues,1,fdata);
	
	GetCtrlAttribute (panelHandle, PANEL_TB_SHOWPHASE1,ATTR_OFF_TEXT, buff2);
	fwrite(&buff2,sizeof buff2,1,fdata);
	GetCtrlAttribute (panelHandle, PANEL_TB_SHOWPHASE2,ATTR_OFF_TEXT, buff2);
	fwrite(&buff2,sizeof buff2,1,fdata);
	GetCtrlAttribute (panelHandle, PANEL_TB_SHOWPHASE3,ATTR_OFF_TEXT, buff2);
	fwrite(&buff2,sizeof buff2,1,fdata);
	GetCtrlAttribute (panelHandle, PANEL_TB_SHOWPHASE4,ATTR_OFF_TEXT, buff2);
	fwrite(&buff2,sizeof buff2,1,fdata);
	GetCtrlAttribute (panelHandle, PANEL_TB_SHOWPHASE5,ATTR_OFF_TEXT, buff2);
	fwrite(&buff2,sizeof buff2,1,fdata);
	GetCtrlAttribute (panelHandle, PANEL_TB_SHOWPHASE6,ATTR_OFF_TEXT, buff2);
	fwrite(&buff2,sizeof buff2,1,fdata);
	GetCtrlAttribute (panelHandle, PANEL_TB_SHOWPHASE7,ATTR_OFF_TEXT, buff2);
	fwrite(&buff2,sizeof buff2,1,fdata);
	
	
	
	fclose(fdata);
}
//**********************************************************************************
void LoadArrays(char savedname[500],int csize)
{
	FILE *fdata;
	int i=0,j=0,k=0;
	int xval=16,yval=16,zval=10;
	char buff[500]="",buff2[100]="";
	strncat(buff,savedname,csize-4);
	strcat(buff,".arr");
	if((fdata=fopen(buff,"r"))==NULL)
	{
	//	exit(1);
	}
	//The Plan:
	//write the dimensionality of the array....
	//szve as x,y,z...
	//scan order is Page,X,Y..
	//So, scanning by channel at each step....
	//Need time info tooo...
	// So...after dim...save appropriate time array. (page ,x)
	// save structures.
	//locked dimensions...16,16,10	
	//The dimensions
	fread(&xval,sizeof xval, 1,fdata);
	fread(&yval,sizeof yval, 1,fdata);
	fread(&zval,sizeof zval, 1,fdata);
	//now for the times.	
	fread(&TimeArray,sizeof TimeArray,1,fdata);
	//and the analog data
	fread(&AnalogTable,sizeof AnalogTable,1,fdata);
	fread(&DigTableValues,sizeof DigTableValues,1,fdata);
	
	fread(&buff2,sizeof buff2,1,fdata);
	SetCtrlAttribute (panelHandle, PANEL_TB_SHOWPHASE1,ATTR_OFF_TEXT, buff2);
	SetCtrlAttribute (panelHandle, PANEL_TB_SHOWPHASE1,ATTR_ON_TEXT, buff2);
	fread(&buff2,sizeof buff2,1,fdata);
	SetCtrlAttribute (panelHandle, PANEL_TB_SHOWPHASE2,ATTR_OFF_TEXT, buff2);
	SetCtrlAttribute (panelHandle, PANEL_TB_SHOWPHASE2,ATTR_ON_TEXT, buff2);
	fread(&buff2,sizeof buff2,1,fdata);
	SetCtrlAttribute (panelHandle, PANEL_TB_SHOWPHASE3,ATTR_OFF_TEXT, buff2);
	SetCtrlAttribute (panelHandle, PANEL_TB_SHOWPHASE3,ATTR_ON_TEXT, buff2);
	fread(&buff2,sizeof buff2,1,fdata);
	SetCtrlAttribute (panelHandle, PANEL_TB_SHOWPHASE4,ATTR_OFF_TEXT, buff2);
	SetCtrlAttribute (panelHandle, PANEL_TB_SHOWPHASE4,ATTR_ON_TEXT, buff2);
	fread(&buff2,sizeof buff2,1,fdata);
	SetCtrlAttribute (panelHandle, PANEL_TB_SHOWPHASE5,ATTR_OFF_TEXT, buff2);
	SetCtrlAttribute (panelHandle, PANEL_TB_SHOWPHASE5,ATTR_ON_TEXT, buff2);
	fread(&buff2,sizeof buff2,1,fdata);
	SetCtrlAttribute (panelHandle, PANEL_TB_SHOWPHASE6,ATTR_OFF_TEXT, buff2);
	SetCtrlAttribute (panelHandle, PANEL_TB_SHOWPHASE6,ATTR_ON_TEXT, buff2);
	fread(&buff2,sizeof buff2,1,fdata);
	SetCtrlAttribute (panelHandle, PANEL_TB_SHOWPHASE7,ATTR_OFF_TEXT, buff2);
	SetCtrlAttribute (panelHandle, PANEL_TB_SHOWPHASE7,ATTR_ON_TEXT, buff2);
	
	
	
	fclose(fdata);

}

//********************************************************************************************
double CalcFcnValue(int fcn,double Vinit,double Vfinal, double timescale,double telapsed)
{
	double value=-99,amplitude,slope,aconst,bconst,tms;
	if (timescale<=0) {timescale=1;}
	tms=telapsed*EventPeriod;
	switch(fcn)
	{
		case 1 ://step function
			value=Vfinal;
			break;
		case 2 ://linear ramp
			amplitude=Vfinal-Vinit;
			slope=amplitude/timescale;
			if(tms>timescale) { value=Vfinal;}
			else {value=Vinit+slope*tms;}
			break;
		case 3 ://exponential
			amplitude=Vfinal-Vinit;
			value=Vfinal-amplitude*exp(-tms/timescale);
			break;
		case 4 :
			amplitude=Vfinal-Vinit;
			aconst=3*amplitude/pow(timescale,2);
			bconst=-2*amplitude/pow(timescale,3);
			if(tms>timescale) 
			{
				value=Vfinal;
			}
			else
			{
				value=Vinit+(aconst*pow(tms,2)+bconst*pow(tms,3));
			}
			break;
	}		
	return value;
}

//**********************************************************************************
void BuildUpdateList(double TMatrix[],struct AnVals AMat[17][500],int DMat[17][500],int numtimes)
//void BuildUpdateList(double TMatrix[],struct AnVals AMat[16][500],double DMat[16][500])

{
//PLAN:  	first...for each Timeblock:  check how many are steps and how many aren't
//  	    calculate all step changes.
//			Loop unitltimes done
//				calc new changes for nonstep
//	int UpdateNum[500000]={-99};
//	int ChNum[500000]={-99};
//	float ChVal[500000]={-99.9};
	
	int *UpdateNum;
	int *ChNum;
	float *ChVal;

	int NewTimeMat[500];
	int i=0,j=0,k=0,m=0,n=0,tau=0;
	int nupcurrent=0,nuptotal=0;
	int usefcn=0,LastDVal=0;  //Bool
	int UsingFcns[18]={0},count=0,ucounter=0,digval,counter,channel;
	double LastAval[18]={0},NewAval;
	int timemult,t,c,bigger;
	int GlobalDelay=40000;
	char buff[100];
	int repeat=0,timesum=0;
	static int didboot=0;
	static int didprocess=0;
	int memused;
	int timeused;
	//Change run button appearance

	time_t tstart,tstop;
	//TODO:	make sure it writes the final value....currently might not.
	// Change the color of the Run button, so we know the program/ADwin is active
	//
	
	SetCtrlAttribute (panelHandle, PANEL_CMD_RUN,ATTR_CMD_BUTTON_COLOR, VAL_GREEN);            
   // DisplayPanel(panelHandle);
  
   
    	tstart=clock();
		timemult=(int)(1/EventPeriod);
		GlobalDelay=EventPeriod/AdwinTick;
		ClearListCtrl(panelHandle,PANEL_DEBUG);
		 
		//make a new time list...using the needed step size.i.e in number of ADWin events
		
		for (i=1;i<=numtimes;i++)
		{
			NewTimeMat[i]=(int)(TMatrix[i]*timemult);
			timesum=timesum+NewTimeMat[i];
		}
		sprintf(buff,"timesum %d",timesum);
		InsertListItem(panelHandle,PANEL_DEBUG,-1,buff,1);
		//dynamically allocate the memory for the time array (instead of using a static array:UpdateNum)
		//We are making an assumption about how many programmable points we may need to use.
		//For now assume that number of channel updates <= 4* #of events.

	if(ChangedVals==1)
    {		
		UpdateNum=calloc((int)((double)timesum*1.2),sizeof(int));
		if(!UpdateNum){ exit(1); }
		ChNum=calloc((int)((double)timesum*4),sizeof(int));
		if(!UpdateNum){ exit(1); }
		ChVal=calloc((int)((double)timesum*4),sizeof(double));
		if(!UpdateNum){ exit(1); }
		
		
		//Loop over timematrix now...
		for (i=1;i<=numtimes;i++)
		{
			// find out how many channels need updating this round...
			// if its a fcn, keep a list of UsingFcns, and change it now
			nupcurrent=0;
			usefcn=0;
			for (j=1;j<=16;j++)	// scan over the analog channel..find update ones.
			{
				LastAval[j]=-99;
				if(!(AMat[j][i].fval==AMat[j][i-1].fval))
				{
					nupcurrent++;
					nuptotal++;
					ChNum[nuptotal]=AChName[j].chnum;
					NewAval=CalcFcnValue(AMat[j][i].fcn,AMat[j][i-1].fval,AMat[j][i].fval,AMat[j][i].tscale,0.0);
	// use 1 of the following 2 lines.  Line 1 for output =entry (in V) on panel
	// line 2 for scaled output.  Maybe rewrite to do a function call with a more complex formula.
					ChVal[nuptotal]=NewAval;
// 					ChVal[nuptotal]=NewAval*AnalogChannelProperties[j].tfcn;
					if(!(AMat[j][i].fcn==1))
					{
						usefcn++;
						UsingFcns[usefcn]=j;	// mark these lines for special attention..more complex	
					}
				}
			}	//now the digital value
			digval=0;	//determine the 16bit digital lines value
			for(k=1;k<=16;k++)
			{
				digval=digval+DMat[k][i]*pow(2,k-1);
			}
			if(!(digval==LastDVal))
			{
				nupcurrent++;
				nuptotal++;
				ChNum[nuptotal]=17;
				ChVal[nuptotal]=digval;
			}
			LastDVal=digval;
			count++;
			UpdateNum[count]=nupcurrent;
			//end of first scan  
			//now do the remainder of the loop...but just the compicated fcns
			t=0;
			while(t<NewTimeMat[i]-1)
			{
				t++;
				k=0;
				nupcurrent=0;
				while(k<usefcn)
				{
					k++;
					c=UsingFcns[k];
					NewAval=CalcFcnValue(AMat[c][i].fcn,AMat[c][i-1].fval,AMat[c][i].fval,AMat[c][i].tscale,t);
					if(fabs(NewAval-LastAval[k])>0.001)
					{
						nupcurrent++;
						nuptotal++;
						ChNum[nuptotal]=AChName[c].chnum;
						ChVal[nuptotal]=NewAval;
						LastAval[k]=NewAval;
					}
				}
				count++;
				UpdateNum[count]=nupcurrent;
			}//Done this element of the TMatrix
		}//done scanning over times array
		//draw in DEBUG box
		bigger=count;
		if(nuptotal>bigger) {bigger=nuptotal;}
	/*	for(i=1;i<bigger+10;i++)
		{
			sprintf(buff,"%d :  %d    %d    %f",i,UpdateNum[i],ChNum[i],ChVal[i]);
			InsertListItem(panelHandle,PANEL_DEBUG,-1,buff,1);
		}
	*/	
		tstop=clock();
		sprintf(buff,"count %d",count);
		InsertListItem(panelHandle,PANEL_DEBUG,-1,buff,1);
		sprintf(buff,"updates %d",nuptotal);
		InsertListItem(panelHandle,PANEL_DEBUG,-1,buff,1);
	
	
		sprintf(buff,"time to generate arrays:   %d",tstop-tstart);
		InsertListItem(panelHandle,PANEL_DEBUG,-1,buff,1);
		if(didboot==0)
		{
			Boot("C:\\ADWIN\\ADWIN10.BTL",0);           
			didboot=1;
		}
		if (didprocess==0)
		{
			processnum=Load_Process("TransferData.TA1");
			didprocess=1;
		}
	
		SetPar(1,count);  	//Let ADwin know how many counts (read as Events) we will be using.
		SetPar(2,GlobalDelay);
		//Command below sends to Data_1, send array val, starting index 1, sends 10 elements.
		SetData_Long(1,UpdateNum,1,count+1);
		SetData_Long(2,ChNum,1,nuptotal+1);
		SetData_Float(3,ChVal,1,nuptotal+1);
		ChangedVals=0;
	}
	tstop=clock();         
	timeused=tstop-tstart;     
	t=Start_Process(processnum);
	tstop=clock();         	

sprintf(buff,"Time to transfer and start ADwin:   %d",timeused);
InsertListItem(panelHandle,PANEL_DEBUG,-1,buff,1);
memused=(count+2*nuptotal)*4;//in bytes
sprintf(buff,"MB of data sent:   %f",(double)memused/1000000);
InsertListItem(panelHandle,PANEL_DEBUG,-1,buff,1);
sprintf(buff,"Transfer Rate:   %f   MB/s",(double)memused/(double)timeused/1000);
InsertListItem(panelHandle,PANEL_DEBUG,-1,buff,1);


SetCtrlAttribute (panelHandle, PANEL_CMD_RUN,ATTR_CMD_BUTTON_COLOR, VAL_LT_GRAY);
//DisplayPanel(panelHandle);
//re-enable the timer if necessary
GetCtrlVal(panelHandle,PANEL_TOGGLEREPEAT,&repeat);
if(repeat==1)
{
	SetCtrlAttribute(panelHandle,PANEL_TIMER,ATTR_ENABLED,1);
	ResetTimer(panelHandle,PANEL_TIMER);
	
}
free(UpdateNum);
free(ChNum);
free(ChVal);

}
