/******************************************************************************************************
* Collection of subroutines for the control of the ADwin measurement data acquisition
* 
* Created : 22.08.1996 from Martin Hotze
*  
* Modified:	01.04.1998 from HPB 
*******************************************************************
* Version 1.00  M.S. 13.06.2001	Same function names as other adwin-developer driver 
* Version 1.01  M.S. 07.08.2001	DeviceNo is now a variable	(earlier version it was a define)
* Version 1.02	M.S. 17.05.2002 Five new functions (Get_Known_DeviceNo, Get_Known_USB_SerialNo, ADwin_Debug_Mode_Off, 
*													ADwin_Debug_Mode_On, GetFifo_Packed_Short, GetFifo_Packed_Long)
* Version 1.03  M.S. 18.07.2003 The function "GetFifo_Packed_Long " is deleted, because it was not working in the requested way.
* Version 1.04  M.S. 24.11.2003 In function "Set_Globaldelay" and "Get_Globaldelay" changed long->short from ProcessNo
*								In function "GetData_Packed_Short", "GetData_Packed_Long", "GetData_Packed_Float" and "GetData_Packed_Double"
*								changed the return value from (long)->(short), now "(short)LoadDLLIfNeeded()"
* Version 1.05  M.S. 05.05.2004 Two undocumented functions added: 'Get_Dev_ID' and 'GetConnectionTyp'
* Version 1.06  M.S. 21.02.2005 'Free_Mem' updated for processor T11
* Version 1.07  M.S. 31.01.2007 'Get_Last_Error_Text' useless Variable 'dllLoadError' deleted and IF-condition angepasst
* Version 1.08  M.S. 16.02.2009 New function 'File2Data'
* Version 1.09  M.S. 18.12.2009 Additional comment for function Get_Dev_ID
* Version 1.10  M.S. 01.12.2010 Support now 64Bit code generation. Testet with Adwin.cpp under VS2008.Net
*                               Depending on the compiler setting, UNICODE or ANSI is used in function LoadLibrary and MessageBox.
*                               Error constanst changed:
*									kFailedToLoadDLLError     1->4000
*									kCouldNotFindFunction     1->4001
* Version 1.11  M.S. 08.12.2010 Default DeviceNo changed from 0x150 (336) to 1.
* Version 1.12  M.S. 12.06.2015 New functions Free_Mem also for T12
*                   This Version supports new T12 functions, also working on older processors
*                   - Get_FPar_All_Double
*                   - Get_FPar_Block_Double
*                   - Get_FPar_Double
*                   - Set_FPar_Double
*
* Function of the DLL ADwin32(32-bit, respectively)
* =====================================================================
* Attention! the file "ADWIN32.DLL" respectively have to be included in a
* subdirectory that can be accessed by Windows ( at best in " \windows" or "\winnt")
* in order to use these functions the program "ADwin9.btl" (or ADwin4.btl or ADwin5.btl or ADwin8.btl
* respectively) has to be loaded to
* the ADwin system!
* The DLL ADwin32.dll respectively include all functions necessary for
* the data transfer between PC and the ADwin system.
* 	
* To Change from ADwin.c ->ADwin.cpp use #include "stdafx.h"
*******************************************************************************************************/

//#include "stdafx.h"	  // necessary for MFC projects
#include "windows.h"
#include "Adwin.h"


//internal error codes only for this c driver
#define kFailedToLoadDLLError     4000    //It was not possible to load the adwin32.dll or adwin64.dll
#define kCouldNotFindFunction     4001    //The called function is not available in the adwin32.dll or adwin64.dll which is used.

/* The variable "DeviceNo" is declared and inizialized to the default value of "1".
   This variable is responsible for all accesses to the ADwin-system(s). */
short DeviceNo = 1;		// Device number 
int	FirstTime  = 1;

static HINSTANCE DLLHandle;      
long  (FAR PASCAL *Clear_Process_Ptr)(short ProcessNo, short Device_No);
long  (FAR PASCAL *Boot_Ptr)(char my_FAR *Filename, short Device_No, long Memsize, short msgbox);
short (FAR PASCAL *Load_Process_Ptr)(char my_FAR *Filename, short Device_No, short msgbox);
short (FAR PASCAL *Start_Process_Ptr)(short Index, short Device_No);
short (FAR PASCAL *Stop_Process_Ptr)(short Index, short Device_No);
short (FAR PASCAL *Set_Par_Ptr)(short Index, long Value, short Device_No);
short (FAR PASCAL *Set_FPar_Ptr)(short Index, float Value, short Device_No);

long  (FAR PASCAL *Get_Par_Ptr)(short Index, short Device_No);
float (FAR PASCAL *Get_FPar_Ptr)(short Index, short Device_No);

short (FAR PASCAL *Set_FPar_Double_Ptr)(short Index, double Value, short Device_No);
double (FAR PASCAL *Get_FPar_Double_Ptr)(short Index, short Device_No);

short (FAR PASCAL *Get_Data_Ptr)(void *Data, short typ, short DataNo, long Startindex, long Count ,short Device_No);
short (FAR PASCAL *Set_Data_Ptr)(void *Data, short typ, short DataNo, long Startindex, long Count ,short Device_No);
short (FAR PASCAL *Save_Fast_Ptr)(char my_FAR *Filename, short DataNo, long Startindex, long Count, short Mode, short Device_No);
short (FAR PASCAL *File2Data_Ptr)(char *Filename, short typ, short DataNo, long Startindex, short Device_No);
short (FAR PASCAL *Get_Fifo_Ptr)(void *Data, short typ, short FifoNo, long Count ,short Device_No);
short (FAR PASCAL *Set_Fifo_Ptr)(void *Data, short typ, short FifoNo, long Count ,short Device_No);
long  (FAR PASCAL *Get_Fifo_Count_Ptr)(short FifoNo, short Device_No);
long  (FAR PASCAL *Get_Fifo_Empty_Ptr)(short FifoNo, short Device_No);
short (FAR PASCAL *Fifo_Clear_Ptr)(short FifoNo, short Device_No);
short (FAR PASCAL *Get_ADC_Ptr)(short Channel, short Device_No);
short (FAR PASCAL *Set_DAC_Ptr)(short Channel, unsigned short Value, short Device_No);
long  (FAR PASCAL *Get_Digin_Ptr)(short Device_No);
short (FAR PASCAL *Set_Digout_Ptr)(short Value, short Device_No);
long  (FAR PASCAL *Get_Digout_Ptr)(short Device_No);
short (FAR PASCAL *Auslastung_Ptr)(short Device_No);
short (FAR PASCAL *Workload_Ptr)(long Priority, short Device_No);
short (FAR PASCAL *ADTest_Version_Ptr)(short Device_No, short msgbox);
long  (FAR PASCAL *AD_Memory_Ptr)(short Device_No);
long  (FAR PASCAL *AD_Memory_all_Ptr)(short Mem_Spec,short Device_No);
long  (FAR PASCAL *AD_Memory_all_byte_Ptr)(short Mem_Spec,short Device_No);
short (FAR PASCAL *AD_Net_Connect_Ptr)(char my_FAR *Protocol, char my_FAR *Address, char my_FAR *Endpoint, char my_FAR *Password, long Message);
short (FAR PASCAL *AD_Net_Disconnect_Ptr)(void);
short (FAR PASCAL *ADProzessorTyp_Ptr)(short Device_No);
long  (FAR PASCAL *AD_GetErrorCode_Ptr)(void);
char  (FAR PASCAL *AD_GetErrorText_Ptr)(long ErrorCode, char *text, long lenght);
short (FAR PASCAL *Get_ADBPar_All_Ptr)(short Startindex, short Count, long *Array, short Device_No);
short (FAR PASCAL *Get_ADBFPar_All_Ptr)(short Startindex, short Count, float *Array, short Device_No);
short (FAR PASCAL *Get_ADBFPar_All_Double_Ptr)(short Startindex, short Count, double *Array, short Device_No);
long  (FAR PASCAL *Get_Data_Length_Ptr)(short DataNo, short Device_No);
long  (FAR PASCAL *ADSetLanguage_Ptr)(long language);
long  (FAR PASCAL *GetData_String_Ptr)(char *Data, long MaxCount,short DataNo, short Device_No);
long  (FAR PASCAL *String_Length_Ptr)(short DataNo, short Device_No);
long  (FAR PASCAL *SetData_String_Ptr)(char *Data,short DataNo, short Device_No);
short (FAR PASCAL *GetData_Packed_Ptr)(void *Data, short Typ, short DataNo, long Startindex, long Count, short Device_No);
short (FAR PASCAL *Get_List_Ptr)(void *Destination,short typ, short Startindex, short proc,long Count, short Device_No);
short (FAR PASCAL *Set_List_Ptr)(void *Source,short typ,short Startindex, long Count, short Device_No);
short (FAR PASCAL *Start_Ptr)(short Index, short Device_No);
short (FAR PASCAL *Stop_Ptr)(short Index, short Device_No);
short (FAR PASCAL *Get_Known_Deviceno_Ptr)(short *Devices, long *Count_Devices);
long  (FAR PASCAL *Get_Known_USB_SerialNo_Ptr)(long *SerialNo);
int   (FAR PASCAL *ADwin_Debug_Mode_On_Ptr)(char my_FAR *Filename, long Size);
int   (FAR PASCAL *ADwin_Debug_Mode_Off_Ptr)(void);
//05.05.04 MS
long  (FAR PASCAL *Get_Dev_ID_Ptr)(short Device_No, long *value);
long  (FAR PASCAL *Get_Connection_Type_Ptr)(short Device_No);

							/* Error message from the DLL On(1)/Off(0) */
