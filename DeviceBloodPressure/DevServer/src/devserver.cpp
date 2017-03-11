#include "StdAfx.h"
#include "devserver.h"
#include "datatype.h"
#include "devtemplete.h"
#include "typedef.h"

__BEGIN_NAMESPACE(DevServer)


IDevServer *CDevServer::m_pgInstance = NULL;


CDevServer::CDevServer(void):
	m_pJsonFormat(NULL)
{
	m_nDevType = Format::UNKNOW_TYPE;
	m_bDevInit = false;
}


CDevServer::~CDevServer(void)
{
	DELPTR(m_pJsonFormat);
	DELARR(m_pSerialParser);
	DELARR(m_pSerialProtocal);	
}

//static CDevServer *GetInstance()
IDevServer *CDevServer::GetInstance()
{
	if (NULL == m_pgInstance)
	{
		m_pgInstance = new CDevServer();
	}
	return m_pgInstance;
}

int CDevServer::InitDevice(IN const char *szDevContent)
{
	if (m_bDevInit) return SUCCESS;	
	m_bDevInit = true;

	E_Status_t nRet = UNKNOW_STATUS;	

	//1、创建JSON格式
	Format::CFormatFactory formatfactory;
	m_pJsonFormat = formatfactory.CreateFormat(Format::JSON_FORMAT_TYPE);
	CheckNullPtr(m_pJsonFormat);

	//2、解析格式
	char *pFormatData = NULL;
	int nDevType = Format::UNKNOW_TYPE;
	 
	nRet = (E_Status_t)m_pJsonFormat->ParseFormat(szDevContent, nDevType, pFormatData);
	CheckStatus(nRet);

	if (Format::SERIAL_DEV_TYPE == nDevType)	//串口设备
	{
		//3、serial解析
		Parser::CParserFactory parserfactory;
		m_pSerialParser = parserfactory.CreateParser(Parser::SERIAL_DATA_TYPE);
		//Parser::CParserFactory *pParserFactory;
		//m_pSerialParser = pParserFactory->GetInstance()->CreateParser(Parser::SERIAL_DATA_TYPE);
		CheckNullPtr(m_pSerialParser);
		nRet = (E_Status_t)m_pSerialParser->Parse(szDevContent, nDevType, pFormatData);
	
		//Format::SerialDevData_t *pSerialDevData = (Format::SerialDevData_t*)pFormatData;	

		//4、调用协议
		Protocal::CProtocalFactory protocalfactory;
		m_pSerialProtocal = protocalfactory.CreateProtocal(Protocal::SERIAL_DATA_TYPE);
		CheckNullPtr(m_pSerialParser);
		char *pProtocalData = NULL;
		int nCount;
		nRet = (E_Status_t)m_pSerialProtocal->ParseProtocal(pFormatData, nDevType, pProtocalData, nCount);

	}
	else if (Format::USB_DEV_TYPE == nDevType)	//USB设备
	{
	}

	m_nDevType = nDevType;

	
	DELPTR(pFormatData);
	return nRet;
}

int CDevServer::OpenDevice()
{
	E_Status_t nRet = UNKNOW_STATUS;

	if (Format::SERIAL_DEV_TYPE == m_nDevType)		//串口设备
	{
		int nDevType = Format::OPEN_DEVICE;		//打开设备
		char *pProtocalData = NULL;
		int nCount;
		nRet = (E_Status_t)m_pSerialProtocal->ParseProtocal(NULL, nDevType, pProtocalData, nCount);
	}
	else if (Format::USB_DEV_TYPE == m_nDevType)	//USB设备
	{
	}
	return nRet;
}


int CDevServer::WriteDevice(IN const char *pData, IN int nSize, \
	OUT char **pResultData,  OUT int &nResultSize)
{
	E_Status_t nRet = UNKNOW_STATUS;

	if (Format::SERIAL_DEV_TYPE == m_nDevType)	//串口设备
	{
		//1、解析格式
		char *pFormatData = NULL;
		int nType = Format::UNKNOW_TYPE;
		CheckNullPtr(m_pJsonFormat);
		nRet = (E_Status_t)m_pJsonFormat->ParseFormat(pData, nType, pFormatData);
		CheckStatus(nRet);

		//2、serial解析
		CheckNullPtr(m_pSerialParser);
		nRet = (E_Status_t)m_pSerialParser->Parse(pData, nType, pFormatData);
		CheckStatus(nRet);
			
		//3、调用协议
		char *pProtocalData = NULL;
		int nLen;
		nRet = (E_Status_t)m_pSerialProtocal->ParseProtocal(pFormatData, nType, pProtocalData, nLen);
		CheckStatus(nRet);

		//4、生成JSON格式数据
		nRet = (E_Status_t)m_pJsonFormat->CreateFormat(pProtocalData, nLen, nType, pResultData, nResultSize);
		CheckStatus(nRet);
		DELARR(pProtocalData);
	
	}
	else if (Format::USB_DEV_TYPE == m_nDevType)	//USB设备
	{
	}
	
	return nRet;
}

int CDevServer::ReadDevice(OUT char **pData, OUT int &nSize)
{
	E_Status_t nRet = UNKNOW_STATUS;
	if (Format::SERIAL_DEV_TYPE == m_nDevType)	//串口设备
	{	
		int nTimeOut = 10000;
		char *pReadData = NULL;
		//nRet = (E_Status_t)m_pSerialDev->ReadDevice(pData, nSize, nTimeOut);
		//3、调用协议
		//char *pProtocalData = NULL;
		//nRet = (E_Status_t)m_pSerialProtocal->ParseProtocal(pFormatData, nType, pProtocalData);

	}
	else if (Format::USB_DEV_TYPE == m_nDevType)	//USB设备
	{
	}
	
	return nRet;
}

int CDevServer::CloseDevice()
{
	E_Status_t nRet = UNKNOW_STATUS;

	if (Format::SERIAL_DEV_TYPE == m_nDevType)	//串口设备
	{
		//nRet = (E_Status_t)m_pSerialDev->CloseDevice();
		//3、调用协议
		int nType = Format::CLOSE_DEVICE;
		char *pProtocalData = NULL;
		int nCount;
		nRet = (E_Status_t)m_pSerialProtocal->ParseProtocal("", nType, pProtocalData, nCount);

	}
	else if (Format::USB_DEV_TYPE == m_nDevType)	//USB设备
	{
	}

	return nRet;
}

int CDevServer::FreeMemory(IN char **pData)
{
	E_Status_t nRet = SUCCESS;
	DELARR(*pData);
	return nRet;
}






__END_NAMESPACE(DevServer)

