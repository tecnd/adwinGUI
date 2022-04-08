// Modified by Seth Aubin on August 2, 2010
//  change to the "ADbasic binary file: "TransferData_August02_2010.TB1"
//  purpose: activate DIO 1 and DIO 2 outputs.


//To DO:  add more DDS 'clips' for copy/paste routines

//2006
//March 9:  Reorder the routines to more closely match the order in which they are executed.
//          Applies to the 'engine' but not the cosmetic/table handling routnes

/*********************** Sandro Gvakharia October 2010 **********************/
//Added DDS vars starting at line 21, added DDS code starting at line 462.
//Works with DDS1, Rb Evaporation
/*********************** Sandro Gvakharia October 2010 **********************/

#include <userint.h>
#include <utility.h>
#include <ansi_c.h>
#include <toolbox.h>
#include "vars.h"
#include "GUIDesign.h"
#include "GUIDesign2.h"
#include "AnalogSettings2.h"
#include "DigitalSettings2.h"


int  GetPage(void);
void DrawNewTable(void);
void CheckActivePages(void);
void SaveArrays(char*,int);
void LoadArrays(char*,int);

//*****************************************************************************************
void LoadSettings(void)
{
	char fsavename[500];
	// prompt for a file, if selected then load the Panel and Arrays
	int status = FileSelectPopupEx("C:\\UserDate\\Data", "*.pan", "", "Load Settings", VAL_LOAD_BUTTON, 0, 0, fsavename);
	if (status == VAL_EXISTING_FILE_SELECTED)
	{
		RecallPanelState(panelHandle, fsavename, 1);
		LoadArrays(fsavename, strlen(fsavename));
		DrawNewTable();
		TOGGLE1_CALLBACK(panelHandle, 0, EVENT_COMMIT, NULL, 0, 0);
	}
}

void InjectDescriptions(int panel, int prop, int start, int offset, FILE *stream)
{
	char str[8];
	char bigbuff[100];
	fseek(stream, start, SEEK_SET);
	for (int i = 0; i < 17; i++)
	{
		GetTableCellVal(panel, prop, MakePoint(i + 1, 1), bigbuff);
		strncpy(str, bigbuff, 8); // Truncate name to 8 chars
		fwrite(str, sizeof str, 1, stream);
		fseek(stream, offset - sizeof str, SEEK_CUR);
	}
}

void InjectCheckbox(int panel, int prop, int start, int offset, FILE *stream)
{
	int check;
	GetCtrlVal(panelHandle, PANEL_CHECKBOX, &check);
	fseek(stream, start + offset, SEEK_SET);
	static char T = 0x01;
	static char F = 0x00;
	if (check)
	{
		fwrite(&T, sizeof T, 1, stream);
	}
	else
	{
		fwrite(&F, sizeof F, 1, stream);
	}
}

