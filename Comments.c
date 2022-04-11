/**
@file Comments.c
Contains code for the comments window.
@author Kerry Wang
*/
#include "Comments.h"

#include "vars.h"

char* string;

int CVICALLBACK COMMENTS_CALLBACK(int panel, int event, void* callbackData,
    int eventData1, int eventData2)
{
    int status = 0;
    switch (event) {
    case EVENT_CLOSE:
        status = HidePanel(commentsHandle);
        break;
    }
    return status;
}
