#include "StdAfx.h"
#include "Acfd1Usb.h"
#include "membuffer.h"
#include <time.h>



CAcfd1Usb *CAcfd1Usb::m_gpAcfd1Usb = NULL;


CAcfd1Usb::CAcfd1Usb(void)
{
	InitializeCriticalSection(&m_csMutex);
}


CAcfd1Usb::~CAcfd1Usb(void)
{
	if (NULL != m_gpAcfd1Usb)
	{
		delete m_gpAcfd1Usb;
		m_gpAcfd1Usb = NULL;
	}
	DeleteCriticalSection(&m_csMutex);
}


CAcfd1Usb *CAcfd1Usb::GetInstance()
{
	if (NULL == m_gpAcfd1Usb)
	{
		m_gpAcfd1Usb = new CAcfd1Usb();
	}
	return m_gpAcfd1Usb;
}

static bool bInitLog = false;
void CAcfd1Usb::InitLog()
{
	/*SharedAppenderPtr pFileAppender(new FileAppender(_T("ADO3ADll.log"), \
	std::ios::app | std::ios::in, \
	true, true)); */
	SharedAppenderPtr  pFileAppender(new  RollingFileAppender("Acfd1Usb.log", 10*1024*1024, 1, true, true));
	pFileAppender->setName("filelibAcfd1Usb");
	std::auto_ptr<Layout> pPatternLayout(new PatternLayout("%D{%Y-%m-%d %H:%M:%S}    - %m [%l]%n"));   
	pFileAppender->setLayout(pPatternLayout);   
	m_Logger = Logger::getInstance("LoggerName");   	
	m_Logger.addAppender(pFileAppender);  
}

USBRES CAcfd1Usb::FindUsbDevice(OUT USBCHAR **pData)
{
	USBDEVICEINFOVEC vec;
	if (m_Usb.FindUsbDevice(vec))
	{
		cJSON *pRoot = cJSON_CreateObject();
		if(NULL == pRoot) return USB_NO_MEMORY;	
		
		cJSON *pArray = cJSON_CreateArray();
		if(NULL == pArray) return USB_NO_MEMORY;	

		cJSON_AddItemToObject(pRoot, "UsbDeviceList", pArray);
		for (int i = 0; i < vec.size(); i++)
		{
			cJSON *pUsbDevice = cJSON_CreateObject();
			USBDEVICEINFO devinfo = vec[i];
			USBCHAR szBuf[USB_LEN] = { 0 };
			sprintf_s(szBuf, sizeof(szBuf), "%d", devinfo.wVID);
			cJSON_AddStringToObject(pUsbDevice, "wVID", szBuf);

			memset(szBuf, 0, sizeof(szBuf));
			sprintf_s(szBuf, sizeof(szBuf), "%d", devinfo.wPID);
			cJSON_AddStringToObject(pUsbDevice, "wPID", szBuf);
			cJSON_AddItemToArray(pArray,pUsbDevice);
		}

		char *p = cJSON_Print(pRoot);		
		if(NULL == p) return USB_NO_MEMORY;

		int nLen = strlen(p);
		*pData = new USBCHAR[nLen + 1];
		*(*pData + nLen) = '\0';
		memcpy(*pData, p, nLen);

		free(p);
		p = NULL;

		cJSON_Delete(pRoot);
	}
	return USB_SUCCESS;
}

USBRES CAcfd1Usb::OpenUsb(USBWORD wVID, USBWORD wPID)
{
	if (!bInitLog)
	{
		bInitLog = true;
		InitLog();
	}

	bool bRet = m_Usb.OpenUsb(wVID, wPID);
	if (!bRet)
	{
		LOG4CPLUS_ERROR(m_Logger, "CAcfd1Usb::OpenUsb error " \
			<<" wVID="<<wVID <<" wPID=" << wPID); 
		return USB_FAIL;
	}
	LOG4CPLUS_INFO(m_Logger, "CAcfd1Usb::OpenUsb success " \
		<<" wVID="<<wVID <<" wPID=" << wPID); 
	return USB_SUCCESS;
}

USBRES CAcfd1Usb::CloseUsb()
{
	if (!m_Usb.CloseUsb())
	{
		return USB_FAIL;
	}
	return USB_SUCCESS;
}


USBRES CAcfd1Usb::WriteData(IN const USBCHAR *pData, IN USBUINT nSize)
{		
	if (NULL == pData) return USB_PARAM_ERROR;

	USBCHAR szCmd[USB_LEN] = { 0 };
	USBRES usbres = ParseJsonCmd(pData, szCmd);
	if (USB_SUCCESS != usbres)
	{
		return usbres;
	}

	usbres = HandleJsonCmd(pData, szCmd);
	if (USB_SUCCESS != usbres)
	{
		return usbres;
	}
	return usbres;

	//usbres = ParseJsonData(pData, nSize);

	////bool bRet = m_Usb.WriteToUsb((USBCHAR*)pData, strlen(pData));
	//bool bRet = m_Usb.WriteToUsb((USBCHAR*)pData, nSize);
	//if (!bRet)
	//{
	//	LOG4CPLUS_ERROR(m_Logger, "CAcfd1Usb::WriteData error" \
	//		<<" write data="<<pData); 
	//	return USB_FAIL;
	//}

	//LOG4CPLUS_INFO(m_Logger, "CAcfd1Usb::WriteData success" \
	//	<<" write data="<<pData); 
	//return USB_SUCCESS;
	
	/*const USBCHAR *cursor = pData;
	parse_status_t status;
	memptr token;
	token_type_t tok_type;

	USBCHAR szCmd[USB_LEN];
	USBUINT nCmdLen = 0;
	usbres = ParseCmd(szCmd, nCmdLen, cursor);

	cursor += nCmdLen;
	usbres = WriteCmd(szCmd, cursor);

	return usbres;*/
}

USBRES CAcfd1Usb::ParseJsonCmd(IN const USBCHAR *pData, OUT USBCHAR *pCmd)
{
	if (NULL == pData) return USB_PARAM_ERROR;

	cJSON *pRoot = cJSON_Parse(pData);
	if( NULL == pRoot ) return USB_PARSE_JSON_ERROR;
	cJSON *pObj = cJSON_GetObjectItem(pRoot, "Cmd");
	if(NULL == pObj) return USB_PARSE_JSON_ERROR;

	sprintf_s(pCmd, USB_LEN, "%s", pObj->valuestring);

	cJSON_Delete(pRoot);
	return USB_SUCCESS;
}