short message=1;            /* Error message if funktion fails */
							/* Iserv, ADBPrLoad and Test.	0 = don't show error message */
							/*								1 = show error message */

/*************************************************/
/* Load DLL and get the address of the function  */
/*************************************************/
int LoadDLLIfNeeded(void)
{
	//If adwin32.dll handle exist, don't load pointers again
    if (DLLHandle)
        return 0;
	//Depending on the compiler setting x86/x64 the correct adwin32.dll or adwin64.dll will be loaded.
	#ifdef _WIN64
		#ifdef UNICODE
			DLLHandle =  LoadLibrary(L"adwin64.dll");
		#else
			DLLHandle =  LoadLibrary("adwin64.dll");
		#endif // !UNICODE
    #else
		#ifdef UNICODE
			DLLHandle =  LoadLibrary(L"adwin32.dll");
		#else
			DLLHandle =  LoadLibrary("adwin32.dll");
		#endif // !UNICODE
	#endif

    if (DLLHandle == NULL) {
        return kFailedToLoadDLLError;
        }
    if ((Boot_Ptr = (long (FAR PASCAL *)(char my_FAR *, short, long, short))GetProcAddress(DLLHandle,"ADboot"))==0)
        goto FunctionNotFoundError;
    if ((Load_Process_Ptr = (short (FAR PASCAL *)(char my_FAR *, short, short))GetProcAddress(DLLHandle,"ADBload"))==0)
        goto FunctionNotFoundError;
    if ((Start_Process_Ptr = (short (FAR PASCAL *)(short, short))GetProcAddress(DLLHandle,"ADB_Start"))==0)
        goto FunctionNotFoundError;
    if ((Stop_Process_Ptr = (short (FAR PASCAL *)(short, short))GetProcAddress(DLLHandle,"ADB_Stop"))==0)
        goto FunctionNotFoundError;
    if ((Set_Par_Ptr = (short (FAR PASCAL *)(short, long, short))GetProcAddress(DLLHandle,"Set_ADBPar"))==0)
        goto FunctionNotFoundError;
    if ((Set_FPar_Ptr = (short (FAR PASCAL *)(short, float, short))GetProcAddress(DLLHandle,"Set_ADBFPar"))==0)
        goto FunctionNotFoundError;
	if ((Set_FPar_Double_Ptr = (short (FAR PASCAL *)(short, double, short))GetProcAddress(DLLHandle,"Set_ADBFPar_Double"))==0)
		goto FunctionNotFoundError;
    if ((Get_Par_Ptr = (long (FAR PASCAL *)(short, short))GetProcAddress(DLLHandle,"Get_ADBPar"))==0)
        goto FunctionNotFoundError;
    if ((Get_FPar_Ptr = (float (FAR PASCAL *)(short, short))GetProcAddress(DLLHandle,"Get_ADBFPar"))==0)
        goto FunctionNotFoundError;
	if ((Get_FPar_Double_Ptr = (double (FAR PASCAL *)(short, short))GetProcAddress(DLLHandle,"Get_ADBFPar_Double"))==0)
		goto FunctionNotFoundError;
    if ((Get_Data_Ptr = (short (FAR PASCAL *)(void*, short, short, long, long, short))GetProcAddress(DLLHandle,"Get_Data"))==0)
        goto FunctionNotFoundError;
    if ((Set_Data_Ptr = (short (FAR PASCAL *)(void*, short, short, long, long, short))GetProcAddress(DLLHandle,"Set_Data"))==0)
        goto FunctionNotFoundError;
    if ((Save_Fast_Ptr = (short (FAR PASCAL *)(char my_FAR *, short, long, long, short, short))GetProcAddress(DLLHandle,"SaveFast"))==0)
        goto FunctionNotFoundError;
    if ((File2Data_Ptr = (short (FAR PASCAL *)(char my_FAR *, short, short, long, short ))GetProcAddress(DLLHandle,"File2Data"))==0)
        goto FunctionNotFoundError;
		if ((Get_Fifo_Ptr = (short (FAR PASCAL *)(void*, short, short, long, short))GetProcAddress(DLLHandle,"Get_Fifo"))==0)
        goto FunctionNotFoundError;
    if ((Set_Fifo_Ptr = (short (FAR PASCAL *)(void*, short, short, long, short))GetProcAddress(DLLHandle,"Set_Fifo"))==0)
        goto FunctionNotFoundError;
    if ((Get_Fifo_Count_Ptr = (long (FAR PASCAL *)(short, short))GetProcAddress(DLLHandle,"Get_Fifo_Count"))==0)
        goto FunctionNotFoundError;
    if ((Get_Fifo_Empty_Ptr = (long (FAR PASCAL *)(short, short))GetProcAddress(DLLHandle,"Get_Fifo_Empty"))==0)
        goto FunctionNotFoundError;
    if ((Fifo_Clear_Ptr = (short (FAR PASCAL *)(short, short))GetProcAddress(DLLHandle,"Clear_Fifo"))==0)
        goto FunctionNotFoundError;
    if ((Get_ADC_Ptr = (short (FAR PASCAL *)(short, short))GetProcAddress(DLLHandle,"Get_ADC"))==0)
        goto FunctionNotFoundError;
    if ((Set_DAC_Ptr = (short (FAR PASCAL *)(short, unsigned short, short))GetProcAddress(DLLHandle,"Set_DAC"))==0)
        goto FunctionNotFoundError;
    if ((Get_Digin_Ptr = (long (FAR PASCAL *)(short))GetProcAddress(DLLHandle,"Get_Digin"))==0)
        goto FunctionNotFoundError;
    if ((Set_Digout_Ptr = (short (FAR PASCAL *)(short, short))GetProcAddress(DLLHandle,"Set_Digout"))==0)
        goto FunctionNotFoundError;
    if ((Get_Digout_Ptr = (long (FAR PASCAL *)(short))GetProcAddress(DLLHandle,"Get_Digout"))==0)
        goto FunctionNotFoundError;
    if ((Auslastung_Ptr = (short (FAR PASCAL *)(short))GetProcAddress(DLLHandle,"AD_Auslastung"))==0)
        goto FunctionNotFoundError;
    if ((Workload_Ptr = (short (FAR PASCAL *)(long, short))GetProcAddress(DLLHandle,"AD_Workload"))==0)
        goto FunctionNotFoundError;
    if ((ADTest_Version_Ptr = (short (FAR PASCAL *)(short, short))GetProcAddress(DLLHandle,"ADTest_Version"))==0)
        goto FunctionNotFoundError;
    if ((AD_Memory_Ptr = (long (FAR PASCAL *)(short))GetProcAddress(DLLHandle,"AD_Memory"))==0)
        goto FunctionNotFoundError;
    if ((AD_Memory_all_Ptr = (long (FAR PASCAL *)(short, short))GetProcAddress(DLLHandle,"AD_Memory_all"))==0)
        goto FunctionNotFoundError;
    if ((AD_Net_Connect_Ptr = (short (FAR PASCAL *)(char my_FAR *, char my_FAR *, char my_FAR *, char my_FAR *, long))GetProcAddress(DLLHandle,"AD_Net_Connect"))==0)
        goto FunctionNotFoundError;
    if ((AD_Net_Disconnect_Ptr = (short (FAR PASCAL *)(void))GetProcAddress(DLLHandle,"AD_Net_Disconnect"))==0)
        goto FunctionNotFoundError;
    if ((AD_GetErrorCode_Ptr = (long (FAR PASCAL *)(void))GetProcAddress(DLLHandle,"ADGetErrorCode"))==0)
        goto FunctionNotFoundError;
    if ((AD_GetErrorText_Ptr = (char (FAR PASCAL *)(long, char *, long))GetProcAddress(DLLHandle,"ADGetErrorText"))==0)
        goto FunctionNotFoundError;
    if ((Get_ADBPar_All_Ptr = (short (FAR PASCAL *)(short, short, long *, short))GetProcAddress(DLLHandle,"Get_ADBPar_All"))==0)
        goto FunctionNotFoundError;	
	if ((Get_ADBFPar_All_Double_Ptr = (short (FAR PASCAL *)(short, short, double *, short))GetProcAddress(DLLHandle,"Get_ADBFPar_All_Double"))==0)
		goto FunctionNotFoundError;
	if ((Get_ADBFPar_All_Ptr = (short (FAR PASCAL *)(short, short, float *, short))GetProcAddress(DLLHandle,"Get_ADBFPar_All"))==0)
        goto FunctionNotFoundError;	
    if ((Get_Data_Length_Ptr = (long (FAR PASCAL *)(short, short))GetProcAddress(DLLHandle,"Get_Data_Length"))==0)
        goto FunctionNotFoundError;	
    if ((ADSetLanguage_Ptr = (long (FAR PASCAL *)(long))GetProcAddress(DLLHandle,"ADSetLanguage"))==0)
        goto FunctionNotFoundError;	
    if ((GetData_String_Ptr = (long (FAR PASCAL *)(char *, long, short, short))GetProcAddress(DLLHandle,"Get_Data_String"))==0)
        goto FunctionNotFoundError;	
    if ((String_Length_Ptr = (long (FAR PASCAL *)(short, short))GetProcAddress(DLLHandle,"Get_Data_String_Length"))==0)
        goto FunctionNotFoundError;	
    if ((SetData_String_Ptr = (long (FAR PASCAL *)(char *, short, short))GetProcAddress(DLLHandle,"Set_Data_String"))==0)
        goto FunctionNotFoundError;	
    if ((GetData_Packed_Ptr = (short (FAR PASCAL *)(void *, short, short, long, long, short))GetProcAddress(DLLHandle,"Get_Data_packed"))==0)
        goto FunctionNotFoundError;	
    if ((ADProzessorTyp_Ptr = (short (FAR PASCAL *)(short Device_No))GetProcAddress(DLLHandle,"ProzessorTyp"))==0)
        goto FunctionNotFoundError;
    if ((Clear_Process_Ptr = (long (FAR PASCAL *)(short, short))GetProcAddress(DLLHandle,"Clear_Process"))==0)
        goto FunctionNotFoundError;
    if ((Get_List_Ptr = (short (FAR PASCAL *)(void *, short, short, short, long, short))GetProcAddress(DLLHandle,"Get_List"))==0)
        goto FunctionNotFoundError;
    if ((Set_List_Ptr = (short (FAR PASCAL *)(void *, short, short, long, short))GetProcAddress(DLLHandle,"Set_List"))==0)
        goto FunctionNotFoundError;
    if ((Start_Ptr = (short (FAR PASCAL *)(short, short))GetProcAddress(DLLHandle,"Start"))==0)
        goto FunctionNotFoundError;
    if ((Stop_Ptr = (short (FAR PASCAL *)(short, short))GetProcAddress(DLLHandle,"Stop"))==0)
        goto FunctionNotFoundError;
    if ((Get_Known_Deviceno_Ptr = (short (FAR PASCAL *)(short *, long *))GetProcAddress(DLLHandle,"Get_Known_Deviceno"))==0)
        goto FunctionNotFoundError;
    if ((Get_Known_USB_SerialNo_Ptr = (long (FAR PASCAL *)(long *))GetProcAddress(DLLHandle,"Get_Known_USB_SerialNo"))==0)
        goto FunctionNotFoundError;
    if ((ADwin_Debug_Mode_On_Ptr = (int (FAR PASCAL *)(char my_FAR *, long))GetProcAddress(DLLHandle,"adwin_debug_mode_on"))==0)
        goto FunctionNotFoundError;
    if ((ADwin_Debug_Mode_Off_Ptr = (int (FAR PASCAL *)(void))GetProcAddress(DLLHandle,"adwin_debug_mode_off"))==0)
        goto FunctionNotFoundError;
    if ((Get_Dev_ID_Ptr = (long (FAR PASCAL *)(short, long *))GetProcAddress(DLLHandle,"get_dev_id"))==0)
        goto FunctionNotFoundError;
    if ((Get_Connection_Type_Ptr = (long (FAR PASCAL *)(short))GetProcAddress(DLLHandle,"ADGetConnectionTyp"))==0)
        goto FunctionNotFoundError;
    if ((AD_Memory_all_byte_Ptr = (long (FAR PASCAL *)(short, short))GetProcAddress(DLLHandle,"AD_Memory_all_byte"))==0)
        goto FunctionNotFoundError;
    return 0;
	
/* Error handling */
FunctionNotFoundError:
	if (FirstTime==1)
	{
		#ifdef UNICODE
			MessageBox(NULL, L"You need a new Version of your ADwin32.dll!\nAt least ADwin32.dll from 14.5.2001!\nYour ADwin32.dll will removed!", L"ADwin32.dll to old!", MB_ICONWARNING);
		#else
			MessageBox(NULL, "You need a new Version of your ADwin32.dll!\nAt least ADwin32.dll from 14.5.2001!\nYour ADwin32.dll will removed!", "ADwin32.dll to old!", MB_ICONWARNING);
		#endif // !UNICODE


		FirstTime = 0;		/* No more messages */
	}
	FreeLibrary(DLLHandle);     /* Remove the DLL */
	DLLHandle = 0;

	return kCouldNotFindFunction;	
}