void SaveSettings(void)
{
	char fsavename[500];
	int status = FileSelectPopupEx("C:\\UserDate\\Data", "*.pan", "", "Save Settings", VAL_SAVE_BUTTON, 0, 0, fsavename);
	if (status == VAL_NO_FILE_SELECTED)
	{
		return;
	}
	// Create file pointers
	FILE *pTemplate = fopen("NewTemplate.pan", "rb");
	if (pTemplate == NULL)
	{
		MessagePopup("File Error", "Template not found");
		return;
	}
	FILE *pOut = fopen(fsavename, "wb");
	if (pOut == NULL)
	{
		fclose(pTemplate);
		MessagePopup("File Error", "Unable to write to output file");
		return;
	}
	// Copy the template
	char buff;
	while (fread(&buff, sizeof buff, 1, pTemplate))
	{
		fwrite(&buff, sizeof buff, 1, pOut);
	}
	// Edit template
	// First page descriptions start at 0xA0, separated by 0x40
	// Checkbox offset by 0x627
	InjectDescriptions(panelHandle, PANEL_LABEL_1, 0xA0, 0x40, pOut);
	InjectCheckbox(panelHandle, PANEL_CHECKBOX, 0xA0, 0x627, pOut);

	// Second page descriptions start at 0x738
	InjectDescriptions(panelHandle, PANEL_LABEL_2, 0x738, 0x40, pOut);
	InjectCheckbox(panelHandle, PANEL_CHECKBOX_2, 0x738, 0x627, pOut);

	// Third page descriptions start at 0xDD0
	InjectDescriptions(panelHandle, PANEL_LABEL_3, 0xDD0, 0x40, pOut);
	InjectCheckbox(panelHandle, PANEL_CHECKBOX_3, 0xDD0, 0x627, pOut);

	// Fourth page descriptions start at 0x1468
	InjectDescriptions(panelHandle, PANEL_LABEL_4, 0x1468, 0x40, pOut);
	InjectCheckbox(panelHandle, PANEL_CHECKBOX_4, 0x1468, 0x627, pOut);

	// Fifth page descriptions start at 0x1B00
	InjectDescriptions(panelHandle, PANEL_LABEL_5, 0x1B00, 0x40, pOut);
	InjectCheckbox(panelHandle, PANEL_CHECKBOX_5, 0x1B00, 0x627, pOut);

	// Sixth page descriptions start at 0x2198
	InjectDescriptions(panelHandle, PANEL_LABEL_6, 0x2198, 0x40, pOut);
	InjectCheckbox(panelHandle, PANEL_CHECKBOX_6, 0x2198, 0x627, pOut);

	// Seventh page descriptions start at 0x2830
	InjectDescriptions(panelHandle, PANEL_LABEL_7, 0x2830, 0x40, pOut);
	InjectCheckbox(panelHandle, PANEL_CHECKBOX_8, 0x2830, 0x627, pOut);

	// Eighth page descriptions start at 0x2EC8
	InjectDescriptions(panelHandle, PANEL_LABEL_8, 0x2EC8, 0x40, pOut);
	InjectCheckbox(panelHandle, PANEL_CHECKBOX_8, 0x2EC8, 0x627, pOut);

	// Ninth page descriptions start at 0x3560
	InjectDescriptions(panelHandle, PANEL_LABEL_9, 0x3560, 0x40, pOut);
	InjectCheckbox(panelHandle, PANEL_CHECKBOX_9, 0x3560, 0x627, pOut);

	// Tenth page descriptions start at 0x3BF8
	InjectDescriptions(panelHandle, PANEL_LABEL_10, 0x3BF8, 0x40, pOut);
	InjectCheckbox(panelHandle, PANEL_CHECKBOX_10, 0x3BF8, 0x627, pOut);

	// Inject scan table values. Start at 0x2CBD4, offset 0x38
	fseek(pOut, 0x2CBD4, SEEK_SET);
	for (int i = 1; i <= 32; i++)
	{
		// Use unsigned int64 instead of double so we can directly use ToBigEndian64()
		unsigned __int64 cellVal;
		GetTableCellVal(panelHandle, PANEL_SCAN_TABLE, MakePoint(1, i), (double *)&cellVal);
		unsigned __int64 cellValCorrected = ToBigEndian64(cellVal);
		fwrite(&cellValCorrected, sizeof cellValCorrected, 1, pOut);
		fseek(pOut, 0x38 - sizeof cellValCorrected, SEEK_CUR);
	}

	fclose(pTemplate);
	fclose(pOut);

	SaveArrays(fsavename, strlen(fsavename));
	MessagePopup("Success", "Success");
}
// Helper function to alternate color every three rows
int ColorPicker(int index)
{
	index--; // correct for 1-based indices
	if ((index / 3) % 2) return VAL_GRAY;
	else return 0x00B0B0B0;
}
//*********************************************************************
void DrawNewTable(void)
{
	// Hide table so draw goes faster
	SetCtrlAttribute(panelHandle, PANEL_ANALOGTABLE, ATTR_VISIBLE, 0);
	SetCtrlAttribute(panelHandle, PANEL_DIGTABLE, ATTR_VISIBLE, 0);
	int page = GetPage(); // Check which page is active.

	CheckActivePages();
	if (ischecked[page] == FALSE)
	{ // dim the tables
		SetCtrlAttribute(panelHandle, PANEL_ANALOGTABLE, ATTR_DIMMED, 1);
		SetCtrlAttribute(panelHandle, PANEL_DIGTABLE, ATTR_DIMMED, 1);
		SetCtrlAttribute(panelHandle, PANEL_TIMETABLE, ATTR_DIMMED, 1);
	}
	else
	{ //undim the tables
		SetCtrlAttribute(panelHandle, PANEL_ANALOGTABLE, ATTR_DIMMED, 0);
		SetCtrlAttribute(panelHandle, PANEL_DIGTABLE, ATTR_DIMMED, 0);
		SetCtrlAttribute(panelHandle, PANEL_TIMETABLE, ATTR_DIMMED, 0);
	}

	for (int i = 1; i <= NUMBEROFCOLUMNS; i++) // scan over the columns
	{

		SetTableCellAttribute(panelHandle, PANEL_TIMETABLE, MakePoint(i, 1), ATTR_CELL_DIMMED, 0);
		for (int j = 1; j <= NUMBERANALOGCHANNELS; j++) // scan over analog channels
		{
			int cmode = AnalogTable[i][j][page].fcn;
			double vnow = AnalogTable[i][j][page].fval;

			if (cmode != 6)
				// write the ending value into the cell
				SetTableCellAttribute(panelHandle, PANEL_ANALOGTABLE, MakePoint(i, j), ATTR_CTRL_VAL, vnow);
			else if (i == 1 && page == 1)
			{
				ConfirmPopup("User Error", "Do not use \"Same as Previous\" Setting for Column 1 Page 1.\nThis results in unstable behaviour.\nResetting Cell Function to default: step");
				SetTableCellAttribute(panelHandle, PANEL_ANALOGTABLE, MakePoint(i, j), ATTR_CTRL_VAL, 0.0);
				cmode = 1;
				AnalogTable[1][j][1].fcn = cmode;
				AnalogTable[1][j][1].fval = 0.0;
				AnalogTable[1][j][1].tscale = 0.0;
			}
			else
			{
				//888 indicates cell will take value of previous cell
				SetTableCellAttribute(panelHandle, PANEL_ANALOGTABLE, MakePoint(i, j), ATTR_CTRL_VAL, 888.0);
			}

			SetTableCellAttribute(panelHandle, PANEL_ANALOGTABLE, MakePoint(i, j), ATTR_TEXT_COLOR, VAL_BLACK);
			// Change the cell color depending on the function type
			switch (cmode)
			{
				case 1: // step
					if (vnow == 0)
					{
						SetTableCellAttribute(panelHandle, PANEL_ANALOGTABLE, MakePoint(i, j), ATTR_TEXT_BGCOLOR, ColorPicker(j));
					}
					else
					{
						SetTableCellAttribute(panelHandle, PANEL_ANALOGTABLE, MakePoint(i, j), ATTR_TEXT_BGCOLOR, VAL_RED);
					}
					break;

				case 2: // linear ramp
					SetTableCellAttribute(panelHandle, PANEL_ANALOGTABLE, MakePoint(i, j), ATTR_TEXT_BGCOLOR, VAL_GREEN);
					break;

				case 3: // exponential ramp
					SetTableCellAttribute(panelHandle, PANEL_ANALOGTABLE, MakePoint(i, j), ATTR_TEXT_BGCOLOR, VAL_BLUE);
					SetTableCellAttribute(panelHandle, PANEL_ANALOGTABLE, MakePoint(i, j), ATTR_TEXT_COLOR, VAL_WHITE);
					break;

				case 4: // constant jerk function
					SetTableCellAttribute(panelHandle, PANEL_ANALOGTABLE, MakePoint(i, j), ATTR_TEXT_BGCOLOR, VAL_MAGENTA);
					break;

				case 5: // Sine wave output
					SetTableCellAttribute(panelHandle, PANEL_ANALOGTABLE, MakePoint(i, j), ATTR_TEXT_BGCOLOR, VAL_CYAN);
					break;

				case 6: //Same as Previous Output
					SetTableCellAttribute(panelHandle, PANEL_ANALOGTABLE, MakePoint(i, j), ATTR_TEXT_BGCOLOR, VAL_YELLOW);
					break;

				default:
					SetTableCellAttribute(panelHandle, PANEL_ANALOGTABLE, MakePoint(i, j), ATTR_TEXT_BGCOLOR, ColorPicker(j));
					break;
			}
		}
		for (int j = 1; j <= NUMBERDIGITALCHANNELS; j++) // scan over analog channels
		{
			// if a digital value is high, colour the cell red
			if (DigTableValues[i][j][page] == 1)
			{
				SetTableCellAttribute(panelHandle, PANEL_DIGTABLE, MakePoint(i, j), ATTR_TEXT_BGCOLOR, VAL_RED);
			}
			else
			{
				SetTableCellAttribute(panelHandle, PANEL_DIGTABLE, MakePoint(i, j), ATTR_TEXT_BGCOLOR, ColorPicker(j));
			}
		} //Done digital drawing.



		// update the times row
		SetTableCellAttribute(panelHandle, PANEL_TIMETABLE, MakePoint(i, 1), ATTR_CTRL_VAL, TimeArray[i][page]);
	}

	if ((currentpage == PScan.Page) && (PScan.Scan_Active == TRUE)) //display the cell active for a parameter scan
	{
		switch (PScan.ScanMode)
		{
		case 0:
			SetTableCellAttribute(panelHandle, PANEL_ANALOGTABLE, MakePoint(PScan.Column, PScan.Row), ATTR_TEXT_BGCOLOR, VAL_DK_YELLOW);
			break;
		case 1:
			SetTableCellAttribute(panelHandle, PANEL_TIMETABLE, MakePoint(PScan.Column, 1), ATTR_TEXT_BGCOLOR, VAL_DK_YELLOW);
			break;
		case 2:
			SetTableCellAttribute(panelHandle, PANEL_ANALOGTABLE, MakePoint(PScan.Column, 25), ATTR_TEXT_BGCOLOR, VAL_DK_YELLOW);
			break;
		}
	}
	// Done drawing, show table
	SetCtrlAttribute(panelHandle, PANEL_ANALOGTABLE, ATTR_VISIBLE, 1);
	SetCtrlAttribute(panelHandle, PANEL_DIGTABLE, ATTR_VISIBLE, 1);
}

