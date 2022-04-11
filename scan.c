/**
@file scan.c
Displays information on the SCAN panel, and reads the information.
This code doesn't act on the scan information, that is done in
GUIDesign.c::UpdateScanValue

Scan has 2 modes of operation: Analog and Time.
*/

#include "Scan.h"

#include <ansi_c.h>
#include <userint.h>

#include "GUIDesign.h"
#include "GUIDesign2.h"
#include "Scan2.h"
#include "vars.h"

int CVICALLBACK CALLBACK_SCANOK(int panel, int control, int event,
    void* callbackData, int eventData1,
    int eventData2)
{
    switch (event) {
    case EVENT_COMMIT:
        ReadScanValues();
        PScan.ScanMode = 0; // set to Analog scan
        HidePanel(panelHandle7);
        HidePanel(panelHandle4);
        break;
    }
    return 0;
}

// last update:  May 11, 2005
// May12, 2005:  initialize for time scans
void InitializeScanPanel(void)
{
    SetCtrlVal(panelHandle7, SCANPANEL_NUM_COLUMN, PScan.Column);
    SetCtrlVal(panelHandle7, SCANPANEL_NUM_ROW, PScan.Row);
    SetCtrlVal(panelHandle7, SCANPANEL_NUM_PAGE, PScan.Page);

    SetCtrlVal(panelHandle7, SCANPANEL_NUM_CHANNEL, AChName[PScan.Row].chnum);
    SetCtrlVal(panelHandle7, SCANPANEL_NUM_DURATION,
        TimeArray[PScan.Column][PScan.Page]);

    SetCtrlVal(panelHandle7, SCANPANEL_RING_MODE, PScan.Analog.Analog_Mode);
    SetCtrlVal(panelHandle7, SCANPANEL_NUM_SCANSTART, PScan.Analog.Start_Of_Scan);
    SetCtrlVal(panelHandle7, SCANPANEL_NUM_SCANEND, PScan.Analog.End_Of_Scan);
    SetCtrlVal(panelHandle7, SCANPANEL_NUM_SCANSTEP, PScan.Analog.Scan_Step_Size);
    SetCtrlVal(panelHandle7, SCANPANEL_NUM_ITERATIONS,
        PScan.Analog.Iterations_Per_Step);

    SetCtrlVal(panelHandle7, SCANPANEL_NUM_TIMESTART, PScan.Time.Start_Of_Scan);
    SetCtrlVal(panelHandle7, SCANPANEL_NUM_TIMEEND, PScan.Time.End_Of_Scan);
    SetCtrlVal(panelHandle7, SCANPANEL_NUM_TIMESTEP, PScan.Time.Scan_Step_Size);
    SetCtrlVal(panelHandle7, SCANPANEL_NUM_TIMEITER,
        PScan.Time.Iterations_Per_Step);

    DisplayPanel(panelHandle7);
}

// last update:
// May 12,2005:  read in values for time and dds, set mode (ANALOG, TIME or DDS)
//               set initial values
void ReadScanValues(void)
{
    GetCtrlVal(panelHandle7, SCANPANEL_NUM_SCANSTART,
        &PScan.Analog.Start_Of_Scan);
    GetCtrlVal(panelHandle7, SCANPANEL_NUM_SCANEND, &PScan.Analog.End_Of_Scan);
    GetCtrlVal(panelHandle7, SCANPANEL_NUM_SCANSTEP,
        &PScan.Analog.Scan_Step_Size);
    GetCtrlVal(panelHandle7, SCANPANEL_NUM_ITERATIONS,
        &PScan.Analog.Iterations_Per_Step);
    GetCtrlVal(panelHandle7, SCANPANEL_RING_MODE, &PScan.Analog.Analog_Mode);

    GetCtrlVal(panelHandle7, SCANPANEL_NUM_TIMESTART, &PScan.Time.Start_Of_Scan);
    GetCtrlVal(panelHandle7, SCANPANEL_NUM_TIMEEND, &PScan.Time.End_Of_Scan);
    GetCtrlVal(panelHandle7, SCANPANEL_NUM_TIMESTEP, &PScan.Time.Scan_Step_Size);
    GetCtrlVal(panelHandle7, SCANPANEL_NUM_TIMEITER,
        &PScan.Time.Iterations_Per_Step);

    ScanVal.Current_Step = 0;
}

int CVICALLBACK CALLBACK_SCAN_CANCEL(int panel, int control, int event,
    void* callbackData, int eventData1,
    int eventData2)
{
    switch (event) {
    case EVENT_COMMIT:
        PScan.Scan_Active = FALSE;
        HidePanel(panelHandle7);
        HidePanel(panelHandle4);

        break;
    }
    return 0;
}

int CVICALLBACK CALLBACK_TIMESCANOK(int panel, int control, int event,
    void* callbackData, int eventData1,
    int eventData2)
{
    switch (event) {
    case EVENT_COMMIT:

        ReadScanValues();
        PScan.ScanMode = 1; // set to Time scan
        HidePanel(panelHandle7);
        HidePanel(panelHandle4);
        break;
    }
    return 0;
}

int CVICALLBACK CMD_GETSCANVALS_CALLBACK(int panel, int control, int event,
    void* callbackData, int eventData1,
    int eventData2)
{
    int cx = 0, cy = 0, cz = 0;
    switch (event) {
    case EVENT_COMMIT:

        PScan.Column = currentx;
        PScan.Row = currenty;
        PScan.Page = currentpage;
        cx = currentx;
        cy = currenty;
        cz = currentpage;
        PScan.Analog.Analog_Mode = AnalogTable[cx][cy][cz].fcn;
        PScan.Analog.Start_Of_Scan = AnalogTable[cx][cy][cz].fval;
        PScan.Analog.End_Of_Scan = AnalogTable[cx][cy][cz].fval;
        PScan.Analog.Scan_Step_Size = 0.1;
        PScan.Analog.Iterations_Per_Step = 1;

        PScan.Time.Start_Of_Scan = TimeArray[cx][cz];
        PScan.Time.End_Of_Scan = TimeArray[cx][cz];
        PScan.Time.Scan_Step_Size = 0.1;
        PScan.Time.Iterations_Per_Step = 1;

        InitializeScanPanel();
        DisplayPanel(panelHandle7);
        break;
    }
    return 0;
}

int CVICALLBACK CHECK_USE_LIST_CALLBACK(int panel, int control, int event,
    void* callbackData, int eventData1,
    int eventData2)
{
    switch (event) {
    case EVENT_COMMIT:

        break;
    }
    return 0;
}