/*****************************************/
/* Connection code to the DLL functions  */
/*****************************************/


/* Down-loads the operating system (BTL-file) to the ADwin-System */
/* ============================================================================== */
long Iserv(char my_FAR *Filename, long Memsize)
{
    long dllLoadError;
    if ((dllLoadError = (long)LoadDLLIfNeeded())!=0)
        return dllLoadError;

    return (*Boot_Ptr)(Filename, DeviceNo, Memsize, message);	/* Boot the ADwin card */
}

/* Down-loads the operating system (BTL-file) to the ADwin-System */
/* ============================================================================== */
long Boot(char my_FAR *Filename, long Memsize)
{
    long dllLoadError;
    if ((dllLoadError = (long)LoadDLLIfNeeded())!=0)
        return dllLoadError;
	
	return (Iserv(Filename, Memsize));							/* Boot the ADwin card */
}

long ADboot(char my_FAR *Filename, short DeviceNo1, long Memsize)
{
    long dllLoadError;
    if ((dllLoadError = (long)LoadDLLIfNeeded())!=0)
        return dllLoadError;
    
	return (*Boot_Ptr)(Filename, DeviceNo1, Memsize, message);  /* Boot the ADwin card */
}

/* Loads an ADbasic-process (a bin file generated by ADbasic) to the ADwin board  */
/* ============================================================================== */
short ADBPrLoad(char my_FAR *Filename)
{
    short dllLoadError;
    if ((dllLoadError = (short)LoadDLLIfNeeded())!=0)
        return dllLoadError;
	
	return (*Load_Process_Ptr)(Filename, DeviceNo, message);	/* Load bin file */
}

/* Loads an ADbasic-process (a bin file generated by ADbasic) to the ADwin board  */
/* ============================================================================== */
short ADBload(char my_FAR *Filename,short DeviceNo1,short msgbox)
{
    short dllLoadError;
    if ((dllLoadError = (short)LoadDLLIfNeeded())!=0)
        return dllLoadError;
	
	return (*Load_Process_Ptr)(Filename, DeviceNo1, msgbox);	/* Load bin file */
}

/* Loads an ADbasic-process (a bin file generated by ADbasic) to the ADwin board  */
/* ============================================================================== */
short Load_Process(char my_FAR *Filename)
{
    short dllLoadError;
    if ((dllLoadError = (short)LoadDLLIfNeeded())!=0)
        return dllLoadError;
	
	return (*Load_Process_Ptr)(Filename, DeviceNo, message);	/* Load bin file */
}

/* Starts process number "ProcessNo" on the ADwin-System						  */
/* ============================================================================== */
short ADBStart (short ProcessNo)
{
    short dllLoadError;
    if ((dllLoadError = (short)LoadDLLIfNeeded())!=0)
        return dllLoadError;

	return (*Start_Process_Ptr)(ProcessNo, DeviceNo);	/* Start process */
}

