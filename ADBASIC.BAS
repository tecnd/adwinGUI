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
*	 OPT_LEVEL = 0
*	 SAVECOMPIL = 0
****************************************
#include ADwpad.inc
#include Adwpda.inc
#include Adwpdio.inc
#include adwinpro.inc
dim i,j,k,lightcount,litup as long
dim DATA_1[5000000] as long
dim DATA_2[5000000] as long
dim data_3[5000000] as float
dim value as float
dim counts,maxcount,updates,i,j,k as long
dim ch as long
dim val as long
dim dataline,clock, ioupdate,ioreset,masterreset as long

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
'Channels 1-16 refer to Analog output lines
'Channel 17 refers to Digital output lines (16 bit word)
'channel 31 refers to digital output lines controlling the DDS
'DDS control lines are digital output lines
'Data line, read clock, io update , reset
' 0-3 data
' 4 I/O reset
' 5 update clock
' 6 master Reset
' 7 (maybe) RF switch  
GLOBALDELAY=PAR_2 ' 40000=>1ms
'DDS digital line setup
dataline=31 				' data line
clock=30				' data clock
ioupdate=29			' update line.  transfers data from buffer into registers
ioreset=28			' resets input buffers
masterreset=27
DigProg1(1,257)
DigProg2(1,257)
DigProg1(2,257)
DigProg2(2,257)

SYNCENABLE(1,dio,1)
SYNCENABLE(2,dio,1)
SYNCENABLE(1,da,1)
SYNCENABLE(2,da,1)



counts=0;
maxcount=Par_1
updates=1
lightcount=0
EVENT:



lightcount=lightcount+1
if(lightcount>=100) then
	lightcount=0
	if(litup=14) then litup=0
	litup=litup+1
	if(litup=1)then
		SETLED(2,da,0)
		SETLED(1,dio,1)		
	endif
	if(litup=2)then
		SETLED(1,dio,0)
		SetLED(2,dio,1)
	endif
	if(litup=3)then
		SETLED(2,dio,0)
		SETLED(3,ext,1)
	ENDIF
	if(litup=4)then
		SETLED(3,ext,0)
		SETLED(4,ad,1)
	endif
	if(litup=5)then	
		SETLED(4,ad,0)
		SETLED(1,da,1)
	endif
	if(litup=6)then	
		SETLED(1,da,0)
		SETLED(2,da,1)
	endif
	if(litup=8)then
		SETLED(2,da,0)
		SETLED(1,da,1)		
	endif
	if(litup=9)then
		SETLED(1,da,0)
		SetLED(4,ad,1)
	endif
	if(litup=10)then
		SETLED(4,ad,0)
		SETLED(3,ext,1)
	ENDIF
	if(litup=11)then
		SETLED(3,ext,0)
		SETLED(2,dio,1)
	endif
	if(litup=12)then	
		SETLED(2,dio,0)
		SETLED(1,dio,1)
	endif
	if(litup=13)then	
		SETLED(1,dio,0)
		'SETLED(2,da,1)
	endif

endif
	' Careful with for loops.  They always do at least 1 execution.
counts=counts+1

SYNCALL()

IF(DATA_1[counts]>=1) then  'A:Check each element for an update
	
	For i=1 to Data_1[counts] 'B: Loop over number of updates at this time
			updates=updates+1
			ch=Data_2[updates]
			if(ch=31) then  'AA: is ch=31, then write 2 bits to the DDS
				if(Data_2[updates]<=3) then	' write 2 bits to the data line of the DDS
					DIGOUT_F(2,dataline,SHIFT_RIGHT((Data_2[updates] and 2),1))
					DIGOUT_F(2,clock,1)
					DIGOUT_F(2,clock,0)
					DIGOUT_F(2,dataline,(Data_2[updates] and 1))
					DIGOUT_F(2,clock,1)
					DIGOUT_F(2,clock,0)
				ELSE 
					IF(Data_2[updates]=4) then 'write to the i/o update pin to transfer to registers
						DIGOUT_F(2,ioreset,1)
						DIGOUT_F(2,ioreset,0)
					Else 
						IF(data_2[updates]=5) then
						DIGOUT_F(2,ioupdate,1)
						DIGOUT_F(2,ioupdate,0)
						Else 
							IF(data_2[updates]=6) then
								DIGOUT_F(2,masterreset,1)
								DIGOUT_F(2,masterreset,0)									
							ENDIF
						endif
					endif
				endif
			ENDIF	'end AA
				
				
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