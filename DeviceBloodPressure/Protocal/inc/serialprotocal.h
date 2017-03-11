#pragma once
#include "iprotocal.h"


//设备
#include "idevinterfacetype.h"
#include "devfactory.h"


__BEGIN_NAMESPACE(Protocal)


class CSerialProtocal : public IProtocal
{
public:
	CSerialProtocal(void);
	~CSerialProtocal(void);

	int ParseProtocal(IN const char *pData, IN int nType, OUT char *&pProtocalData, OUT int &nLen);

protected:
	int ParseMessageProtocal(IN const char *pData, OUT char *pProtocalData);
	int ParseRecordsDataProtocal(IN const char *pData, OUT char *&pProtocalData, OUT int &nLen);
	int ParseRecordSumProtocal(IN const char *pData, OUT char *pProtocalData, OUT int &nCount);

protected:
	int SetData( IN const char *pData, \
				IN int nLen,	\
				IN const char nCmd,\
				IN const char nMode, \
				OUT char &nPacketLen,	\
				IN bool IsParam = false);			//写设备数据
	int GetData(OUT char **pData, IN int nLen);		//读设备数据

	//操作设备函数
protected:
	int InitDevice(IN const char *szDevContent);
	int OpenDevice();
	int CloseDevice();
	int WriteDivice(const char *pData, IN int nSize, IN int nTimeOut);
	int ReadDivice(char **pData, IN int &nSize, IN int nTimeOut);
	int FreeMemory(IN char **pData);

protected:
	//与协议有关的命令函数
	int SetDataHand(OUT char &nPacketLen);	//握手
	int SetDataClr(OUT char &nPacketLen);	//数据清零
	
	//获取用户ID和名称
	int GetDataUserId(OUT char *&pReadData, OUT char &nPacketLen);
	int GetDataUserName(OUT char *&pReadData, OUT char &nPacketLen);
	//获取记录总数
	int GetDataRecordSum(OUT int &nSum, OUT char &nPacketLen);
	//读取指定记录号数据
	int GetDataSpecifiedRecordId(IN short sRecordId, OUT char *&pReadData, OUT char &nPacketLen);
	
	
	//CRC检验
	unsigned short Crc16cal(unsigned char p[], unsigned char num);

private:
	Dev::IDevInterfaceType *m_pSerialDev;	//设备
};


__END_NAMESPACE(Parser)


