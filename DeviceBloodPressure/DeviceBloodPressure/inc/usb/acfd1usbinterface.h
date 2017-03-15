//////////////////////////////////////////////////////////////////////////
/// @par edan co. All Rights Reserved, Copyright (C) 2016-2020.
/// @brief   [serial com read/write]
///
/// @file    serialcominterface.h
/// @author  zhangxianhui
/// @date    2016-12-23
/// @version V1.0
/// @note author date modify
/// @note zhangxianhui 2016-12-23 create
/// @note zhangxianhui 2016-12-23 modify
//////////////////////////////////////////////////////////////////////////
#ifndef ACFD1USB_INTERFACE_H
#define ACFD1USB_INTERFACE_H
//#include <iostream>



#ifdef LIBUSBEXPORT                                                 
#define USBEXPORT extern "C" __declspec(dllexport)
#else
#define USBEXPORT extern "C" __declspec(dllimport)
#endif


#define INOUT                    
#define IN                       
#define OUT 


typedef void *HANDLE;
typedef bool USBBOOL;
typedef unsigned char USBBYTE;
typedef unsigned int USBUINT;
typedef int  USBINT;
typedef char  USBCHAR;
typedef float  USBFLOAT;
typedef double   USBDOUBLE;
typedef unsigned short USBWORD;
typedef unsigned int USBUINT;
typedef unsigned long USBDWORD;



#define  USB_LEN							64
//string length
#define USB_STRINF_LEN						128
//string max length
#define MAX_USB_STRINF_LEN					256



typedef enum USBRES
{
	USB_SUCCESS,				//operation success	
	USB_EXCEPTION,				//operation exception
	USB_FAIL,					//fail
	USB_PARSE_JSON_ERROR,		//parse json error	
	USB_PARAM_ERROR,            //param error	
	USB_NO_MEMORY,				//no memory
	USB_NOT_FOUND_DEVICE,		//not find	
	USB_NOT_CONNECT				//not connect
};



//USBEXPORT USBBOOL OpenUsb();
USBEXPORT USBRES Acfd1FindUsbDevice(OUT USBCHAR **pData);
USBEXPORT USBRES Acfd1OpenUsb(IN USBWORD wVID, IN USBWORD wPID);
USBEXPORT USBRES Acfd1CloseUsb();
USBEXPORT USBRES Acfd1ReadData(OUT USBCHAR **pData, OUT USBUINT &nSize);
USBEXPORT USBRES Acfd1ReadDataRef(OUT USBCHAR *&pData, OUT USBUINT &nSize);
USBEXPORT USBRES Acfd1WriteData(IN USBCHAR *pData, IN USBUINT nSize);
USBEXPORT USBRES Acfd1FreeMemory(IN USBCHAR *&pData);
















#endif