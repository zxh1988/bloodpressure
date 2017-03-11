#include "StdAfx.h"
#include "serialprotocal.h"

#include "datatype.h"

#include <time.h>


__BEGIN_NAMESPACE(Protocal)




CSerialProtocal::CSerialProtocal(void):m_pSerialDev(NULL)
{
	//设备初始化
	Dev::CDevFactory devfactory;
	Dev::E_DevInterfaceType_t eDevType = Dev::DEV_SERIAL_TYPE;
	m_pSerialDev = devfactory.CreateDevInterface(eDevType);
}


CSerialProtocal::~CSerialProtocal(void)
{
	DELPTR(m_pSerialDev);
}

//写设备数据
int CSerialProtocal::SetData(IN const char *pData, \
							IN int nLen,
							IN const char nCmd, \
							IN const char nMode, \
							OUT char &nPacketLen,	\
							IN bool IsParam)
{
	E_Status_t nRet = SUCCESS;
	CheckNullPtr(pData);

	nPacketLen = 0;

	char szPacket[SERIAL_PROTOCAL_LEN] = "\0";
	char *pPacketTemp = szPacket;
	//1、标识0x5A
	char nTag = 0x5A;
	*pPacketTemp = nTag; 
	pPacketTemp += 1;

	//2、字节长度(先空着，后面再存)
	pPacketTemp += 1;

	//3、命令
	*pPacketTemp = nCmd;
	pPacketTemp += 1;

	//4、数据参数（可有可无）
	char nParm = 0;
	if (IsParam)
	{
		nParm = nMode;
		*pPacketTemp = nParm;
		pPacketTemp += 1;
	}

	//对应的值
	char nDataLen = 0;
	//nDataLen = strlen(pData);
	nDataLen = nLen;
	memcpy(pPacketTemp, pData, nDataLen);

	/*长度=起始符+包字节数+命令+所带的参数+ CRC16*/
	nPacketLen = sizeof(char)/*nTag*/ + sizeof(char)/*nDataLen*/+ nDataLen + sizeof(char)/*nCmd*/ + nParm/*param*/ + sizeof(short)/*crc*/;
	szPacket[1] = nPacketLen;

	//5、计算检验和CRC
	unsigned short usWriteCrc16 = Crc16cal((unsigned char*)szPacket, nPacketLen - 2);
	szPacket[nPacketLen - 1] =  usWriteCrc16 & 0xff;
	szPacket[nPacketLen - 2] = (usWriteCrc16 >> 8) & 0xff;

	//6、写数据
	int nTimeOut = 1000;
	nRet = (E_Status_t)WriteDivice(szPacket, nPacketLen, nTimeOut);
	CheckStatus(nRet);

	//读取串口数据	
	char *pReadData = NULL;
	int nReadLen = GetData(&pReadData, nPacketLen);
	if (nReadLen <= 0) return READ_DEVICE_FAIL;

	//校验
	//unsigned short usReadCrc16 = Crc16cal((unsigned char*)pReadData, nReadLen - 2);
	unsigned short  CRC16Lo = 0xff;
	unsigned short CRC16Hi = 0xff;
	CRC16Lo = pReadData[nReadLen - 1] & 0xff;
	CRC16Hi = pReadData[nReadLen - 2] & 0xff;
	unsigned short usReadCrc16 = (CRC16Hi << 8) | CRC16Lo ;
	//unsigned short usReadCrc16 = CRC16Hi | (CRC16Lo << 8) ;
	if (usReadCrc16 != usWriteCrc16)
	{
		DELARR(pReadData);
		return READ_DEVICE_FAIL;
	}
	DELARR(pReadData);

	return nRet;
}



int CSerialProtocal::InitDevice(IN const char *szDevContent)
{
	return m_pSerialDev->InitDevice((char*)szDevContent);
}

int CSerialProtocal::OpenDevice()
{
	E_Status_t eRet = SUCCESS;
	eRet = (E_Status_t)m_pSerialDev->OpenDevice();
	CheckStatus(eRet);	
}

int CSerialProtocal::CloseDevice()
{
	return m_pSerialDev->CloseDevice();
}

