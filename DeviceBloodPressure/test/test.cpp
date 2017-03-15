// test.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "libbpdevinterface.h"
#include <iostream>


int _tmain(int argc, _TCHAR* argv[])
{

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
	//0、初始化设备
	int nRet = BpInitDevice(pSerialDevContent);

	//1、打开设备TODO:
	nRet = BpOpenDevice();

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

	char *pResultData = NULL;
	int nResultSize = 0;
	//设置测量时间模式
	nRet = BpWriteDevice(pWriteData, strlen(pWriteData), &pResultData, nResultSize);

	//读数据
	nRet = BpWriteDevice(pRrcd, strlen(pRrcd), &pResultData, nResultSize);

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