//***********************************************************************************

void CVICALLBACK MENU_CALLBACK(int menuBar, int menuItem, void *callbackData,
							   int panel)
{
	switch (menuItem)
	{
	case MENU_FILE_SAVESET:
		SaveSettings();
		break;

	case MENU_FILE_LOADSET:
		LoadSettings();
		break;
	}
}

//********************************************************************************************
int GetPage(void)
{
	return currentpage;
	//Scan and find out what page we are on....
}

//*************************************************************************************
int CVICALLBACK TOGGLE1_CALLBACK(int panel, int control, int event,
								 void *callbackData, int eventData1, int eventData2)
{

	switch (event)
	{
	case EVENT_COMMIT:
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE1, 1);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE2, 0);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE3, 0);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE4, 0);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE5, 0);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE6, 0);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE7, 0);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE8, 0);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE9, 0);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE10, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_1, ATTR_VISIBLE, 1);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_2, ATTR_VISIBLE, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_3, ATTR_VISIBLE, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_4, ATTR_VISIBLE, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_5, ATTR_VISIBLE, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_6, ATTR_VISIBLE, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_7, ATTR_VISIBLE, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_8, ATTR_VISIBLE, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_9, ATTR_VISIBLE, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_10, ATTR_VISIBLE, 0);
		currentpage = 1;
		DrawNewTable();

		break;
	}
	return 0;
}

int CVICALLBACK TOGGLE2_CALLBACK(int panel, int control, int event,
								 void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
	case EVENT_COMMIT:
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE1, 0);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE2, 1);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE3, 0);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE4, 0);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE5, 0);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE6, 0);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE7, 0);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE8, 0);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE9, 0);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE10, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_1, ATTR_VISIBLE, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_2, ATTR_VISIBLE, 1);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_3, ATTR_VISIBLE, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_4, ATTR_VISIBLE, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_5, ATTR_VISIBLE, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_6, ATTR_VISIBLE, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_7, ATTR_VISIBLE, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_8, ATTR_VISIBLE, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_9, ATTR_VISIBLE, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_10, ATTR_VISIBLE, 0);
		currentpage = 2;
		DrawNewTable();
		break;
	}
	return 0;
}

