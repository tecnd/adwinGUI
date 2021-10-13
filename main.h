#ifndef MAIN_H
#define MAIN_H

int LabelArray[NUMBEROFPAGES + 1];
int ButtonArray[NUMBEROFPAGES + 1];

void Initialization(void);
void ConvertIntToStr(int int_val, char *int_str);
void DrawCanvasArrows(void);
void setVisibleLabel(int);

#endif
