#include <ansi_c.h>
#include <userint.h>
#include <cvirte.h>
//#include "Adwin.h"

//#include "AnalogSettings.c" 
		
#include "AnalogSettings.h"
#include "GUIDesign.h"
#include "math.h"

struct AnVals{
	int		fcn;		//fcn is an integer refering to a function to use.
						// 0-step, 1-linear, 2- exp, 3- 'S' curve
	double 	fval;		//the final value
	double	tscale;		//the timescale to approach final value
	} ;
//Function declarations
//in GUIDesign.c
void LoadSettings(void);
void SaveSettings(void);
void LoadLastSettings(int check);
void RunOnce(void);
void ShiftColumn(int col,int dir);
// In AnalogSettings.c
void SetAnalogChannels(void);
void ReadAnalogChannels(void);

// In DigitalSettings.h
void SetDigitalChannels(void);
void ReadDigitalChannels(void);


int  GetPage(void);
void SetControlPanel(void);
void DrawNewTable(int dimmed);
void CheckActivePages(void);

void SaveArrays(char*,int);
void LoadArrays(char*,int);
							 
void BuildUpdateList(double TMatrix[],struct AnVals AMat[16][500],int DMat[16][500],int tsize); 
//void BuildUpdateList(double TMatrix[],double DMat[16][500])