int CSerialProtocal::WriteDivice(const char *pData, IN int nSize, IN int nTimeOut)
{
	return m_pSerialDev->WriteDevice(pData, nSize, nTimeOut);
}

int CSerialProtocal::ReadDivice(char **pData, IN int &nSize, IN int nTimeOut)
{
	return m_pSerialDev->ReadDevice(pData, nSize, nTimeOut);
}

int CSerialProtocal::FreeMemory(IN char **pData)
{
	return m_pSerialDev->FreeMemory(pData);
}

int CSerialProtocal::ParseProtocal(IN const char *pData, IN int nType, OUT char *&pParsedData, OUT int &nLen)
{
	E_Status_t nRet = UNKNOW_STATUS;
	switch(nType)
	{
	case Format::SERIAL_DEV_TYPE:
		//设备初始化
		nRet = (E_Status_t)InitDevice(pData);
		break;
	case Format::USB_DEV_TYPE:
		break;
	case Format::OPEN_DEVICE:
		nRet = (E_Status_t)OpenDevice();  
		break;
	case Format::CLOSE_DEVICE:
		nRet = (E_Status_t)CloseDevice();
		break;
	case Format::SERIAL_MESSAGE_PROTOCAL:
		nRet = (E_Status_t)ParseMessageProtocal(pData, pParsedData);
		break;
	case Format::SERIAL_RECORD_SUM:
		nRet = (E_Status_t)ParseRecordSumProtocal(pData, pParsedData, nLen);
		break;
	case Format::SERIAL_RECORD_DATA:
		nRet = (E_Status_t)ParseRecordsDataProtocal(pData, pParsedData, nLen);
		break;
	case Format::USB_RRCD_PROTOCAL:
		break;
	default:
		break;
	}

	return nRet;
}

int CSerialProtocal::ParseMessageProtocal(IN const char *pData, OUT char *pProtocalData)
{
	E_Status_t eRet = UNKNOW_STATUS;
	Format::MessureTime_t *pMessageTime = (Format::MessureTime_t *)pData;

	char nPacketLen = 0;
	char nReadLen = 0;
	char *pReadData = NULL;

	//0、握手 0x52
	eRet = (E_Status_t)SetDataHand(nPacketLen);
	//CheckStatus(eRet);
	//握手 0x52
	//char nPacketLen;
	eRet = (E_Status_t)SetDataHand(nPacketLen);
	CheckStatus(eRet);


	//清除设备记录数据0x57
	eRet = (E_Status_t)SetDataClr(nPacketLen);
	CheckStatus(eRet);

	//设置设备时间为当前系统时间0x4F
	time_t curtime = time(NULL);
	tm tim;
	localtime_s(&tim, &curtime);
	int year = tim.tm_year + 1900;
	int mon = tim.tm_mon + 1;
	int day = tim.tm_mday;
	int hour = tim.tm_hour;
	int min = tim.tm_min;
	//int nSec = tim.tm_sec;
	char szSysTime[MAX_LEN];
	memset(szSysTime, 0, sizeof(szSysTime));
	//sprintf_s(szSysTime, sizeof(szSysTime), "%d%d%d%d%d", year,mon,day,hour,min);

	szSysTime[0] = year - 2000;
	szSysTime[1] = mon;
	szSysTime[2] = day;
	szSysTime[3] = hour;
	szSysTime[4] = min;
	//2017310105只发送时间年后两位数(17310105)
	eRet = (E_Status_t)SetData(szSysTime, 5, 0x4F, 1, nPacketLen);
	CheckStatus(eRet);

	//设置是否使用屏幕显示0x46  0x01:启用 0x00:禁用：
	eRet = (E_Status_t)SetData("", 0, 0x46, 1, nPacketLen, true);
	CheckStatus(eRet);

	//设置用户名0x41
	eRet = (E_Status_t)SetData(pMessageTime->szUserName, strlen(pMessageTime->szUserName), 0x41, 0, nPacketLen);
	CheckStatus(eRet);

	//1、设置用户ID 0x42
	eRet = (E_Status_t)SetData(pMessageTime->szUserId, strlen(pMessageTime->szUserId),0x42, 0, nPacketLen);
	//eRet = (E_Status_t)SetUserId(pMessageTime->szUserId, nPacketLen);	
	CheckStatus(eRet);

	//2、设置测量模式 0x47
	nPacketLen = 0;
	eRet = (E_Status_t)SetData("", 0, 0x47, pMessageTime->nMode, nPacketLen, true);
	//eRet = (E_Status_t)SetMode(pMessageTime->nMode, nPacketLen);
	CheckStatus(eRet);

	//白天开始时间(0x48),
	char szDayTime[MIN_LEN] = "\0";
	szDayTime[0] = pMessageTime->nDayStartHour;
	szDayTime[1] = pMessageTime->nDayStartMin;	
	eRet = (E_Status_t)SetData(szDayTime, 2, 0x48, 0, nPacketLen);
	CheckStatus(eRet);

	//白天间隔时间(0x49)
	memset(szDayTime, 0, sizeof(szDayTime));
	szDayTime[0] = pMessageTime->nDayInternal;
	eRet = (E_Status_t)SetData(szDayTime, 1, 0x49, 0, nPacketLen);
	CheckStatus(eRet);

	
	//夜间开始时间(0x4A),
	memset(szDayTime, 0, sizeof(szDayTime));
	szDayTime[0] = pMessageTime->nNightStartHour;
	szDayTime[1] = pMessageTime->nNightStartMin;	
	eRet = (E_Status_t)SetData(szDayTime, 2, 0x4A, 0, nPacketLen);
	CheckStatus(eRet);

	//夜间间隔时间(0x4B)
	memset(szDayTime, 0, sizeof(szDayTime));
	szDayTime[0] = pMessageTime->nNigthInternal;
	eRet = (E_Status_t)SetData(szDayTime, 1, 0x4B, 0, nPacketLen);
	CheckStatus(eRet);
	
	//设置LCD显示的血压单位0x7A, ：0x00--kPa，0x01--mmHg
	eRet = (E_Status_t)SetData("", 0, 0x7A, 1, nPacketLen, true);
	CheckStatus(eRet);


	//通信结束0x90
	eRet = (E_Status_t)SetData("", 0, 0x90, 0, nPacketLen);
	CheckStatus(eRet);

	return eRet;
}