/* Starts process number "ProcessNo" on the ADwin-System						  */
/* ============================================================================== */
short Start_Process(short ProcessNo)
{
    short dllLoadError;
    if ((dllLoadError = (short)LoadDLLIfNeeded())!=0)
        return dllLoadError;
	
	return (*Start_Process_Ptr)(ProcessNo, DeviceNo);	/* Start process */
}

/* Stops process number "ProcessNo" on the ADwin-System							  */
/* ============================================================================== */
short ADBStop (short ProcessNo)
{
    short dllLoadError;
    if ((dllLoadError = (short)LoadDLLIfNeeded())!=0)
        return dllLoadError;

	return (*Stop_Process_Ptr)(ProcessNo, DeviceNo);	/* Stop process */
}

/* Stops process number "ProcessNo" on the ADwin-System							  */
/* ============================================================================== */
short Stop_Process(short ProcessNo)
{
    short dllLoadError;
    if ((dllLoadError = (short)LoadDLLIfNeeded())!=0)
        return dllLoadError;
	
	return (*Stop_Process_Ptr)(ProcessNo, DeviceNo);	/* Stop process */
}

/* Sets a parameter (PAR_"Index") on the ADwin-System to "Value"				  */
/* ============================================================================== */
short SetPar (short Index, long Value)
{
    short dllLoadError;
    if ((dllLoadError = (short)LoadDLLIfNeeded())!=0)
        return dllLoadError;
	
	return (*Set_Par_Ptr)(Index, Value, DeviceNo);		/* Set parameter */
}

/* Sets a parameter (PAR_"Index") on the ADwin-System to "Value"				  */
/* ============================================================================== */
short Set_Par (short Index, long Value)
{
    short dllLoadError;
    if ((dllLoadError = (short)LoadDLLIfNeeded())!=0)
        return dllLoadError;
	
	return (*Set_Par_Ptr)(Index, Value, DeviceNo);		/* Set parameter */
}

/* Sets a float parameter (FPAR_"Index") on the ADwin-System to "Value"			  */
/* ============================================================================== */
short SetFPar (short Index, float Value)
{
    short dllLoadError;
    if ((dllLoadError = (short)LoadDLLIfNeeded())!=0)
        return dllLoadError;

	return (*Set_FPar_Ptr)(Index, Value, DeviceNo);		/* Set float parameter */
}

/* Sets a parameter (FPAR_"Index") on the ADwin-System to "Value"				  */
/* ============================================================================== */
short Set_FPar (short Index, float Value)
{
    short dllLoadError;
    if ((dllLoadError = (short)LoadDLLIfNeeded())!=0)
        return dllLoadError;
	
	return (*Set_FPar_Ptr)(Index, Value, DeviceNo);		/* Set float parameter */
}

/* Returns double value of parameter (FPAR_"Index") from the ADwin-System          */
/* ============================================================================== */
short Set_FPar_Double(short Index, double Value)
{
    short dllLoadError;
    if ((dllLoadError = (short)LoadDLLIfNeeded())!=0)
        return dllLoadError;
	
	return (*Set_FPar_Double_Ptr)(Index, Value, DeviceNo);            /* Set Double parameter */
}

/* Returns the value of parameter (PAR_"Index") from the ADwin-System			  */
/* ============================================================================== */
long GetPar(short Index)
{
    long dllLoadError;
    if ((dllLoadError = (long)LoadDLLIfNeeded())!=0)
        return dllLoadError;

    return (*Get_Par_Ptr)(Index,DeviceNo);				/* Get parameter */
}

/* Returns the value of parameter (PAR_"Index") from the ADwin-System			  */
/* ============================================================================== */
long Get_Par(short Index)
{
    long dllLoadError;
    if ((dllLoadError = (long)LoadDLLIfNeeded())!=0)
        return dllLoadError;
    
	return (*Get_Par_Ptr)(Index,DeviceNo);				/* Get parameter */
}

/* Returns float value of parameter (FPAR_"Index") from the ADwin-System          */
/* ============================================================================== */
float GetFPar(short Index)
{
    int dllLoadError;
    if ((dllLoadError = LoadDLLIfNeeded())!=0)
        return (float)dllLoadError;
	
	return (*Get_FPar_Ptr)(Index, DeviceNo);            /* Get float parameter */
}

/* Returns float value of parameter (FPAR_"Index") from the ADwin-System          */
/* ============================================================================== */
float Get_FPar(short Index)
{
    int dllLoadError;
    if ((dllLoadError = LoadDLLIfNeeded())!=0)
        return (float)dllLoadError;
	
	return (*Get_FPar_Ptr)(Index, DeviceNo);            /* Get float parameter */
}

/* Returns double value of parameter (FPAR_"Index") from the ADwin-System          */
/* ============================================================================== */
double Get_FPar_Double(short Index)
{
    int dllLoadError;
    if ((dllLoadError = LoadDLLIfNeeded())!=0)
        return (double)dllLoadError;
		
	return	(*Get_FPar_Double_Ptr)(Index, DeviceNo);            /* Get float parameter */
}
/* If T12: Gets all 80 ADwin double parameters (FPar_1 - FPar_80) into a double array					  */
/* If T9..T11: Gets all 80 ADwin float parameters (FPar_1 - FPar_80) into a double array					  */
/* ============================================================================== */
short Get_FPar_All_Double (double Array[])
{
    short dllLoadError;
    if ((dllLoadError = (short)LoadDLLIfNeeded())!=0)
        return dllLoadError;
		
	return	(*Get_ADBFPar_All_Double_Ptr)(1, 80, Array, DeviceNo);            /* Get float parameter */
}

/* Gets a block of ADwin double parameters into a double array					  */
/* ============================================================================== */
short Get_FPar_Block_Double(double Array[], short Startindex, short Count)
{
    short dllLoadError;
    if ((dllLoadError = (short)LoadDLLIfNeeded())!=0)
        return dllLoadError;
	
    return (*Get_ADBFPar_All_Double_Ptr)(Startindex, Count, Array, DeviceNo);
}

/* Element/e aus einem ADbasic Datensatz vom Transputer holen.
            Übergabe in short ARRAY / wird in Doku nicht mehr beschrieben */
/* ==============================================================================*/
short GetData(short DataNo, short Data[], long Startindex, long Count )
{
    short dllLoadError;
    if ((dllLoadError = (short)LoadDLLIfNeeded())!=0)
        return dllLoadError;
	
	return (*Get_Data_Ptr)(Data, TYPE_SHORT, DataNo, Startindex, Count, DeviceNo); /* Datensatz holen / get array of data */
}


/* Returns array-element(s) of long-type from ADwin-System-array (DATA_"DataNo")  */
/* ============================================================================== */
short GetlData(short DataNo, long Data[], long Startindex, long Count)
{
    short dllLoadError;
    if ((dllLoadError = (short)LoadDLLIfNeeded())!=0)
        return dllLoadError;

	return (*Get_Data_Ptr)(Data, TYPE_LONG, DataNo, Startindex, Count, DeviceNo);
}

/* Returns array-element(s) of long-type from ADwin-System-array (DATA_"DataNo")  */
/* ============================================================================== */
short GetData_Long (short DataNo, long Data[], long Startindex, long Count)
{
    short dllLoadError;
    if ((dllLoadError = (short)LoadDLLIfNeeded())!=0)
        return dllLoadError;

	return (*Get_Data_Ptr)(Data, TYPE_LONG, DataNo, Startindex, Count, DeviceNo);
}

/* Returns array-element(s) of double-type from ADwin-System-array (DATA_"DataNo") */
/* ==============================================================================  */
short GetData_Double (short DataNo, double Data[],long Startindex, long Count)
{
    short dllLoadError;
    if ((dllLoadError = (short)LoadDLLIfNeeded())!=0)
        return dllLoadError;

	return (*Get_Data_Ptr)(Data, TYPE_DOUBLE, DataNo, Startindex, Count, DeviceNo);
}

/* Returns array-element(s) of float-type from ADwin-System-array (DATA_"DataNo") */
/* ============================================================================== */
short GetfData(short DataNo, float Data[], long Startindex, long Count)
{
    short dllLoadError;
    if ((dllLoadError = (short)LoadDLLIfNeeded())!=0)
        return dllLoadError;

	return (*Get_Data_Ptr)(Data, TYPE_FLOAT, DataNo, Startindex, Count, DeviceNo);
}