int CVICALLBACK TOGGLE3_CALLBACK(int panel, int control, int event,
								 void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
	case EVENT_COMMIT:
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE1, 0);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE2, 0);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE3, 1);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE4, 0);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE5, 0);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE6, 0);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE7, 0);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE8, 0);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE9, 0);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE10, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_1, ATTR_VISIBLE, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_2, ATTR_VISIBLE, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_3, ATTR_VISIBLE, 1);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_4, ATTR_VISIBLE, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_5, ATTR_VISIBLE, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_6, ATTR_VISIBLE, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_7, ATTR_VISIBLE, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_8, ATTR_VISIBLE, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_9, ATTR_VISIBLE, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_10, ATTR_VISIBLE, 0);
		currentpage = 3;
		DrawNewTable();
		break;
	}
	return 0;
}

int CVICALLBACK TOGGLE4_CALLBACK(int panel, int control, int event,
								 void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
	case EVENT_COMMIT:
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE1, 0);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE2, 0);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE3, 0);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE4, 1);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE5, 0);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE6, 0);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE7, 0);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE8, 0);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE9, 0);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE10, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_1, ATTR_VISIBLE, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_2, ATTR_VISIBLE, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_3, ATTR_VISIBLE, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_4, ATTR_VISIBLE, 1);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_5, ATTR_VISIBLE, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_6, ATTR_VISIBLE, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_7, ATTR_VISIBLE, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_8, ATTR_VISIBLE, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_9, ATTR_VISIBLE, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_10, ATTR_VISIBLE, 0);
		currentpage = 4;
		DrawNewTable();
		break;
	}
	return 0;
}

int CVICALLBACK TOGGLE5_CALLBACK(int panel, int control, int event,
								 void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
	case EVENT_COMMIT:
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE1, 0);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE2, 0);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE3, 0);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE4, 0);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE5, 1);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE6, 0);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE7, 0);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE8, 0);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE9, 0);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE10, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_1, ATTR_VISIBLE, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_2, ATTR_VISIBLE, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_3, ATTR_VISIBLE, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_4, ATTR_VISIBLE, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_5, ATTR_VISIBLE, 1);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_6, ATTR_VISIBLE, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_7, ATTR_VISIBLE, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_8, ATTR_VISIBLE, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_9, ATTR_VISIBLE, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_10, ATTR_VISIBLE, 0);
		currentpage = 5;
		DrawNewTable();
		break;
	}
	return 0;
}

int CVICALLBACK TOGGLE6_CALLBACK(int panel, int control, int event,
								 void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
	case EVENT_COMMIT:
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE1, 0);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE2, 0);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE3, 0);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE4, 0);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE5, 0);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE6, 1);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE7, 0);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE8, 0);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE9, 0);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE10, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_1, ATTR_VISIBLE, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_2, ATTR_VISIBLE, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_3, ATTR_VISIBLE, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_4, ATTR_VISIBLE, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_5, ATTR_VISIBLE, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_6, ATTR_VISIBLE, 1);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_7, ATTR_VISIBLE, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_8, ATTR_VISIBLE, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_9, ATTR_VISIBLE, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_10, ATTR_VISIBLE, 0);
		currentpage = 6;
		DrawNewTable();
		break;
	}
	return 0;
}

int CVICALLBACK TOGGLE7_CALLBACK(int panel, int control, int event,
								 void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
	case EVENT_COMMIT:
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE1, 0);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE2, 0);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE3, 0);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE4, 0);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE5, 0);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE6, 0);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE7, 1);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE8, 0);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE9, 0);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE10, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_1, ATTR_VISIBLE, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_2, ATTR_VISIBLE, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_3, ATTR_VISIBLE, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_4, ATTR_VISIBLE, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_5, ATTR_VISIBLE, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_6, ATTR_VISIBLE, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_7, ATTR_VISIBLE, 1);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_8, ATTR_VISIBLE, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_9, ATTR_VISIBLE, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_10, ATTR_VISIBLE, 0);
		currentpage = 7;
		DrawNewTable();
		break;
	}
	return 0;
}

int CVICALLBACK TOGGLE8_CALLBACK(int panel, int control, int event,
								 void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
	case EVENT_COMMIT:
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE1, 0);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE2, 0);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE3, 0);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE4, 0);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE5, 0);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE6, 0);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE7, 0);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE8, 1);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE9, 0);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE10, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_1, ATTR_VISIBLE, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_2, ATTR_VISIBLE, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_3, ATTR_VISIBLE, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_4, ATTR_VISIBLE, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_5, ATTR_VISIBLE, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_6, ATTR_VISIBLE, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_7, ATTR_VISIBLE, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_8, ATTR_VISIBLE, 1);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_9, ATTR_VISIBLE, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_10, ATTR_VISIBLE, 0);
		currentpage = 8;
		DrawNewTable();
		break;
	}
	return 0;
}

