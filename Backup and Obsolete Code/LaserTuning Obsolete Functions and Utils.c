/* DDSRAMP MK1 Backup */

/*  The following is all the code required to execute a frequency ramp on the AD9858 DDS via rabbit (For the Laser Tuning 
	Project). This code is now obsolete in light of the MK2 algorithm which takes advantage of the freq sweep mode on the 
	DDS. 
	
	In order to achieve the desired ramp (amplitude and time), a frequency step size is calculated and then the rabbit is 
	made to increment the frequency in the current profile by that step size a number of times. Each increment must be 
	preceded by a trigger from the adwin which is the means for the temporal control. */

// For BuildLaserUpdates in LaserTuning.c

else if(cmode==2)			 //freq ramp
{
	rampHeight=calcRamp(FreqShadow,MetaLaserArray[laserNum][i].fval,&pfdUpdate,&newFreq);
	if(pfdUpdate>0)  // Set the new frequency under new profile and change pfd divders
	{
		profileShadow=!profileShadow;
		cmdList[cmdCount]=cmd_setFreq(profileShadow,newFreq,&cmdLengthList[cmdCount]);
		cmdCount++;
						
		cmdList[cmdCount]=cmd_setPFD_Div(pfdUpdate,&cmdLengthList[cmdCount]);		;
		cmdCount++;
							
		cmdList[cmdCount]=cmd_waitForTrigger(&cmdLengthList[cmdCount]);		;
		cmdCount++;
		MetaTriggerArray[laserNum][i]++;
		trigCount++;
							
		cmdList[cmdCount]=cmd_FUD(&cmdLengthList[cmdCount]);		;
		cmdCount++;
			
		cmdList[cmdCount]=cmd_setProfile(profileShadow,&cmdLengthList[cmdCount]);		;
		cmdCount++;
							
		}
						
		cmdList[cmdCount]=cmd_rampDDS(rampHeight,MetaTimeArray[i],profileShadow,1,&MetaTriggerArray[laserNum][i],&cmdLengthList[cmdCount]);   // @@ change this to trig enables (1)!
		cmdCount++;
						
		//Set Freq to exact value (necessary because of stepsize rounding)
		cmdList[cmdCount]=cmd_setFreq(profileShadow,calcFreq(MetaLaserArray[laserNum][i].fval),&cmdLengthList[cmdCount]);
		cmdCount++;
						
					
}


//For LaserTuning.h
double calcRamp(double laserStartFreq,double laserEndFreq,int *dividerUpdate,double *newDDSFreq)
/*  Calculates the required ramp parameters for the DDS in order to achieve the desired ramp in laser freq
	laserStartFreq and laserEndFreq are in MHz
	return value is the DDSrampHeight
	dividerUpdate gives the required pfd divder update val (0 if not required) 
	newDDSFreq if non zero indicates a DDSFreq update is required prior to ramp starting
	*/
{
	double rampHeight;
	double largerFreq;
	*newDDSFreq=0;
	
	
	if(laserStartFreq*laserEndFreq>0)
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
		printf("Error cannot ramp across 0"); //@@
	
	return rampHeight;


}


