/*  LaserTuning.h 
	Header File for LaserTuning.c
	Author: David Burns, July 2006 */


#ifndef LZTUNEDEF
#define LZTUNEDEF

#include <tcpsupp.h>
#include <ansi_c.h>
#include <stdio.h>
#include <cvirte.h>		
#include "vars.h"

#define MAXCMDNUM 2000		   //Max number of commands the rabbit can store in a single event list
#define HEARTCMD 0x7F          //Stay Alive Data Byte sent by rabbit


/*  RabbitTCPcom.h  */


void BuildLaserUpdates(struct LaserTableValues MetaLaserArray[NUMBERLASERS][500],double MetaTimeArray[500],unsigned int MetaTriggerArray[NUMBERLASERS][500],int numtimes);
unsigned int tcpConnect(int laserNum);
int tcpDisconnect(unsigned int tcp_handle);
int TCP_Comm_Callback(unsigned handle, int xType, int errCode, void *callbackData);
char* tcp_errorlookup(int tcp_error_code);
int tcpSendCmdList(unsigned int tcp_handle,unsigned char* cmdList[MAXCMDNUM],int* cmdLengthList,int cmdCount);
int checkDividerSetting(double newLaserFreq);
double calcRamp(double laserStartFreq,double laserEndFreq,int *dividerUpdate,double *newDDSFreq);
double calcFreq(double LaserFreq);
void tcpTriggering(unsigned int tcp_handle,unsigned trigCount);
int newProfile(int *profile);

#endif


