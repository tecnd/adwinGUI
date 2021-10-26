'<ADbasic Header, Headerversion 001.001>
' Process_Number                 = 1
' Initial_Processdelay           = 3000
' Eventsource                    = Timer
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
dim f as float
INIT:
  pattern = 0
  f = 0
  DigProg1(1,65535)
EVENT:
  'pattern = Shift_Left(1, 31)
  f = -2147483648.0
  pattern = f
  Dig_WriteLatch32(1, pattern)
  Dig_Latch(1)
