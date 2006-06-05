#ifndef GUIDESIGN2_H
#define GUIDESIGN2_H

#include <ansi_c.h>
#include <userint.h>          
#include "Adwin.h"
#include "vars.h"
#include "AnalogControl2.h"
#include "DDSControl2.h"
#include "ddstranslator.h"
#include "DDSSettings2.h"
#include "Scan2.h"
#define PRINT_TO_DEBUG //if defined, outputs the array to the debug window

struct AnVals{
	int		fcn;		//fcn is an integer refering to a function to use.
						// 0-step, 1-linear, 2- exp, 3- 'S' curve
	double 	fval;		//the final value
	double	tscale;		//the timescale to approach final value
	} ;

void LoadSettings(void);   
void SaveSettings(void);
void LoadLastSettings(int check);
void ShiftColumn3(int col, int page,int dir);    
void RunOnce(void); 
int  GetPage(void);
void DrawNewTable(int dimmed);
void CheckActivePages(void);
void SaveArrays(char*,int);
void LoadArrays(char*,int);
void ExportPanel(char*,int);
void BuildUpdateList(double TMatrix[],struct AnVals AMat[NUMBERANALOGCHANNELS+1][500],int DMat[NUMBERDIGITALCHANNELS+1][500],ddsoptions_struct DDSArray[500],ddsoptions_struct DDS2Array[500],dds3options_struct DDS3Array[500], int tsize); 
double CalcFcnValue(int fcn,double Vinit,double Vfinal, double timescale,double telapsed,double celltime);

void ReshapeAnalogTable(int,int,int);
void ReshapeDigitalTable(int,int,int);
void SetChannelDisplayed(int display_setting); //analog, digital of both
void SetDisplayType(int display_setting); //toggle graphic and numeric
double CheckIfWithinLimits(double OutputVoltage, int linenumber);
void SaveLastGuiSettings(void);
void OptimizeTimeLoop(int *,int,int*);
void UpdateScanValue(int);
void ScanSetUp(void);
void ExportScanBuffer(void);

void MoveCanvasStart(int,int); // start arrow indicator, (x pos,on/off (i.e. True/False));
void MoveCanvasEnd(int,int); // end arrow indicator, (x pos,on/off (i.e. True/False));
void DrawLoopIndicators(void);// draw lines to indicate looping region

void CVICALLBACK Dig_Cell_Copy(int panelHandle, int controlID, int MenuItemID, void *callbackData);		  //Right Click
void CVICALLBACK Dig_Cell_Paste(int panelHandle, int controlID, int MenuItemID, void *callbackData);	  //Menu Copy and
void CVICALLBACK Analog_Cell_Copy(int panelHandle, int controlID, int MenuItemID, void *callbackData);	  //Paste Features
void CVICALLBACK Analog_Cell_Paste(int panelHandle, int controlID, int MenuItemID, void *callbackData);   //

int scancount;
#endif
