#pragma once
#include "iparserinterface.h"

#include "typedef.h"
#include "datatype.h"


#define IN
#define OUT


#ifdef WIN32
#define strncasecmp strnicmp
#else
/* Other systems have strncasecmp */
#endif



#define DEVICE_COM			"Serial"		//串口设备名称


//串口类型字定义
#define SERIAL_NAME			"Com"
#define SERIAL_BAUD			"Baud"
#define SERIAL_PARITY		"Parity"
#define SERIAL_DATABITS		"DataBits"
#define SERIAL_STOPBITS		"StopBits"


//星脉测量时间字符串定义
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
	SerialDataParser(void){};
	~SerialDataParser(void){};

public:
	int Parse(IN const char *pData, IN int &nType, OUT char *&pParsedData);

protected:
	int ParseSerialDevFormat(IN const char *pData, OUT char *&pFormatData);
	int ParseMessureTimeMode(IN const char *pData, OUT char *&pFormatData);


};


__END_NAMESPACE(Parser)
