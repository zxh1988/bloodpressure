#pragma once
#include "typedef.h"

#define IN
#define OUT


__BEGIN_NAMESPACE(Format)

#define TAG							"TAG"
#define DEVICE_TYPE					"DeviceType"
#define DEVICE_HINGMED_NAME			"HingMed_ABP"	//星脉盒子
#define DEVICE_ACF_NAME				"ACF"			//艾讯

#define DEVICE_COM					"Serial"		//串口设备名称
#define DEVICE_USB					"USB"			//USB 设备名称
#define DEVICE_WRITE				"Write"			//写命令

#define READ_RECORD_SUM				"GetRecordSum"	//读取记录总数
#define READ_RECORD_DATA			"RRCD"			//读取记录数据



#define PARAM				"Param"
//串口类型
#define SERIAL_NAME			"Com"
#define SERIAL_BAUD			"Baud"
#define SERIAL_PARITY		"Parity"
#define SERIAL_DATABITS		"DataBits"
#define SERIAL_STOPBITS		"StopBits"


class CJsonWrap
{
public:
	CJsonWrap(void){};
	~CJsonWrap(void){};

public:
	int ParseFormat(IN const char *pData, IN int nType, OUT char *&pFormatData);
	int CreateFormat(IN const char *pData, OUT char *pFormatData);

protected:
	int ParseSerialDevFormat(IN const char *pData, OUT char *&pFormatData);
	int ParseSerialProtocalFormat(IN const char *pData, IN int nType, OUT char *&pFormatData);

	int ParseUsbDevFormat(IN const char *pData, IN int nType, OUT char *&pFormatData);
	int ParseUsbProtocalFormat(IN const char *pData, IN int nType, OUT char *&pFormatData);

	
	int CreateSerialDevFormat(IN const char *pData, OUT char *pFormatData);

};


__END_NAMESPACE(Format)

