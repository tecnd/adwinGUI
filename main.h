#ifndef MAIN_H
#define MAIN_H

#include "vars.h"

// Arrays to hold runtime-generated controls
int LabelArray[NUMBEROFPAGES + 1];
int ButtonArray[NUMBEROFPAGES + 1];
int CheckboxArray[NUMBEROFPAGES + 1];

void setVisibleLabel(int);
#endif
