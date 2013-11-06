****************************************
*	 Prozeﬂnummer = 1
*	 Delay = 1000
*	 Eventsource = 0
*	 Number of Loops = 0
*	 Priorit‰t = 0
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

Function V(value) as float
	V=(value+10)/20*65535
ENDFUNCTION

INIT:
SYNCENABLE(1,da,1)
SYNCENABLE(2,da,1)
SYNCENABLE(3,da,1)
SYNCENABLE(4,da,1)

EVENT:
END


FINISH:
	
WRITEDAC(2,6,V(0))
WRITEDAC(2,8,V(0))

SYNCALL()
EXIT






