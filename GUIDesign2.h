#ifndef GUIDESIGN2_H
#define GUIDESIGN2_H
#include "vars.h"
#define PRINT_TO_DEBUG //if defined, outputs the array to the debug window

void LoadSettings(void);
void SaveSettings(void);
void ShiftColumn3(int col, int page,int dir);
void RunOnce(void);
int  GetPage(void);
void DrawNewTable(int dimmed);
void CheckActivePages(void);
void SaveArrays(char*,int);
void LoadArrays(char*,int);
void ExportPanel(char*,int);
double CalcFcnValue(int fcn,double Vinit,double Vfinal, double timescale,double telapsed,double celltime);

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
void CVICALLBACK Scan_Table_Load(int panelHandle, int controlID, int MenuItemID, void *callbackData);
int CVICALLBACK TOGGLE_CALLBACK(int, int, int, void *, int, int);

int scancount;
#endif
