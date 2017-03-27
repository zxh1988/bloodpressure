#pragma once
#include "typedef.h"

#define IN
#define OUT


#define MIN_LEN					32
#define MAX_LEN					512
#define SERIAL_PROTOCAL_LEN		256




#define TAG					"TAG"
#define CMD					"Cmd"
#define PARAM				"Param"

#define MEASURE_MODE		"SET"			//设置测量模式
#define HELP_COMMAND		"HELP"			//帮助命令
#define RESET_COMMAND		"RESET"			//复位命令
#define SDT_COMMAND			"SDT"			//设置时间命令
#define ST_COMMAND			"ST"			//设置日期命令
#define RDA_COMMAND			"RDA"			//数模转换命令
#define RDF_COMMAND			"RDF"
#define RRCD_COMMAND		"RRCD"			//读取数据命令
#define VER_COMMAND			"VEC"
#define CLR_COMMAND			"CLR"			//清除数据命令




#define DEVICE_HINGMED_NAME			"HingMed_ABP"	//星脉盒子名称
#define DEVICE_ACF_NAME				"ACF"			//艾讯盒子名称


#define READ_RECORD_SUM				"GetRecordSum"	//读取记录总数
#define READ_RECORD_DATA			"RRCD"			//读取记录数据








__BEGIN_NAMESPACE(Format)



#pragma pack(push,1)


typedef struct 
{
	const char *szName;
	int id;

}str_int_entry, *pstr_int_entry;



typedef enum E_Dev_Type
{
	UNKNOW_DEV_TYPE		= -1,
	HingMed_ABP_DEVICE,			//星脉设备
	ACF_DEVICE					//艾讯设备
}Dev_Type_t;



typedef enum E_DataType
{
	UNKNOW_TYPE		= -1,	
	HingMed_ABP_DEV_INIT,		//星脉串口设备
	HingMed_ABP_MEASURE_MODE,	//设置星脉测量时间模式	
	HingMed_ABP_GET_RECORD_DATA,			//获取所有记录数据
	HingMed_ABP_CLC_RECORD_DATA,			//设备内记录清零
	ACF_DEV_INIT,				//艾讯USB设备	
	ACF_MEASURE_MODE,			//设置艾讯测量时间模式
	ACF_GET_RECORD_DATA,			//获取所有记录数据
	ACF_CLC_RECORD_DATA,			//设备内记录清零
	OPEN_DEVICE,				//打开设备
	CLOSE_DEVICE				//关闭设备	

}E_DataType_t;



typedef struct tag_Head_Param
{
	char szTag[MAX_LEN];
	char szCmd[MIN_LEN];

}HeadParam_t, *pHeadParam_t;


/*Com："名称"，
nBaud："波特率"，
parity："奇偶检验位"，
DataBits："数据位"，
StopBits:"停止位"*/
typedef struct tag_SerialDev
{
	HeadParam_t Head;
	char szCom[MIN_LEN];
	int nBaud;
	char nParity;
	int nDataBits;
	int nStopBits;

}SerialDevData_t;


//HID Usb类型
typedef struct tag_UsbDev
{
	HeadParam_t Head;
	int wPID;
	int wVID;

}UsbDevData_t;


typedef struct tag_MessureTime
{
	HeadParam_t Head;
	char szUserId[MAX_LEN];
	char szUserName[MAX_LEN];
	int nMode;
	int nDayStartHour;
	int nDayStartMin;
	int nDayInternal;
	int nNightStartHour;
	int nNightStartMin;
	int nNigthInternal;

}MessureTime_t, *pMessureTime_t;

typedef struct tag_UserIdAndName
{
	char szUserId[MIN_LEN];
	char szUserName[MIN_LEN];

}UserIdAndName_t, *pUserIdAndName_t;


typedef struct tag_RecordData
{	
	short nRecordId;	//记录对应的ID编号
	unsigned short nSys;			//舒张压
	unsigned short nDia;			//收缩压
	unsigned short nRate;		//心率
	char nYear;			//年
	char nMon;			//月
	char nDay;			//日
	char nHour;			//时
	char nMin;			//分
	char nEc;			//
	char nAuto;

}RecordData_t, *pRecordData_t;


typedef struct {
	int nId;
	short nSys;			//舒张压
	short nDia;			//收缩压
	short nRate;		//心率
	char nYear;			//年
	char nMon;			//月
	char nDay;			//日
	char nHour;			//时
	char nMin;			//分
	char nSec;			//秒

}acf_record_data_t;


#pragma pack(pop,1)


template<class T>
class CDataType
{
public:
	CDataType(void) {};
	~CDataType(void) {};

public:
	int ParseFormat(IN const char *pData, OUT T *&pFormatData)
	{
		E_Status_t nRet = UNKNOW_STATUS;
		const char *pBuf = pData;

		if (sizeof(SerialDevData_t) == sizeof(T))
		{
			int aa = 0;

		}
		else if (sizeof(UsbDevData_t) == sizeof(T))
		{

		}
		else{}
		return nRet;
	}

	int CreateFormat(IN const char *pData, OUT T *pFormatData)
	{

		return 0;
	}

	/*int ParseSerialDevFormat(IN const char *pData, OUT char *&pFormatData);
	int ParseUsbDevFormat(IN const char *pData, OUT char *&pFormatData);

	int CreateSerialDevFormat(IN const char *pData, OUT char *pFormatData);
	int CreateUsbDevFormat(IN const char *pData, OUT char *pFormatData);*/
	


};




__END_NAMESPACE(Format)