USBRES CAcfd1Usb::HandleJsonCmd(IN const USBCHAR *pData, IN const USBCHAR *pCmd)
{
	USBRES usbres;
	if (0 == strnicmp(pCmd, "HELP", strlen("HELP")))
	{		
		usbres = Help();
		if (USB_SUCCESS != usbres)
		{
			return usbres;
		}
	}
	else if (0 == strnicmp(pCmd, "SET", strlen("SET")))
	{
		usbres = HandleSetCmd(pData, pCmd);
		if (USB_SUCCESS != usbres)
		{
			return usbres;
		}
	}
	else if (0 == strnicmp(pCmd, "RSET", strlen("RSET")))
	{
		usbres = ReadRSet();
		if (USB_SUCCESS != usbres)
		{
			return usbres;
		}
	}
	else if (0 == strnicmp(pCmd, "SDT", strlen("SDT")))
	{
	}
	else if (0 == strnicmp(pCmd, "ST", strlen("ST")))
	{
	}
	else if (0 == strnicmp(pCmd, "RDA", strlen("RDA")))
	{
	}
	else if (0 == strnicmp(pCmd, "RDF", strlen("RDF")))
	{
	}
	else if (0 == strnicmp(pCmd, "RRCD", strlen("RRCD")))
	{
		usbres = ReadRecord();
		if (USB_SUCCESS != usbres)
		{
			return usbres;
		}
	}
	else if (0 == strnicmp(pCmd, "VER", strlen("VER")))
	{
	}
	else if (0 == strnicmp(pCmd, "CLR", strlen("CLR")))
	{
		usbres = ClearRecord();
		if (USB_SUCCESS != usbres)
		{
			return usbres;
		}

		//read
		USBCHAR *pData = NULL;
		USBUINT nLen = 0;
		USBRES usbres = ReadData(&pData, nLen);
		if (USB_SUCCESS != usbres)
		{
			return usbres;
		}
		delete[] pData;
		pData = NULL;
	}
	else
	{
		return USB_FAIL;
	}

	return USB_SUCCESS;
}

USBRES CAcfd1Usb::HandleSetCmd(IN const USBCHAR *pData, IN const USBCHAR *pCmd)
{
	USBRES usbres;
	SET set = { 0 };

	usbres = ParseJsonSetCmd(pData, set);
	if (USB_SUCCESS != usbres)
	{
		return usbres;
	}

	//mode
	if (0 == set.nMode)
	{

	}
	else if(1 == set.nMode)		//1——按时间表测量
	{
		//write
		usbres = SetId(set.nID);
		if (USB_SUCCESS != usbres)
		{
			return usbres;
		}

		usbres = SetAutoTable();
		if (USB_SUCCESS != usbres)
		{
			return usbres;
		}

		for (int i = 0; i < set.AutoTableModeVec.size(); i++)
		{
			AUTOTABLE autotable = { 0 };
			autotable = set.AutoTableModeVec[i];
			usbres = setAutoTableEntry(i, autotable.nHour, autotable.nMin, autotable.nInterval);
			if (USB_SUCCESS != usbres)
			{
				return usbres;
			}
		}

		usbres = SetDate(set.CurSysTime.nYear,set.CurSysTime.nMon,set.CurSysTime.nDay);
		if (USB_SUCCESS != usbres)
		{
			return usbres;
		}
		usbres = SetTime(set.CurSysTime.nHour, set.CurSysTime.nMin);
		if (USB_SUCCESS != usbres)
		{
			return usbres;
		}

	}
	else if(2 == set.nMode)		//2——自动
	{
		usbres = SetId(set.nID);
		if (USB_SUCCESS != usbres)
		{
			return usbres;
		}
		usbres = SetAuto();
		if (USB_SUCCESS != usbres)
		{
			return usbres;
		}
		
		usbres = SetDate(set.CurSysTime.nYear,set.CurSysTime.nMon,set.CurSysTime.nDay);
		if (USB_SUCCESS != usbres)
		{
			return usbres;
		}
		usbres = SetTime(set.CurSysTime.nHour, set.CurSysTime.nMin);
		if (USB_SUCCESS != usbres)
		{
			return usbres;
		}
	}
	else if(3 == set.nMode)		//3——手动时间间隔,自动测试
	{
		//1 set id
		usbres = SetId(set.nID);
		if (USB_SUCCESS != usbres)
		{
			return usbres;
		}	
		//2 set manual
		usbres = SetManual(set.HandleIntervalMode.nDay, set.HandleIntervalMode.nNight);
		if (USB_SUCCESS != usbres)
		{
			return usbres;
		}
		//3 set date
		usbres = SetDate(set.CurSysTime.nYear,set.CurSysTime.nMon,set.CurSysTime.nDay);
		if (USB_SUCCESS != usbres)
		{
			return usbres;
		}
		//4 set time
		usbres = SetTime(set.CurSysTime.nHour, set.CurSysTime.nMin);
		if (USB_SUCCESS != usbres)
		{
			return usbres;
		}
	}
	else if(4 == set.nMode)
	{
	}
	else
	{
		return USB_FAIL;
	}
	return USB_SUCCESS;
}

USBRES CAcfd1Usb::ReadData(USBCHAR **pData, USBUINT &nSize)
{
	nSize = 0;

	membuffer rxBuf;
	membuffer_init(&rxBuf);

	double start = GetTickCount();
	//read
	bool bRet = m_Usb.ReadFromUsb(&rxBuf);
	double  end = GetTickCount();
	std::cout << "m_Usb.ReadFromUsb() run time:" << end-start << std::endl;

	if (!bRet)
	{
		LOG4CPLUS_ERROR(m_Logger, "CAcfd1Usb::ReadData error" \
			<<" read data="<<rxBuf.buf); 
		return USB_FAIL;
	}
	//if (NULL == rxBuf.buf) return USB_FAIL; 


	membuffer data;
	membuffer_init(&data);

	start = GetTickCount();
	//parse rxbuf
	USBRES usbres = ParseRxBuf(&data, rxBuf.buf, rxBuf.length);
	end = GetTickCount();
	std::cout << "ParseRxBuf run time:" << end-start << std::endl;
	if (USB_SUCCESS != usbres)
	{
		return usbres;
	}

	*pData = new USBCHAR[data.length+1];
	*(*pData + data.length) = '\0';
	if (NULL != pData)
	{
		memcpy(*pData, data.buf, data.length);
	}
	nSize = data.length;

	membuffer_destroy(&rxBuf);
	membuffer_destroy(&data);

	return USB_SUCCESS;
}

USBRES CAcfd1Usb::ReadDataRef(OUT USBCHAR *&pData, OUT USBUINT &nSize)
{
	nSize = 0;

	membuffer rxBuf;
	membuffer_init(&rxBuf);

	//read
	bool bRet = m_Usb.ReadFromUsb(&rxBuf);
	if (!bRet)
	{
		LOG4CPLUS_ERROR(m_Logger, "CAcfd1Usb::ReadData error" \
			<<" read data="<<rxBuf.buf); 
		return USB_FAIL;
	}
	//if (NULL == rxBuf.buf) return USB_FAIL; 


	membuffer data;
	membuffer_init(&data);

	//parse rxbuf
	USBRES usbres = ParseRxBuf(&data, rxBuf.buf, rxBuf.length);
	if (USB_SUCCESS != usbres)
	{
		return usbres;
	}

	pData = new USBCHAR[data.length+1];
	*(pData + data.length) = '\0';
	if (NULL != pData)
	{
		memcpy(pData, data.buf, data.length);
	}
	nSize = data.length;

	membuffer_destroy(&rxBuf);
	membuffer_destroy(&data);

	return USB_SUCCESS;
}