int CSerialProtocal::GetDataUserId(OUT char *&pUserId, OUT char &nPacketLen)
{
	E_Status_t eRet = UNKNOW_STATUS;

	nPacketLen = 0;
	char szPacket[SERIAL_PROTOCAL_LEN] = "\0";
	char *pPacketTemp = szPacket;
	//1、标识0x5A
	char nTag = 0x5A;
	*pPacketTemp = nTag; 
	pPacketTemp += 1;

	//2、字节长度(先空着，后面再存)
	pPacketTemp += 1;

	//3、取得设备用户ID命令 0x56
	char nCmd = 0x56;
	*pPacketTemp = nCmd;
	pPacketTemp += 1;

	//清零值
	char nDataLen = 0;

	/*长度=起始符+包字节数+命令+所带的参数+ CRC16*/
	nPacketLen = sizeof(char)/*nTag*/ + sizeof(char)/*nDataLen*/+ nDataLen + sizeof(char)/*nCmd*/ + 0/*param*/ + sizeof(short)/*crc*/;
	szPacket[1] = nPacketLen;

	//4、数据参数(无)

	//5、计算检验和CRC
	short nCheckSum = 0;
	unsigned short usWriteCrc16 = Crc16cal((unsigned char*)szPacket, nPacketLen - 2);
	szPacket[nPacketLen - 1] =  usWriteCrc16 & 0xff;
	szPacket[nPacketLen - 2] = (usWriteCrc16 >> 8) & 0xff;

	//6、写数据
	int nTimeOut = 1000;
	eRet = (E_Status_t)WriteDivice(szPacket, nPacketLen, nTimeOut);
	CheckStatus(eRet);

	//读取设备用户ID值,
	char *pReadData = NULL;
	nPacketLen = 0x30;
	int nReadLen = GetData(&pReadData, nPacketLen);
	if (nReadLen <= 0) return READ_DEVICE_FAIL;

	//校验
	unsigned short usReadCrc16 = Crc16cal((unsigned char*)pReadData, nReadLen - 2);
	unsigned short  CRC16Lo = 0xff;
	unsigned short CRC16Hi = 0xff;
	CRC16Lo = pReadData[nReadLen - 1] & 0xff;
	CRC16Hi = pReadData[nReadLen - 2] & 0xff;
	unsigned short usNewReadCrc16 = (CRC16Hi << 8) | CRC16Lo ;
	if (usReadCrc16 != usNewReadCrc16)
	{
		DELARR(pReadData);
		return READ_DEVICE_FAIL;
	}

	int nUserIdLen = nReadLen - 2/*CRC*/ - 3/*Tag+Len+Cmd*/;
	pUserId = new char[nUserIdLen + 1];
	//*(pUserId + nUserIdLen) = '\0';	
	memset(pUserId, 0, nUserIdLen+1);
	
	memcpy(pUserId, &pReadData[3], nUserIdLen);

	DELARR(pReadData);
	return eRet;
}

