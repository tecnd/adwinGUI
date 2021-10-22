'<ADbasic Header, Headerversion 001.001>
' Process_Number                 = 1
' Initial_Processdelay           = 1000
' Eventsource                    = Timer
' Control_long_Delays_for_Stop   = No
' Priority                       = High
' Version                        = 1
' ADbasic_Version                = 6.3.1
' Optimize                       = No
' Stacksize                      = 1000
' Info_Last_Save                 = NIGHTSHADE  NIGHTSHADE\labadmin
'<Header End>
#include ADwinPRO_ALL.inc
'Updated to new ADbasic code - Ben Sofka
dim i,j,k,lightcount,litup,eventcount,delaymultinuse as long
dim DATA_1[5000000] as long
dim DATA_2[5000000] as long
dim DATA_3[5000000] as float
dim DATA_4[100] as long  ' a list of which channels are reset to zero on completion
dim value as float
dim counts,maxcount,updates as long
dim ch as long
dim val,val_lower,val_upper,val_lower2,val_upper2 as long
dim tempval as long 
dim digitallow as long
dim delay,delayinuse as long
dim numbertoskip as long
dim leddelay as float

Function V(value) as float
  V=(value+10)/20*65535
ENDFUNCTION

SUB Light_LED(lit) as  
  SELECTCASE lit
    CASE 1
      P2_SET_LED(2,0)
      P2_SET_LED(1,1)    
    CASE 2    
      P2_SET_LED(1,0)
      P2_SET_LED(2,1)
    CASE 3
      P2_SET_LED(2,0)
      P2_SET_LED(3,1)
    CASE 4
      P2_SET_LED(3,0)
      P2_SET_LED(4,1)
    CASE 5
      P2_SET_LED(4,0)
      P2_SET_LED(1,1)
    CASE 6
      P2_SET_LED(1,0)
      P2_SET_LED(2,1)
    CASE 8
      P2_SET_LED(2,0)
      P2_SET_LED(1,1)    
    CASE 9
      P2_SET_LED(1,0)
      P2_SET_LED(4,1)
    CASE 10
      P2_SET_LED(4,0)
      P2_SET_LED(3,1)
    CASE 11
      P2_SET_LED(3,0)
      P2_SET_LED(2,1)
    CASE 12
      P2_SET_LED(2,0)
      P2_SET_LED(1,1)
    CASE 13
      P2_SET_LED(1,0)
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
  'events to write. )
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
  delay=PAR_2 ' 300000=>1ms for T11 Processor: Updated July 14 2009 - Ben Sofka
  PROCESSDELAY=delay

  DigProg1(1,65535)
  DigProg2(1,65535)
  DigProg1(2,65535)
  DigProg2(2,65535)
  
  ' Seth Aubin: removed these lines --> we do not have a 3rd DIO card
  'DigProg1(3,65535)
  'DigProg2(3,65535)
  ' ------------------------------------------------------------------
  
  'Configure cards for synchronous output
  SYNCENABLE(1,dio,1)
  SYNCENABLE(2,dio,1)
  
  ' Seth Aubin: removed these lines --> we do not have a 3rd DIO card
  'SYNCENABLE(3,dio,1)
  ' ------------------------------------------------------------------

  SYNCENABLE(1,da,1)
  SYNCENABLE(2,da,1)
  SYNCENABLE(3,da,1)
  SYNCENABLE(4,da,1)

  numbertoskip=0
  counts=1
  maxcount=PAR_1
  updates=1
  lightcount=0
  eventcount=0
  leddelay=8000000/PROCESSDELAY
  PAR_11=DATA_1[1]
  PAR_12=DATA_1[2]
  PAR_13=DATA_1[3]
  FPar_11=DATA_2[1]
  FPar_12=DATA_2[2]
  FPar_13=DATA_2[3]
 
  'Seth Aubin (May 25, 2012): removed these lines --> we do not have a DDS
  '
  '' Make sure the DDS lines are low.
  'DIGOUT_F(2,clock,0)
  'DIGOUT_F(2,dataline,0)
  'DIGOUT_F(2,ioreset,0)
  'DIGOUT_F(2,ioupdate,0)
  'DIGOUT_F(2,masterreset,0)
  