USBRES CAcfd1Usb::SendSetCommand(std::string szCmd)
{
	//EnterCriticalSection(&m_csMutex);
	bool bRet = m_Usb.WriteToUsb((USBCHAR*)szCmd.c_str(), szCmd.size());
	//LeaveCriticalSection(&m_csMutex);
	if (!bRet)
	{
		LOG4CPLUS_ERROR(m_Logger, "CAcfd1Usb::SendSetCommand error" \
			<<" write data="<<szCmd.c_str()); 
		return USB_FAIL;
	}
	LOG4CPLUS_INFO(m_Logger, "CAcfd1Usb::SendSetCommand success" \
		<<" write data="<<szCmd.c_str()); 

	return USB_SUCCESS;
}

USBRES CAcfd1Usb::SendReadCommand(std::string szCmd)
{
	//EnterCriticalSection(&m_csMutex);
	bool bRet = m_Usb.WriteToUsb((USBCHAR*)szCmd.c_str(), szCmd.size());
	//LeaveCriticalSection(&m_csMutex);
	if (!bRet)
	{
		LOG4CPLUS_ERROR(m_Logger, "CAcfd1Usb::SendReadCommand error" \
			<<" write data="<<szCmd.c_str()); 	
		return USB_FAIL;
	}
	LOG4CPLUS_INFO(m_Logger, "CAcfd1Usb::SendReadCommand success" \
		<<" write data="<<szCmd.c_str()); 

	return USB_SUCCESS;
}

std::string CAcfd1Usb::GetCurrentCommad()
{


	return 0;
}

std::string CAcfd1Usb::Int2Hex(int value, int digit)
{
	USBCHAR szVal[USB_LEN] = { 0 };
	sprintf_s(szVal, sizeof(szVal), "%X", value);
	return szVal;
}

USBRES CAcfd1Usb::Help()
{
	return SendReadCommand("?\n\r");
}

USBRES CAcfd1Usb::ReadRSet()
{
	return SendReadCommand("rset\n\r");
}

USBRES CAcfd1Usb::ReadRecord()
{
	return SendReadCommand("rrcd\n\r");
}

USBRES CAcfd1Usb::ClearRecord()
{
	return SendReadCommand("clr\n\r");
}


USBRES CAcfd1Usb::Set(USBINT address, USBINT value)
{
	return SendSetCommand("set " + Int2Hex(address, 2) + " " + Int2Hex(value, 2) + "\n\r");
}

USBRES CAcfd1Usb::SetId(USBINT id)
{
	//write
	USBRES usbres = Set(0, (id & 0x00FF));
	if (USB_SUCCESS != usbres)
	{
		return usbres;
	}
	//read
	USBCHAR *pData = NULL;
	USBUINT nLen = 0;
	usbres = ReadData(&pData, nLen);
	if (USB_SUCCESS != usbres)
	{
		return usbres;
	}
	delete[] pData;
	pData = NULL;

	//write
	usbres = Set(1, (id & 0xFF00) >> 8);
	if (USB_SUCCESS != usbres)
	{
		return usbres;
	}

	//read
	nLen = 0;
	usbres = ReadData(&pData, nLen);
	if (USB_SUCCESS != usbres)
	{
		return usbres;
	}
	delete[] pData;
	pData = NULL;

	return usbres;
}

USBRES CAcfd1Usb::SetDate(USBUINT nYear, USBUINT nMon, USBUINT nDay)		//sdt
{
	USBRES usbres = SendSetCommand("sdt " + Int2Hex(nYear, 4) + " " + Int2Hex(nMon, 2) + " " + Int2Hex(nDay, 2) + "\n\r");
	if (USB_SUCCESS != usbres)
	{
		return usbres;
	}

	USBCHAR *pData = NULL;
	USBUINT nLen = 0;
	usbres = ReadData(&pData, nLen);
	if (USB_SUCCESS != usbres)
	{
		return usbres;
	}
	delete[] pData;
	pData = NULL;

	return USB_SUCCESS;
}

USBRES CAcfd1Usb::SetTime(USBUINT nHour, USBUINT nMin)			//st
{
	USBRES usbres = SendSetCommand("st " + Int2Hex(nHour, 2) + " " + Int2Hex(nMin, 2) + "\n\r");
	if (USB_SUCCESS != usbres)
	{
		return usbres;
	}

	USBCHAR *pData = NULL;
	USBUINT nLen = 0;
	usbres = ReadData(&pData, nLen);
	if (USB_SUCCESS != usbres)
	{
		return usbres;
	}
	delete[] pData;
	pData = NULL;

	return USB_SUCCESS;
}

USBRES CAcfd1Usb::SetEmpty(int address)
{
	return SendSetCommand("set " + Int2Hex(address, 2) + " " + " 0" );
}

USBRES CAcfd1Usb::SetAuto()
{
 // 2--自动，白天15分钟晚上30分钟测一次
	//write
	USBRES usbres = Set(2, 2);
	if (USB_SUCCESS != usbres)
	{
		return usbres;
	}
	//read
	USBCHAR *pData = NULL;
	USBUINT nLen = 0;
	usbres = ReadData(&pData, nLen);
	if (USB_SUCCESS != usbres)
	{
		return usbres;
	}
	delete[] pData;
	pData = NULL;

	return USB_SUCCESS;
}

USBRES CAcfd1Usb::SetManual(USBINT intervalDay, USBINT intervalNight)
{
	 // 3--手动时间间隔,自动测试
	//1、
	USBRES usbres = Set(3, intervalDay % 256);
	if (USB_SUCCESS != usbres)
	{
		return usbres;
	}
	USBCHAR *pData = NULL;
	USBUINT nLen = 0;
	usbres = ReadData(&pData, nLen);
	if (USB_SUCCESS != usbres)
	{
		return usbres;
	}
	delete[] pData;
	pData = NULL;

	//2、
	usbres = Set(4, intervalNight % 256);
	if (USB_SUCCESS != usbres)
	{
		return usbres;
	}
	pData = NULL;
	nLen = 0;
	usbres = ReadData(&pData, nLen);
	if (USB_SUCCESS != usbres)
	{
		return usbres;
	}
	delete[] pData;
	pData = NULL;
	
	//3、
	usbres = Set(2, 3);
	if (USB_SUCCESS != usbres)
	{
		return usbres;
	}

	pData = NULL;
	nLen = 0;
	usbres = ReadData(&pData, nLen);
	if (USB_SUCCESS != usbres)
	{
		return usbres;
	}
	delete[] pData;
	pData = NULL;

	return USB_SUCCESS;
}

USBRES CAcfd1Usb::SetAutoTable()
{
	USBRES usbres = Set(2, 1);
	if (USB_SUCCESS != usbres)
	{
		return usbres;
	}

	USBCHAR *pData = NULL;
	USBUINT nLen = 0;
	usbres = ReadData(&pData, nLen);
	if (USB_SUCCESS != usbres)
	{
		return usbres;
	}
	delete[] pData;
	pData = NULL;

	return USB_SUCCESS;
}