/* Returns array-element(s) of float-type from ADwin-System-array (DATA_"DataNo") */
/* ============================================================================== */
short GetData_Float (short DataNo, float Data[], long Startindex, long Count)
{
    short dllLoadError;
    if ((dllLoadError = (short)LoadDLLIfNeeded())!=0)
        return dllLoadError;

	return (*Get_Data_Ptr)(Data, TYPE_FLOAT, DataNo, Startindex, Count, DeviceNo);
}

/* Sets array-element(s) of ADwin-System from short-array						  */
/* ============================================================================== */
short SetData(short DataNo, short Data[], long Startindex, long Count)
{
    short dllLoadError;
    if ((dllLoadError = (short)LoadDLLIfNeeded())!=0)
        return dllLoadError;
	
	return (*Set_Data_Ptr)(Data, TYPE_SHORT, DataNo, Startindex, Count, DeviceNo);
}

/* Sets array-element(s) of ADwin-System from long-array						  */
/* ============================================================================== */
short SetlData(short DataNo, long Data[], long Startindex, long Count)
{
    short dllLoadError;
    if ((dllLoadError = (short)LoadDLLIfNeeded())!=0)
        return dllLoadError;
	
	return (*Set_Data_Ptr)(Data, TYPE_LONG, DataNo, Startindex, Count, DeviceNo);
}

/* Sets array-element(s) of ADwin-System from long-array						  */
/* ============================================================================== */
short SetData_Long(short DataNo, long Data[], long Startindex, long Count)
{
    short dllLoadError;
    if ((dllLoadError = (short)LoadDLLIfNeeded())!=0)
        return dllLoadError;

	return (*Set_Data_Ptr)(Data, TYPE_LONG, DataNo, Startindex, Count, DeviceNo);
}

/* Sets array-element(s) of ADwin-System from double-array						  */
/* ============================================================================== */
short SetData_Double(short DataNo, double Data[], long Startindex, long Count)
{
    short dllLoadError;
    if ((dllLoadError = (short)LoadDLLIfNeeded())!=0)
        return dllLoadError;

	return (*Set_Data_Ptr)(Data, TYPE_DOUBLE, DataNo, Startindex, Count, DeviceNo);
}

/* Sets array-element(s) of ADwin-System from float-array						  */
/* ============================================================================== */
short SetfData(short DataNo, float Data[], long Startindex, long Count)
{
    short dllLoadError;
    if ((dllLoadError = (short)LoadDLLIfNeeded())!=0)
        return dllLoadError;
	
	return (*Set_Data_Ptr)(Data, TYPE_FLOAT, DataNo, Startindex, Count, DeviceNo);
}

/* Sets array-element(s) of ADwin-System from float-array						   */
/* =============================================================================== */
short SetData_Float(short DataNo, float Data[], long Startindex, long Count)
{
    short dllLoadError;
    if ((dllLoadError = (short)LoadDLLIfNeeded())!=0)
        return dllLoadError;
	
	return (*Set_Data_Ptr)(Data, TYPE_FLOAT, DataNo, Startindex, Count, DeviceNo);
}

/* Writes array-elements of ADwin-System immediately to harddisk			    */
/* ============================================================================ */
short Data2File(char my_FAR *Filename, short DataNo, long Startindex, long Count, short Mode)
{
    short dllLoadError;
    if ((dllLoadError = (short)LoadDLLIfNeeded())!=0)
        return dllLoadError;
	
	return (*Save_Fast_Ptr)(Filename, DataNo, Startindex, Count, Mode, DeviceNo);
}

/* Writes a file immediately from harddisk to array-elements of ADwin-System 			    */
/* ============================================================================ */
short File2Data(char my_FAR *Filename, short DataType, short DataNo, long Startindex)
{
    short dllLoadError;
    if ((dllLoadError = (short)LoadDLLIfNeeded())!=0)
        return dllLoadError;
	//Hint: DataType constant
	//			TYPE_LONG;   2= long
  // 			TYPE_FLOAT;  5= float
  //			TYPE_DOUBLE; 6= double
	return (*File2Data_Ptr)(Filename, DataType, DataNo, Startindex, DeviceNo);
}


/* Fetching the short-element(s) from a ADwin-System FIFO						  */
/* ============================================================================== */
short GetFifo(short FifoNo, short Data[], long Count)
{
    short dllLoadError;
    if ((dllLoadError = (short)LoadDLLIfNeeded())!=0)
        return dllLoadError;
	
	return (*Get_Fifo_Ptr)(Data, TYPE_SHORT, FifoNo, Count, DeviceNo);
}

/* Fetching the long-element(s) from a ADwin-System FIFO						  */
/* ============================================================================== */
short GetlFifo(short FifoNo, long Data[], long Count)
{
    short dllLoadError;
    if ((dllLoadError = (short)LoadDLLIfNeeded())!=0)
        return dllLoadError;
	
	return (*Get_Fifo_Ptr)(Data, TYPE_LONG, FifoNo, Count, DeviceNo);
}

/* Fetching the long-element(s) from a ADwin-System FIFO						  */
/* ============================================================================== */
short GetFifo_Long(short FifoNo, long Data[], long Count)
{
    short dllLoadError;
    if ((dllLoadError = (short)LoadDLLIfNeeded())!=0)
        return dllLoadError;
	
	return (*Get_Fifo_Ptr)(Data, TYPE_LONG, FifoNo, Count, DeviceNo);
}

/* Fetching the double-element(s) from a ADwin-System FIFO						  */
/* ============================================================================== */
short GetFifo_Double(short FifoNo, double Data[], long Count)
{
    short dllLoadError;
    if ((dllLoadError = (short)LoadDLLIfNeeded())!=0)
        return dllLoadError;
	
	return (*Get_Fifo_Ptr)(Data, TYPE_DOUBLE, FifoNo, Count, DeviceNo);
}

/* Fetching the float-element(s) from a ADwin-System FIFO						  */
/* ============================================================================== */
short GetfFifo(short FifoNo, float Data[], long Count)
{
    short dllLoadError;
    if ((dllLoadError = (short)LoadDLLIfNeeded())!=0)
        return dllLoadError;
	
	return (*Get_Fifo_Ptr)(Data, TYPE_FLOAT, FifoNo, Count, DeviceNo);
}

/* Fetching the float-element(s) from a ADwin-System FIFO						  */
/* ============================================================================== */
short GetFifo_Float(short FifoNo, float Data[], long Count)
{
    short dllLoadError;
    if ((dllLoadError = (short)LoadDLLIfNeeded())!=0)
        return dllLoadError;
	
	return (*Get_Fifo_Ptr)(Data, TYPE_FLOAT, FifoNo, Count, DeviceNo);
}

/* Sets FIFO-element(s) of ADwin-System from short-array						  */
/* ============================================================================== */
short SetFifo(short FifoNo, short Data[], long Count)
{
    short dllLoadError;
    if ((dllLoadError = (short)LoadDLLIfNeeded())!=0)
        return dllLoadError;
	
	return (*Set_Fifo_Ptr)(Data, TYPE_SHORT, FifoNo, Count, DeviceNo);
}

/* Sets FIFO-element(s) of ADwin-System from long-array							  */
/* ============================================================================== */
short SetlFifo(short FifoNo, long Data[], long Count)
{
    short dllLoadError;
    if ((dllLoadError = (short)LoadDLLIfNeeded())!=0)
        return dllLoadError;

	return (*Set_Fifo_Ptr)(Data, TYPE_LONG, FifoNo, Count, DeviceNo);
}

/* Sets FIFO-element(s) of ADwin-System from long-array							  */
/* ============================================================================== */
short SetFifo_Long(short FifoNo, long Data[], long Count)
{
    short dllLoadError;
    if ((dllLoadError = (short)LoadDLLIfNeeded())!=0)
        return dllLoadError;

	return (*Set_Fifo_Ptr)(Data, TYPE_LONG, FifoNo, Count, DeviceNo);
}

/* Sets FIFO-element(s) of ADwin-System from double-array						  */
/* ============================================================================== */
short SetFifo_Double(short FifoNo, double Data[], long Count)
{
    short dllLoadError;
    if ((dllLoadError = (short)LoadDLLIfNeeded())!=0)
        return dllLoadError;

	return (*Set_Fifo_Ptr)(Data, TYPE_DOUBLE, FifoNo, Count, DeviceNo);
}

/* Sets FIFO-element(s) of ADwin-System from float-array						  */
/* ============================================================================== */
short SetfFifo(short FifoNo, float Data[], long Count)
{
    short dllLoadError;
    if ((dllLoadError = (short)LoadDLLIfNeeded())!=0)
        return dllLoadError;

	return (*Set_Fifo_Ptr)(Data, TYPE_FLOAT, FifoNo, Count, DeviceNo);
}

