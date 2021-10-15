#ifndef MAIN_H
#define MAIN_H

// Arrays to hold runtime-generated controls
int LabelArray[NUMBEROFPAGES + 1];
int ButtonArray[NUMBEROFPAGES + 1];
int CheckboxArray[NUMBEROFPAGES + 1];

void Initialization(void);
void setVisibleLabel(int);

#endif
