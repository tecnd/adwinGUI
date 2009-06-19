/* AutoConfigGen2.h */

#include "vars.h"

void LoadLaserData(char savedname[500],int nameSize);
void SaveScanData(void);
void LoadScanData(void);
void updateDisplay(void);
void sendEventList(void);
int checkDividerSetting(double newLaserFreq);
double calcFreq(double LaserFreq);
int tcpSendCmdList(unsigned int tcp_handle,unsigned char* cmdList[MAXCMDNUM],int* cmdLengthList,int cmdCount);
unsigned int tcpConnect(int laserNum);
int tcpDisconnect(unsigned int tcp_handle);
char* tcp_errorlookup(int tcp_error_code);
int TCP_Comm_Callback(unsigned handle, int xType, int errCode, void *callbackData);
