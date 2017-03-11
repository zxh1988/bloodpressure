#include "StdAfx.h"
#include "serialdataparser.h"
#include "cJSON.h"
#include <iostream>


__BEGIN_NAMESPACE(Parser)


SerialDataParser::SerialDataParser(void)
{
}


SerialDataParser::~SerialDataParser(void)
{
}

int SerialDataParser::Parse(IN const char *pData, IN int nType, OUT char *&pParsedData)
{
	E_Status_t nRet = UNKNOW_STATUS;
	switch(nType)
	{
	case Format::SERIAL_DEV_TYPE:
		nRet = (E_Status_t)ParseSerialDevFormat(pData, pParsedData);
		break;
	case Format::USB_DEV_TYPE:
		break;
	case Format::SERIAL_MESSAGE_PROTOCAL:
		nRet = (E_Status_t)ParseMessureTime(pData, pParsedData);
		break;
	case Format::SERIAL_RECORD_SUM:
		nRet = (E_Status_t)ParseRecordSum(pData, pParsedData);
		break;
	case Format::SERIAL_RECORD_DATA:
		nRet = SUCCESS;
		break;
	case Format::USB_RRCD_PROTOCAL:
		nRet = SUCCESS;
		break;
	default:
		break;
	}

	return nRet;
}

int SerialDataParser::ParseSerialDevFormat(IN const char *pData, OUT char *&pFormatData)
{
	E_Status_t nRet = SUCCESS;

	CheckNullPtr(pData);
	const char *pBuf = pData;

	cJSON *pRoot;
	if (pRoot = cJSON_Parse(pBuf), NULL == pRoot) return JSON_FORMAT_ERROR;

	//1、Tag type
	cJSON *pTagType = cJSON_GetObjectItem(pRoot, TAG);
	CheckNullPtr(pTagType);
	Format::SerialDevData_t devdata = {0};

	//2、param
	cJSON *pParamObj = cJSON_GetObjectItem(pRoot, PARAM);

	//2.1serial name
	cJSON *pSerialName = cJSON_GetObjectItem(pParamObj, SERIAL_NAME);
	sprintf_s(devdata.szCom, sizeof(devdata.szCom), "%s", pSerialName->valuestring);

	//2.2 serial baud
	cJSON *pBaud = cJSON_GetObjectItem(pParamObj, SERIAL_BAUD);
	devdata.nBaud = atoi(pBaud->valuestring);

	//2.3 serial parity
	cJSON *pSerialParity = cJSON_GetObjectItem(pParamObj, SERIAL_PARITY);
	devdata.nParity = atoi(pSerialParity->valuestring);

	//2.4 serial databits
	cJSON *pDataBits = cJSON_GetObjectItem(pParamObj, SERIAL_DATABITS);
	devdata.nDataBits = atoi(pDataBits->valuestring);

	//2.5 serial stopbits
	cJSON *pStopBits = cJSON_GetObjectItem(pParamObj, SERIAL_STOPBITS);
	devdata.nStopBits =  atoi(pStopBits->valuestring);

	int nLen = sizeof(Format::SerialDevData_t);
	pFormatData = new char[nLen + 1];
	CheckNullPtr(pFormatData);
	*(pFormatData + nLen) = '\0';
	memcpy(pFormatData, &devdata, nLen);

	cJSON_Delete(pRoot);
	pRoot = NULL;

	return nRet;
}

