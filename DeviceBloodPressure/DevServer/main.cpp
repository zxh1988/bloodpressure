#include "stdafx.h"
#include "devserver.h"
#include <iostream>



int _tmain(int argc, _TCHAR* argv[])
{
	DevServer::IDevServer *pDevServer = new DevServer::CDevServer();

	//DevServer::IDevServer *pDevServer;
	char *pSerialDevContent = "{	\
							  \"TAG\":\"Serial\",	\
							  \"Param\":{	\
							  \"Com\":\"com9\",		\
							  \"Baud\":\"19200\",	\
							  \"Parity\":\"0\",	\
							  \"DataBits\":\"8\",		\
							  \"StopBits\":\"0\"		\
							  }	\
							  }";
	int nFormatType = 0;	//0:json,1:xml,2:protobuf

	char *pUsbDevContent = "{\"wVID\" : \"1155\", \"wPID\" : \"22352\"}";
	int nUsbDevType = 1;

	//0、初始化设备
	int nRet = pDevServer->InitDevice(pSerialDevContent);
	//pSerialDev->InitDevice(pUsbDevContent, nUsbDevType);
	//1、打开设备TODO:
	nRet = pDevServer->OpenDevice();

	//2、写设备TODO:/*HingMed_ABP表示星脉设备*/
	char *pWriteData = "{\
					   \"TAG\":\"HingMed_ABP\", \
					   \"Cmd\":\"MessureTime\", \
					   \"Param\":\{\
					   \"UserID\": \"112566\", \
					   \"UserName\": \"WSD\", \
					   \"Mode\": \"1\", \
							\"DayStartHourTime\":\"8\",	\
							\"DayStartMinTime\":\"30\",	\
							\"DayInternal\":\"30\",	\
							\"NightStartHourTime\":\"21\",	\
							\"NightStartMinTime\":\"0\",	\
							\"NightInternal\":\"30\"	\
							}		\
}";

	char *pRrcd = "{\"TAG\":\"RRCD\"\}";	//获取总记录数
	char *pRecordSum = "{\"TAG\":\"GetRecordSum\"}";	//获取总记录数
	char *pIdData = "{\
					\"TAG\":\"GetRecordIdData\", \
					\"Cmd\":\"RecordId\"}";		//获取指定的记录数据
	char *pClr = "{\"Cmd\":\"CLR\"}";
	//char *pRset = "{\"Cmd\":\"RSET\"}";
	//char *pHelp = "{\"Cmd\":\"HELP\"}";
	char *pResultData = NULL;
	int nResultSize = 0;
	//设置测量时间模式
	//nRet = pDevServer->WriteDevice(pWriteData, strlen(pWriteData), &pResultData, nResultSize);

	//读数据
	nRet = pDevServer->WriteDevice(pRrcd, strlen(pRrcd), &pResultData, nResultSize);
	//nRet = pDevServer->WriteDevice(pIdData, strlen(pWriteData));
	std::cout<<"Record Data=    "<<pResultData<<std::endl;

	//3、读设备TODO:
	char **pReadData = NULL;
	int nSize;
	nRet = pDevServer->ReadDevice(pReadData, nSize);


	//4、释放内存TODO:
	nRet = pDevServer->FreeMemory(&pResultData);

	//5、关闭设备TODO:
	nRet = pDevServer->CloseDevice();


	delete pDevServer;
	pDevServer = NULL;

	getchar();
	return 0;
}