/* Sets FIFO-element(s) of ADwin-System from float-array						  */
/* ============================================================================== */
short SetFifo_Float(short FifoNo, float Data[], long Count)
{
    short dllLoadError;
    if ((dllLoadError = (short)LoadDLLIfNeeded())!=0)
        return dllLoadError;
	
	return (*Set_Fifo_Ptr)(Data, TYPE_FLOAT, FifoNo, Count, DeviceNo);
}

/* Getting the number of elements in the FIFO									  */
/* ============================================================================== */
long GetFifoCount(short FifoNo)
{
    long dllLoadError;
    if ((dllLoadError = (long)LoadDLLIfNeeded())!=0)
        return dllLoadError;
	
	return (*Get_Fifo_Count_Ptr)(FifoNo, DeviceNo);
}

/* Getting the number of elements in the FIFO									  */
/* ============================================================================== */
long Fifo_Full(short FifoNo)
{
    long dllLoadError;
    if ((dllLoadError = (long)LoadDLLIfNeeded())!=0)
        return dllLoadError;
	
	return (*Get_Fifo_Count_Ptr)(FifoNo, DeviceNo);
}

/* Returns number of free elements in FIFO										  */
/* ============================================================================== */
long GetFifoEmpty(short FifoNo)
{
    long dllLoadError;
    if ((dllLoadError = (long)LoadDLLIfNeeded())!=0)
        return dllLoadError;
	
	return (*Get_Fifo_Empty_Ptr)(FifoNo, DeviceNo);
}

/* Returns number of free elements in FIFO										  */
/* ============================================================================== */
long Fifo_Empty (short FifoNo)
{
    long dllLoadError;
    if ((dllLoadError = (long)LoadDLLIfNeeded())!=0)
        return dllLoadError;
	
	return (*Get_Fifo_Empty_Ptr)(FifoNo, DeviceNo);
}

/* Initiats read and write pointer of FIFO										  */
/* ============================================================================== */
short ClearFifo (short FifoNo)
{
    short dllLoadError;
    if ((dllLoadError = (short)LoadDLLIfNeeded())!=0)
        return dllLoadError;
	
	return (*Fifo_Clear_Ptr)(FifoNo, DeviceNo);
}

/* Initiats read and write pointer of FIFO										  */
/* ============================================================================== */
short Fifo_Clear (short FifoNo)
{
    short dllLoadError;
    if ((dllLoadError = (short)LoadDLLIfNeeded())!=0)
        return dllLoadError;
	
	return (*Fifo_Clear_Ptr)(FifoNo, DeviceNo);
}

/* Returns measured value of analog input channel								  */
/* ============================================================================== */
unsigned short ADC(short Value)
{
    unsigned short dllLoadError;
    if ((dllLoadError = (unsigned short)LoadDLLIfNeeded())!=0)
        return dllLoadError;
	
	return (*Get_ADC_Ptr)(Value, DeviceNo);   /* Messergebnis abholen */
}


/* Sets analog output "Channel" to value "Value" (in digits)					  */
/* ============================================================================== */
short SetDAC(short Channel, unsigned short Value)
{
    short dllLoadError;
    if ((dllLoadError = (short)LoadDLLIfNeeded())!=0)
        return dllLoadError;
	
	return (*Set_DAC_Ptr)(Channel, Value, DeviceNo);
}

/* Sets analog output "Channel" to value "Value" (in digits)					  */
/* ============================================================================== */
short DAC(short Channel, unsigned short Value)
{
    short dllLoadError;
    if ((dllLoadError = (short)LoadDLLIfNeeded())!=0)
        return dllLoadError;
	
	return (*Set_DAC_Ptr)(Channel, Value, DeviceNo);
}

/* Returns state of digital inputs (bits 0...15)								  */
/* ============================================================================== */
long DigIn()
{
    long dllLoadError;
    if ((dllLoadError = (long)LoadDLLIfNeeded())!=0)
        return dllLoadError;
	
	return (*Get_Digin_Ptr)(DeviceNo);           /* Ergebnis abholen */
}

/* Returns state of digital inputs (bits 0...15)								  */
/* ============================================================================== */
long Get_Digin()
{
    long dllLoadError;
    if ((dllLoadError = (long)LoadDLLIfNeeded())!=0)
        return dllLoadError;
	
	return (*Get_Digin_Ptr)(DeviceNo);           /* Ergebnis abholen */
}

/* Outputs the value which is found in "Value" on the digital outputs			  */
/* ============================================================================== */
short SetDigOut (short Value)
{
    short dllLoadError;
    if ((dllLoadError = (short)LoadDLLIfNeeded())!=0)
        return dllLoadError;
	
	return (*Set_Digout_Ptr)(Value, DeviceNo);
}

/* Outputs the value which is found in "Value" on the digital outputs             */
/* ============================================================================== */
short Set_Digout (short Value)
{
    short dllLoadError;
    if ((dllLoadError = (short)LoadDLLIfNeeded())!=0)
        return dllLoadError;
	
	return (*Set_Digout_Ptr)(Value, DeviceNo);
}

/* Returning the value of the digital outputs					                  */
/* ============================================================================== */
long DigOut()
{
    long dllLoadError;
    if ((dllLoadError = (long)LoadDLLIfNeeded())!=0)
        return dllLoadError;
	
	return (*Get_Digout_Ptr)(DeviceNo);          /* Ergebnis abholen */
}

/* Returning the value of the digital outputs					                  */
/* ============================================================================== */

long Get_Digout()
{
    long dllLoadError;
    if ((dllLoadError = (long)LoadDLLIfNeeded())!=0)
        return dllLoadError;
	
	return (*Get_Digout_Ptr)(DeviceNo);          /* Ergebnis abholen */
}

/* Returns how busy the CPU is (processor usage in percent)                       */
/* ============================================================================== */
short Auslast()
{
    short dllLoadError;
    if ((dllLoadError = (short)LoadDLLIfNeeded())!=0)
        return dllLoadError;
	
	return (*Auslastung_Ptr)(DeviceNo);          /* Ergebnis abholen */
}

/* Returns how busy the CPU is (processor usage in percent)                       */
/* ============================================================================== */
short Workload(long Priority)
{
    short dllLoadError;
    if ((dllLoadError = (short)LoadDLLIfNeeded())!=0)
        return dllLoadError;

	return (*Workload_Ptr)(Priority, DeviceNo);          /* Ergebnis abholen */
}

/* Checks if the CPU is accessible (previously booted and running)				  */
/* ============================================================================== */
short Test()
{
    short dllLoadError;
    if ((dllLoadError = (short)LoadDLLIfNeeded())!=0)
        return dllLoadError;
	
	return (*ADTest_Version_Ptr)(DeviceNo, message);
}

/* Checks if the CPU is accessible (previously booted and running)				  */
/* ============================================================================== */
short Test_Version()
{
    short dllLoadError;
    if ((dllLoadError = (short)LoadDLLIfNeeded())!=0)
        return dllLoadError;
	
	return (*ADTest_Version_Ptr)(DeviceNo, message);
}

/* checks the momentary free memory of the ADwin-System return value is the		  */
/* free memory in byte only for T4,T5 and T8									  */
/* ============================================================================== */
long Freemem()
{
    long dllLoadError;
    if ((dllLoadError = (long)LoadDLLIfNeeded())!=0)
        return dllLoadError;
	
	return (*AD_Memory_Ptr)(DeviceNo);          /* Get Freemem */
}

/* Returns value of free memory from the ADwin-9 System(in bytes)				  */
/* ============================================================================== */
long Freemem_T9(short Mem_Spec)
{
    long dllLoadError;
    if ((dllLoadError = (long)LoadDLLIfNeeded())!=0)
        return dllLoadError;
	
	switch(Mem_Spec)
	{
		case 0:
			return (*AD_Memory_Ptr)(DeviceNo);				     /* T2, T4, T5, T8 */
		case 1:
			return ((*AD_Memory_all_Ptr)(Mem_Spec, DeviceNo)*6); /* T9-PM */
		case 3:
		case 4:
			return ((*AD_Memory_all_Ptr)(Mem_Spec, DeviceNo)*4); /* T9-DM_Local, T9-DM_Extern */
		default:
			return 255;
	}         
}


