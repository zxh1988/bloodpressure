// test.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "libbpdevinterface.h"
#include <iostream>


int _tmain(int argc, _TCHAR* argv[])
{

	char *pSerialDevContent = "{	\
							   \"TAG\":\"HingMed_ABP\", \
							  \"Cmd\":\"Serial\",	\
							  \"Param\":{	\
							  \"Com\":\"com9\",		\
							  \"Baud\":\"19200\",	\
							  \"Parity\":\"0\",	\
							  \"DataBits\":\"8\",		\
							  \"StopBits\":\"0\"		\
							  }	\
							  }";

	char *pUsbDevContent = "{	\
						    \"TAG\":\"ACF\", \
							  \"Cmd\":\"Usb\",	\
							  \"Param\":{	\
							  \"wVID\":\"1155\",		\
							  \"wPID\":\"22352\"	\
							  }	\
							  }";
	//0、初始化设备
	int nRet = BpInitDevice(pSerialDevContent);

	//1、打开设备TODO:
	nRet = BpOpenDevice();

	//2、写设备TODO:/*HingMed_ABP表示星脉设备*/
	char *pWriteData = "{\
					   \"TAG\":\"HingMed_ABP\", \
					   \"Cmd\":\"SET\", \
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

	char *pRrcd = "{\"TAG\":\"HingMed_ABP\",\
					\"Cmd\":\"RRCD\"\}";	//获取总记录数


	//艾讯设备
	//1=自动表数据格式
	char *pJson = "{ \
				  \"TAG\":\"ACF\", \
				  \"Cmd\":\"SET\", \
				  \"Param\": { \
				  \"ID\": \"16441\", \
				  \"Mode\": \"1\", \
				  \"TimeList\": [ \
				  { \
				  \"Hour\": \"8\", \
				  \"Min\":\"10\", \
				  \"Interval\": \"15\" \
				  }, \
				  { \
				  \"Hour\": \"7\", \
				  \"Min\":\"16\", \
				  \"Interval\": \"30\" \
				  } \
				  , \
				  { \
				  \"Hour\": \"0\", \
				  \"Min\":\"0\", \
				  \"Interval\": \"0\" \
				  } \
				  , \
				  { \
				  \"Hour\": \"0\", \
				  \"Min\":\"0\", \
				  \"Interval\": \"0\" \
				  } \
				  , \
				  { \
				  \"Hour\": \"0\", \
				  \"Min\":\"0\", \
				  \"Interval\": \"0\" \
				  } \
				  , \
				  { \
				  \"Hour\": \"0\", \
				  \"Min\":\"0\", \
				  \"Interval\": \"0\" \
				  } \
				  ] \
				  } \
				  }";

	//2=自动间隔数据格式
	char *pAutoJson = "{ \
					  \"TAG\":\"ACF\", \
					  \"Cmd\":\"SET\", \
					  \"Param\": { \
					  \"ID\": \"16441\", \
					  \"Mode\": \"2\" \
					  } \
					  }";

	//3=手动时间间隔
	char *pHandleJson = "{ \
						\"TAG\":\"ACF\", \
						\"Cmd\":\"SET\", \
						\"Param\": { \
						\"ID\": \"16441\", \
						\"Mode\": \"3\", \
						\"Day\": \"30\", \
						\"Night\": \"60\"  \
						} \
						}";

	char *pResultData = NULL;
	int nResultSize = 0;
	//设置测量时间模式
	nRet = BpWriteDevice(pWriteData, strlen(pWriteData), &pResultData, nResultSize);
	printf("pWriteData size = %d, pWriteData Data= %s", strlen(pWriteData),pWriteData);
	//读数据
	//nRet = BpWriteDevice(pRrcd, strlen(pRrcd), &pResultData, nResultSize);
	//printf("nResultSize = %d, Record Data= %s", nResultSize,pResultData);

	//3、读设备TODO:
	char **pReadData = NULL;
	int nSize;
	nRet = BpReadDevice(pReadData, nSize);

	//4、释放内存TODO:
	nRet = BpFreeMemory(&pResultData);

	//5、关闭设备TODO:
	nRet = BpCloseDevice();


	getchar();
	return 0;
}

