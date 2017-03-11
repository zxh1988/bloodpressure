#pragma once
#include "typedef.h"

#define IN
#define OUT
#define MIN_LEN					32
#define MAX_LEN					512
#define SERIAL_PROTOCAL_LEN		256


__BEGIN_NAMESPACE(Format)



#pragma pack(push,1)

typedef enum E_DevType
{
	UNKNOW_TYPE		= -1,
	SERIAL_DEV_TYPE,			//串口设备
	USB_DEV_TYPE,				//USB设备
	OPEN_DEVICE,				//打开设备
	CLOSE_DEVICE,				//关闭设备	
	SERIAL_MESSAGE_PROTOCAL,	//设置测量时间模式
	SERIAL_RECORD_SUM,			//盒子记录条数
	SERIAL_SPECIFIED_RECORD,	//取得特定某一条记录
	SERIAL_RECORD_DATA,			//获取所有记录数据
	CLC_RECORD,					//设备内记录清零
	USB_RRCD_PROTOCAL

}E_DevType_t;



/*Com："名称"，
nBaud："波特率"，
parity："奇偶检验位"，
DataBits："数据位"，
StopBits:"停止位"*/
typedef struct tag_SerialDev
{
	char szCom[MIN_LEN];
	int nBaud;
	char nParity;
	int nDataBits;
	int nStopBits;

}SerialDevData_t;

typedef struct tag_SerialProtocal
{
	char nType;
	int nLen;
	char nCmd;
	char nParam;
	short nCrc;
}SerialProtocal_t;


typedef struct tag_UsbDev
{
	int wPID;
	int wVID;

}UsbDevData_t;



typedef struct tag_MessureTime
{
	char szTag[MAX_LEN];
	char szCmd[MIN_LEN];
	char szUserId[MAX_LEN];
	char szUserName[MAX_LEN];
	int nMode;
	int nDayStartHour;
	int nDayStartMin;
	int nDayInternal;
	int nNightStartHour;
	int nNightStartMin;
	int nNigthInternal;

}MessureTime_t;

typedef struct tag_UserIdAndName
{
	char szUserId[MIN_LEN];
	char szUserName[MIN_LEN];
}UserIdAndName_t;


typedef struct tag_RecordData
{	
	short nRecordId;	//记录对应的ID编号
	short nSys;			//舒张压
	short nDia;			//收缩压
	short nRate;		//心率
	char nYear;			//年
	char nMon;			//月
	char nDay;			//日
	char nHour;			//时
	char nMin;			//分
	char nEc;			//
	char nAuto;

}RecordData_t, *pRecordData_t;


typedef struct tag_CmdKey
{
	char szTag[MAX_LEN];
	char szCmd[MIN_LEN];

}CmdKey_t;


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