int SerialDataParser::ParseMessureTime(IN const char *pData, OUT char *&pFormatData)
{
	E_Status_t eRet = SUCCESS;

	CheckNullPtr(pData);
	const char *pBuf = pData;

	cJSON *pRoot;
	if (pRoot = cJSON_Parse(pBuf), NULL == pRoot) return JSON_FORMAT_ERROR;

	//1、Tag type
	cJSON *pTagType = cJSON_GetObjectItem(pRoot, TAG);
	CheckNullPtr(pTagType);
	Format::MessureTime_t messuredata = {0};

	sprintf_s(messuredata.szTag, sizeof(messuredata.szTag), "%s", pTagType->valuestring);
	
	//cmd
	cJSON *pCmd = cJSON_GetObjectItem(pRoot, CMD);
	sprintf_s(messuredata.szCmd, sizeof(messuredata.szCmd), "%s", pCmd->valuestring);

	//2、param
	cJSON *pParamObj = cJSON_GetObjectItem(pRoot, PARAM);

	//2.1 User ID
	cJSON *pID = cJSON_GetObjectItem(pParamObj, MESSAGE_USER_ID);
	sprintf_s(messuredata.szUserId, sizeof(messuredata.szUserId), "%s", pID->valuestring);

	//2.1 User Name
	cJSON *pUserName = cJSON_GetObjectItem(pParamObj, MESSAGE_USER_NAME);
	sprintf_s(messuredata.szUserName, sizeof(messuredata.szUserName), "%s", pUserName->valuestring);

	//2.2 mode
	cJSON *pMode = cJSON_GetObjectItem(pParamObj, MESSAGE_MODE);
	messuredata.nMode = atoi(pMode->valuestring);

	//2.3 白天开始小时
	cJSON *pDayStartHourTime = cJSON_GetObjectItem(pParamObj, MESSAGE_DAY_START_HOUR);
	messuredata.nDayStartHour = atoi(pDayStartHourTime->valuestring);

	//2.4 白天开始分钟
	cJSON *pDayStartMinTime = cJSON_GetObjectItem(pParamObj, MESSAGE_DAY_START_MIN);
	messuredata.nDayStartMin = atoi(pDayStartMinTime->valuestring);

	//2.5 白天间隔时间
	cJSON *pDayInternal = cJSON_GetObjectItem(pParamObj, MESSAGE_DAY_INTERNAL);
	messuredata.nDayInternal =  atoi(pDayInternal->valuestring);

	//2.3 夜晚开始小时
	cJSON *pNightStartHourTime = cJSON_GetObjectItem(pParamObj, MESSAGE_NIGHT_START_HOUR);
	messuredata.nNightStartHour = atoi(pNightStartHourTime->valuestring);

	//2.4 夜晚开始分钟
	cJSON *pNightStartMinTime = cJSON_GetObjectItem(pParamObj, MESSAGE_NIGHT_START_MIN);
	messuredata.nNightStartMin = atoi(pNightStartMinTime->valuestring);

	//2.5 夜晚间隔时间
	cJSON *pNightInternal = cJSON_GetObjectItem(pParamObj, MESSAGE_NIGHT_INTERNAL);
	messuredata.nNigthInternal =  atoi(pNightInternal->valuestring);

	int nLen = sizeof(Format::MessureTime_t);
	pFormatData = new char[nLen + 1];
	CheckNullPtr(pFormatData);
	*(pFormatData + nLen) = '\0';
	memcpy(pFormatData, &messuredata, nLen);

	cJSON_Delete(pRoot);
	pRoot = NULL;
	
	return eRet;
}


int SerialDataParser::ParseRecordSum(IN const char *pData, OUT char *&pFormatData)
{
	E_Status_t eRet = SUCCESS;

	CheckNullPtr(pData);
	const char *pBuf = pData;

	cJSON *pRoot;
	if (pRoot = cJSON_Parse(pBuf), NULL == pRoot) return JSON_FORMAT_ERROR;

	//1、Tag类型
	cJSON *pTagType = cJSON_GetObjectItem(pRoot, TAG);
	CheckNullPtr(pTagType);

	Format::CmdKey_t cmdkey = {0};
	sprintf_s(cmdkey.szTag, sizeof(cmdkey.szTag), "%s", pTagType->valuestring);

	//2、cmd
	//cJSON *pRecordSum = cJSON_GetObjectItem(pRoot, READ_RECORD_SUM);
	//sprintf_s(cmdkey.szCmd, sizeof(cmdkey.szCmd), "%s", pRecordSum->valuestring);


	cJSON_Delete(pRoot);
	pRoot = NULL;
	return eRet;
}



__END_NAMESPACE(Parser)