int CSerialProtocal::GetDataUserName(OUT char *&pUserName, OUT char &nPacketLen)
{
	E_Status_t eRet = UNKNOW_STATUS;

	nPacketLen = 0;

	char szPacket[SERIAL_PROTOCAL_LEN] = "\0";

	char *pPacketTemp = szPacket;
	//1、标识0x5A
	char nTag = 0x5A;
	*pPacketTemp = nTag; 
	pPacketTemp += 1;

	//2、字节长度(先空着，后面再存)
	pPacketTemp += 1;

	//3、取得设备用户ID命令 0x56
	char nCmd = 0x55;
	*pPacketTemp = nCmd;
	pPacketTemp += 1;

	//清零值
	char nDataLen = 0;

	/*长度=起始符+包字节数+命令+所带的参数+ CRC16*/
	nPacketLen = sizeof(char)/*nTag*/ + sizeof(char)/*nDataLen*/+ nDataLen + sizeof(char)/*nCmd*/ + 0/*param*/ + sizeof(short)/*crc*/;
	szPacket[1] = nPacketLen;

	//4、数据参数(无)

	//5、计算检验和CRC
	short nCheckSum = 0;
	unsigned short usWriteCrc16 = Crc16cal((unsigned char*)szPacket, nPacketLen - 2);
	szPacket[nPacketLen - 1] =  usWriteCrc16 & 0xff;
	szPacket[nPacketLen - 2] = (usWriteCrc16 >> 8) & 0xff;

	//6、写数据
	int nTimeOut = 1000;
	eRet = (E_Status_t)WriteDivice(szPacket, nPacketLen, nTimeOut);
	CheckStatus(eRet);

	//读取设备用户名值,长度0x0b
	char *pReadData = NULL;
	nPacketLen = 0x30;
	int nReadLen = GetData(&pReadData, nPacketLen);
	if (nReadLen <= 0) return READ_DEVICE_FAIL;

	//校验
	unsigned short usReadCrc16 = Crc16cal((unsigned char*)pReadData, nReadLen - 2);
	unsigned short  CRC16Lo = 0xff;
	unsigned short CRC16Hi = 0xff;
	CRC16Lo = pReadData[nReadLen - 1] & 0xff;
	CRC16Hi = pReadData[nReadLen - 2] & 0xff;
	unsigned short usNewReadCrc16 = (CRC16Hi << 8) | CRC16Lo ;
	if (usReadCrc16 != usNewReadCrc16)
	{
		DELARR(pReadData);
		return READ_DEVICE_FAIL;
	}
	
	int nUserNameLen = nReadLen - 2/*CRC*/ - 3/*Tag+Len+Cmd*/;
	pUserName = new char[nUserNameLen + 1];
	memset(pUserName, 0, nUserNameLen+1);
	memcpy(pUserName, &pReadData[3], nUserNameLen);

	DELARR(pReadData);
	return eRet;
}