USBRES CAcfd1Usb::setAutoTableEntry(int index, int hour, int minute, int interval)
{
	int address = 5 + index * 3;
	USBRES usbres = Set(address, hour);
	if (USB_SUCCESS != usbres)
	{
		return usbres;
	}

	USBCHAR *pData = NULL;
	USBUINT nLen = 0;
	usbres = ReadData(&pData, nLen);
	if (USB_SUCCESS != usbres)
	{
		return usbres;
	}
	delete[] pData;
	pData = NULL;


	usbres = Set(address + 1, minute);
	if (USB_SUCCESS != usbres)
	{
		return usbres;
	}

	pData = NULL;
	nLen = 0;
	usbres = ReadData(&pData, nLen);
	if (USB_SUCCESS != usbres)
	{
		return usbres;
	}
	delete[] pData;
	pData = NULL;

	usbres = Set(address + 2, interval);
	if (USB_SUCCESS != usbres)
	{
		return usbres;
	}

	pData = NULL;
	nLen = 0;
	usbres = ReadData(&pData, nLen);
	if (USB_SUCCESS != usbres)
	{
		return usbres;
	}
	delete[] pData;
	pData = NULL;

	return usbres;
}

USBRES CAcfd1Usb::SetAutoTableEmpty(int index)
{
	int address = 5 + index * 3;
	USBRES usbres = SetEmpty(address);
	if (USB_SUCCESS != usbres)
	{
		return usbres;
	}

	//read
	USBCHAR *pData = NULL;
	USBUINT nLen = 0;
	usbres = ReadData(&pData, nLen);
	if (USB_SUCCESS != usbres)
	{
		return usbres;
	}
	delete[] pData;
	pData = NULL;

	usbres = SetEmpty(address + 1);
	if (USB_SUCCESS != usbres)
	{
		return usbres;
	}

	//read
	*pData = NULL;
	nLen = 0;
	usbres = ReadData(&pData, nLen);
	if (USB_SUCCESS != usbres)
	{
		return usbres;
	}
	delete[] pData;
	pData = NULL;

	usbres = SetEmpty(address + 2);
	if (USB_SUCCESS != usbres)
	{
		return usbres;
	}

	//read
	*pData = NULL;
	nLen = 0;
	usbres = ReadData(&pData, nLen);
	if (USB_SUCCESS != usbres)
	{
		return usbres;
	}
	delete[] pData;
	pData = NULL;

	return USB_SUCCESS;
}

USBRES CAcfd1Usb::ParseJsonData(IN const USBCHAR *pData, IN USBUINT nSize)
{
	if (NULL == pData) return USB_PARAM_ERROR;

	cJSON *pRoot = cJSON_Parse(pData);
	if( NULL == pRoot ) return USB_PARSE_JSON_ERROR;
	cJSON *pObj = cJSON_GetObjectItem(pRoot, "Cmd");
	if(NULL == pObj) return USB_PARSE_JSON_ERROR;

	USBCHAR szCmd[USB_LEN] = { 0 };
	sprintf_s(szCmd, USB_LEN, "%s", pObj->valuestring);
	
	if (0 == strnicmp(szCmd, "SET", strlen("SET")))
	{
		SET set;
		memset(&set, 0, sizeof(set));
		sprintf_s(set.szCmd, sizeof(set.szCmd), "%s", szCmd);

		cJSON *pParamObj = cJSON_GetObjectItem(pRoot, "Param");
		if( NULL == pParamObj ) return USB_PARSE_JSON_ERROR;
		cJSON *pID = cJSON_GetObjectItem(pParamObj, "ID");
		if( NULL == pID ) return USB_PARSE_JSON_ERROR;
		set.nID = atoi(pID->valuestring);
		cJSON *pMode = cJSON_GetObjectItem(pParamObj, "Mode");	
		if( NULL == pMode ) return USB_PARSE_JSON_ERROR;
		USBUINT nMode = atoi(pMode->valuestring);
		if (0 == nMode)
		{
		}
		else if (1 == nMode)		//1——按时间表测量
		{
			set.nMode = nMode;	
			cJSON *pArray = cJSON_GetObjectItem(pParamObj,"TimeList");
			if( NULL == pArray ) return USB_PARSE_JSON_ERROR;
			int nItemCnt = cJSON_GetArraySize(pArray);
			for(int i = 0; i < nItemCnt; i++)
			{
				AUTOTABLE autotable = { 0 };
				cJSON *pItem = cJSON_GetArrayItem(pArray, i);
				if(NULL != pItem)
				{
					cJSON *pHour = cJSON_GetObjectItem(pItem,"Hour");
					if( NULL == pHour ) return USB_PARSE_JSON_ERROR;					
					autotable.nHour = atoi(pHour->valuestring);	

					cJSON *pMin = cJSON_GetObjectItem(pItem,"Min");
					if( NULL == pMin ) return USB_PARSE_JSON_ERROR;					
					autotable.nMin = atoi(pMin->valuestring);	

					cJSON *pInterval = cJSON_GetObjectItem(pItem,"Interval");
					if( NULL == pInterval ) return USB_PARSE_JSON_ERROR;					
					autotable.nInterval = atoi(pInterval->valuestring);	
				}
				else
				{
					continue;
				}
				set.AutoTableModeVec.push_back(autotable);
			}
		}
		else if (2 == nMode)	//2——自动，白天15分钟晚上30分钟测一次
		{
			set.nMode = nMode;
			//set.AutoMode.nDay = 15;
			//set.AutoMode.nNight = 30;
		}
		else if (3 == nMode)	//3——手动时间间隔,自动测试
		{
			set.nMode = nMode;

			cJSON *pDay = cJSON_GetObjectItem(pParamObj, "Day");	
			if( NULL == pDay ) return USB_PARSE_JSON_ERROR;	
			set.HandleIntervalMode.nDay = atoi(pDay->valuestring);

			cJSON *pNight = cJSON_GetObjectItem(pParamObj, "Night");	
			if( NULL == pNight ) return USB_PARSE_JSON_ERROR;	
			set.HandleIntervalMode.nNight = atoi(pNight->valuestring);
		}
		else if (4 == nMode)
		{
		}
		else
		{
			return USB_FAIL;
		}
	}
	else if (0 == strnicmp(szCmd, "SDT", strlen("SDT")))
	{
	}
	else if (0 == strnicmp(szCmd, "ST", strlen("ST")))
	{
	}
	else
	{
		return USB_FAIL;
	}
	cJSON_Delete(pRoot);

	return USB_SUCCESS;
}

