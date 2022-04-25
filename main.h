/**
@file main.h
@brief Defines arrays for dynamically-generated labels, buttons, and checkboxes.
*/
#ifndef MAIN_H
#define MAIN_H

#include "vars.h"

// Arrays to hold runtime-generated controls
/**
@brief Array of label references.
*/
int LabelArray[NUMBEROFPAGES + 1];
/**
@brief Array of page button references.
*/
int ButtonArray[NUMBEROFPAGES + 1];
/**
@brief Array of checkbox references.
*/
int CheckboxArray[NUMBEROFPAGES + 1];

void setVisibleLabel(int);
#endif
