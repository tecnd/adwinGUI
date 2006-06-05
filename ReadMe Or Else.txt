Mar 10, 2006

Abandon hope, all ye who enter here.


This file is intended to provided some insight into the workings of the ADwin GUI program.

Caveats:  This was programmed in LabWindows.  LabWindows is great for rapid development of software, but has some quirks.  Like, C/C++, Labwindows uses base 0 for arrays, however, all the GUI elements work on a base 1 system, so some confusion has happened.  ADBasic is also base 1.


.h Files
Don't modify the .h files.  These are LabWindows generated.  Any changes you make to them are lost on recompile.  If you need to add pre-declarations of non-LabWindows put them in the blah2.h files.













Parts of the program

Main.c	
{
	main:  
		GUI initialization, (i.e. installation of panels)
                Global Variables initialized to 0.
	
	initialization:
		Initializes more variables,
		Build the GUI tables to the appropriate size, given by NUMBERIGITALCHANNELS
 		  and NUMBERANALOGCHANNELS
		Hide/Show appropriate panel components and does some component placement.
	DrawCanvasArrows
		Draws arrows on a canvas, to be use with looping command
}

Adwin.c	
	contains DLL commands to communicate with ADwin unit.  Do not modify!!!!


GuiDesign.c    The 'engine'
{
*********** important subroutines ****************
RunOnce  		Takes the 10 'pages' and creates a single page, a 'meta' list.  
BuildUpdateList		The 'real' engine.  This takes the Meta-arrays and generates the 3 arrays that are transferred to the ADwin.
			Also runs routines to generate the DDS commands (only the 1st is currently enabled)
			Boots ADwin and sends the program to it.  


CalcFcnValue		pretty straightforward.  Calculates next value of the analog channel.
Optimize TimeLoop	compresses strings of zero's in the updatenum[] array.  Replaces with a negative number
UpdateScanValue		Determine next value in a scan.
Timer_Callback		Controls what happens when repeated runs occur...i.e. do we scan a value?
SCANSETTING_CALLBACK

DrawNewTable
DigTableCallback
TimeTableCallback
Cmd_Run_Callback
Cmd_Scan_Callback
Cmd_Stop_Callback

BootAdwin
BootLoad


//array i/o
LoadArrays
SaveArrays
EXPORT_PANEL_CALLBACK
ExportPanel
ConfigExport
ExportScanBuffer
ClearPanel
// array manipulation   	
INSERTCOLUMN_CALLBACK
DELETECOLUMN_CALLBACK
ShiftColumn
COPYCOLUMN_CALLBACK
PASTECOLUMN_CALLBACK
NUM_INSERTIONPAGE_CALLBACK
NUM_INSERTIONCOL_CALLBACK



SetChannelDisplayed
CheckIfWithinLimits
CVICALLBACK RESETZERO_CALLBACK
COMPRESSION_CALLBACK 
SHOWARRAY_CALLBACK
DDS_OFF_CALLBACK
SIMPLETIMING_CALLBACK

CVICALLBACK STREAM_CALLBACK

CVICALLBACK SWITCH_LOOP_CALLBACK




DDSSETUP_CALLBACK
LoadSettings
LoadLastSettings
SaveSettings
DigitalSetCallback
AnalogSetCallback
AnalogTableCallback
GetPage

ToggleN_Callback   (10 of these....cosmetic)
Menu_Bedub_callback

SetDGxx_Callback   xx is delay in us (4 of these)

// cosmetic routines
DrawLoopIndicators.
TitleN_Callback 	(10 of these...cosmetic again)
MoveCanvasStart
MoveCanvasEnd
NOTECHECK_CALLBACK
TGLNUMERIC_CALLBACK	pretty obsolete
DISPLAYDIAL_CALLBACK	pretty obsolete
ReshapeAnalogTable  	pretty obsolete
ReshapeDigitalTable	pretty obsolete
SetDisplayType		pretty obsolete
}



















Parameter controls
AnalogSettings.c    --	Used to set parameters that define how an analog channel is to be 					used.  I.e.  set voltage limits, proportionality constants, offsets.
			Attached to AnalogSetting.uir

DigitalSettings.c   --  much like AnalogSettings.c			
			Attached to DigitalSettings.uir

DDSSettings.c	    --  Attached to DDSSettings.uir
			Set DDS parameters, frequency and PLL multiplier.  
			Since Mar 9, 2006 it is unnecessary to use this panel.

Scan.c		   --   Attached to Scan.uir
			Initializes scan parameters (reading from GuiDesign,uir)
                        Sets what scan mode to use, analog value, time duration, 
                          dds frequency limit, or DDS frequency offset.


DDSControl.c	   --   Attached to DDSControl.uir
			Used to set DDS amplitude, frequency start and stop points
                          and on/off

AnalogControl.c	   --   Attached to AnalogControl.uir
			Sets analog channel values at that time column.
			Can set the channel output waveform (i.e. step, linear ramp, exponential 			          ramp, constant jerk or sine wave




Adding more analog functions:
If adding another function, just add another case in CVICALLBACK RING_CTRLMODE_CALLBACK (located in AnalogControl.c), and modify the ring control on the 
AnalogControl.uir file, and add function handling in CalcFcnVal() in GUIDesign.c