int CVICALLBACK TOGGLE9_CALLBACK(int panel, int control, int event,
								 void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
	case EVENT_COMMIT:
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE1, 0);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE2, 0);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE3, 0);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE4, 0);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE5, 0);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE6, 0);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE7, 0);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE8, 0);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE9, 1);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE10, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_1, ATTR_VISIBLE, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_2, ATTR_VISIBLE, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_3, ATTR_VISIBLE, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_4, ATTR_VISIBLE, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_5, ATTR_VISIBLE, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_6, ATTR_VISIBLE, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_7, ATTR_VISIBLE, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_8, ATTR_VISIBLE, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_9, ATTR_VISIBLE, 1);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_10, ATTR_VISIBLE, 0);
		currentpage = 9;
		DrawNewTable();
		break;
	}
	return 0;
}

int CVICALLBACK TOGGLE10_CALLBACK(int panel, int control, int event,
								  void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
	case EVENT_COMMIT:
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE1, 0);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE2, 0);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE3, 0);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE4, 0);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE5, 0);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE6, 0);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE7, 0);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE8, 0);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE9, 0);
		SetCtrlVal(panelHandle, PANEL_TB_SHOWPHASE10, 1);

		SetCtrlAttribute(panelHandle, PANEL_LABEL_1, ATTR_VISIBLE, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_2, ATTR_VISIBLE, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_3, ATTR_VISIBLE, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_4, ATTR_VISIBLE, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_5, ATTR_VISIBLE, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_6, ATTR_VISIBLE, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_7, ATTR_VISIBLE, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_8, ATTR_VISIBLE, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_9, ATTR_VISIBLE, 0);
		SetCtrlAttribute(panelHandle, PANEL_LABEL_10, ATTR_VISIBLE, 1);

		currentpage = 10;
		DrawNewTable();
		break;
	}
	return 0;
}

void CheckActivePages(void)
{
	int bool;
	GetCtrlVal(panelHandle, PANEL_CHECKBOX, &bool);
	ischecked[1] = bool;
	GetCtrlVal(panelHandle, PANEL_CHECKBOX_2, &bool);
	ischecked[2] = bool;
	GetCtrlVal(panelHandle, PANEL_CHECKBOX_3, &bool);
	ischecked[3] = bool;
	GetCtrlVal(panelHandle, PANEL_CHECKBOX_4, &bool);
	ischecked[4] = bool;
	GetCtrlVal(panelHandle, PANEL_CHECKBOX_5, &bool);
	ischecked[5] = bool;
	GetCtrlVal(panelHandle, PANEL_CHECKBOX_6, &bool);
	ischecked[6] = bool;
	GetCtrlVal(panelHandle, PANEL_CHECKBOX_7, &bool);
	ischecked[7] = bool;
	GetCtrlVal(panelHandle, PANEL_CHECKBOX_8, &bool);
	ischecked[8] = bool;
	GetCtrlVal(panelHandle, PANEL_CHECKBOX_9, &bool);
	ischecked[9] = bool;
	GetCtrlVal(panelHandle, PANEL_CHECKBOX_10, &bool);
	ischecked[10] = bool;
}

//**********************************************************************************
void LoadArrays(char savedname[500], int csize)
{
	/* Laods all Panel attributes and values which are not saved automatically by the NI function SavePanelState.
	   the values are stored in the .arr file by SaveArrays
	   x,y, and zval give the 3D table dimensions but are not critical to the saving/loading operation

	   Note that if the lengths of any of the data arrays are changed previous saves will not be able to be laoded.
	   If necessary See AdwinGUI Panel Converter V11-V12 (created June 01, 2006)
	*/

	FILE *fdata;
	int xval = 16, yval = 16, zval = 10;
	char buff[500] = "", buff2[100] = "";
	strncat(buff, savedname, csize - 4);
	strcat(buff, ".arr");
	if ((fdata = fopen(buff, "r")) == NULL)
	{
		MessagePopup("Load Error", "Failed to load data arrays");
		//	exit(1);
	}

	fread(&xval, sizeof xval, 1, fdata);
	fread(&yval, sizeof yval, 1, fdata);
	fread(&zval, sizeof zval, 1, fdata);
	//now for the times.
	fread(&TimeArray, (sizeof TimeArray), 1, fdata);
	//and the analog data
	fread(&AnalogTable, (sizeof AnalogTable), 1, fdata);
	fread(&DigTableValues, (sizeof DigTableValues), 1, fdata);
	fread(&AChName, (sizeof AChName), 1, fdata);
	fread(&DChName, sizeof DChName, 1, fdata);
	fread(&ddstable, (sizeof ddstable), 1, fdata);
	fread(&buff2, sizeof buff2, 1, fdata);

	SetCtrlAttribute(panelHandle, PANEL_TB_SHOWPHASE1, ATTR_OFF_TEXT, buff2);
	SetCtrlAttribute(panelHandle, PANEL_TB_SHOWPHASE1, ATTR_ON_TEXT, buff2);
	fread(&buff2, sizeof buff2, 1, fdata);

	SetCtrlAttribute(panelHandle, PANEL_TB_SHOWPHASE2, ATTR_OFF_TEXT, buff2);
	SetCtrlAttribute(panelHandle, PANEL_TB_SHOWPHASE2, ATTR_ON_TEXT, buff2);
	fread(&buff2, sizeof buff2, 1, fdata);
	SetCtrlAttribute(panelHandle, PANEL_TB_SHOWPHASE3, ATTR_OFF_TEXT, buff2);
	SetCtrlAttribute(panelHandle, PANEL_TB_SHOWPHASE3, ATTR_ON_TEXT, buff2);
	fread(&buff2, sizeof buff2, 1, fdata);
	SetCtrlAttribute(panelHandle, PANEL_TB_SHOWPHASE4, ATTR_OFF_TEXT, buff2);
	SetCtrlAttribute(panelHandle, PANEL_TB_SHOWPHASE4, ATTR_ON_TEXT, buff2);
	fread(&buff2, sizeof buff2, 1, fdata);
	SetCtrlAttribute(panelHandle, PANEL_TB_SHOWPHASE5, ATTR_OFF_TEXT, buff2);
	SetCtrlAttribute(panelHandle, PANEL_TB_SHOWPHASE5, ATTR_ON_TEXT, buff2);
	fread(&buff2, sizeof buff2, 1, fdata);
	SetCtrlAttribute(panelHandle, PANEL_TB_SHOWPHASE6, ATTR_OFF_TEXT, buff2);
	SetCtrlAttribute(panelHandle, PANEL_TB_SHOWPHASE6, ATTR_ON_TEXT, buff2);
	fread(&buff2, sizeof buff2, 1, fdata);
	SetCtrlAttribute(panelHandle, PANEL_TB_SHOWPHASE7, ATTR_OFF_TEXT, buff2);
	SetCtrlAttribute(panelHandle, PANEL_TB_SHOWPHASE7, ATTR_ON_TEXT, buff2);
	fread(&dds2table, (sizeof dds2table), 1, fdata);
	fread(&dds3table, (sizeof dds3table), 1, fdata);

	fread(&DDSFreq.extclock, sizeof DDSFreq.extclock, 1, fdata);
	fread(&DDSFreq.PLLmult, sizeof DDSFreq.PLLmult, 1, fdata);
	DDSFreq.clock = DDSFreq.extclock * DDSFreq.PLLmult;

	fclose(fdata);

	SetAnalogChannels();
	SetDigitalChannels();
}