int CSerialProtocal::ParseRecordSumProtocal(IN const char *pData, OUT char *pProtocalData, OUT int &nCount)
{
	E_Status_t eRet = UNKNOW_STATUS;
	
	nCount = 0;
	char nPacketLen;

	//0、握手 0x52
	eRet = (E_Status_t)SetDataHand(nPacketLen);
	//CheckStatus(eRet);
	//握手 0x52
	eRet = (E_Status_t)SetDataHand(nPacketLen);
	CheckStatus(eRet);

	//取得记录条数量命令0x53
	int nRecordSum = 0;
	eRet = (E_Status_t)GetDataRecordSum(nRecordSum, nPacketLen);
	CheckStatus(eRet);

	nCount = nRecordSum;

	return eRet;
}


int CSerialProtocal::ParseRecordsDataProtocal(IN const char *pData, OUT char *&pProtocalData, OUT int &nLen)
{
	E_Status_t eRet = UNKNOW_STATUS;

	nLen = 0;
	char nPacketLen;

	//0、握手 0x52
	eRet = (E_Status_t)SetDataHand(nPacketLen);
	//CheckStatus(eRet);
	//握手 0x52
	eRet = (E_Status_t)SetDataHand(nPacketLen);
	CheckStatus(eRet);

	//取得记录条数量命令0x53
	int nRecordSum = 0;
	eRet = (E_Status_t)GetDataRecordSum(nRecordSum, nPacketLen);
	CheckStatus(eRet);

	if (nRecordSum <= 0) return NO_RECORD_DATA;
	

	//nLen = 用户ID + 用户名称 + 记录个数
	int nUserDataLen = sizeof(Format::UserIdAndName_t);
	int nRecordDataLen = sizeof(Format::RecordData_t);
	nLen = nUserDataLen + nRecordSum * nRecordDataLen;
	pProtocalData = new char[nLen + 1];
	memset(pProtocalData, 0, nLen+1);
	char *pTemp = pProtocalData;


	//获取用户ID
	char *pUserId = NULL;
	eRet = (E_Status_t)GetDataUserId(pUserId, nPacketLen);
	CheckStatus(eRet);

	//获取用户名称
	char *pUserName = NULL;
	eRet = (E_Status_t)GetDataUserName(pUserName, nPacketLen);
	CheckStatus(eRet);

	Format::UserIdAndName_t userIdAndName = {0};
	sprintf_s(userIdAndName.szUserId, sizeof(userIdAndName.szUserId), "%s", pUserId);
	sprintf_s(userIdAndName.szUserName, sizeof(userIdAndName.szUserName), "%s", pUserName);

	memcpy(pTemp, &userIdAndName, nUserDataLen);	//赋值

	DELARR(pUserId);
	DELARR(pUserName);
	pTemp += nUserDataLen;

	for (int i = 0; i < nRecordSum; i++)
	{
		//读取指定数据命令0x54
		char *pReadData = NULL;
		eRet = (E_Status_t)GetDataSpecifiedRecordId(i, pReadData, nPacketLen);
		CheckStatus(eRet);

		Format::RecordData_t recorddata = {0};
		recorddata.nRecordId = i;
		recorddata.nSys = ((pReadData[3] << 8) & 0xff) | pReadData[4];	//舒张压
		recorddata.nDia = ((pReadData[5] << 8) & 0xff) | pReadData[6];	//收缩压
		recorddata.nRate = ((pReadData[7] << 8) & 0xff) | pReadData[8];	//心率
		recorddata.nYear = pReadData[9];
		recorddata.nMon = pReadData[10];
		recorddata.nDay = pReadData[11];
		recorddata.nHour = pReadData[12];
		recorddata.nMin = pReadData[13];
		recorddata.nEc = pReadData[14];
		recorddata.nAuto = pReadData[15];

		memcpy(pTemp, &recorddata, nRecordDataLen);
		pTemp += nRecordDataLen;

		DELARR(pReadData);
	}

	return eRet;
}


int CSerialProtocal::GetData(char **pData, IN int nLen)
{
	int nTimeOut = 2000;
	int nReadLen = ReadDivice(pData, nLen, nTimeOut);

	return nReadLen;
}

