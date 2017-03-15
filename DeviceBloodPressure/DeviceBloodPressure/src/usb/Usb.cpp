#include "StdAfx.h"
#include "Usb.h"

static const GUID GUID_KEYBOARD_MOUSE[] = 
{
	// USB人体学输入设备
	{ 0x745A17A0, 0x74D3, 0x11D0, { 0xB6, 0xFE, 0x00, 0xA0, 0xC9, 0x0F, 0x57, 0xDA } },
	// 键盘
	{ 0x4D36E96B, 0xE325, 0x11CE, { 0xBF, 0xC1, 0x08, 0x00, 0x2B, 0xE1, 0x03, 0x18 } }
};




CUsb::CUsb(void):m_hUsb(INVALID_HANDLE_VALUE)
{
}


CUsb::~CUsb(void)
{
	if (INVALID_HANDLE_VALUE != m_hUsb)
	{
		CloseHandle(m_hUsb);  
		m_hUsb = INVALID_HANDLE_VALUE;  
	}
}


bool CUsb::FindUsbDevice(USBDEVICEINFOVEC &vec)
{
	BOOL bRet = FALSE;  
	GUID hidGuid;  // 设备GUID标识
	HDEVINFO hardwareDeviceInfo;  //设备结构数组指针
	SP_INTERFACE_DEVICE_DATA deviceInfoData;  
	//SP_DEVINFO_DATA DeviceInfoData
	PSP_INTERFACE_DEVICE_DETAIL_DATA functionClassDeviceData = NULL;  
	ULONG predictedLength = 0;  
	ULONG requiredLength = 0;  

	deviceInfoData.cbSize = sizeof(SP_INTERFACE_DEVICE_DATA);  
	//1.：查找本系统中HID类的GUID标识
	HidD_GetHidGuid(&hidGuid);  
	//2.：获取设备信息
	hardwareDeviceInfo = SetupDiGetClassDevs(&hidGuid, NULL,NULL, (DIGCF_PRESENT | DIGCF_DEVICEINTERFACE));  
	if(INVALID_HANDLE_VALUE == hardwareDeviceInfo)
	{
		return false;
	}

	for (int i = 0; i < 128; i++)  
	{  
		// 枚举符合该GUID的设备接口
		if (!SetupDiEnumDeviceInterfaces(hardwareDeviceInfo, 0,&hidGuid, i, &deviceInfoData)) continue;  //接口i索引值 以0为起始位置
	/*	if(deviceInfoData.InterfaceClassGuid != GUID_KEYBOARD_MOUSE[0]
			&& deviceInfoData.InterfaceClassGuid != GUID_KEYBOARD_MOUSE[1])
		{*/
	
		// 取得该设备接口的细节(设备路径)
		SetupDiGetDeviceInterfaceDetail(hardwareDeviceInfo, &deviceInfoData,NULL, 0, &requiredLength, NULL);  
		predictedLength = requiredLength;  
		// 申请设备接口数据空间
		functionClassDeviceData = (PSP_INTERFACE_DEVICE_DETAIL_DATA)malloc(predictedLength);  
		if (!functionClassDeviceData) continue;  
		functionClassDeviceData->cbSize = sizeof(SP_INTERFACE_DEVICE_DETAIL_DATA);  
		if (!SetupDiGetDeviceInterfaceDetail(hardwareDeviceInfo,&deviceInfoData, functionClassDeviceData, predictedLength,&requiredLength, NULL)) 
		{
			free(functionClassDeviceData);
			break;  
		}

		////打开 启动设备
		HANDLE hUsb = CreateFile(functionClassDeviceData->DevicePath,GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL,OPEN_EXISTING, 0, NULL);  
		//HANDLE hUsb = CreateFile(functionClassDeviceData->DevicePath,GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL,OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);  
		if (hUsb != INVALID_HANDLE_VALUE)  
		{  
			HIDD_ATTRIBUTES attri;  
			if (!HidD_GetAttributes(hUsb, &attri))
			{
				CloseHandle(hUsb);
			}
			
			//std::string szDevicePathName = functionClassDeviceData->DevicePath;
			//GetDeviceCapabilities();

			PHIDP_PREPARSED_DATA	PreparsedData;
			HidD_GetPreparsedData(hUsb, &PreparsedData);
			HIDP_CAPS Capabilities;
			HidP_GetCaps(PreparsedData, &Capabilities);

			//if (1 == Capabilities.Usage)
			//{
				USBDEVICEINFO devinfo = { 0 };
				devinfo.wVID = attri.VendorID;
				devinfo.wPID = attri.ProductID;
				vec.push_back(devinfo);

			//}

			HidD_FreePreparsedData(PreparsedData);

			bRet = TRUE;
			CloseHandle(hUsb);  
			hUsb == INVALID_HANDLE_VALUE;

			free(functionClassDeviceData);
			//break; 
		}  	
		//}
	}  
	SetupDiDestroyDeviceInfoList(hardwareDeviceInfo);  

	return bRet; 
}