USBRES CAcfd1Usb::ParseJsonSetCmd(IN const USBCHAR *pData, OUT SET &set)
{
	if (NULL == pData) return USB_PARAM_ERROR;

	cJSON *pRoot = cJSON_Parse(pData);
	if( NULL == pRoot ) return USB_PARSE_JSON_ERROR;
	cJSON *pObj = cJSON_GetObjectItem(pRoot, "Cmd");
	if(NULL == pObj) return USB_PARSE_JSON_ERROR;

	USBCHAR szCmd[USB_LEN] = { 0 };
	sprintf_s(szCmd, USB_LEN, "%s", pObj->valuestring);

	memset(&set, 0, sizeof(set));
	sprintf_s(set.szCmd, sizeof(set.szCmd), "%s", szCmd);

	cJSON *pParamObj = cJSON_GetObjectItem(pRoot, "Param");
	if( NULL == pParamObj ) return USB_PARSE_JSON_ERROR;
	cJSON *pID = cJSON_GetObjectItem(pParamObj, "ID");
	if( NULL == pID ) return USB_PARSE_JSON_ERROR;
	set.nID = atoi(pID->valuestring);
	cJSON *pMode = cJSON_GetObjectItem(pParamObj, "Mode");	
	if( NULL == pMode ) return USB_PARSE_JSON_ERROR;
	USBUINT nMode = atoi(pMode->valuestring);
	if (0 == nMode)
	{
	}
	else if (1 == nMode)		//1——按时间表测量
	{
		set.nMode = nMode;	

		cJSON *pArray = cJSON_GetObjectItem(pParamObj,"TimeList");
		if( NULL == pArray ) return USB_PARSE_JSON_ERROR;
		int nItemCnt = cJSON_GetArraySize(pArray);
		for(int i = 0; i < nItemCnt; i++)
		{
			AUTOTABLE autotable = { 0 };
			cJSON *pItem = cJSON_GetArrayItem(pArray,i);
			if(NULL != pItem)
			{
				cJSON *pHour = cJSON_GetObjectItem(pItem,"Hour");
				if( NULL == pHour ) return USB_PARSE_JSON_ERROR;					
				autotable.nHour = atoi(pHour->valuestring);	

				cJSON *pMin = cJSON_GetObjectItem(pItem,"Min");
				if( NULL == pMin ) return USB_PARSE_JSON_ERROR;					
				autotable.nMin = atoi(pMin->valuestring);	

				cJSON *pInterval = cJSON_GetObjectItem(pItem,"Interval");
				if( NULL == pInterval ) return USB_PARSE_JSON_ERROR;					
				autotable.nInterval = atoi(pInterval->valuestring);	
			}			
			set.AutoTableModeVec.push_back(autotable);
		}
	}
	else if (2 == nMode)	//2——自动，白天15分钟晚上30分钟测一次
	{
		set.nMode = nMode;
		set.AutoMode.nDay = 15;
		set.AutoMode.nNight = 30;
	}
	else if (3 == nMode)	//3——手动时间间隔,自动测试
	{
		set.nMode = nMode;
		cJSON *pDay = cJSON_GetObjectItem(pParamObj, "Day");
		if( NULL == pDay ) return USB_PARSE_JSON_ERROR;	
		set.HandleIntervalMode.nDay = atoi(pDay->valuestring);
		cJSON *pNight = cJSON_GetObjectItem(pParamObj, "Night");	
		if( NULL == pNight ) return USB_PARSE_JSON_ERROR;	
		set.HandleIntervalMode.nNight = atoi(pNight->valuestring);
	}
	else if (4 == nMode)
	{
	}
	else
	{
		return USB_FAIL;
	}

	time_t curtime = time(0);
	tm tim;
	localtime_s(&tim,&curtime); 

	set.CurSysTime.nYear = tim.tm_year + 1900;
	set.CurSysTime.nMon = tim.tm_mon + 1;
	set.CurSysTime.nDay = tim.tm_mday;
	set.CurSysTime.nHour = tim.tm_hour;
	set.CurSysTime.nMin = tim.tm_min;
	set.CurSysTime.nSec = tim.tm_sec;

	cJSON_Delete(pRoot);

	return USB_SUCCESS;
}


USBRES CAcfd1Usb::ParseCmd(OUT USBCHAR *pCmd, OUT USBUINT &nCmdLen, IN const USBCHAR *cursor)
{
	if (NULL == cursor) return USB_PARAM_ERROR;

	const USBCHAR *cur = cursor;
	//parse cmd
	parse_status_t status;
	memptr token;
	token_type_t tok_type;

	while(NULL != *cur)
	{			
		status = ScannerGetToken(cur, &token, &tok_type);
		if (status != (parse_status_t)PARSE_OK) 
		{
			return USB_FAIL;
		}		
		switch (tok_type) 
		{		
		case TT_IDENTIFIER:		
			break;
		case TT_WHITESPACE:
			break;			
		case TT_CRLF:
			break;
		case TT_CTRL:
			break;
		case TT_SEPARATOR:
			if (' ' == *token.buf || '\t' == *token.buf)
			{					
				break;				
			}
			break;	
		case TT_QUOTEDSTRING:
			break;
		default:
			break;
		}
		cur += token.length;
	}

	nCmdLen = cur - token.buf;
	memcpy(pCmd, cur, nCmdLen);
	return USB_SUCCESS;
}

USBRES CAcfd1Usb::ParseSetCmdData(OUT SET &setdata, IN const USBCHAR *cursor)
{
	if (NULL == cursor) return USB_PARAM_ERROR;

	const USBCHAR *cur = cursor;
	//parse cmd
	parse_status_t status;
	memptr token;
	token_type_t tok_type;
	USBCHAR szCmdData[USB_LEN] = { 0 };
	USBUINT nData[32] = { 0 };
	int i = 0;
	while(NULL != *cur)
	{			
		status = ScannerGetToken(cur, &token, &tok_type);
		if (status != (parse_status_t)PARSE_OK) 
		{
			return USB_FAIL;
		}	
		int nLen = 0;
		switch (tok_type) 
		{		
		case TT_IDENTIFIER:		
			nLen = cur - token.buf;
			memcpy(szCmdData, cur, nLen);
			nData[i] = atoi(szCmdData);	
			break;
		case TT_WHITESPACE:
			break;			
		case TT_CRLF:
			break;
		case TT_CTRL:
			break;
		case TT_SEPARATOR:
			if (' ' == *token.buf || '\t' == *token.buf)
			{	
				i = 0;			
			}
			break;	
		case TT_QUOTEDSTRING:
			break;
		default:
			break;
		}
		cur += token.length;
	}

	//setdata.nAddr = nData[0];
	//setdata.nData = nData[1];

	return USB_SUCCESS;
}


USBRES CAcfd1Usb::WriteCmd(IN const USBCHAR *cmd, IN const USBCHAR *cursor)
{
	if (NULL == cmd) return USB_PARAM_ERROR;

	USBRES usbres;
	if (0 == strnicmp(cmd, "set", strlen("set")))	//setup
	{
		SET setdata = { 0 };
		sprintf_s(setdata.szCmd, sizeof(setdata.szCmd), "%s", strlen("set"));
		usbres = ParseSetCmdData(setdata, cursor);
		if (USB_SUCCESS != usbres)
		{
			return usbres;
		}
		bool bRet = m_Usb.WriteToUsb((USBCHAR*)cmd, strlen(cmd));
		if (!bRet)
		{
			return USB_FAIL;
		}
	}
	else if (0 == strnicmp(cmd, "sdt", strlen("set")))	//setup date
	{
		
	}
	else if (0 == strnicmp(cmd, "clr", strlen("set")))
	{
		
	}
	else
	{}	

	return usbres;
}




USBRES CAcfd1Usb::FreeMemory(IN USBCHAR *&pData)
{
	if (NULL != pData)
	{
		delete[] pData;
		pData = NULL;
	}
	return USB_SUCCESS;
}



