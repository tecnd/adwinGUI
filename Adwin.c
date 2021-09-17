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


#define kFailedToLoadDLLError     1
#define kCouldNotFindFunction     1

/* The variable "DeviceNo" is declared and inizialized to the default value of 0x150.
   This variable is responsible for all accesses to the ADwin-system(s). */
short DeviceNo = 0x1;		// Device number 
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
short (FAR PASCAL *Get_Data_Ptr)(void *Data, short typ, short DataNo, long Startindex, long Count ,short Device_No);
short (FAR PASCAL *Set_Data_Ptr)(void *Data, short typ, short DataNo, long Startindex, long Count ,short Device_No);
short (FAR PASCAL *Save_Fast_Ptr)(char my_FAR *Filename, short DataNo, long Startindex, long Count, short Mode, short Device_No);
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
short (FAR PASCAL *AD_Net_Connect_Ptr)(char my_FAR *Protocol, char my_FAR *Address, char my_FAR *Endpoint, char my_FAR *Password, long Message);
short (FAR PASCAL *AD_Net_Disconnect_Ptr)(void);
short (FAR PASCAL *ADProzessorTyp_Ptr)(short Device_No);
long  (FAR PASCAL *AD_GetErrorCode_Ptr)(void);
char  (FAR PASCAL *AD_GetErrorText_Ptr)(long ErrorCode, char *text, long lenght);
short (FAR PASCAL *Get_ADBPar_All_Ptr)(short Startindex, short Count, long *Array, short Device_No);
short (FAR PASCAL *Get_ADBFPar_All_Ptr)(short Startindex, short Count, float *Array, short Device_No);
long  (FAR PASCAL *Get_Data_Length_Ptr)(short DataNo, short Device_No);
long  (FAR PASCAL *ADSetLanguage_Ptr)(long language);
long  (FAR PASCAL *GetData_String_Ptr)(char *Data, long MaxCount,short DataNo, short Device_No);
long  (FAR PASCAL *String_Length_Ptr)(short DataNo, short Device_No);
long  (FAR PASCAL *SetData_String_Ptr)(char *Data,short DataNo, short Device_No);
short  (FAR PASCAL *GetData_Packed_Ptr)(void *Data, short Typ, short DataNo, long Startindex, long Count, short Device_No);
short (FAR PASCAL *Get_List_Ptr)(void *Destination,short typ, short Startindex, short proc,long Count, short Device_No);
short (FAR PASCAL *Set_List_Ptr)(void *Source,short typ,short Startindex, long Count, short Device_No);
short (FAR PASCAL *Start_Ptr)(short Index, short Device_No);
short (FAR PASCAL *Stop_Ptr)(short Index, short Device_No);
short (FAR PASCAL *Get_Known_Deviceno_Ptr)(short *Devices, long *Count_Devices);
long  (FAR PASCAL *Get_Known_USB_SerialNo_Ptr)(long *SerialNo);
int   (FAR PASCAL *ADwin_Debug_Mode_On_Ptr)(char my_FAR *Filename, long Size);
int   (FAR PASCAL *ADwin_Debug_Mode_Off_Ptr)(void);

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

    DLLHandle =  LoadLibrary("adwin32.dll");
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
    if ((Get_Par_Ptr = (long (FAR PASCAL *)(short, short))GetProcAddress(DLLHandle,"Get_ADBPar"))==0)
        goto FunctionNotFoundError;
    if ((Get_FPar_Ptr = (float (FAR PASCAL *)(short, short))GetProcAddress(DLLHandle,"Get_ADBFPar"))==0)
        goto FunctionNotFoundError;
    if ((Get_Data_Ptr = (short (FAR PASCAL *)(void*, short, short, long, long, short))GetProcAddress(DLLHandle,"Get_Data"))==0)
        goto FunctionNotFoundError;
    if ((Set_Data_Ptr = (short (FAR PASCAL *)(void*, short, short, long, long, short))GetProcAddress(DLLHandle,"Set_Data"))==0)
        goto FunctionNotFoundError;
    if ((Save_Fast_Ptr = (short (FAR PASCAL *)(char my_FAR *, short, long, long, short, short))GetProcAddress(DLLHandle,"SaveFast"))==0)
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

    return 0;

