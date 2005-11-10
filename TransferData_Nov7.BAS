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
dim i,j,k,lightcount,litup,eventcount as long
dim DATA_1[5000000] as long
dim DATA_2[5000000] as long
dim data_3[5000000] as float
dim data_4[30] as long	' a list of which channels are reset to zero on completion
dim value as float
dim counts,maxcount,updates,i,j,k as long
dim ch as long
dim val,val_lower,val_upper,val_lower2,val_upper2 as long
dim tempval as long 
dim dataline,clock, ioupdate,ioreset,masterreset as long
dim dataline9858,clock9858,ioupdate9858,ioreset9858,masterreset9858 as long
dim digitallow as long
dim delay,delayinuse as long
dim numbertoskip as long
dim leddelay as float
dim digitalactive,digital2active as long
Function V(value) as float
	V=(value+10)/20*65535
ENDFUNCTION

SUB Light_LED(lit) as 
	SELECTCASE lit
		CASE 1
			SETLED(2,da,0)
			SETLED(1,dio,1)		
		CASE 2		
			SETLED(1,dio,0)
			SetLED(2,dio,1)
		CASE 3
			SETLED(2,dio,0)
			SETLED(3,ext,1)
		CASE 4
			SETLED(3,ext,0)
			SETLED(4,ad,1)
		CASE 5
			SETLED(4,ad,0)
			SETLED(1,da,1)
		CASE 6
			SETLED(1,da,0)
			SETLED(2,da,1)
		CASE 8
			SETLED(2,da,0)
			SETLED(1,da,1)		
		CASE 9
			SETLED(1,da,0)
			SetLED(4,ad,1)
		CASE 10
			SETLED(4,ad,0)
			SETLED(3,ext,1)
		CASE 11
			SETLED(3,ext,0)
			SETLED(2,dio,1)
		CASE 12
			SETLED(2,dio,0)
			SETLED(1,dio,1)
		CASE 13
			SETLED(1,dio,0)
			'SETLED(2,da,1)
	ENDSELECT
ENDSUB

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
'Channels 1-32 refer to Analog output lines
'Channel 101,102 refers to Digital output lines (16 bit word x2)
'Channel 103,104 refer to Digital output lines (16 bit word X2)
'channel 51-53 refers to digital output lines controlling the DDS
'DDS control lines are digital output lines
'Data line, read clock, io update , reset
' 0-3 data
' 4 I/O reset
' 5 update clock
' 6 master Reset
' 7 (maybe) RF switch  
delay=PAR_2 ' 40000=>1ms
GlobalDelay=delay
'DDS digital line setup
dataline=31 				' data line
clock=30				' data clock
ioupdate=29			' update line.  transfers data from buffer into registers
ioreset=28			' resets input buffers
masterreset=27
'DDS2 digital lines (base 0)
dataline9858=21
clock9858=22
ioupdate9858=19
'ioreset9858=23
masterreset9858=18
DigProg1(1,65535)
DigProg2(1,65535)
DigProg1(2,65535)
DigProg2(2,65535)

SYNCENABLE(1,dio,1)
SYNCENABLE(2,dio,1)
SYNCENABLE(1,da,1)
SYNCENABLE(2,da,1)
SYNCENABLE(3,da,1)

numbertoskip=0
counts=1
maxcount=Par_1
updates=1
lightcount=0
eventcount=0
leddelay=8000000/GLOBALDELAY
Par_11=Data_1[1]
Par_12=Data_1[2]
Par_13=Data_1[3]
FPar_11=Data_2[1]
FPar_12=Data_2[2]
FPar_13=Data_2[3]
DIGOUT_F(2,clock,0)
DIGOUT_F(2,dataline,0)
DIGOUT_F(2,ioreset,0)
DIGOUT_F(2,ioupdate,0)
DIGOUT_F(2,masterreset,0)


EVENT:
digitalactive=0
digital2active=0
eventcount=eventcount+1
if(eventcount>leddelay) then
	eventcount=0			
	if(litup=14) then 
		litup=0
	endif
	litup=litup+1
	Light_LED(litup)		
endif
	' Careful with for loops.  They always do at least 1 execution.
delayinuse=delay

digitalactive=0
val_lower=0
val_upper=0
val_lower2=0
val_upper2=0
SYNCALL()
counts=counts+1
if(Data_1[counts]<0) then
	delayinuse=delay*(-1*Data_1[counts])
endif
GLOBALDELAY=delayinuse

IF((99>DATA_1[counts]) and (DATA_1[counts]>=1)) then  'A:Check each element for an update
	For i=1 to Data_1[counts] 'B: Loop over number of updates at this time
		updates=updates+1
		ch=Data_2[updates]