USBRES CAcfd1Usb::ParseRxBuf(OUT membuffer *pdata, \
						IN const USBCHAR *prxBuf, \
						OUT USBINT nSize)
{
	if (NULL == prxBuf) return USB_FAIL;

	const USBCHAR *cursor = prxBuf;

	//parse token
	USBCHAR *pCmd = NULL;
	USBINT nLen = 0;
	USBRES usbres = GetTokenString(pCmd, nLen, prxBuf);
	if (USB_SUCCESS != usbres)
	{
		return usbres;
	}

	//parse cmd
	cursor = cursor + nLen + 2;
	usbres = ParseData(pdata, pCmd, cursor, nSize - nLen);
	if (USB_SUCCESS != usbres)
	{
		LOG4CPLUS_INFO(m_Logger, "CAcfd1Usb::ParseRxBuf error" \
			<<" cmd="<<pCmd \
			<<" original data="<<cursor); 
		return usbres;
	}
	LOG4CPLUS_INFO(m_Logger, "CAcfd1Usb::ParseRxBuf success" \
		<<" cmd="<<pCmd \
		<<" parsed data="<<(NULL != pdata->buf) ? (pdata->buf) : ""); 

	if (NULL != pCmd)
	{
		delete[] pCmd;
		pCmd = NULL;
	}
	
	return usbres;	
}

USBRES CAcfd1Usb::GetTokenString(USBCHAR *&pBuf, USBINT &nSize, const USBCHAR *prxBuf)
{
	if (NULL == prxBuf) return USB_PARAM_ERROR;

	int nLen = 0;
	const USBCHAR *cursor = prxBuf;
	char c = *cursor;
	bool bFound = false;
	while(c)
	{
		if (c == TOKCHAR_LF)
		{
			cursor += 1;
			c = *cursor;
			if (c == TOKCHAR_CR)
			{		
				cursor += 1;
				nLen = cursor - prxBuf - 2;
				pBuf = new USBCHAR[nLen+1];
				if (NULL != pBuf)
				{
					*(pBuf + nLen) = '\0';
					memcpy(pBuf, prxBuf, nLen);
				}	
				bFound =  true;
				break;
			}
		}
		else
		{
			cursor += 1;
			c = *cursor;
		}
	}
	nSize = nLen;
	return (!bFound) ? USB_FAIL : USB_SUCCESS;
}

USBRES CAcfd1Usb::ParseData(OUT membuffer *pdata, \
						IN const USBCHAR *cmd, \
						IN const USBCHAR *cursor, \
						OUT USBINT nSize)
{
	if (NULL == cmd || NULL == cursor) return USB_PARAM_ERROR;
	
	USBRES usbres = USB_FAIL;
	if (0 == strnicmp(cmd, "rrcd", strlen("rrcd")))
	{
		usbres = ParseRrcdCmdData(pdata, cursor, nSize);
	}
	else if (0 == strnicmp(cmd, "rset", strlen("rset")))
	{
		usbres = ParseRsetCmdData(pdata, cursor, nSize);
	}
	else if (0 == strnicmp(cmd, "rda", strlen("rda")))	//parse have problem
	{
	}
	else if (0 == strnicmp(cmd, "rdf", strlen("rdf")))	
	{
	}
	else if (0 == strnicmp(cmd, "ver", strlen("rdf")))	
	{
	}
	else if (0 == strnicmp(cmd, "set", strlen("set")))
	{
		USBCHAR *pCmd = NULL;
		USBINT nLen = 0;
		usbres = GetTokenString(pCmd, nLen, cursor);

		membuffer_append(pdata, pCmd, strlen(pCmd));
		delete[] pCmd;
	}
	else if (0 == strnicmp(cmd, "sdt", strlen("sdt")))
	{
		USBCHAR *pCmd = NULL;
		USBINT nLen = 0;
		usbres = GetTokenString(pCmd, nLen, cursor);

		membuffer_append(pdata, pCmd, strlen(pCmd));
		delete[] pCmd;
	}
	else if (0 == strnicmp(cmd, "st", strlen("st")))
	{
		USBCHAR *pCmd = NULL;
		USBINT nLen = 0;
		usbres = GetTokenString(pCmd, nLen, cursor);

		membuffer_append(pdata, pCmd, strlen(pCmd));
		delete[] pCmd;
	}
	else if (0 == strnicmp(cmd, "clr", strlen("clr")))
	{
		USBCHAR *pCmd = NULL;
		USBINT nLen = 0;
		usbres = GetTokenString(pCmd, nLen, cursor);

		membuffer_append(pdata, pCmd, strlen(pCmd));
		delete[] pCmd;
	}
	else
	{
		return USB_FAIL;
	}
	return usbres;
}


USBRES CAcfd1Usb::ParseRrcdCmdData(OUT membuffer *pdata, \
							IN const USBCHAR *cursor, \
							OUT USBINT nSize)
{
	
	if (NULL == cursor) return USB_PARAM_ERROR;

	const USBCHAR *cur = cursor;
	//1、parse record box id
	USBCHAR *szId = NULL;
	USBINT nLen = 0;
	USBRES usbres = GetTokenString(szId, nLen, cur);
	if (USB_SUCCESS != usbres)
	{
		return usbres;
	}
	cur = cur + nLen;

	//hex convert to int
	char ccInt = 0;
	uint64_t ulnValue = 0;
	for(uint16_t usIdx = 0; szId[usIdx] && (usIdx < nLen); usIdx++)
	{					
		if(-1 == (ccInt = HexToInt(szId[usIdx])))
			return USB_FAIL;
		ulnValue = ulnValue << 4;
		ulnValue |= ccInt;
	}

	if (NULL != szId)
	{
		delete[] szId;
	}
	//record box id
	int nId = 0;
	nId = ulnValue;
	nId = (ulnValue & 0x00ff) << 8;		//id高8位值
	nId |=  (ulnValue & 0xff00) >> 8;	//id低8位值
	char szBuf[128] = { 0 };
	sprintf_s(szBuf, sizeof(szBuf), "%d", nId);

	//2、generate json 
	cJSON *pRoot = cJSON_CreateObject();
	if(NULL == pRoot) return USB_NO_MEMORY;	
	cJSON_AddStringToObject(pRoot,"ID",szBuf);		
	cJSON *pMeasureDataList = cJSON_CreateArray();
	if (NULL == pMeasureDataList) return USB_NO_MEMORY;
	cJSON_AddItemToObject(pRoot, "MeasureData", pMeasureDataList);
	
	//3、parse data
	parse_status_t status;
	memptr token;
	token_type_t tok_type;

	USBUINT nDataType = 0;
	USBUINT nData[8] = { 0 };

	while(NULL != *cur)
	{			
		status = ScannerGetToken(cur, &token, &tok_type);
		if (status != (parse_status_t)PARSE_OK) 
		{
			return USB_FAIL;
		}		

		char cInt = 0;
		uint64_t ulValue = 0;		
		switch (tok_type) 
		{		
			case TT_IDENTIFIER:		
				for(uint16_t usIdx = 0; token.buf[usIdx] && (usIdx < token.length); usIdx++)
				{					
					if(-1 == (cInt = HexToInt(token.buf[usIdx])))
						return USB_FAIL;
					ulValue = ulValue << 4;
					ulValue |= cInt;
				}
				nData[nDataType++] = ulValue;
				break;
			case TT_WHITESPACE:
				break;			
			case TT_CRLF:
				break;
			case TT_CTRL:
				break;
			case TT_SEPARATOR:
				if (',' == *token.buf)
				{		
					//将一个一帧数据保存成json格式
					GenRrcdCmdDataJson(pdata, pMeasureDataList, nData, 8);
					nDataType = 0;
					memset(nData, 0, sizeof(nData));
				}
				break;	
			case TT_QUOTEDSTRING:
				break;
			default:
				break;
		}
		cur += token.length;
	}

	char *p = cJSON_Print(pRoot);		
	if(NULL == p) return USB_NO_MEMORY;

	membuffer_append(pdata, p, strlen(p));
	//delete[] p;
	free(p);
	p = NULL;

	cJSON_Delete(pRoot);
	return USB_SUCCESS;
}