int CSerialProtocal::SetDataHand(OUT char &nPacketLen)
{
	E_Status_t nRet = SUCCESS;
	char szPacket[SERIAL_PROTOCAL_LEN] = "\0";

	char *pPacketTemp = szPacket;
	//1、标识0x5A
	char nTag = 0x5A;
	*pPacketTemp = nTag; 
	pPacketTemp += 1;

	//2、字节长度(先空着，后面再存)
	pPacketTemp += 1;

	//3、调用握手命令 0x52
	char nCmd = 0x52;
	*pPacketTemp = nCmd;
	pPacketTemp += 1;

	//握手值
	char nDataLen = strlen("RiXon");	//握手字符串
	memcpy(pPacketTemp, "RiXon", nDataLen);

	/*长度=起始符+包字节数+命令+所带的参数+ CRC16*/
	nPacketLen = sizeof(char)/*nTag*/ + sizeof(char)/*nDataLen*/+ nDataLen + sizeof(char)/*nCmd*/ + 0/*param*/ + sizeof(short)/*crc*/;
	szPacket[1] = nPacketLen;

	//4、数据参数(无)

	//5、计算检验和CRC
	short nCheckSum = 0;
	unsigned short usWriteCrc16 = Crc16cal((unsigned char*)szPacket, nPacketLen - 2);
	szPacket[nPacketLen - 1] =  usWriteCrc16 & 0xff;
	szPacket[nPacketLen - 2] = (usWriteCrc16 >> 8) & 0xff;

	//6、写数据
	int nTimeOut = 1000;
	nRet = (E_Status_t)WriteDivice(szPacket, nPacketLen, nTimeOut);
	CheckStatus(nRet);

	//读取握手数据	
	char *pReadData = NULL;
	int nReadLen = GetData(&pReadData, nPacketLen);
	if (nReadLen <= 0) return READ_DEVICE_FAIL;
	
	//校验
	//unsigned short usReadCrc16 = Crc16cal((unsigned char*)pReadData, nReadLen - 2);
	unsigned short  CRC16Lo = 0xff;
	unsigned short CRC16Hi = 0xff;
	CRC16Lo = pReadData[nReadLen - 1] & 0xff;
	CRC16Hi = pReadData[nReadLen - 2] & 0xff;
	//unsigned short usReadCrc16 = (CRC16Hi << 8) | CRC16Lo ;
	unsigned short usReadCrc16 = CRC16Hi | (CRC16Lo << 8) ;
	if (usReadCrc16 != usWriteCrc16)
	{
		DELARR(pReadData);
		return READ_DEVICE_FAIL;
	}

	DELARR(pReadData);

	return nRet;
}

int CSerialProtocal::GetDataRecordSum(OUT int &nSum, OUT char &nPacketLen)
{
	E_Status_t eRet = SUCCESS;

	nSum = 0;
	nPacketLen = 0;

	char szPacket[SERIAL_PROTOCAL_LEN] = "\0";

	char *pPacketTemp = szPacket;
	//1、标识0x5A
	char nTag = 0x5A;
	*pPacketTemp = nTag; 
	pPacketTemp += 1;

	//2、字节长度(先空着，后面再存)
	pPacketTemp += 1;

	//3、取得设备记录条数命令 0x53
	char nCmd = 0x53;
	*pPacketTemp = nCmd;
	pPacketTemp += 1;

	//清零值
	char nDataLen = 0;

	/*长度=起始符+包字节数+命令+所带的参数+ CRC16*/
	nPacketLen = sizeof(char)/*nTag*/ + sizeof(char)/*nDataLen*/+ nDataLen + sizeof(char)/*nCmd*/ + 0/*param*/ + sizeof(short)/*crc*/;
	szPacket[1] = nPacketLen;

	//4、数据参数(无)

	//5、计算检验和CRC
	short nCheckSum = 0;
	unsigned short usWriteCrc16 = Crc16cal((unsigned char*)szPacket, nPacketLen - 2);
	szPacket[nPacketLen - 1] =  usWriteCrc16 & 0xff;
	szPacket[nPacketLen - 2] = (usWriteCrc16 >> 8) & 0xff;

	//6、写数据
	int nTimeOut = 1000;
	eRet = (E_Status_t)WriteDivice(szPacket, nPacketLen, nTimeOut);
	CheckStatus(eRet);

	//读取记录总数,这里下位机会多返回两个字节的长度,所以nPacketLen长度加2
	char *pReadData = NULL;
	nPacketLen = nPacketLen + 2;
	int nReadLen = GetData(&pReadData, nPacketLen);
	if (nReadLen <= 0) return READ_DEVICE_FAIL;

	//校验
	unsigned short usReadCrc16 = Crc16cal((unsigned char*)pReadData, nReadLen - 2);
	unsigned short  CRC16Lo = 0xff;
	unsigned short CRC16Hi = 0xff;
	CRC16Lo = pReadData[nReadLen - 1] & 0xff;
	CRC16Hi = pReadData[nReadLen - 2] & 0xff;
	unsigned short usNewReadCrc16 = (CRC16Hi << 8) | CRC16Lo ;
	if (usReadCrc16 != usNewReadCrc16)
	{
		DELARR(pReadData);
		return READ_DEVICE_FAIL;
	}

	//取得记录总数
	char nSumLo = pReadData[nPacketLen - 3];
	char nSumHi = pReadData[nPacketLen - 4];
	nSum = ((nSumHi << 8) | nSumLo) & 0xff;

	DELARR(pReadData);

	return eRet;
}

