****************************************
*	 Prozeßnummer = 1
*	 Delay = 1000
*	 Eventsource = 0
*	 Number of Loops = 0
*	 Priorität = 0
*	 Version = 1
*	 FastStop = 0
*	 AdbasicVersion = 4000001
*	 ATSRAM = 0
*	 OPT_LEVEL = 1
*	 SAVECOMPIL = 0
****************************************
#include ADwpad.inc
#include Adwpda.inc
#include Adwpdio.inc
#include adwinpro.inc
dim i,j,k as long
dim DATA_1[50000] as long
dim DATA_2[50000] as long
dim data_3[50000] as float
dim value as float
dim counts,maxcount,updates,i,j,k as long
dim ch as integer
dim val as integer
Function V(value) as float
	V=(value+10)/20*65525
ENDFUNCTION
INIT:
'Jan 16/2004
'expecting 3 arrays.
'Data_1:  A list which specifies how many channels will be updated per event
'Data_2:  A list of channels to be updates
'Data_3:  A list of values for the channels

'Data_2 and Data_3 have a 1:1 correspondence.  
'Stepping through data2 and 3 is controlled by Data 1
'Computer send an integer to Par_1.  This tells us how many
'events to write. (Or if we should just run continuously)
GLOBALDELAY=40000 ' 40000=>1ms
SYNCENABLE(1,dio,1)
SYNCENABLE(2,dio,1)
SYNCENABLE(1,da,1)
SYNCENABLE(2,da,1)

counts=0;
maxcount=Par_1
updates=1;
FPar_1=Data_3[1]
FPar_2=Data_3[2]
FPar_3=Data_3[3]
FPar_4=Data_3[4]
FPar_5=Data_3[5]
FPar_6=Data_3[6]
FPar_7=Data_3[7]
FPar_8=Data_3[8]
FPar_9=Data_3[9]
FPar_10=Data_3[10]

Par_1=Data_2[1]
Par_2=Data_2[2]
Par_3=Data_2[3]
Par_4=Data_2[4]
Par_5=Data_2[5]
Par_6=Data_2[6]
Par_7=Data_2[7]
Par_8=Data_2[8]
Par_9=Data_2[9]
Par_10=Data_2[10]



EVENT:
	' Careful with for loops.  They always do at least 1 execution.
counts=counts+1
SYNCALL()

IF(DATA_1[counts]>=1) then  'A:Check each element for an update
	
	For i=1 to Data_1[counts] 'B: Loop over number of updates at this time
			updates=updates+1
			ch=Data_2[updates]
			if(ch>0) then					'C: 
				if (ch<=17) then 		'D:  C and D check for valid channel numbers
			      ' Now we know the channel is between 0 and 17, so write out the info
						IF (ch=17) then 
								val=Data_3[updates]
								FPar_15=val
								DIG_WRITELATCH1(2,val)
						ENDIF	
						IF (ch<=8) THEN WRITEDAC(1,ch,V(Data_3[updates]))
						If (ch<17) then
								IF (ch>8) then WRITEDAC(2,ch-8,V(Data_3[updates]))
						ENDIF
				ENDIF					' end D	
		ENDIF							' end C
	
	NEXT i						'B for loop	
ENDIF				'A:

If(counts=maxcount+1) then end


FINISH:
'Clear all channels
Sleep(GlobalDelay/4)
For i =1 to 8
	 WRITEDAC(1,i,V(0)) 	
	 WRITEDAC(2,i,V(0))
Next i
DIG_WRITELATCH1(2,0)
SYNCALL