USBRES CAcfd1Usb::ParseRsetCmdData(OUT membuffer *pdata, IN const USBCHAR *cursor, OUT USBINT nSize)
{
	if (NULL == cursor) return USB_PARAM_ERROR;

	const USBCHAR *cur = cursor;
	//1、parse record box id
	USBCHAR *szId = NULL;
	USBINT nLen = 0;
	USBRES usbres = GetTokenString(szId, nLen, cur);
	if (USB_SUCCESS != usbres)
	{
		return usbres;
	}
	cur = cur + nLen + 2;

	//hex convert to int
	int nVal = HexToASCII(szId);

	if (NULL != szId)
	{
		delete[] szId;
	}
	//record box id
	int nId = 0;
	nId = (nVal & 0x00ff) << 8;
	nId |=  (nVal & 0xff00) >> 8;
	char szBuf[128] = { 0 };
	sprintf_s(szBuf, sizeof(szBuf), "%d", nId);
	
	//3、generate json 
	cJSON *pRoot = cJSON_CreateObject();
	if(NULL == pRoot) return USB_NO_MEMORY;	
	cJSON_AddStringToObject(pRoot,"ID",szBuf);	

	//2、parse set time
	USBCHAR *szpSetTime = NULL;
	nLen = 0;
	usbres = GetTokenString(szpSetTime, nLen, cur);
	if (USB_SUCCESS != usbres)
	{
		return usbres;
	}
	cur = cur + nLen;

	std::string szSetTime = szpSetTime;
	if (NULL != szpSetTime)
	{
		delete[] szpSetTime;
	}

	std::string szYear = szSetTime.substr(0,4);
	std::string szMon = szSetTime.substr(5,2);
	std::string szDay = szSetTime.substr(8,2);
	std::string szHour = szSetTime.substr(11,2);
	std::string szMin = szSetTime.substr(14,2);
	std::string szSec = szSetTime.substr(17,2);

	sprintf_s(szBuf, sizeof(szBuf), "%s", szYear.c_str());
	int year = HexToASCII(szBuf);
	sprintf_s(szBuf, sizeof(szBuf), "%s", szMon.c_str());
	int mon = HexToASCII(szBuf);
	sprintf_s(szBuf, sizeof(szBuf), "%s", szDay.c_str());
	int day = HexToASCII(szBuf);
	sprintf_s(szBuf, sizeof(szBuf), "%s", szHour.c_str());
	int hour = HexToASCII(szBuf);
	sprintf_s(szBuf, sizeof(szBuf), "%s", szMin.c_str());
	int min = HexToASCII(szBuf);
	sprintf_s(szBuf, sizeof(szBuf), "%s", szSec.c_str());
	int sec = HexToASCII(szBuf);
	sprintf_s(szBuf, sizeof(szBuf), "%04d-%02d-%02d %02d:%02d:%02d", year,mon,day,hour,min,sec);

	cJSON_AddStringToObject(pRoot,"CurTime",szBuf);	


	cJSON *pSetTimeList = cJSON_CreateArray();
	if (NULL == pSetTimeList) return USB_NO_MEMORY;
	cJSON_AddItemToObject(pRoot, "TimeList", pSetTimeList);

	//3、parse data
	parse_status_t status;
	memptr token;
	token_type_t tok_type;

	USBUINT nDataType = 0;
	USBUINT nData[8] = { 0 };

	while(NULL != *cur)
	{			
		status = ScannerGetToken(cur, &token, &tok_type);
		if (status != (parse_status_t)PARSE_OK) 
		{
			return USB_FAIL;
		}		

		char cInt = 0;
		uint64_t ulValue = 0;		
		switch (tok_type) 
		{		
		case TT_IDENTIFIER:		
			for(uint16_t usIdx = 0; token.buf[usIdx] && (usIdx < token.length); usIdx++)
			{					
				if(-1 == (cInt = HexToInt(token.buf[usIdx])))
					return USB_FAIL;
				ulValue = ulValue << 4;
				ulValue |= cInt;
			}
			nData[nDataType++] = ulValue;
			break;
		case TT_WHITESPACE:
			break;			
		case TT_CRLF:
			break;
		case TT_CTRL:
			break;
		case TT_SEPARATOR:
			if (',' == *token.buf || ';' == *token.buf)
			{					
				GenRsetCmdDataJson(pdata,pSetTimeList, nData, 8);
				nDataType = 0;
				memset(nData, 0, sizeof(nData));
			}
			break;	
		case TT_QUOTEDSTRING:
			break;
		default:
			break;
		}
		cur += token.length;
	}

	char *p = cJSON_Print(pRoot);		//p point need release
	if(NULL == p) return USB_NO_MEMORY;

	membuffer_append(pdata, p, strlen(p));
	//delete[] p;
	free(p);
	p = NULL;

	cJSON_Delete(pRoot);
	return USB_SUCCESS;
}