bool CUsb::OpenUsb()
{
	BOOL bRet = FALSE;  
	GUID hidGuid;  // 设备GUID标识
	HDEVINFO hardwareDeviceInfo;  //设备结构数组指针
	SP_INTERFACE_DEVICE_DATA deviceInfoData;  
	PSP_INTERFACE_DEVICE_DETAIL_DATA functionClassDeviceData = NULL;  
	ULONG predictedLength = 0;  
	ULONG requiredLength = 0;  
	if (m_hUsb != INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_hUsb);  
		m_hUsb == INVALID_HANDLE_VALUE;
	}
	deviceInfoData.cbSize = sizeof(SP_INTERFACE_DEVICE_DATA);  
	//1.：查找本系统中HID类的GUID标识
	HidD_GetHidGuid(&hidGuid);  
	//2.：获取设备信息
	hardwareDeviceInfo = SetupDiGetClassDevs(&hidGuid, NULL,NULL, (DIGCF_PRESENT | DIGCF_DEVICEINTERFACE));  
	if(INVALID_HANDLE_VALUE == hardwareDeviceInfo)
	{
		return false;
	}

	for (int i = 0; i < 128; i++)  
	{  
		// 枚举符合该GUID的设备接口
		if (!SetupDiEnumDeviceInterfaces(hardwareDeviceInfo, 0,&hidGuid, i, &deviceInfoData)) continue;  //接口i索引值 以0为起始位置
		// 取得该设备接口的细节(设备路径)
		SetupDiGetDeviceInterfaceDetail(hardwareDeviceInfo, &deviceInfoData,NULL, 0, &requiredLength, NULL);  
		predictedLength = requiredLength;  
		// 申请设备接口数据空间
		functionClassDeviceData = (PSP_INTERFACE_DEVICE_DETAIL_DATA)malloc(predictedLength);  
		if (!functionClassDeviceData) continue;  
		functionClassDeviceData->cbSize = sizeof(SP_INTERFACE_DEVICE_DETAIL_DATA);  
		if (!SetupDiGetDeviceInterfaceDetail(hardwareDeviceInfo,&deviceInfoData, functionClassDeviceData, predictedLength,&requiredLength, NULL)) 
		{
			free(functionClassDeviceData);
			break;  
		}

		////打开 启动设备
		//m_hUsb = CreateFile(functionClassDeviceData->DevicePath,GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL,OPEN_EXISTING, 0, NULL);  
		m_hUsb = CreateFile(functionClassDeviceData->DevicePath,GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL,OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);  
		if (m_hUsb != INVALID_HANDLE_VALUE)  
		{  
			HIDD_ATTRIBUTES attri;  
			if (!HidD_GetAttributes(m_hUsb, &attri))
			{
				CloseHandle(m_hUsb);
			}

			m_szDevicePathName = functionClassDeviceData->DevicePath;
			 
			GetDeviceCapabilities();

			bRet = TRUE;
			free(functionClassDeviceData);
			break; 
		}  	
	}  
	SetupDiDestroyDeviceInfoList(hardwareDeviceInfo);  

	return bRet;  
}


