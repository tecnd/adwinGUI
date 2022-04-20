/**
@file Comments.c
@brief Contains code for the comments window.
@author Kerry Wang
*/
#include "Comments.h"

#include "vars.h"

/**
@brief Panel event handler.
*/
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