//*****************************************************************************************
void SaveArrays(char savedname[500], int csize)
{
	/* Saves all Panel attributes and values which are not saved automatically by the NI function SavePanelState
	   The values are stored in the .arr file
	*/

	char buff[500] = "", buff2[80];
	strncpy(buff, savedname, csize - 4);
	strcat(buff, ".arr");

	FILE *fdata = fopen(buff, "wb");
	if (fdata == NULL)
	{
		MessagePopup("Save Error", "Failed to save");
		return;
	}

	// make properly sized structures and copy over values
	double NewTimeArray[21][15] = {0};
	for (int col = 0; col < NUMBEROFCOLUMNS + 1; col++)
	{
		for (int page = 0; page < NUMBEROFPAGES; page++)
		{
			NewTimeArray[col][page] = TimeArray[col][page];
		}
	}

	struct AnalogTableValues NewAnalogTable[21][41][15] = {0};
	for (int col = 0; col < NUMBEROFCOLUMNS + 1; col++)
	{
		for (int channel = 0; channel < NUMBERANALOGCHANNELS + 1; channel++)
		{
			for (int page = 0; page < NUMBEROFPAGES; page++)
			{
				NewAnalogTable[col][channel][page] = AnalogTable[col][channel][page];
			}
		}
	}

	int NewDigTableValues[21][70][15] = {0};
	for (int col = 0; col < NUMBEROFCOLUMNS + 1; col++)
	{
		for (int channel = 0; channel < MAXDIGITAL; channel++)
		{
			for (int page = 0; page < NUMBEROFPAGES; page++)
			{
				NewDigTableValues[col][channel][page] = DigTableValues[col][channel][page];
			}
		}
	}

	struct AnalogChannelProperties NewAChName[50] = {0};
	for (int channel = 0; channel < MAXANALOG; channel++)
	{
		NewAChName[channel] = AChName[channel];
	}

	struct DigitalChannelProperties NewDChName[70] = {0};
	for (int channel = 0; channel < MAXDIGITAL; channel++)
	{
		NewDChName[channel] = DChName[channel];
	}

	// Write to file
	fwrite(&NewTimeArray, sizeof NewTimeArray, 1, fdata);

	fwrite(&NewAnalogTable, sizeof NewAnalogTable, 1, fdata);
	fwrite(&NewDigTableValues, sizeof NewDigTableValues, 1, fdata);

	fwrite(&NewAChName, sizeof NewAChName, 1, fdata);
	fwrite(&NewDChName, sizeof NewDChName, 1, fdata);

	// Page names
	GetCtrlAttribute(panelHandle, PANEL_TB_SHOWPHASE1, ATTR_OFF_TEXT, buff2);
	fwrite(&buff2, sizeof buff2, 1, fdata);
	GetCtrlAttribute(panelHandle, PANEL_TB_SHOWPHASE2, ATTR_OFF_TEXT, buff2);
	fwrite(&buff2, sizeof buff2, 1, fdata);
	GetCtrlAttribute(panelHandle, PANEL_TB_SHOWPHASE3, ATTR_OFF_TEXT, buff2);
	fwrite(&buff2, sizeof buff2, 1, fdata);
	GetCtrlAttribute(panelHandle, PANEL_TB_SHOWPHASE4, ATTR_OFF_TEXT, buff2);
	fwrite(&buff2, sizeof buff2, 1, fdata);
	GetCtrlAttribute(panelHandle, PANEL_TB_SHOWPHASE5, ATTR_OFF_TEXT, buff2);
	fwrite(&buff2, sizeof buff2, 1, fdata);
	GetCtrlAttribute(panelHandle, PANEL_TB_SHOWPHASE6, ATTR_OFF_TEXT, buff2);
	fwrite(&buff2, sizeof buff2, 1, fdata);
	GetCtrlAttribute(panelHandle, PANEL_TB_SHOWPHASE7, ATTR_OFF_TEXT, buff2);
	fwrite(&buff2, sizeof buff2, 1, fdata);
	GetCtrlAttribute(panelHandle, PANEL_TB_SHOWPHASE8, ATTR_OFF_TEXT, buff2);
	fwrite(&buff2, sizeof buff2, 1, fdata);
	GetCtrlAttribute(panelHandle, PANEL_TB_SHOWPHASE9, ATTR_OFF_TEXT, buff2);
	fwrite(&buff2, sizeof buff2, 1, fdata);
	GetCtrlAttribute(panelHandle, PANEL_TB_SHOWPHASE10, ATTR_OFF_TEXT, buff2);
	fwrite(&buff2, sizeof buff2, 1, fdata);
	// Need four more buttons
	char emptyName[80] = "Empty";
	for (int i = 0; i < 4; i++)
	{
		fwrite(&emptyName, sizeof emptyName, 1, fdata);
	}

	fclose(fdata);
}
/************************************************************************
Author: Stefan
-------
Date Created: August 2004
-------
Description: Resizes the analog table on the gui
-------
Return Value: void
-------
Parameters: new top, left and height values for the list box
*************************************************************************/
void ReshapeAnalogTable(int top, int left, int height)
{
	int tempint;
	int modheight;

	SetTableRowAttribute(panelHandle, PANEL_ANALOGTABLE, -1, ATTR_SIZE_MODE, VAL_USE_EXPLICIT_SIZE);
	SetTableRowAttribute(panelHandle, PANEL_ANALOGTABLE, -1, ATTR_ROW_HEIGHT, (height) / (NUMBERANALOGCHANNELS + NUMBERDDS));
	SetTableRowAttribute(panelHandle, PANEL_TBL_ANAMES, -1, ATTR_SIZE_MODE, VAL_USE_EXPLICIT_SIZE);
	SetTableRowAttribute(panelHandle, PANEL_TBL_ANAMES, -1, ATTR_ROW_HEIGHT, (height) / (NUMBERANALOGCHANNELS + NUMBERDDS));
	SetTableRowAttribute(panelHandle, PANEL_TBL_ANALOGUNITS, -1, ATTR_SIZE_MODE, VAL_USE_EXPLICIT_SIZE);
	SetTableRowAttribute(panelHandle, PANEL_TBL_ANALOGUNITS, -1, ATTR_ROW_HEIGHT, (height) / (NUMBERANALOGCHANNELS + NUMBERDDS));
	modheight = (NUMBERANALOGCHANNELS + NUMBERDDS) * (int)((height) / (NUMBERANALOGCHANNELS + NUMBERDDS)) + 3;

	//resize the analog table and all it's related list boxes
	SetCtrlAttribute(panelHandle, PANEL_ANALOGTABLE, ATTR_HEIGHT, modheight);
	SetCtrlAttribute(panelHandle, PANEL_ANALOGTABLE, ATTR_LEFT, left);
	SetCtrlAttribute(panelHandle, PANEL_ANALOGTABLE, ATTR_TOP, top);

	SetCtrlAttribute(panelHandle, PANEL_TBL_ANAMES, ATTR_LEFT, left - 165);
	SetCtrlAttribute(panelHandle, PANEL_TBL_ANAMES, ATTR_TOP, top);
	SetCtrlAttribute(panelHandle, PANEL_TBL_ANAMES, ATTR_HEIGHT, modheight);

	SetCtrlAttribute(panelHandle, PANEL_TBL_ANALOGUNITS, ATTR_HEIGHT, modheight);
	SetCtrlAttribute(panelHandle, PANEL_TBL_ANALOGUNITS, ATTR_TOP, top);
	SetCtrlAttribute(panelHandle, PANEL_TBL_ANALOGUNITS, ATTR_LEFT, left + 705);

	// move the DDS offsets
	tempint = height / 27;
	SetCtrlAttribute(panelHandle, PANEL_NUM_DDS_OFFSET, ATTR_TOP, top + height - 3 * tempint - 6);
	SetCtrlAttribute(panelHandle, PANEL_NUM_DDS_OFFSET, ATTR_LEFT, 880);
	SetCtrlAttribute(panelHandle, PANEL_NUM_DDS2_OFFSET, ATTR_TOP, top + height - 2 * tempint - 5);
	SetCtrlAttribute(panelHandle, PANEL_NUM_DDS2_OFFSET, ATTR_LEFT, 880);
	SetCtrlAttribute(panelHandle, PANEL_NUM_DDS3_OFFSET, ATTR_TOP, top + height - 1 * tempint - 4);
	SetCtrlAttribute(panelHandle, PANEL_NUM_DDS3_OFFSET, ATTR_LEFT, 880);
}
/************************************************************************
Author: Stefan
-------
Date Created: August 2004
-------
Description: Resizes the digital table on the gui
-------
Return Value: void
-------
Parameters: new top, left and height values for the list box
*************************************************************************/
void ReshapeDigitalTable(int top, int left, int height)
{
	SetCtrlAttribute(panelHandle, PANEL_DIGTABLE, ATTR_HEIGHT, height);
	SetCtrlAttribute(panelHandle, PANEL_DIGTABLE, ATTR_LEFT, left);
	SetCtrlAttribute(panelHandle, PANEL_DIGTABLE, ATTR_TOP, top);
	SetCtrlAttribute(panelHandle, PANEL_TBL_DIGNAMES, ATTR_TOP, top);
	SetCtrlAttribute(panelHandle, PANEL_TBL_DIGNAMES, ATTR_LEFT, left - 165);
	SetCtrlAttribute(panelHandle, PANEL_TBL_DIGNAMES, ATTR_HEIGHT, height);

	for (int j = 1; j <= NUMBERDIGITALCHANNELS; j++)
	{
		SetTableRowAttribute(panelHandle, PANEL_DIGTABLE, j, ATTR_SIZE_MODE, VAL_USE_EXPLICIT_SIZE);
		SetTableRowAttribute(panelHandle, PANEL_DIGTABLE, j, ATTR_ROW_HEIGHT, (height) / (NUMBERDIGITALCHANNELS));
		SetTableRowAttribute(panelHandle, PANEL_TBL_DIGNAMES, j, ATTR_SIZE_MODE, VAL_USE_EXPLICIT_SIZE);

		SetTableRowAttribute(panelHandle, PANEL_TBL_DIGNAMES, j, ATTR_ROW_HEIGHT, (height) / (NUMBERDIGITALCHANNELS));
	}
}