'***************************************************DDS1 commands (Rb Evaporation)***********************
		if(ch=51) then  'AA: is ch=31, then write 2 bits to the DDS
			if(Data_3[updates]<=3) then	' write 2 bits to the data line of the DDS
				tempval = Data_3[updates] 'have to convert to long first
				' on Mar 25, we added opto-couplers to the DDS interfacing
				' this adds a 500ns (roughly) delay to each update
				' 3 updates takes 500ns, so each one opto-coupler delay is 
				'160 ns different from another, timing will be ruined.
				' so, double each line so pulses are 1us instead
				DIGOUT_F(2,dataline,SHIFT_RIGHT((tempval and 2),1))
				DIGOUT_F(2,dataline,SHIFT_RIGHT((tempval and 2),1)) 
 				DIGOUT_F(2,clock,1)
				DIGOUT_F(2,clock,1)				
				DIGOUT_F(2,clock,0)
				DIGOUT_F(2,clock,0)
				DIGOUT_F(2,dataline,(tempval and 1))
				DIGOUT_F(2,dataline,(tempval and 1))
				DIGOUT_F(2,clock,1)
				DIGOUT_F(2,clock,1)
				DIGOUT_F(2,clock,0)
				DIGOUT_F(2,clock,0)

			ELSE 
				IF(Data_3[updates]=4) then 'write to the i/o update pin to transfer to registers
					'DIGOUT_F(2,ioreset,1)
					'DIGOUT_F(2,ioreset,0)
				Else 
					IF(data_3[updates]=5) then
						DIGOUT_F(2,ioupdate,1)
						DIGOUT_F(2,ioupdate,0)
					Else 
						IF(data_3[updates]=6) then
							DIGOUT_F(2,masterreset,1)
							DIGOUT_F(2,masterreset,0)									
						ENDIF
					endif
				endif
			endif
		ENDIF	'end AA
'***************************************************DDS2 commands ()***********************			
		if(ch=52) then  'AA: is ch=31, then write 2 bits to the DDS
			if(Data_3[updates]<=3) then	' write 2 bits to the data line of the DDS
				tempval = Data_3[updates] 'have to convert to long first
				DIGOUT_F(2,dataline9858,SHIFT_RIGHT((tempval and 2),1))
				DIGOUT_F(2,dataline9858,SHIFT_RIGHT((tempval and 2),1)) 
 				DIGOUT_F(2,clock9858,1)
				DIGOUT_F(2,clock9858,1)				
				DIGOUT_F(2,clock9858,0)
				DIGOUT_F(2,clock9858,0)
				DIGOUT_F(2,dataline9858,(tempval and 1))
				DIGOUT_F(2,dataline9858,(tempval and 1))
				DIGOUT_F(2,clock9858,1)
				DIGOUT_F(2,clock9858,1)
				DIGOUT_F(2,clock9858,0)
				DIGOUT_F(2,clock9858,0)

			ELSE 
				IF(Data_3[updates]=4) then 'write to the i/o update pin to transfer to registers
					'DIGOUT_F(2,ioreset9858,1)
					'DIGOUT_F(2,ioreset9858,0)
				Else 
					IF(data_3[updates]=5) then
						DIGOUT_F(2,ioupdate9858,1)
						DIGOUT_F(2,ioupdate9858,0)
					Else 
						IF(data_3[updates]=6) then
							DIGOUT_F(2,masterreset9858,1)
							DIGOUT_F(2,masterreset9858,0)									
						ENDIF
					endif
				endif
			endif
		ENDIF	'end AA
'***********************Digital outs**********
		if(ch=101) then
			val_lower=Data_3[updates]
	    digitalactive=1	
		endif
	  if(ch=102) then
			val_upper=Data_3[updates]
			digitalactive=1
		endif
		if(digitalactive=1) then
			DIG_WRITELATCH32(2,val_lower+2^16*val_upper)
	  endif
	  if(ch=103) then
			val_lower2=Data_3[updates]
	    digital2active=1	
		endif
	  if(ch=104) then
			val_upper2=Data_3[updates]
			digital2active=1
		endif
		if(digital2active=1) then
			DIG_WRITELATCH32(2,val_lower2+2^16*val_upper2)
	  endif	
'*****************Analog outs***********************			
		if((ch>=1)and(ch<=8)) then
			writedac(1,ch,V(Data_3[updates]))
		endif
			
		if((ch>=9)and(ch<=16)) then
			writedac(2,ch-8,V(Data_3[updates]))
		endif
			
		if((ch>=17)and(ch<=24)) then
			writedac(3,ch-16,V(Data_3[updates]))
		endif
	  
		if((ch>=25)and(ch<=32)) then
			writedac(4,ch-24,V(Data_3[updates]))
		endif
'*******done all updates*****							
	NEXT i						'B for loop	
ENDIF				'A:
If(counts>=maxcount+1) then end


FINISH:
'Clear all channels
Par_3=counts
Par_4=maxcount

'If (Data_4[27]=0) then
	For i =1 to 8
		if(Data_4[i]=1) then
			WRITEDAC(1,i,V(0)) 	
		ENDIF
		if(Data_4[i+8]=1) then
			WRITEDAC(2,i,V(0)) 	
		ENDIF
		if(Data_4[i+16]=1) then
			WRITEDAC(3,i,V(0)) 	
		ENDIF
	Next i
'	digitallow=2^16-1-Data_4[25]  ' should be a 16 bit number
'	DIG_WRITELATCH1(2,digitallow)
'	digitallow=2^8-1-Data_4[26]   ' should be an 8 bit number
'	DIG_WRITELATCH2(2,digitallow)
'
  'set line 28 high
	DIG_WRITELATCH32(2,2^28)
'ENDIF
SYNCALL()