/* Returns value of free memory from the ADwin-2, -4, -5 ,-8, -9, -10 and -11 System(in Bytes) */
/* Returns value of free memory from the ADwin-12 System(in kBytes) */
/* ================================================================================= */
long Free_Mem(short Mem_Spec)
{
    long dllLoadError;
    if ((dllLoadError = (long)LoadDLLIfNeeded())!=0)
        return dllLoadError;

	switch(Mem_Spec)
	{
		case 0:	/* 0 = T2, T4, T5, T8 */
			return (*AD_Memory_Ptr)(DeviceNo);										
		/* 1 = PM : T9, T10, T11 */
		/* 2 = EM : T11 */
		/* 3 = DM_Local    : T9, T10, T11 */
		/* 4 = DRAM_Extern : T9, T10, T11 */
		
		/* 5 = Cacheable  : T12   in kBytes */
		/* 6 = Uncacheable: T12   in kBytes */		
		default:
			return (*AD_Memory_all_byte_Ptr)(Mem_Spec, DeviceNo);
	}    
}

/* Error messages for Test, ADBload, Boot & Net_Connect    0/1 -> (off/on)        */
/* ============================================================================== */
void ErrMessage(short OnOff)
{
	message=OnOff;
}

/* Error messages for Test, ADBload, Boot & Net_Connect    0/1 -> (off/on)        */
/* ============================================================================== */
void Show_Errors(short OnOff)
{
  message=OnOff;
}

/* Gets access to the ADwin board in another server via network.				  */
/* On this server the program ADserver has to run, which will be provided		  */
/* together with ADbasic.														  */
/* ============================================================================== */
short Net_Connect(char my_FAR *Protocol, char my_FAR *Address, char my_FAR *Endpoint, char my_FAR *Password)
{
    short dllLoadError;
    if ((dllLoadError = (short)LoadDLLIfNeeded())!=0)
        return dllLoadError;

	return (*AD_Net_Connect_Ptr)(Protocol, Address, Endpoint, Password, message);
}

/* Disconnects network access to the ADwin-card									  */
/* in another server															  */
/* ============================================================================== */
short Net_Disconnect()
{
    short dllLoadError;
    if ((dllLoadError = (short)LoadDLLIfNeeded())!=0)
        return dllLoadError;

	return (*AD_Net_Disconnect_Ptr)();
}

/* Gets the error code of the last error occured								  */
/* ============================================================================== */
long Get_Last_Error()
{
    long dllLoadError;
    if ((dllLoadError = (long)LoadDLLIfNeeded())!=0)
        return dllLoadError;

	return (*AD_GetErrorCode_Ptr)();
}

/* Gets the error text of the "Last error"										  */
/* ============================================================================== */
char* Get_Last_Error_Text(long Last_Error) 
{
    long		merker;
		static char Fehlertext[255];

    if ((short)LoadDLLIfNeeded()!=0)
        return "dllLoadError in Get_Last_Error_Text";

	merker = sizeof(Fehlertext);
	(*AD_GetErrorText_Ptr)(Last_Error, Fehlertext, merker);

	return Fehlertext;

}

/* Returns the Processortype												      */
/* ============================================================================== */
short Processor_Type()
{
	short merker,Processor_Type;
    short dllLoadError;
    if ((dllLoadError = (short)LoadDLLIfNeeded())!=0)
        return dllLoadError;
	
	  merker = (*ADProzessorTyp_Ptr)(DeviceNo);
	  Processor_Type = merker;
 
	  if (merker == 146)
		Processor_Type = 5;
	  if (merker == 1000)
		Processor_Type = 9;
	  // The other processor types are returned direct with the correct number

	return (Processor_Type);
}

/* Clear the Process															  */
/* ============================================================================== */
long Clear_Process(short ProcessNo)
{
    long dllLoadError;
    if ((dllLoadError = (long)LoadDLLIfNeeded())!=0)
        return dllLoadError;
    
	return (*Clear_Process_Ptr)(ProcessNo,DeviceNo);
}

/* Returns the Processstatus													  */
/* ============================================================================== */
long Process_Status(short ProcessNo)
{
	long ProcessStatus;
	short erg;

	erg = (short)(-100 + ProcessNo);
	ProcessStatus = Get_Par(erg);
	
	return (ProcessStatus);
}
  
/* Gets a block of ADwin long parameters into a long array						  */
/* ============================================================================== */
short Get_Par_Block(long Array[], short Startindex, short Count)
{
    short dllLoadError;
    if ((dllLoadError = (short)LoadDLLIfNeeded())!=0)
        return dllLoadError;
    
	return (*Get_ADBPar_All_Ptr)(Startindex, Count, Array, DeviceNo);
}

/* Gets a block of ADwin integer parameters into a long array					  */
/* ============================================================================== */
short Get_FPar_Block(float Array[], short Startindex, short Count)
{
    short dllLoadError;
    if ((dllLoadError = (short)LoadDLLIfNeeded())!=0)
        return dllLoadError;
    
	return (*Get_ADBFPar_All_Ptr)(Startindex, Count, Array, DeviceNo);
}

/* Gets all 80 long parameters (Par_1 - Par_80) into a long array				  */
/* ============================================================================== */
short Get_Par_All(long Array[])
{
    short dllLoadError;
    if ((dllLoadError = (short)LoadDLLIfNeeded())!=0)
        return dllLoadError;
    
	return (*Get_ADBPar_All_Ptr)(1, 80, Array, DeviceNo);
}

/* Gets all 80 float parameters (Par_1 - Par_80) into a long array				  */
/* ============================================================================== */
short Get_FPar_All(float Array[])
{
    short dllLoadError;
    if ((dllLoadError = (short)LoadDLLIfNeeded())!=0)
        return dllLoadError;
    
	return (*Get_ADBFPar_All_Ptr)(1, 80, Array, DeviceNo);
}

/* Returns the lenght of a data													  */
/* ============================================================================== */
long Data_Length(long DataNo)
{
    long dllLoadError;
    if ((dllLoadError = (long)LoadDLLIfNeeded())!=0)
        return dllLoadError;
    
	return (*Get_Data_Length_Ptr)((short)DataNo, DeviceNo);
}

/* Sets a globaldelay for a specified prozess on the ADwin-System				  */
/* ============================================================================== */
long Set_Globaldelay(short ProcessNo, long Globaldelay)
{
    long dllLoadError;
    if ((dllLoadError = (long)LoadDLLIfNeeded())!=0)
        return dllLoadError;
	
	return Set_Par((short)(-90+ProcessNo), Globaldelay);
}

/* Sets a processdelay for a specified prozess on the ADwin-System				  */
/* ============================================================================== */
long Set_Processdelay(short ProcessNo, long Processdelay)
{
	return Set_Globaldelay(ProcessNo, Processdelay);
}

/* Gets a globaldelay for a specified prozess on the ADwin-System				  */
/* ============================================================================== */
long Get_Globaldelay(short ProcessNo)
{
    long dllLoadError;
    if ((dllLoadError = (long)LoadDLLIfNeeded())!=0)
        return dllLoadError;
	
	return Get_Par((short)(-90+ProcessNo));
}

/* Gets a processdelay for a specified prozess on the ADwin-System				  */
/* ============================================================================== */
long Get_Processdelay(short ProcessNo)
{
	return Get_Globaldelay(ProcessNo);
}


/* Sets the language for the error messages										  */
/* ============================================================================== */
long Set_Language(long language)
{
    long dllLoadError;
    if ((dllLoadError = (long)LoadDLLIfNeeded())!=0)
        return dllLoadError;
	
	return (*ADSetLanguage_Ptr)(language);
}

/* Returns array-element(s) of char-type from ADwin-System-array (DATA_"DataNo")  */
/* ============================================================================== */
long GetData_String(long DataNo, char *Data, long MaxCount)
{
    long dllLoadError;
    if ((dllLoadError = (long)LoadDLLIfNeeded())!=0)
        return dllLoadError;
	
	return (*GetData_String_Ptr)(Data, MaxCount+1, (short)DataNo, DeviceNo);
}

/* Returns the lenght of a string												  */
/* ============================================================================== */
long String_Length(long DataNo)
{
    long dllLoadError;
    if ((dllLoadError = (long)LoadDLLIfNeeded())!=0)
        return dllLoadError;
	
	return (*String_Length_Ptr)((short)DataNo, DeviceNo);
}

/* Write a String into a (DATA_"DataNo")										  */
/* ============================================================================== */
long SetData_String(long DataNo, char *Data)
{
    long dllLoadError;
    if ((dllLoadError = (long)LoadDLLIfNeeded())!=0)
        return dllLoadError;
	
	return (*SetData_String_Ptr)(Data, (short)DataNo, DeviceNo);
}

