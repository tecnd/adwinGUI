'<ADbasic Header, Headerversion 001.001>
' Process_Number                 = 1
' Initial_Processdelay           = 3000
' Eventsource                    = External
' Control_long_Delays_for_Stop   = No
' Priority                       = High
' Version                        = 1
' ADbasic_Version                = 6.3.1
' Optimize                       = Yes
' Optimize_Level                 = 2
' Stacksize                      = 1000
' Info_Last_Save                 = NIGHTSHADE  NIGHTSHADE\labadmin
'<Header End>
#include ADwinPRO_ALL.inc
dim i,j,k,delaymultinuse as long
dim DATA_1[5000000] as long
dim DATA_2[5000000] as long
dim DATA_3[5000000] as float
dim DATA_4[100] as long  ' a list of which channels are reset to zero on completion
dim counts,maxcount,updates as long
dim ch as long

Function V(value) as float
  V=(value+10)/20*65535
ENDFUNCTION

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
  'Channel 101,102 refers to Digital output lines (32 bit word x2)
  
  ' PROCESSDELAY=PAR_2 ' 300000=>1ms for T11 Processor: Updated July 14 2009 - Ben Sofka
  CPU_EVENT_CONFIG(0,1,1)
  
  DigProg1(1,65535)
  DigProg2(1,65535)
  DigProg1(2,65535)
  DigProg2(2,65535)
  
  'Configure cards for synchronous output
  SyncEnable(1,dio,1)
  SyncEnable(2,dio,1)
  P2_Sync_Enable(1, 0FFFFh)
  P2_Sync_Enable(2, 0FFFFh)
  P2_Sync_Enable(3, 0FFFFh)
  P2_Sync_Enable(4, 0FFFFh)

  counts=1
  maxcount=PAR_1
  updates=1
  delaymultinuse=0
  
  ' Debug display
  PAR_11=DATA_1[1]
  PAR_12=DATA_1[2]
  PAR_13=DATA_1[3]
  FPar_11=DATA_2[1]
  FPar_12=DATA_2[2]
  FPar_13=DATA_2[3]

EVENT:
  ' Check for delay
  if (delaymultinuse = 0) then
    ' Force synchronized output now.  We are sending out the data programmed in the last cycle.
    ' This way all the outputs are updated at the beginning of an event, which is well timed.
    ' Doing SyncAll() at the end of an event causes the channel outputs to move around in time,
    ' depending on how many channels are being programmed.
    P2_Sync_All(1111b)  ' Sync Pro II modules
    SyncAll()           ' Sync Pro I modules
    
    ' If we see a negative number, interpret it as a multi-event delay
    if(DATA_1[counts]<0) then
      delaymultinuse=-1*DATA_1[counts]
    endif
  
    Inc counts  ' Get number of updates in next event
  
    IF((99>DATA_1[counts]) and (DATA_1[counts]>=1)) then  'A:Check each element of DATA_1 for an update
      For i=1 to DATA_1[counts] 'B: Loop over number of updates at this time
        Inc updates
        ch=DATA_2[updates]
        '*****************Analog outs***********************      
        if((ch>=1) and (ch<=32)) then
          AnalogWrite(ch,DATA_3[updates])
        endif
        '***********************Digital outs**********
        if(ch=101) then
          DIG_WRITELATCH32(1, DATA_3[updates]) ' Seth Aubin, Aug. 2, 2010: "2" -->"1"
        endif
        
        if(ch=102) then
          DIG_WRITELATCH32(2, DATA_3[updates]) ' Seth Aubin, Aug. 2, 2010: "3" -->"2"
        endif      
      NEXT i            'B for loop  
    ENDIF        'A:
    If(counts>=maxcount+1) then end ' Go to Finish: after final event
  else
    ' Waiting out multi-event delay, decrease number of events to wait by 1
    Dec delaymultinuse
  endif
  
FINISH:
  'Clear all channels
  
  'Debug display
  PAR_3=counts ' Should be one more than maxcount
  PAR_4=maxcount

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

  P2_SYNC_ALL(1)

