#pragma once
#include "iparserinterface.h"

#include "typedef.h"
#include "datatype.h"


#define IN
#define OUT



#define TAG							"TAG"
#define CMD							"Cmd"
#define PARAM						"Param"

#define DEVICE_TYPE					"DeviceType"
#define DEVICE_HINGMED_NAME			"HingMed_ABP"	//星脉盒子
#define DEVICE_ACF_NAME				"ACF"			//艾讯

#define DEVICE_COM					"Serial"		//串口设备名称
#define DEVICE_USB					"USB"			//USB设备名称
#define DEVICE_WRITE				"Write"			//写命令

#define READ_RECORD_SUM				"GetRecordSum"	//读取记录总数
#define READ_RECORD_DATA			"RRCD"			//读取记录数据


//串口类型字定义
#define SERIAL_NAME			"Com"
#define SERIAL_BAUD			"Baud"
#define SERIAL_PARITY		"Parity"
#define SERIAL_DATABITS		"DataBits"
#define SERIAL_STOPBITS		"StopBits"

//测量时间字符串定义
#define MESSAGE_USER_ID				"UserID"			//用户ID
#define MESSAGE_USER_NAME			"UserName"			//用户名称
#define MESSAGE_MODE				"Mode"				//模式：0表示固定模式，1表示标准模式
#define MESSAGE_DAY_START_HOUR		"DayStartHourTime"	//白天开始小时
#define MESSAGE_DAY_START_MIN		"DayStartMinTime"	//白天开始分钟
#define MESSAGE_DAY_INTERNAL		"DayInternal"		//白天间隔时间
#define MESSAGE_NIGHT_START_HOUR	"NightStartHourTime"//夜晚开始小时
#define MESSAGE_NIGHT_START_MIN		"NightStartMinTime"	//夜晚开始分钟
#define MESSAGE_NIGHT_INTERNAL		"NightInternal"		//夜晚间隔时间




__BEGIN_NAMESPACE(Parser)



class SerialDataParser :
	public IParserInterface
{
public:
	SerialDataParser(void);
	~SerialDataParser(void);

public:
	int Parse(IN const char *pData, IN int nType, OUT char *&pParsedData);

	int ParseSerialDevFormat(IN const char *pData, OUT char *&pFormatData);
	int ParseMessureTime(IN const char *pData, OUT char *&pFormatData);
	int ParseRecordSum(IN const char *pData, OUT char *&pFormatData);


};


__END_NAMESPACE(Parser)
