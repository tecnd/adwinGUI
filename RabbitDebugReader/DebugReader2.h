/*  DebugReader2.h 
	Author: David Burns: August 14, 2006 */
	
int TCP_Comm_Callback(unsigned handle, int xType, int errCode, void *callbackData);
void setLed(int rabbitNum,int status);
char* tcp_errorlookup(int tcp_error_code);
void writeText(int rabbitNum,char *txt,int logtext);
int ForceComm_Callback(unsigned handle, int xType, int errCode, void *callbackData);
void LoadLaserData(char savedname[500],int nameSize);
void saveText(int rabbitNum,char *txt);
