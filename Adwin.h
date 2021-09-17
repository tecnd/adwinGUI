/*************************************************************
* Include File zum Einbinden der Prototypen f�r die Aufrufe an 
* die ADwin-Karten.
* Erstellt am : 30.05.2001 von MS
* ge�ndert am :	12.06.2001 von MS
				15.05.2002 von MS
**************************************************************/

#ifndef __adwin_h__
#define __adwin_h__

#ifdef _WIN32
#define	my_FAR
#else
#define my_FAR __far
#endif


#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

/* The variable "DeviceNo" is declared and inizialized to the default value of 0x150 in file adwin.c /adwin.cpp.
   This variable is responsible for all accesses to the ADwin-system(s). */
extern short DeviceNo;
/* Prototypen von oft ben�tigten Me�- und Steuerungsfunktionen aus der Datei adwin.c */

long  Iserv(char my_FAR *Filename, long Memsize);
long	Boot(char my_FAR *Filename, long Memsize);
long	ADboot(char *Filename, short DeviceNo, long Memsize);
short	ADBPrLoad(char my_FAR *Filename);
short	Load_Process(char my_FAR *Filename); 
short	ADBload(char *Filename, short DeviceNo, short msgbox);
short	ADBStart(short ProcessNo);
short	Start_Process(short ProcessNo);
short	ADBStop(short ProcessNo);
short	Stop_Process(short ProcessNo);
short	SetPar(short Index, long Value);
short	Set_Par(short Index, long Value);
short	SetFPar(short Index, float Value);
short	Set_FPar(short Index, float Value);
long    GetPar(short Index);
long	Get_Par(short Index);
float	GetFPar (short Index);
float	Get_FPar (short Index);
short	GetData (short DataNo, short Data[], long Startindex, long Count);
short	GetlData (short DataNo,  long Data[], long Startindex, long Count);
short	GetData_Long (short DataNo, long Data[],long Startindex, long Count);
short	GetfData (short DataNo, float Data[], long Startindex, long Count);
short	GetData_Float (short DataNo, float Data[], long Startindex, long Count);
short	SetData (short DataNo, short Data[], long Startindex, long Count);
short	SetlData (short DataNo, long Data[], long Startindex, long Count);
short	SetData_Long (short DataNo, long Data[], long Startindex, long Count);
short	SetfData (short DataNo, float Data[], long Startindex, long Count);
short	SetData_Float (short DataNo, float Data[], long Startindex, long Count);
short	Data2File(char my_FAR *Filename, short DataNo, long Startindex, long Count, short Mode);
short	GetFifo (short FifoNo, short Data[], long Count);
short	GetlFifo (short FifoNo, long Data[], long Count);
short	GetFifo_Long(short FifoNo, long Data[], long Count);
short	GetfFifo (short FifoNo, float Data[], long Count);
short	GetFifo_Float(short FifoNo, float Data[], long Count);
short	SetFifo (short FifoNo, short Data[], long Count);
short	SetlFifo (short FifoNo, long Data[], long Count);
short	SetFifo_Long (short FifoNo, long Data[], long Count);
short	SetfFifo (short FifoNo, float Data[], long Count);
short	SetFifo_Float (short FifoNo, float Data[], long Count);
long	Fifo_Full (short FifoNo);
long	GetFifoEmpty (short FifoNo);
long	Fifo_Empty (short FifoNo);
short	ClearFifo (short FifoNo);
short	Fifo_Clear (short FifoNo);
long	GetFifoCount (short FifoNo);
unsigned short ADC(short Channel);
short	SetDAC(short Channel, unsigned short Value);
short	DAC(short Channel, unsigned short Value);
long	DigIn(void); 
long	Get_Digin(void);
short	SetDigOut(short Value);
short	Set_Digout(short Value);
long	DigOut(void);
long	Get_Digout(void);
short	Auslast(void);
short	Workload(long Priority);
short	Test(void);
short	Test_Version(void);
long	Freemem(void);
long	Free_Mem(short Mem_Spec);
long	Freemem_T9(short typ);
void	ErrMessage(short OnOff);
void	Show_Errors(short OnOff);
short	Net_Connect(char my_FAR *Protocol, char my_FAR *Address, char my_FAR *Endpoint, char my_FAR *Password);
short	Net_Disconnect(void);
long	Get_Last_Error(void);
char*	Get_Last_Error_Text(long Last_Error); 
short	Processor_Type(void);
long	Clear_Process(short ProcessNo);
long	Process_Status(short ProcessNo);
short	Get_Par_Block(long Array[], short Startindex, short Count);
short	Get_FPar_Block(float Array[], short Startindex, short Count);
short	Get_Par_Block(long Array[], short Startindex, short Count);
short	Get_Par_All(long Array[]);
short	Get_FPar_All(float Array[]);
long	Data_Length(long DataNo);
short	Get_FPar_All_Double(double Array[]);
short	Get_FPar_Block_Double(double Array[], short Startindex, short Count);
short	GetData_Double(short DataNo, double Data[],long Startindex, long Count);
short	SetData_Double(short DataNo, double Data[], long Startindex, long Count);
short	GetFifo_Double(short FifoNo, double Data[], long Count);
short	SetFifo_Double(short FifoNo, double Data[], long Count);
long	Set_Globaldelay(long ProcessNo, long Globaldelay);
long	Get_Globaldelay(long ProcessNo);
long	Set_Language(long language);
long	GetData_String(long DataNo, char *Data, long MaxCount);
long	String_Length(long DataNo);
long	SetData_String(long DataNo, char *Data);
short	Get_Known_DeviceNo(short Devices[], long *Count_Devices);
long	Get_Known_USB_SerialNo(long SerialNo[]);
int		ADwin_Debug_Mode_On(char my_FAR *Filename, long Size);
int		ADwin_Debug_Mode_Off(void);


// undocumented
long	GetActivate(short ProcessNo);
long	Activate_PC(short ProcessNo);
short	GetData_Packed_Short(long DataNo, short *Data, long Startindex, long Count);
short	GetData_Packed_Long(long DataNo, long *Data, long Startindex, long Count);
short	GetData_Packed_Float(long DataNo, float *Data, long Startindex, long Count);
short	GetData_Packed_Double(long DataNo, double *Data, long Startindex, long Count);
short	GetFifo_Packed_Short(short FifoNo, short Data[], long Count);
short   GetFifo_Packed_Long(short FifoNo, long Data[], long Count);

short	Get_Data_Packed(void *Data, short Typ, short DataNo, long Startindex, long Count, short DeviceNo);


short	SetOpt(short Index, long Value);
long	GetOpt(short Index);
short	SetDACListe(short Channel, long Count, short Value[]);
short	ReadADListe(short Channel, long Count, short Value[]);
short	ZykStart(short ProzessNoZyk);
short	ZykStop(short ProzessNoZyk);


#ifdef __cplusplus
}
#endif 	

#endif //__adwin_h__