USBRES CAcfd1Usb::GenRrcdCmdDataJson(OUT membuffer *pdata, IN cJSON *pMeasureDataList, IN USBUINT *pRrcdData, IN USBUINT nSize)
{
	if (NULL == pMeasureDataList) return USB_PARAM_ERROR;


	char szBuf[128];
	MEASUREDATA measuredata = { 0 };

	measuredata.nTime = (pRrcdData[1] << 16);//测量时间	
	//measuredata.nTime <<= 16;
	measuredata.nTime |= pRrcdData[0];		//测量时间	
	
	//int nHR = (pRrcdData[2] & 0x00ff) << 8;	//HR高8位值
	//int nHR |=  (pRrcdData[2] & 0xff00) >> 8;	//HR低8位值
	int nHR = (pRrcdData[2] & 0x00ff);			//心率HR低8位值
	measuredata.HR = nHR;					//心率
	
	//int nDP = (pRrcdData[3] & 0x00ff) << 8;	//舒张压DP高8位值
	//nDP |= (pRrcdData[3] & 0xff00) >> 8;	//舒张压DP低8位值
	int nDP = (pRrcdData[3] & 0x00ff);		//舒张压DP低8位值	
	measuredata.DP = nDP;					//舒张压	

	//int nSP = (pRrcdData[4] & 0x00ff) << 8;		//收缩压HR高8位值
	//nSP |= (pRrcdData[4] & 0xff00) >> 8;		//收缩压HR低8位值
	int nSP = (pRrcdData[4] & 0x00ff);			//收缩压HR低8位值
	measuredata.SP = nSP;			//收缩压

	measuredata.nID = pRrcdData[5];
	measuredata.nReserve = pRrcdData[6];

	memset(szBuf, 0 ,sizeof(szBuf));


	if (measuredata.nTime > 0)
	{
		time_t curtime = measuredata.nTime - 28800;
		tm tim;
		localtime_s(&tim,&curtime); 
		int day,mon,year,hour,min,sec;
		day = tim.tm_mday;
		mon = tim.tm_mon + 1;
		year = tim.tm_year + 1900;
		hour = tim.tm_hour;
		min = tim.tm_min;
		sec = tim.tm_sec;
		sprintf_s(szBuf, sizeof(szBuf), "%04d-%02d-%02d %02d:%02d:%02d", year,mon,day,hour,min,sec);
	}
	else
	{}	


	cJSON *pMeasureData = cJSON_CreateObject();
	if(NULL == pMeasureData) return USB_NO_MEMORY;	

	cJSON_AddStringToObject(pMeasureData,"Time",szBuf);

	memset(szBuf, 0 ,sizeof(szBuf));
	sprintf_s(szBuf, sizeof(szBuf), "%d", measuredata.SP);
	cJSON_AddStringToObject(pMeasureData,"SP", szBuf);

	memset(szBuf, 0 ,sizeof(szBuf));
	sprintf_s(szBuf, sizeof(szBuf), "%d", measuredata.DP);
	cJSON_AddStringToObject(pMeasureData,"DP",szBuf);

	memset(szBuf, 0 ,sizeof(szBuf));
	sprintf_s(szBuf, sizeof(szBuf), "%d", measuredata.HR);
	cJSON_AddStringToObject(pMeasureData,"HR",szBuf);
	cJSON_AddStringToObject(pMeasureData,"AverHR","");
	cJSON_AddStringToObject(pMeasureData,"Status","");
	cJSON_AddStringToObject(pMeasureData,"IsValid","");
	cJSON_AddStringToObject(pMeasureData,"Position","");

	cJSON_AddItemToArray(pMeasureDataList,pMeasureData);

	return USB_SUCCESS;
}

USBRES CAcfd1Usb::GenRsetCmdDataJson(OUT membuffer *pdata, \
						IN cJSON *pTimeList, \
						IN USBUINT *pRsetData, \
						IN USBUINT nSize)
{
	if (NULL == pTimeList) return USB_PARAM_ERROR;

	char szBuf[128];

	cJSON *pTime = cJSON_CreateObject();
	if(NULL == pTime) return USB_NO_MEMORY;	

	memset(szBuf, 0 ,sizeof(szBuf));
	sprintf_s(szBuf, sizeof(szBuf), "%d:%d", pRsetData[0],pRsetData[1]);
	cJSON_AddStringToObject(pTime,"Time", szBuf);	//时间

	memset(szBuf, 0 ,sizeof(szBuf));
	sprintf_s(szBuf, sizeof(szBuf), "%d", pRsetData[2]);
	cJSON_AddStringToObject(pTime,"Interval", szBuf);	//间隔

	cJSON_AddItemToArray(pTimeList,pTime);
	return USB_SUCCESS;
}


parse_status_t CAcfd1Usb::ScannerGetToken(INOUT const USBCHAR *scanner, OUT memptr *token, OUT token_type_t *tok_type)
{
	if (NULL == scanner) return PARSE_FAILURE;
	
	const USBCHAR *cursor;
	USBINT c;
	token_type_t token_type;
	int got_end_quote;


	cursor = scanner;
	c = *cursor;


	if (IsIdentifierChar(c))
	{
		/* scan identifier */
		token->buf = (USBCHAR*)cursor++;
		token_type = TT_IDENTIFIER;
		while (IsIdentifierChar(*cursor))
			cursor++;
		/* calc token length */
		token->length = (size_t)cursor - (size_t)token->buf;
	}
	else if (c == ' ' || c == '\t')
	{
		token->buf = (USBCHAR*)cursor++;
		token_type = TT_WHITESPACE;
		while ((*cursor == ' ' || *cursor == '\t'))
			cursor++;
		token->length = (size_t)cursor - (size_t)token->buf;

	}
	else if (c == TOKCHAR_CR) 
	{
		/* scan CRLF */
		token->buf = (USBCHAR*)cursor++;
		if (*cursor != TOKCHAR_LF) 
		{
			/* couldn't match CRLF; match as CR */
			token_type = TT_CTRL;	/* ctrl char */
			token->length = (size_t)1;
		}
		else
		{
			/* got CRLF */
			token->length = (size_t)2;
			token_type = TT_CRLF;
			cursor++;
		}
	}
	else if (c == TOKCHAR_LF) /* accept \n as CRLF */
	{
		token->buf = (USBCHAR*)cursor++;
		token->length = (size_t)1;
		token_type = TT_CRLF;
	}
	else if (c == '"') 
	{
		/* quoted text */
		token->buf = (USBCHAR*)cursor++;
		token_type = TT_QUOTEDSTRING;
		got_end_quote = FALSE;
		//while (cursor < null_terminator) {
			c = *cursor++;
			if (c == '"') 
			{
				got_end_quote = TRUE;
			//	break;
			} 
			else if (c == '\\') 
			{
				//if (cursor < null_terminator) {
					c = *cursor++;
					/* the char after '\\' could be ANY octet */
				//}
				/* else, while loop handles incomplete buf */
			} 
			else if (IsQdtextChar(c))
			{
				/* just accept char */
			} 
			else
				/* bad quoted text */
				return PARSE_FAILURE;
		//}
		if (got_end_quote)
			token->length = (size_t)cursor - (size_t)token->buf;
		else 
		{	/* incomplete */

			//assert(cursor == null_terminator);
			return PARSE_INCOMPLETE;
		}
	}
	else if (IsSeparatorChar(c)) 
	{
		/* scan separator */
		token->buf = (USBCHAR*)cursor++;
		token_type = TT_SEPARATOR;
		token->length = (size_t)1;
	}
	else if (IsControlChar(c)) 
	{
		/* scan ctrl char */
		token->buf = (USBCHAR*)cursor++;
		token_type = TT_CTRL;
		token->length = (size_t)1;
	}
	else
	{
		//error
		return PARSE_FAILURE;
	}

	//scanner->cursor += token->length;	/* move to next token */
	*tok_type = token_type;
	return PARSE_OK;
}

int8_t CAcfd1Usb::HexToInt(char c)
{
	if('0' <= c && c <= '9')
		return c - '0';
	else if ('A' <= c&&c <= 'F')
		return 10 + c-'A';
	else if ('a' <= c && c <= 'f')
		return 10 + c - 'a';
	else 
		return -1;
}

uint64_t CAcfd1Usb::HexToASCII(const char* szIn)
{
	if(NULL == szIn) return -1;
	//if('0' != szIn[0] || 'x' != szIn[1])
	//{//说明输入的格式串不是十六进制数据
	//	//return atoll(szIn);
	//	return atol(szIn);
	//}
	//16进制串转换成整型
	char cInt;
	uint64_t ulValue = 0;
	//for(uint16_t usIdx = 2; szIn[usIdx]; usIdx++)
	for(uint16_t usIdx = 0; szIn[usIdx]; usIdx++)
	{
		if(-1 == (cInt = HexToInt(szIn[usIdx])))
			return -1;
		ulValue = ulValue << 4;
		ulValue |= cInt;
	}
	return ulValue;
}











