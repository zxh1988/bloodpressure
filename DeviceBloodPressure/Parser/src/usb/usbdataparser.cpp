#include "StdAfx.h"
#include "usbdataparser.h"
#include "cJSON.h"
#include <iostream>

#include <time.h>




__BEGIN_NAMESPACE(Parser)





int CUsbDataParser::Parse(IN const char *pData, IN int &nType, OUT char *&pParsedData)
{
	E_Status_t eRet = UNKNOW_STATUS;
	CheckNullPtr(pData);
	const char *pBuf = pData;

	cJSON *pRoot;
	if (pRoot = cJSON_Parse(pBuf), NULL == pRoot) return JSON_FORMAT_ERROR;
	//1、Tag type
	cJSON *pTagType = cJSON_GetObjectItem(pRoot, TAG);
	CheckNullPtr(pTagType);

	Format::HeadParam_t headparam = { 0 };
	sprintf_s(headparam.szTag, sizeof(headparam.szTag), pTagType->valuestring);

	//cmd
	cJSON *pCmd = cJSON_GetObjectItem(pRoot, CMD);
	sprintf_s(headparam.szCmd, sizeof(headparam.szCmd), pCmd->valuestring);

	cJSON_Delete(pRoot);
	pRoot = NULL;

	eRet = (E_Status_t)HandleJsonCmd(pData, headparam.szCmd, nType, pParsedData);

	return eRet;
}


int CUsbDataParser::HandleJsonCmd(IN const char *pData, IN const char *pCmd, OUT int &nType, OUT char *&pFormatData)
{
	E_Status_t eRet = SUCCESS;

	CheckNullPtr(pData);
	const char *pBuf = pData;
	nType = Format::UNKNOW_TYPE;

	if (0 == strncasecmp(pCmd, DEVICE_USB, strlen(pCmd)))	//设备初始化
	{		
		eRet = (E_Status_t)ParseUsbDevFormat(pBuf, pFormatData);
		nType = Format::ACF_DEV_INIT;
	}
	else if (0 == strncasecmp(pCmd, MEASURE_MODE, strlen(pCmd)))//设置时间测量模式
	{	
		eRet = (E_Status_t)HandleSetCmd(pBuf, MEASURE_MODE, pFormatData);
		nType = Format::ACF_MEASURE_MODE;
	}
	else if (0 == strncasecmp(pCmd, RRCD_COMMAND, strlen(pCmd)))//读RRCD数据
	{
		nType = Format::ACF_GET_RECORD_DATA;
	}
	else if (0 == strncasecmp(pCmd, CLR_COMMAND, strlen(pCmd)))	//清除RRCD数据
	{
		nType = Format::ACF_CLC_RECORD_DATA;
	}
	else if (0 == strncasecmp(pCmd, SDT_COMMAND, strlen(pCmd)))
	{
		//TODO::未实现
	}
	else if (0 == strncasecmp(pCmd, ST_COMMAND, strlen(pCmd)))
	{
		//TODO::未实现
	}
	else
	{
		nType = Format::UNKNOW_TYPE;
	}	
	return eRet;
}

int CUsbDataParser::ParseUsbDevFormat(IN const char *pData, OUT char *&pFormatData)
{
	E_Status_t nRet = SUCCESS;

	CheckNullPtr(pData);
	const char *pBuf = pData;

	cJSON *pRoot;
	if (pRoot = cJSON_Parse(pBuf), NULL == pRoot) return JSON_FORMAT_ERROR;

	//1、Tag type
	cJSON *pTagType = cJSON_GetObjectItem(pRoot, TAG);
	CheckNullPtr(pTagType);

	Format::HeadParam_t headparam = { 0 };
	sprintf_s(headparam.szTag, sizeof(headparam.szTag), pTagType->valuestring);

	//cmd
	cJSON *pCmd = cJSON_GetObjectItem(pRoot, CMD);
	sprintf_s(headparam.szCmd, sizeof(headparam.szCmd), pCmd->valuestring);

	Format::UsbDevData_t usbdata = {0};
	memcpy(&usbdata.Head, &headparam, sizeof(usbdata.Head));

	//2、param
	cJSON *pParamObj = cJSON_GetObjectItem(pRoot, PARAM);

	//2.1wVID
	cJSON *pwVID = cJSON_GetObjectItem(pParamObj, WVID);
	usbdata.wVID = atoi(pwVID->valuestring);

	//2.2 wPID
	cJSON *pwPID = cJSON_GetObjectItem(pParamObj, WPID);
	usbdata.wPID = atoi(pwPID->valuestring);

	int nLen = sizeof(Format::UsbDevData_t);
	pFormatData = new char[nLen + 1];
	CheckNullPtr(pFormatData);
	*(pFormatData + nLen) = '\0';
	memcpy(pFormatData, &usbdata, nLen);

	cJSON_Delete(pRoot);
	pRoot = NULL;

	return nRet;
}

