#ifndef GUIDESIGN2_H
#define GUIDESIGN2_H

void LoadSettings(void);
void SaveSettings(void);
int  GetPage(void);
void DrawNewTable(int dimmed);
void CheckActivePages(void);
void SaveArrays(char*,int);
void LoadArrays(char*,int);

void ReshapeAnalogTable(int,int,int);
void ReshapeDigitalTable(int,int,int);
void SetChannelDisplayed(int display_setting); //analog, digital of both

#endif
