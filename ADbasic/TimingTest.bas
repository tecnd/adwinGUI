'<ADbasic Header, Headerversion 001.001>
' Process_Number                 = 1
' Initial_Processdelay           = 3000
' Eventsource                    = External
' Control_long_Delays_for_Stop   = No
' Priority                       = High
' Version                        = 1
' ADbasic_Version                = 6.3.1
' Optimize                       = Yes
' Optimize_Level                 = 1
' Stacksize                      = 1000
' Info_Last_Save                 = NIGHTSHADE  NIGHTSHADE\labadmin
'<Header End>
#Include ADwinPro_All.Inc

dim pattern as long
dim delay as long

INIT:
  delay = 3
  pattern = 0
  CPU_EVENT_CONFIG(0,1,1)
  DigProg1(1,65535)
  Dig_WriteLatch32(1, 0) ' turn outputs off
  Dig_Latch(1)

EVENT:
  if (delay <= 0) then
    ' Alternate digital 32 on and off
    if (pattern = 0) then
      pattern = Shift_Left(1, 31)
    else
      pattern = 0
    endif
    
    Dig_WriteLatch32(1, pattern)
    Dig_Latch(1)
  else
    Dec(delay)
  endif
  