//for RabbitCom9859.h
unsigned char* cmd_rampDDS(double rampHeight,double rampTime,int profile,int trigEnable, unsigned int *triggerNum, int *cmdLength)
/*  This command ramps the dds from the current frequency to current+rampHeight (rampHeight can be negative) in time rampTime 
	trigEnable enables external (TCP or adwin triggering for each ramp step) 
	rampHeight in Hz
	
	cmd[1] byte enables triggering, cmd[2] byte: profile
	cmd[3]-cmd[6] selects the step size (twos complement long), cmd[7]-cmd[10] set the number of steps */
{
	unsigned char *cmd = (unsigned char *)malloc(RAMPDDS_SIZE*sizeof(char));
	int* stepSize;
	unsigned int* numSteps; 
	int i,j,msbFound,tempStepSize,roundCount;
	const int stepRound = 4; //We round the stepSize to this many significant bits
	
	msbFound=0;
	roundCount=0;
	
	numSteps=(unsigned int*)&cmd[7];
	stepSize=(int*)&cmd[3];
	
	cmd[1]=trigEnable;
	if (profile>=0&&profile<=3)
	{
		*cmdLength=RAMPDDS_SIZE;
		cmd[0]=RAMPDDS;
		cmd[2]=profile;
		
		*numSteps=(unsigned int) (rampTime/MIN_RAMP_STEPTIME);    //by default set to max number of steps
		tempStepSize=(int)((double)(rampHeight*MIN_RAMP_STEPTIME/rampTime)*pow(2,32)/(DDSCLK));
		
		printf("un-rounded: %d\n",tempStepSize);
		*stepSize=0;
		
		//here we round the step size to the 4MSbs, this reduces the minimum step time at a trade off of a quick step at
		//the end of the ramp
		for(i=3;i>=0;i--)
		{
			for(j=7;j>=0;j--)
			{
				if(tempStepSize/(int_pow(2,j)*int_pow(2,8*i))>0)
				{
					msbFound++;
					tempStepSize-=int_pow(2,j)*int_pow(2,8*i);
					if(roundCount<stepRound)
						*stepSize+=int_pow(2,j)*int_pow(2,8*i);
				}
				if(msbFound>0)
					roundCount++;	
				//printf("Data: %d\n",(unsigned)int_pow(2,j)*int_pow(2,8*i));
				
			}
		}
		
		printf("rounded: %d\n",*stepSize);
		
		if(*numSteps<2|stepSize==0)
			printf("Invalid ramp Error\n"); // @@
	}
	else 
		printf("Invalid Profile\n"); // @@
	
	if (trigEnable)
		*triggerNum=*numSteps;
	return cmd;
}

//InBuildUpdateList   GuiDesign.c
//Cycles through needed laser triggers
for(k=0;k<NUMBERLASERS;k++)
{
 	if(LaserTriggerArray[k][i]>0&&t>nextTrig[k])
 	{
		nupcurrent++;
		nuptotal++;
		ChNum[nuptotal]=60;
		//ChVal corresponds to the actual channel updated on the Adwin's 2nd digital bank (card3)
		ChVal[nuptotal]=LaserProperties[k].DigitalChannel-100; 
					
		LaserTriggerArray[k][i]--;
		nextTrig[k]+=triggerInterval[k];
 	}
}


//Dynamic C Function for the Rabbbit: Goes in AD9858Control.lib
/***********************************FUNCTION exct_rampDDS *****************************************/
void exct_rampDDS(unsigned char* cmd)
/* Ramps the DDS frequency in a series of discrete steps
	cmd[1] byte enables triggering, cmd[2] byte: profile
	cmd[3]-cmd[6] selects the FTW step size (twos complement long), cmd[7]-cmd[10] set the number of steps
   Note minimum time step is currently roughly 10ms -because of opamp rise time, test after board development */

{
	unsigned long i;
   unsigned long* numSteps;
   unsigned long* freq;
   long* stepSize;
   unsigned long* startFreq;
   unsigned char incrementCMD[6];

   //for testing
   unsigned long rampTime,finalTime;
   rampTime=MS_TIMER;


   stepSize=(long*)&cmd[3];
   numSteps=(unsigned long*)&cmd[7];
   freq=(unsigned long*)&incrementCMD[2];
   startFreq=(unsigned long*)DDS_FTWShadow[cmd[2]];
   *freq=*startFreq;
   incrementCMD[1]=cmd[2];



   for (i=0;i<*numSteps;i++)
   {
   	*freq+=*stepSize;
      if(cmd[1])
      {
         //cmd_TCPTriggerListen();    //tcp trig used for testing
       	//cmd_AWTriggerListen()       wait for adwin trigger

      }

      	//do triggering
      //printf("Setting Freq: %ld\n",*freq);
      exct_setFreq(incrementCMD);
   }
   finalTime=MS_TIMER-rampTime;
   printf("\nNum Steps: %ld Time: %ld\n",*numSteps,finalTime);

}