/* Returns array-element(s) of short-type from packed ADwin-System-array (DATA_"DataNo") */
/* ============================================================================== */
short GetData_Packed_Short(long DataNo, short *Data, long Startindex, long Count)
{
    short dllLoadError;
    if ((dllLoadError = (short)LoadDLLIfNeeded())!=0)
        return dllLoadError;

	return (*GetData_Packed_Ptr)(Data, TYPE_SHORT, (short)DataNo, Startindex, Count, DeviceNo);
}

/* Returns array-element(s) of long-type from packed ADwin-System-array (DATA_"DataNo") */
/* ============================================================================== */
short GetData_Packed_Long(long DataNo, long *Data, long Startindex, long Count)
{
    short dllLoadError;
    if ((dllLoadError = (short)LoadDLLIfNeeded())!=0)
        return dllLoadError;

	return (*GetData_Packed_Ptr)(Data, TYPE_LONG, (short)DataNo, Startindex, Count, DeviceNo);
}

/* Returns array-element(s) of float-type from packed ADwin-System-array (DATA_"DataNo") */
/* ============================================================================== */
short GetData_Packed_Float(long DataNo, float *Data, long Startindex, long Count)
{
    short dllLoadError;
    if ((dllLoadError = (short)LoadDLLIfNeeded())!=0)
        return dllLoadError;

	return (*GetData_Packed_Ptr)(Data, TYPE_FLOAT, (short)DataNo, Startindex, Count, DeviceNo);
}

/* Returns array-element(s) of double-type from packed ADwin-System-array (DATA_"DataNo") */
/* ============================================================================== */
short GetData_Packed_Double(long DataNo, double *Data, long Startindex, long Count)
{
    short dllLoadError;
    if ((dllLoadError = (short)LoadDLLIfNeeded())!=0)
        return dllLoadError;

	return (*GetData_Packed_Ptr)(Data, TYPE_DOUBLE, (short)DataNo, Startindex, Count, DeviceNo);
}


/* Returns state of ACTIVATE_PC-flag of process number "ProcessNo"                */
/* ============================================================================== */
long GetActivate(short ProcessNo)
{
  short parno;
  long erg;

  parno = (short)(930+ProcessNo-1000);
  erg = Get_Par(parno);     /* Parameter holen */
  if (erg == 1)
	  Set_Par(parno, 0);
  
  return (erg);
}

/* Returns state of ACTIVATE_PC-flag of process number "ProcessNo"				  */
/* ============================================================================== */
long Activate_PC(short ProcessNo)
{
  short parno;
  long erg;

  parno = (short)(930+ProcessNo-1000);
  erg = Get_Par(parno);     /* Parameter holen */
  if (erg == 1)
	  Set_Par(parno, 0);
  
  return (erg);
}

/* Set option on the ADwin-cart to the value "Value"							  */
/* ============================================================================== */
short SetOpt (short Index, long Value)
{
    short dllLoadError;
    if ((dllLoadError = (short)LoadDLLIfNeeded())!=0)
        return dllLoadError;

	return (Set_Par((short)(Index-1000), Value));
}

/* Get option from the ADwin-cart												  */
/* ============================================================================== */
long GetOpt(short Index)
{
    long dllLoadError;
    if ((dllLoadError = (long)LoadDLLIfNeeded())!=0)
        return dllLoadError;

	return (Get_Par((short)(Index-1000)));          
}

/* Das short Array auf welches iw zeigt als Ausgabesignalform auf ADwin-Karte ausgeben */
/* ============================================================================== */
short SetDACListe(short Channel, long Count, short Value[])
{
    short dllLoadError;
    if ((dllLoadError = (short)LoadDLLIfNeeded())!=0)
        return dllLoadError;

	return (Set_List_Ptr(Value, TYPE_SHORT, Channel, Count, DeviceNo));
}

/* Eine Messwertliste abfragen und in ein short Array schreiben					  */
/* ============================================================================== */
short ReadADListe(short Channel, long Count, short Value[])
{
    short dllLoadError;
    if ((dllLoadError = (short)LoadDLLIfNeeded())!=0)
        return dllLoadError;

	return (Get_List_Ptr(Value, TYPE_SHORT, Channel, 1, Count, DeviceNo));
}

/* Starts process number "ProcessNoZyk" on the ADwin-System						  */
/* ============================================================================== */
short ZykStart(short ProzessNoZyk)
{
    short dllLoadError;
    if ((dllLoadError = (short)LoadDLLIfNeeded())!=0)
        return dllLoadError;

	return (*Start_Ptr)(ProzessNoZyk, DeviceNo);	/* Start process */
}

/* Stops process number "ProcessNoZyk" on the ADwin-System						  */
/* ============================================================================== */
short ZykStop(short ProzessNoZyk)
{
    short dllLoadError;
    if ((dllLoadError = (short)LoadDLLIfNeeded())!=0)
        return dllLoadError;
	
	return (*Stop_Ptr)(ProzessNoZyk, DeviceNo);	/* Start process */
}

/* Reads out the known device numbers which are configured in the PC			  */
/* ============================================================================== */
short Get_Known_DeviceNo(short *Devices, long *Count_Devices)
{
    short dllLoadError;
    if ((dllLoadError = (short)LoadDLLIfNeeded())!=0)
        return dllLoadError;
	
	return (*Get_Known_Deviceno_Ptr)(Devices, Count_Devices);	/* Start process */
}

/* Reads out the known USB device serialnumbers which are configured in the PC			  */
/* ============================================================================== */
long Get_Known_USB_SerialNo(long *SerialNo)
{
    short dllLoadError;
    if ((dllLoadError = (short)LoadDLLIfNeeded())!=0)
        return dllLoadError;
	
	return (*Get_Known_USB_SerialNo_Ptr)(SerialNo);	/* Start process */
}

/* Reads out the known USB device serialnumbers which are configured in the PC			  */
/* ============================================================================== */
int ADwin_Debug_Mode_On(char my_FAR *Filename, long Size)
{
    short dllLoadError;
    if ((dllLoadError = (short)LoadDLLIfNeeded())!=0)
        return dllLoadError;
	
	return (*ADwin_Debug_Mode_On_Ptr)(Filename, Size);	/* Start process */
}

/* Reads out the known USB device serialnumbers which are configured in the PC			  */
/* ============================================================================== */
int ADwin_Debug_Mode_Off(void)
{
    short dllLoadError;
    if ((dllLoadError = (short)LoadDLLIfNeeded())!=0)
        return dllLoadError;
	
	return (*ADwin_Debug_Mode_Off_Ptr)();	/* Start process */
}

/* Returns Fifo-element(s) of short-type from packed ADwin-Fifo-array (DATA_"DataNo") */
/* ============================================================================== */
short GetFifo_Packed_Short(short FifoNo, short Data[], long Count)
{
    short dllLoadError;
    if ((dllLoadError = (short)LoadDLLIfNeeded())!=0)
        return dllLoadError;

	return (*Get_Fifo_Ptr)(Data, TYPE_LONG, FifoNo, Count, DeviceNo);
}

/* The function "GetFifo_Packed_Long " is deleted, because it was not working in the requested way. */
/* ============================================================================== */

/* Reads out the Type of Connection */
/* ============================================================================== */
long Get_Connection_Type(void)
{
    short dllLoadError;
    if ((dllLoadError = (short)LoadDLLIfNeeded())!=0)
        return dllLoadError;

	return (*Get_Connection_Type_Ptr)(DeviceNo);

	/*Return Value:  -1: ERROR */
	/*                0: PORT (ISA/LINK) */
	/*                1: USB -> Device connected */
	/*                2: TCP/IP */
	/*               11: USB -> Device not connected */
}


/* 05.05.2004 MS
Important:

If an application program reads the MAC for identifying a special ADwin-system and will refuse operation , if the MAC does not match, then the following issue should be considered :

What if an ADwin-system breaks and needs to be replaced ?

Neither Jaeger Messtechnik nor Keithley Instruments will provide a spare part with an IDENTICAL  MAC address or USB- serialnumber !

The application program has to be prepared somehow, that a replacement with an ADwin-system with a DIFFERENT MAC or USB- serialnumber might be necessary !

Firewall settings:
The (*.exe) program, which uses Get_DEV_ID has to be added as an exception in the (XP, Vista, ...) firewall settings.

AND:  It must be ensured, that the (UDP) Port 7000 is not blocked by the firewall.
*/

/* Reads out the USB device serialnumber / the MAC-Adress from specified System   */
/* ============================================================================== */
long Get_Dev_ID(long *value)
{
    short dllLoadError;
    if ((dllLoadError = (short)LoadDLLIfNeeded())!=0)
        return dllLoadError;

	return (*Get_Dev_ID_Ptr)(DeviceNo, value);
}


// End of file