EVENT:
  P2_SYNC_ALL(1111b)    'Force synchronized output now.  We are sending out the data programmed in the last cycle.
  ' THis way all the outputs are updated at the beginning of an event, which is well timed.
  ' Doing Syncall() at the end of an event causes the channel outputs to move around in time, depending on how many channels
  ' are being programmed.

  eventcount=eventcount+delaymultinuse
  if(eventcount>leddelay) then
    eventcount=0      
    if(litup=14) then 
      litup=0
    endif
    litup=litup+1
    Light_LED(litup)    
  endif

  delayinuse=delay   ' reset the PROCESSDELAY
  if(DATA_1[counts]<0) then    ' if we see a negative number, interpret it as a multiplicative factor on the delay
    delayinuse=delay*(-1*DATA_1[counts])
    delaymultinuse=-1*DATA_1[counts]
  endif
  PROCESSDELAY=delayinuse


  ' reset the variables controlling digital output.
  val_lower=0
  val_upper=0
  val_lower2=0
  val_upper2=0
  counts=counts+1  'Number of events so far

  ' Added by Seth Aubin on August 2, 2010.
  DIG_LATCH(1)  'required for operation of DIO 1 ..suppose to be like P2_SYNC_ALL
  DIG_LATCH(2)  'required for operation of DIO 1 ..suppose to be like P2_SYNC_ALL

  IF((99>DATA_1[counts]) and (DATA_1[counts]>=1)) then  'A:Check each elementof DATA_1 for an update
    For i=1 to DATA_1[counts] 'B: Loop over number of updates at this time
      updates=updates+1
      ch=DATA_2[updates]

      '*****************Analog outs***********************      
      if((ch>=1) and (ch<=32)) then
        AnalogWrite(ch,DATA_3[updates])
      endif
    
      '*****************DDS outs*************************
      'if(ch=51) then  'AA: is ch=51, then act on the code to write to the DDS1
      '  DDS1Write(DATA_3[updates])
      'endif      
    
      'if(ch=52) then  'BB: is ch=52, then write 2 bits to the DDS2
      '  DDS2Write(DATA_3[updates])
      'ENDIF  'end BB

      '***********************Digital outs**********
      if(ch=101) then
        val_lower=DATA_3[updates]
        DIG_WRITELATCH32(1,val_lower) ' Seth Aubin, Aug. 2, 2010: "2" -->"1"
      endif
      
      if(ch=102) then
        val_upper=DATA_3[updates]
        DIG_WRITELATCH32(2,val_upper) ' Seth Aubin, Aug. 2, 2010: "3" -->"2"
      endif
            
    NEXT i            'B for loop  
  ENDIF        'A:
  If(counts>=maxcount+1) then end


FINISH:
  'Clear all channels
  PAR_3=counts
  PAR_4=maxcount

  'If (Data_4[27]=0) then
  For i= 1 to 8
    if(DATA_4[i]=1) then
      P2_WRITE_DAC(1,i,V(0))   
    ENDIF
    if(DATA_4[i+8]=1) then
      P2_WRITE_DAC(2,i,V(0))   
    ENDIF
    if(DATA_4[i+16]=1) then
      P2_WRITE_DAC(3,i,V(0))   
    ENDIF
    if(DATA_4[i+24]=1) then
      P2_WRITE_DAC(4,i,V(0))   
    ENDIF
  Next i
  
  'Seth Aubin (25 May 2012): removed this line --> we do not have a DDS.
  '
  ''set line 28 high... forget why...
  'DIG_WRITELATCH32(2,2^28)
  ''ENDIF

  P2_SYNC_ALL(1)

  '***************************************************
SUB AnalogWrite(achannel, avalue) 
  if((achannel>=1)and(achannel<=8)) then
    P2_WRITE_DAC(1,achannel,V(avalue))
  endif
      
  if((achannel>=9)and(achannel<=16)) then
    P2_WRITE_DAC(2,achannel-8,V(avalue))
  endif
      
  if((achannel>=17)and(achannel<=24)) then
    P2_WRITE_DAC(3,achannel-16,V(avalue))
  endif
    
  if((achannel>=25)and(achannel<=32)) then
    P2_WRITE_DAC(4,achannel-24,V(avalue))
  endif
ENDSUB