int CSerialProtocal::GetDataSpecifiedRecordId(IN short snRecordId, OUT char *&pReadData, OUT char &nPacketLen)
{
	E_Status_t eRet = SUCCESS;
	
	nPacketLen = 0;
	char szPacket[SERIAL_PROTOCAL_LEN] = "\0";

	char *pPacketTemp = szPacket;
	//1、标识0x5A
	char nTag = 0x5A;
	*pPacketTemp = nTag; 
	pPacketTemp += 1;

	//2、字节长度(先空着，后面再存)
	pPacketTemp += 1;

	//3、取得设备指数据定记录命令 0x54
	char nCmd = 0x54;
	*pPacketTemp = nCmd;
	pPacketTemp += 1;

	//清零值
	char nDataLen = 2;

	/*长度=起始符+包字节数+命令+所带的参数+ CRC16*/
	nPacketLen = sizeof(char)/*nTag*/ + sizeof(char)/*nDataLen*/+ nDataLen + sizeof(char)/*nCmd*/ + 0/*param*/ + sizeof(short)/*crc*/;
	szPacket[1] = nPacketLen;

	//4、数据参数(有)4
	char nRecordIdLo = (snRecordId & 0xff);
	char nRecordIdHi = (snRecordId >> 8) & 0xff;
	szPacket[3] = nRecordIdHi;
	szPacket[4] = nRecordIdLo;

	//5、计算检验和CRC
	short nCheckSum = 0;
	unsigned short usWriteCrc16 = Crc16cal((unsigned char*)szPacket, nPacketLen - 2);
	szPacket[nPacketLen - 1] =  usWriteCrc16 & 0xff;
	szPacket[nPacketLen - 2] = (usWriteCrc16 >> 8) & 0xff;

	//6、写数据
	int nTimeOut = 1000;
	eRet = (E_Status_t)WriteDivice(szPacket, nPacketLen, nTimeOut);
	CheckStatus(eRet);

	//读取指定记录数据
	//char *pReadData = NULL;
	nPacketLen = 0x12;		//指定长度为18
	int nReadLen = GetData(&pReadData, nPacketLen);
	if (nReadLen <= 0) return READ_DEVICE_FAIL;

	//校验
	unsigned short usReadCrc16 = Crc16cal((unsigned char*)pReadData, nReadLen - 2);
	unsigned short  CRC16Lo = 0xff;
	unsigned short CRC16Hi = 0xff;
	CRC16Lo = pReadData[nReadLen - 1] & 0xff;
	CRC16Hi = pReadData[nReadLen - 2] & 0xff;
	unsigned short usNewReadCrc16 = (CRC16Hi << 8) | CRC16Lo ;
	if (usReadCrc16 != usNewReadCrc16)
	{
		DELARR(pReadData);
		return READ_DEVICE_FAIL;
	}
	//DELARR(pReadData);

	return eRet;
}


