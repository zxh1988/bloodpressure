#include "StdAfx.h"
#include "jsonwrap.h"
#include "datatype.h"
#include "cJSON.h"

#include <iostream>


__BEGIN_NAMESPACE(Format)



int CJsonWrap::ParseFormat(IN const char *pData, IN int nType, OUT char *&pFormatData)
{
	E_Status_t nRet = UNKNOW_STATUS;
	switch(nType)
	{
	case SERIAL_DEV_TYPE:
		nRet = (E_Status_t)ParseSerialDevFormat(pData, pFormatData);
		break;
	case USB_DEV_TYPE:
		nRet = (E_Status_t)ParseUsbDevFormat(pData, nType, pFormatData);
		break;
	case SERIAL_MESSAGE_PROTOCAL:

		break;
	case USB_RRCD_PROTOCAL:

		break;
	default:
		break;
	}
	return nRet;
}

int CJsonWrap::CreateFormat(IN const char *pData, OUT char *pFormatData)
{
	E_Status_t nRet = UNKNOW_STATUS;


	return nRet;
}

/*
{
"DeviceType":"Serial",
"param":{
	Com："名称"，
	nBaud："波特率"，
	parity："奇偶检验位"，
	DataBits："数据位"，
	StopBits:"停止位"
	}
}
*/
int CJsonWrap::ParseSerialDevFormat(IN const char *pData, OUT char *&pFormatData)
{
	E_Status_t nRet = UNKNOW_STATUS;

	CheckNullPtr(pData);
	const char *pBuf = pData;
	
	cJSON *pRoot;
	if (pRoot = cJSON_Parse(pBuf), NULL == pRoot) return FAIL;

	//1、device type
	cJSON *pTagType = cJSON_GetObjectItem(pRoot, TAG);
	CheckNullPtr(pTagType);
	SerialDevData_t devdata = {0};
	
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

	int nLen = sizeof(SerialDevData_t);
	pFormatData = new char[nLen + 1];
	CheckNullPtr(pFormatData);
	*(pFormatData + nLen) = '\0';
	memcpy(pFormatData, &devdata, nLen);
	
	cJSON_Delete(pRoot);
	pRoot = NULL;
	nRet = SUCCESS;

	return nRet;
}

int CJsonWrap::ParseUsbDevFormat(IN const char *pData, IN int nType, OUT char *&pFormatData)
{
	E_Status_t nRet = UNKNOW_STATUS;


	return nRet;
}




__END_NAMESPACE(Format)