bool CUsb::OpenUsb(int wVID, int wPID)
{
	BOOL bRet = FALSE;  
	GUID hidGuid;					// 设备GUID标识
	HDEVINFO hardwareDeviceInfo;	//设备结构数组指针
	SP_INTERFACE_DEVICE_DATA deviceInfoData;  
	PSP_INTERFACE_DEVICE_DETAIL_DATA functionClassDeviceData = NULL;  
	ULONG predictedLength = 0;  
	ULONG requiredLength = 0;

	if (m_hUsb != INVALID_HANDLE_VALUE)
	{
		CloseHandle(m_hUsb);  
		m_hUsb == INVALID_HANDLE_VALUE;
	}
		  
	deviceInfoData.cbSize = sizeof(SP_INTERFACE_DEVICE_DATA);  
	//1.：查找本系统中HID类的GUID标识
	HidD_GetHidGuid(&hidGuid);  
	 //2.：获取设备信息
	hardwareDeviceInfo = SetupDiGetClassDevs(&hidGuid, NULL,NULL, (DIGCF_PRESENT | DIGCF_DEVICEINTERFACE));  
	if(INVALID_HANDLE_VALUE == hardwareDeviceInfo)
	{
		return false;
	}
	
	for (int i = 0; i < 128; i++)  
	{  
		 // 枚举符合该GUID的设备接口
		if (!SetupDiEnumDeviceInterfaces(hardwareDeviceInfo, 0,&hidGuid, i, &deviceInfoData)) continue;  //接口i索引值 以0为起始位置
		// 取得该设备接口的细节(设备路径)
		SetupDiGetDeviceInterfaceDetail(hardwareDeviceInfo, &deviceInfoData,NULL, 0, &requiredLength, NULL);  
		predictedLength = requiredLength;  
		// 申请设备接口数据空间
		functionClassDeviceData = (PSP_INTERFACE_DEVICE_DETAIL_DATA)malloc(predictedLength);  
		if (!functionClassDeviceData) continue;  
		functionClassDeviceData->cbSize = sizeof(SP_INTERFACE_DEVICE_DETAIL_DATA);  
		if (!SetupDiGetDeviceInterfaceDetail(hardwareDeviceInfo,&deviceInfoData, functionClassDeviceData, predictedLength,&requiredLength, NULL)) 
		{
			free(functionClassDeviceData);
			break;  
		}

		////打开 启动设备
		//m_hUsb = CreateFile(functionClassDeviceData->DevicePath,GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, NULL);  
		m_hUsb = CreateFile(functionClassDeviceData->DevicePath,GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL,OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);  
		if (m_hUsb != INVALID_HANDLE_VALUE)  
		{  
			HIDD_ATTRIBUTES attri;  
			if (!HidD_GetAttributes(m_hUsb, &attri))
			{
				CloseHandle(m_hUsb);
			}
			 
			if ((attri.VendorID == wVID) && (attri.ProductID == wPID))  
			{  
				bRet = TRUE;
				GetDeviceCapabilities();
				free(functionClassDeviceData);
				break;  
			}  
			//CloseHandle(handle);  
			//handle = INVALID_HANDLE_VALUE;  
		}  	
	}  
	SetupDiDestroyDeviceInfoList(hardwareDeviceInfo);  
	return bRet;  
}

void CUsb::GetDeviceCapabilities()
{
	PHIDP_PREPARSED_DATA	PreparsedData;
	HidD_GetPreparsedData(m_hUsb, &PreparsedData);
	HidP_GetCaps(PreparsedData, &m_Capabilities);

	HidD_FreePreparsedData(PreparsedData);
}


bool CUsb::ReadFromUsb(membuffer *prxBuf)
{
	BYTE rBuffer[128] = {0};
	DWORD dwRet = 0;
	BOOL fRes;
	BOOL bRet;

	DWORD dwRead;
	BOOL fWaitingOnRead = FALSE;
	OVERLAPPED osReader = {0};

	USHORT nInReportLen = m_Capabilities.InputReportByteLength;

	osReader.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (osReader.hEvent == NULL)
	{
		return FALSE;
	}
	//ResetEvent(osReader.hEvent);

	if (!fWaitingOnRead) 
	{
		// Issue read operation.
		#define READ_TIMEOUT      500      // milliseconds
		//DWORD dwRes = WaitForSingleObject(osReader.hEvent, READ_TIMEOUT);
		DWORD dwRes = WaitSignalTimeOut(READ_TIMEOUT, osReader);
		do 
		{	
			bRet = ReadFile(m_hUsb, rBuffer, nInReportLen, &dwRet, &osReader);
			if (!bRet) 
			{
				DWORD dRes = GetLastError();
				if (dRes != ERROR_IO_PENDING)     // read not delayed?
				{
					// Error in communications; report it.
					return false;
				}
				else
				{
					fWaitingOnRead = TRUE;
				}
			}
			else 
			{    
				// read completed immediately		
				int nLen = strlen((char*)&rBuffer[1]);
				membuffer_append(prxBuf, &rBuffer[1], nLen);
			}
		} while (bRet && nInReportLen == dwRet);
	}

	//timeout read
	if (fWaitingOnRead) 	
	{
#define READ_TIMEOUT      500      // milliseconds
			DWORD dwRes = WaitForSingleObject(osReader.hEvent, READ_TIMEOUT);
			//ResetEvent(osReader.hEvent);
			switch(dwRes)
			{
				// Read completed.
			case WAIT_OBJECT_0:
				if (!GetOverlappedResult(m_hUsb, &osReader, &dwRead, FALSE))
				//if (!GetOverlappedResult(m_hUsb, &osReader, &dwRead, TRUE))
				{
					// Error in communications; report it.
					return false;
				}
				else
				{
					// Read completed successfully.									
					int nLen = strlen((char*)&rBuffer[1]);
					membuffer_append(prxBuf, &rBuffer[1], nLen);
				}
				//  Reset flag so that another opertion can be issued.
				fWaitingOnRead = FALSE;
				break;
			case WAIT_TIMEOUT:
				// Operation isn't complete yet. fWaitingOnRead flag isn't
				// changed since I'll loop back around, and I don't want
				// to issue another read until the first one finishes.
				//
				// This is a good time to do some background work.
				fRes = CancelIo(m_hUsb);
				break;  
			case WAIT_FAILED:
				fRes = FALSE;
				break;
			default:
				// Error in the WaitForSingleObject; abort.
				// This indicates a problem with the OVERLAPPED structure's
				// event handle.
				break;
			}
	}

	//ResetEvent(osReader.hEvent);
	CloseHandle(osReader.hEvent);
	osReader.hEvent = INVALID_HANDLE_VALUE;
	return true;
}