int CSerialProtocal::SetDataClr(OUT char &nPacketLen)		//数据清零
{
	E_Status_t eRet = SUCCESS;
	char szPacket[SERIAL_PROTOCAL_LEN] = "\0";

	char *pPacketTemp = szPacket;
	//1、标识0x5A
	char nTag = 0x5A;
	*pPacketTemp = nTag; 
	pPacketTemp += 1;

	//2、字节长度(先空着，后面再存)
	pPacketTemp += 1;

	//3、数据清零命令 0x57
	char nCmd = 0x57;
	*pPacketTemp = nCmd;
	pPacketTemp += 1;

	//清零值
	char nDataLen = 0;

	/*长度=起始符+包字节数+命令+所带的参数+ CRC16*/
	nPacketLen = sizeof(char)/*nTag*/ + sizeof(char)/*nDataLen*/+ nDataLen + sizeof(char)/*nCmd*/ + 0/*param*/ + sizeof(short)/*crc*/;
	szPacket[1] = nPacketLen;

	//4、数据参数(无)

	//5、计算检验和CRC
	short nCheckSum = 0;
	unsigned short usWriteCrc16 = Crc16cal((unsigned char*)szPacket, nPacketLen - 2);
	szPacket[nPacketLen - 1] =  usWriteCrc16 & 0xff;
	szPacket[nPacketLen - 2] = (usWriteCrc16 >> 8) & 0xff;

	//6、写数据
	int nTimeOut = 1000;
	eRet = (E_Status_t)WriteDivice(szPacket, nPacketLen, nTimeOut);
	CheckStatus(eRet);

	//读取清除数据
	char *pReadData = NULL;
	nPacketLen = nPacketLen + 2;
	int nReadLen = GetData(&pReadData, nPacketLen);
	if (nReadLen <= 0) return READ_DEVICE_FAIL;

	//校验
	unsigned short usReadNewCrc16 = Crc16cal((unsigned char*)pReadData, nReadLen - 2);
	unsigned short  CRC16Lo = 0xff;
	unsigned short CRC16Hi = 0xff;
	CRC16Lo = pReadData[nReadLen - 1] & 0xff;
	CRC16Hi = pReadData[nReadLen - 2] & 0xff;
	unsigned short usReadCrc16 = (CRC16Hi << 8) | CRC16Lo ;
	//unsigned short usReadCrc16 = CRC16Hi | (CRC16Lo << 8) ;
	if (usReadCrc16 != usReadNewCrc16)
	{
		DELARR(pReadData);
		return READ_DEVICE_FAIL;
	}

	DELARR(pReadData);

	return eRet;
}





/******************************************************************************
**函数名:Crc16cal
**入  口:
**返  回:
**功  能: 16位奇偶校验
*******************************************************************************/

unsigned short CSerialProtocal::Crc16cal(unsigned char p[], unsigned char num)
{
	unsigned char CL = 1;
	unsigned char CH = 0xA0;
	unsigned char SaveHi;
	unsigned char Flag;

	unsigned char  CRC16Lo = 0xff;
	unsigned char CRC16Hi = 0xff;

	unsigned char *pData = p;
	if (NULL == pData) return (unsigned short) (((CRC16Hi << 8) | CRC16Lo) & 0xffffUL);

	CRC16Lo = 0xff;
	unsigned char SaveLo;
	unsigned char  i;
	for(i = 0; i < num; i++)
	{
		CRC16Lo = (unsigned char)(CRC16Lo ^ p[i]);
		for(Flag = 0; Flag < 8; Flag++)
		{
			SaveHi = CRC16Hi;
			SaveLo = CRC16Lo;
			CRC16Hi = (unsigned char)(CRC16Hi / 2);
			CRC16Lo = (unsigned char)(CRC16Lo / 2);
			if((SaveHi & 0x1) == 1)
			{
				CRC16Lo = (unsigned char)(CRC16Lo | 0x80);
			}
			if((SaveLo & 0x1) == 1)
			{
				CRC16Hi = (unsigned char)(CRC16Hi ^ CH);
				CRC16Lo = (unsigned char)(CRC16Lo ^ CL);
			}
		}
	}

	return (unsigned short) (((CRC16Hi << 8) | CRC16Lo) & 0xffffUL);
}





__END_NAMESPACE(Parser)

