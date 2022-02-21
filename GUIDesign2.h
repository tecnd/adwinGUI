#ifndef GUIDESIGN2_H
#define GUIDESIGN2_H
#include "vars.h"

void DrawNewTable(int);

//Right click menus
void CVICALLBACK Dig_Cell_Copy(int, int, int, void *);
void CVICALLBACK Dig_Cell_Paste(int, int, int, void *);
void CVICALLBACK Analog_Cell_Copy(int, int, int, void *);
void CVICALLBACK Analog_Cell_Paste(int, int, int, void *);
void CVICALLBACK Scan_Table_Load(int, int, int, void *);
int CVICALLBACK TOGGLE_CALLBACK(int, int, int, void *, int, int);
int CVICALLBACK CHECKBOX_CALLBACK(int, int, int, void *, int, int);
#endif