bool CUsb::WriteToUsb(const char *lpBuffer, unsigned int dwSize)
{
	BYTE wBuffer[128] = { 0 };
	DWORD dwRet = 0;
	BOOL bRet;
	
	OVERLAPPED osWrite = {0};
	DWORD dwWritten = 0;
	BOOL fRes;

	USHORT nOutReportLen = m_Capabilities.OutputReportByteLength;


	// Create this writes OVERLAPPED structure hEvent.
	osWrite.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (osWrite.hEvent == NULL)
		// Error creating overlapped event handle.
		return FALSE;

	//ResetEvent(osWrite.hEvent);

	// Issue write.
	memcpy(&wBuffer[1], lpBuffer, dwSize);
	if (!WriteFile(m_hUsb, wBuffer, nOutReportLen, &dwWritten, &osWrite)) 
	{
		DWORD dRes = GetLastError();
		if (dRes != ERROR_IO_PENDING) 
		{ 
			// WriteFile failed, but it isn't delayed. Report error and abort.
			fRes = FALSE;
		}
		else 
		{
			// Write is pending.
#define WRITE_TIMEOUT 1000 //write timeout
			DWORD dwRes = WaitForSingleObject(osWrite.hEvent, WRITE_TIMEOUT);
			//ResetEvent(osWrite.hEvent);
			switch(dwRes)
			{
				// Read completed.
			case WAIT_OBJECT_0:
				if (!GetOverlappedResult(m_hUsb, &osWrite, &dwWritten, TRUE))
					fRes = FALSE;
				else
					// Write operation completed successfully.
					fRes = TRUE;
				break;
			case WAIT_TIMEOUT:
				// This is a good time to do some background work.
				fRes = CancelIo(m_hUsb);
				break;  
			case WAIT_FAILED:
				fRes = FALSE;
				break;
			default:
				// Error in the WaitForSingleObject; abort.
				// This indicates a problem with the OVERLAPPED structure's
				// event handle.
				break;
			}
		}
	}
	else
	{
		// WriteFile completed immediately.
		fRes = TRUE;
	}

	//ResetEvent(osWrite.hEvent);
	CloseHandle(osWrite.hEvent);
	osWrite.hEvent = INVALID_HANDLE_VALUE;
	return fRes;
}

bool CUsb::WaitSignalTimeOut(int nTimeOut, OVERLAPPED os)
{
	BOOL bRet;
	BOOL fRes;
	DWORD dwBytes = 0;

	DWORD dwRes = WaitForSingleObject(os.hEvent, nTimeOut);
	//ResetEvent(osWrite.hEvent);
	switch(dwRes)
	{
		// Read completed.
	case WAIT_OBJECT_0:
		if (!GetOverlappedResult(m_hUsb, &os, &dwBytes, TRUE))
			fRes = FALSE;
		else
			// Write operation completed successfully.
			fRes = TRUE;
		break;
	case WAIT_TIMEOUT:
		// This is a good time to do some background work.
		fRes = CancelIo(m_hUsb);
		//fRes = TRUE;
		break; 
	case WAIT_FAILED:
		fRes = FALSE;
		break;
	default:
		// Error in the WaitForSingleObject; abort.
		// This indicates a problem with the OVERLAPPED structure's
		// event handle.
		break;
	}
	return fRes;
}

bool CUsb::CloseUsb()
{
	if (INVALID_HANDLE_VALUE != m_hUsb)
	{
		CloseHandle(m_hUsb);
		m_hUsb = INVALID_HANDLE_VALUE;
	}
	return true;
}