/************************************************************************
Author: David McKay
-------
Date Created: August 23, 2004
-------
Date Modified: August 23, 2004
-------
Description: Sets which channel to display: analog, digital or both
-------
Return Value: void
-------
Parameter1:
int display_setting: channel to display
1: both
2: analog
3: digital
*************************************************************************/
void SetChannelDisplayed(int display_setting)
{
	int toppos1, toppos2, leftpos, heightpos;
	//hide everything
	SetCtrlAttribute(panelHandle, PANEL_DIGTABLE, ATTR_VISIBLE, 0);
	SetCtrlAttribute(panelHandle, PANEL_ANALOGTABLE, ATTR_VISIBLE, 0);
	SetCtrlAttribute(panelHandle, PANEL_TBL_ANAMES, ATTR_VISIBLE, 0);
	SetCtrlAttribute(panelHandle, PANEL_TBL_ANALOGUNITS, ATTR_VISIBLE, 0);
	SetCtrlAttribute(panelHandle, PANEL_TBL_DIGNAMES, ATTR_VISIBLE, 0);
	//ATTR_LABEL_VISIBLE

	switch (display_setting)
	{
	case 1:				 //both
		heightpos = 550; // 380 for 25 rows works... use 410 for 27 rows
		toppos1 = 155;
		leftpos = 170;
		toppos2 = toppos1 + heightpos;
		//toppos2=1280-100-heightpos;
		//toppos2=155+380+50;

		ReshapeAnalogTable(toppos1, 170, heightpos); //passed top, left and height
		ReshapeDigitalTable(toppos2, 170, heightpos - 60);

		SetCtrlAttribute(panelHandle, PANEL_DIGTABLE, ATTR_VISIBLE, 1);
		SetCtrlAttribute(panelHandle, PANEL_ANALOGTABLE, ATTR_VISIBLE, 1);
		SetCtrlAttribute(panelHandle, PANEL_TBL_ANAMES, ATTR_VISIBLE, 1);
		SetCtrlAttribute(panelHandle, PANEL_TBL_ANALOGUNITS, ATTR_VISIBLE, 1);
		SetCtrlAttribute(panelHandle, PANEL_TBL_DIGNAMES, ATTR_VISIBLE, 1);

		break;

	case 2: //analog table

		ReshapeAnalogTable(155, 170, 600); //passed top, left and height

		SetCtrlAttribute(panelHandle, PANEL_ANALOGTABLE, ATTR_VISIBLE, 1);
		SetCtrlAttribute(panelHandle, PANEL_TBL_ANAMES, ATTR_VISIBLE, 1);
		SetCtrlAttribute(panelHandle, PANEL_TBL_ANALOGUNITS, ATTR_VISIBLE, 1);
		break;

	case 3: // digital table

		//passed top, left and height
		ReshapeDigitalTable(155, 170, 500);

		SetCtrlAttribute(panelHandle, PANEL_DIGTABLE, ATTR_VISIBLE, 1);
		SetCtrlAttribute(panelHandle, PANEL_TBL_DIGNAMES, ATTR_VISIBLE, 1);
		break;
	}

	SetCtrlVal(panelHandle, PANEL_DISPLAYDIAL, display_setting);
	DrawNewTable();
	return;
}

/**
Callback triggered by selecting Exit from the menubar. Directly calls PANEL_CALLBACK() with event EVENT_CLOSE.
@author Kerry Wang
*/
void CVICALLBACK EXIT(int menuBar, int menuItem, void *callbackData, int panel)
{
	PANEL_CALLBACK(panelHandle, EVENT_CLOSE, NULL, 0, 0);
}

/**
Callback for the panel. Handles closing the program.
@author Kerry Wang
*/
int CVICALLBACK PANEL_CALLBACK(int panel, int event, void *callbackData, int eventData1, int eventData2)
{
	int status = 0;
	switch(event)
	{
		case EVENT_CLOSE:
			status = ConfirmPopup("Exit", "Are you sure you want to quit?\nUnsaved data will be lost.");
			if (status == 1)
				QuitUserInterface(1); // kills the GUI and ends program
			break;
	}
	return status;
}