int CUsbDataParser::HandleSetCmd(IN const char *pData, IN const char *pCmd, OUT char *&pFormatData)
{
	E_Status_t eRet = SUCCESS;
	CheckNullPtr(pData);
	const char *pBuf = pData;

	SET set = { 0 };
	eRet = (E_Status_t)ParseJsonSetCmd(pData, set);
	CheckStatus(eRet);

	int nLen = sizeof(SET);
	pFormatData = new char[nLen + 1];
	CheckNullPtr(pFormatData);
	*(pFormatData + nLen) = '\0';
	memcpy(pFormatData, &set, nLen);
	
	return eRet;
}


int CUsbDataParser::ParseJsonSetCmd(IN const char *pData, OUT SET &set)
{
	if (NULL == pData) return FAIL;

	cJSON *pRoot = cJSON_Parse(pData);
	if( NULL == pRoot ) return JSON_FORMAT_ERROR;
	cJSON *pObj = cJSON_GetObjectItem(pRoot, "Cmd");
	if(NULL == pObj) return JSON_FORMAT_ERROR;

	char szCmd[MAX_LEN] = { 0 };
	sprintf_s(szCmd, sizeof(szCmd), "%s", pObj->valuestring);

	memset(&set, 0, sizeof(set));
	sprintf_s(set.szCmd, sizeof(set.szCmd), "%s", szCmd);

	cJSON *pParamObj = cJSON_GetObjectItem(pRoot, "Param");
	if( NULL == pParamObj ) return JSON_FORMAT_ERROR;
	cJSON *pID = cJSON_GetObjectItem(pParamObj, "ID");
	if( NULL == pID ) return JSON_FORMAT_ERROR;
	set.nID = atoi(pID->valuestring);
	cJSON *pMode = cJSON_GetObjectItem(pParamObj, "Mode");	
	if( NULL == pMode ) return JSON_FORMAT_ERROR;
	int nMode = atoi(pMode->valuestring);
	if (0 == nMode)
	{
	}
	else if (1 == nMode)		//1——按时间表测量
	{
		set.nMode = nMode;	

		cJSON *pArray = cJSON_GetObjectItem(pParamObj,"TimeList");
		if( NULL == pArray ) return JSON_FORMAT_ERROR;
		int nItemCnt = cJSON_GetArraySize(pArray);
		for(int i = 0; i < (nItemCnt <= 6 ? nItemCnt : 6); i++)	//最大只能设置6个自动表
		{
			AUTOTABLE autotable = { 0 };
			cJSON *pItem = cJSON_GetArrayItem(pArray,i);
			if(NULL != pItem)
			{
				cJSON *pHour = cJSON_GetObjectItem(pItem,"Hour");
				if( NULL == pHour ) return JSON_FORMAT_ERROR;					
				autotable.nHour = atoi(pHour->valuestring);	

				cJSON *pMin = cJSON_GetObjectItem(pItem,"Min");
				if( NULL == pMin ) return JSON_FORMAT_ERROR;					
				autotable.nMin = atoi(pMin->valuestring);	

				cJSON *pInterval = cJSON_GetObjectItem(pItem,"Interval");
				if( NULL == pInterval ) return JSON_FORMAT_ERROR;					
				autotable.nInterval = atoi(pInterval->valuestring);	
			}	
			set.AutoTable[i] = autotable;
			//set.AutoTableModeVec.push_back(autotable);
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
		if( NULL == pDay ) return JSON_FORMAT_ERROR;	
		set.HandleIntervalMode.nDay = atoi(pDay->valuestring);
		cJSON *pNight = cJSON_GetObjectItem(pParamObj, "Night");	
		if( NULL == pNight ) return JSON_FORMAT_ERROR;	
		set.HandleIntervalMode.nNight = atoi(pNight->valuestring);
	}
	else if (4 == nMode)
	{
	}
	else
	{
		return FAIL;
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

	return SUCCESS;
}



__END_NAMESPACE(Parser)