/* Error handling */
FunctionNotFoundError:
	if (FirstTime==1)
	{
		MessageBox(NULL, "You need a new Version of your ADwin32.dll!\nAt least ADwin32.dll from 14.5.2001!\nYour ADwin32.dll will removed!","ADwin32.dll to old!", MB_ICONWARNING);
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

/* Element/e aus einem ADbasic Datensatz vom Transputer holen.
            Übergabe in short ARRAY / wird in Doku nicht mehr beschrieben */
/* ==============================================================================*/
short GetData(short DataNo, short Data[], long Startindex, long Count )
{
    short dllLoadError;
    if ((dllLoadError = (short)LoadDLLIfNeeded())!=0)
        return dllLoadError;
	
	return (*Get_Data_Ptr)(Data, 1, DataNo, Startindex, Count, DeviceNo); /* Datensatz holen / get array of data */
}


/* Returns array-element(s) of long-type from ADwin-System-array (DATA_"DataNo")  */
/* ============================================================================== */
short GetlData(short DataNo, long Data[], long Startindex, long Count)
{
    short dllLoadError;
    if ((dllLoadError = (short)LoadDLLIfNeeded())!=0)
        return dllLoadError;

	return (*Get_Data_Ptr)(Data, 2, DataNo, Startindex, Count, DeviceNo);
}

/* Returns array-element(s) of long-type from ADwin-System-array (DATA_"DataNo")  */
/* ============================================================================== */
short GetData_Long (short DataNo, long Data[], long Startindex, long Count)
{
    short dllLoadError;
    if ((dllLoadError = (short)LoadDLLIfNeeded())!=0)
        return dllLoadError;

	return (*Get_Data_Ptr)(Data, 2, DataNo, Startindex, Count, DeviceNo);
}

/* Returns array-element(s) of double-type from ADwin-System-array (DATA_"DataNo") */
/* ==============================================================================  */
short GetData_Double (short DataNo, double Data[],long Startindex, long Count)
{
    short dllLoadError;
    if ((dllLoadError = (short)LoadDLLIfNeeded())!=0)
        return dllLoadError;

	return (*Get_Data_Ptr)(Data, 6, DataNo, Startindex, Count, DeviceNo);
}

/* Returns array-element(s) of float-type from ADwin-System-array (DATA_"DataNo") */
/* ============================================================================== */
short GetfData(short DataNo, float Data[], long Startindex, long Count)
{
    short dllLoadError;
    if ((dllLoadError = (short)LoadDLLIfNeeded())!=0)
        return dllLoadError;

	return (*Get_Data_Ptr)(Data, 5, DataNo, Startindex, Count, DeviceNo);
}


/* Returns array-element(s) of float-type from ADwin-System-array (DATA_"DataNo") */
/* ============================================================================== */
short GetData_Float (short DataNo, float Data[], long Startindex, long Count)
{
    short dllLoadError;
    if ((dllLoadError = (short)LoadDLLIfNeeded())!=0)
        return dllLoadError;

	return (*Get_Data_Ptr)(Data, 5, DataNo, Startindex, Count, DeviceNo);
}

/* Sets array-element(s) of ADwin-System from short-array						  */
/* ============================================================================== */
short SetData(short DataNo, short Data[], long Startindex, long Count)
{
    short dllLoadError;
    if ((dllLoadError = (short)LoadDLLIfNeeded())!=0)
        return dllLoadError;
	
	return (*Set_Data_Ptr)(Data, 1, DataNo, Startindex, Count, DeviceNo);
}

/* Sets array-element(s) of ADwin-System from long-array						  */
/* ============================================================================== */
short SetlData(short DataNo, long Data[], long Startindex, long Count)
{
    short dllLoadError;
    if ((dllLoadError = (short)LoadDLLIfNeeded())!=0)
        return dllLoadError;
	
	return (*Set_Data_Ptr)(Data, 2, DataNo, Startindex, Count, DeviceNo);
}

/* Sets array-element(s) of ADwin-System from long-array						  */
/* ============================================================================== */
short SetData_Long(short DataNo, long Data[], long Startindex, long Count)
{
    short dllLoadError;
    if ((dllLoadError = (short)LoadDLLIfNeeded())!=0)
        return dllLoadError;

	return (*Set_Data_Ptr)(Data, 2, DataNo, Startindex, Count, DeviceNo);
}

/* Sets array-element(s) of ADwin-System from double-array						  */
/* ============================================================================== */
short SetData_Double(short DataNo, double Data[], long Startindex, long Count)
{
    short dllLoadError;
    if ((dllLoadError = (short)LoadDLLIfNeeded())!=0)
        return dllLoadError;

	return (*Set_Data_Ptr)(Data, 6, DataNo, Startindex, Count, DeviceNo);
}

/* Sets array-element(s) of ADwin-System from float-array						  */
/* ============================================================================== */
short SetfData(short DataNo, float Data[], long Startindex, long Count)
{
    short dllLoadError;
    if ((dllLoadError = (short)LoadDLLIfNeeded())!=0)
        return dllLoadError;
	
	return (*Set_Data_Ptr)(Data, 5, DataNo, Startindex, Count, DeviceNo);
}

/* Sets array-element(s) of ADwin-System from float-array						   */
/* =============================================================================== */
short SetData_Float(short DataNo, float Data[], long Startindex, long Count)
{
    short dllLoadError;
    if ((dllLoadError = (short)LoadDLLIfNeeded())!=0)
        return dllLoadError;
	
	return (*Set_Data_Ptr)(Data, 5, DataNo, Startindex, Count, DeviceNo);
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

/* Fetching the short-element(s) from a ADwin-System FIFO						  */
/* ============================================================================== */
short GetFifo(short FifoNo, short Data[], long Count)
{
    short dllLoadError;
    if ((dllLoadError = (short)LoadDLLIfNeeded())!=0)
        return dllLoadError;
	
	return (*Get_Fifo_Ptr)(Data, 1, FifoNo, Count, DeviceNo);
}

/* Fetching the long-element(s) from a ADwin-System FIFO						  */
/* ============================================================================== */
short GetlFifo(short FifoNo, long Data[], long Count)
{
    short dllLoadError;
    if ((dllLoadError = (short)LoadDLLIfNeeded())!=0)
        return dllLoadError;
	
	return (*Get_Fifo_Ptr)(Data, 2, FifoNo, Count, DeviceNo);
}

/* Fetching the long-element(s) from a ADwin-System FIFO						  */
/* ============================================================================== */
short GetFifo_Long(short FifoNo, long Data[], long Count)
{
    short dllLoadError;
    if ((dllLoadError = (short)LoadDLLIfNeeded())!=0)
        return dllLoadError;
	
	return (*Get_Fifo_Ptr)(Data, 2, FifoNo, Count, DeviceNo);
}

/* Fetching the double-element(s) from a ADwin-System FIFO						  */
/* ============================================================================== */
short GetFifo_Double(short FifoNo, double Data[], long Count)
{
    short dllLoadError;
    if ((dllLoadError = (short)LoadDLLIfNeeded())!=0)
        return dllLoadError;
	
	return (*Get_Fifo_Ptr)(Data, 6, FifoNo, Count, DeviceNo);
}

/* Fetching the float-element(s) from a ADwin-System FIFO						  */
/* ============================================================================== */
short GetfFifo(short FifoNo, float Data[], long Count)
{
    short dllLoadError;
    if ((dllLoadError = (short)LoadDLLIfNeeded())!=0)
        return dllLoadError;
	
	return (*Get_Fifo_Ptr)(Data, 5, FifoNo, Count, DeviceNo);
}

/* Fetching the float-element(s) from a ADwin-System FIFO						  */
/* ============================================================================== */
short GetFifo_Float(short FifoNo, float Data[], long Count)
{
    short dllLoadError;
    if ((dllLoadError = (short)LoadDLLIfNeeded())!=0)
        return dllLoadError;
	
	return (*Get_Fifo_Ptr)(Data, 5, FifoNo, Count, DeviceNo);
}

/* Sets FIFO-element(s) of ADwin-System from short-array						  */
/* ============================================================================== */
short SetFifo(short FifoNo, short Data[], long Count)
{
    short dllLoadError;
    if ((dllLoadError = (short)LoadDLLIfNeeded())!=0)
        return dllLoadError;
	
	return (*Set_Fifo_Ptr)(Data, 1, FifoNo, Count, DeviceNo);
}

/* Sets FIFO-element(s) of ADwin-System from long-array							  */
/* ============================================================================== */
short SetlFifo(short FifoNo, long Data[], long Count)
{
    short dllLoadError;
    if ((dllLoadError = (short)LoadDLLIfNeeded())!=0)
        return dllLoadError;

	return (*Set_Fifo_Ptr)(Data, 2, FifoNo, Count, DeviceNo);
}

/* Sets FIFO-element(s) of ADwin-System from long-array							  */
/* ============================================================================== */
short SetFifo_Long(short FifoNo, long Data[], long Count)
{
    short dllLoadError;
    if ((dllLoadError = (short)LoadDLLIfNeeded())!=0)
        return dllLoadError;

	return (*Set_Fifo_Ptr)(Data, 2, FifoNo, Count, DeviceNo);
}

/* Sets FIFO-element(s) of ADwin-System from double-array						  */
/* ============================================================================== */
short SetFifo_Double(short FifoNo, double Data[], long Count)
{
    short dllLoadError;
    if ((dllLoadError = (short)LoadDLLIfNeeded())!=0)
        return dllLoadError;

	return (*Set_Fifo_Ptr)(Data, 6, FifoNo, Count, DeviceNo);
}

/* Sets FIFO-element(s) of ADwin-System from float-array						  */
/* ============================================================================== */
short SetfFifo(short FifoNo, float Data[], long Count)
{
    short dllLoadError;
    if ((dllLoadError = (short)LoadDLLIfNeeded())!=0)
        return dllLoadError;

	return (*Set_Fifo_Ptr)(Data, 5, FifoNo, Count, DeviceNo);
}

/* Sets FIFO-element(s) of ADwin-System from float-array						  */
/* ============================================================================== */
short SetFifo_Float(short FifoNo, float Data[], long Count)
{
    short dllLoadError;
    if ((dllLoadError = (short)LoadDLLIfNeeded())!=0)
        return dllLoadError;
	
	return (*Set_Fifo_Ptr)(Data, 5, FifoNo, Count, DeviceNo);
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

/* Returns value of free memory from the ADwin-2, -4, -5 ,-8 and -9 System(in bytes) */
/* ================================================================================= */
long Free_Mem(short Mem_Spec)
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
    short		dllLoadError;

    if ((dllLoadError = (short)LoadDLLIfNeeded())!=0)
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

/* Gets all 80 float parameters (FPar_1 - FPar_80) into a double array			  */
/* ============================================================================== */
short Get_FPar_All_Double(double Array[])
{	
	short res;
	float fArray[80];
    short dllLoadError, i;
    if ((dllLoadError = (short)LoadDLLIfNeeded())!=0)
        return dllLoadError;
    
	res = (*Get_ADBFPar_All_Ptr)(1, 80, fArray, DeviceNo);
	
	for (i=0; i<80; i++)
	{
		Array[i] = fArray[i];
	}

	return res; 
}

/* Gets a block of ADwin float parameters into a double array					  */
/* ============================================================================== */
short Get_FPar_Block_Double(double Array[], short Startindex, short Count)
{	
	short res;
	float fArray[80];
    short dllLoadError, i;
    if ((dllLoadError = (short)LoadDLLIfNeeded())!=0)
        return dllLoadError;
    
	res = (*Get_ADBFPar_All_Ptr)(Startindex, Count, fArray, DeviceNo);
	
	for (i=0; i<Count; i++)
	{
		Array[i] = fArray[i];
	}

	return res; 
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
long Set_Globaldelay(long ProcessNo, long Globaldelay)
{
    long dllLoadError;
    if ((dllLoadError = (long)LoadDLLIfNeeded())!=0)
        return dllLoadError;
	
	return Set_Par((short)(-90+ProcessNo), Globaldelay);
}

/* Sets a globaldelay for a specified prozess on the ADwin-System				  */
/* ============================================================================== */
long Get_Globaldelay(long ProcessNo)
{
    long dllLoadError;
    if ((dllLoadError = (long)LoadDLLIfNeeded())!=0)
        return dllLoadError;
	
	return Get_Par((short)(-90+ProcessNo));
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
    if ((dllLoadError = (long)LoadDLLIfNeeded())!=0)
        return dllLoadError;

	return (*GetData_Packed_Ptr)(Data, 1, (short)DataNo, Startindex, Count, DeviceNo);
}

/* Returns array-element(s) of long-type from packed ADwin-System-array (DATA_"DataNo") */
/* ============================================================================== */
short GetData_Packed_Long(long DataNo, long *Data, long Startindex, long Count)
{
    short dllLoadError;
    if ((dllLoadError = (long)LoadDLLIfNeeded())!=0)
        return dllLoadError;

	return (*GetData_Packed_Ptr)(Data, 2, (short)DataNo, Startindex, Count, DeviceNo);
}

/* Returns array-element(s) of float-type from packed ADwin-System-array (DATA_"DataNo") */
/* ============================================================================== */
short GetData_Packed_Float(long DataNo, float *Data, long Startindex, long Count)
{
    short dllLoadError;
    if ((dllLoadError = (long)LoadDLLIfNeeded())!=0)
        return dllLoadError;

	return (*GetData_Packed_Ptr)(Data, 5, (short)DataNo, Startindex, Count, DeviceNo);
}

/* Returns array-element(s) of double-type from packed ADwin-System-array (DATA_"DataNo") */
/* ============================================================================== */
short GetData_Packed_Double(long DataNo, double *Data, long Startindex, long Count)
{
    short dllLoadError;
    if ((dllLoadError = (long)LoadDLLIfNeeded())!=0)
        return dllLoadError;

	return (*GetData_Packed_Ptr)(Data, 6, (short)DataNo, Startindex, Count, DeviceNo);
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

	return (Set_List_Ptr(Value, 1, Channel, Count, DeviceNo));
}

/* Eine Messwertliste abfragen und in ein short Array schreiben					  */
/* ============================================================================== */
short ReadADListe(short Channel, long Count, short Value[])
{
    short dllLoadError;
    if ((dllLoadError = (short)LoadDLLIfNeeded())!=0)
        return dllLoadError;

	return (Get_List_Ptr(Value, 1, Channel, 1, Count, DeviceNo));
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

	return (*Get_Fifo_Ptr)(Data, 2, FifoNo, Count, DeviceNo);
}

/* Returns Fifo-element(s) of long-type from packed ADwin-Fifo-array (DATA_"DataNo") */
/* ============================================================================== */
short GetFifo_Packed_Long(short FifoNo, long Data[], long Count)
{
    short dllLoadError;
    if ((dllLoadError = (short)LoadDLLIfNeeded())!=0)
        return dllLoadError;

	return (*Get_Fifo_Ptr)(Data, 2, FifoNo, Count, DeviceNo);
}

// End of file


