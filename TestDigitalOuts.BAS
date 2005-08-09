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
dim i,counts,maxcount as long
INIT:
counts=0
maxcount=600000
GlobalDelay=40000
DigProg1(1,65535)
DigProg2(1,65535)
DigProg1(2,65535)
DigProg2(2,65535)
i=0
EVENT:
'SYNCALL()
if(i=0) then
	digout_f(2,0,1)
	DIGOUT_F(2,31,0)
else
	digout_F(2,i,1)
	digout_F(2,i-1,0)
endif

i=i+1
counts=counts+1

if (i>31) then
	i=0
endif

If(counts>=maxcount+1) then end


FINISH:
'Clear all channels
Par_3=counts
Par_4=maxcount
