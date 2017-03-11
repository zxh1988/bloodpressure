#include "StdAfx.h"
#include "SerialDev.h"
#include "iformat.h"
#include "devfactory.h"
#include "datatype.h"


__BEGIN_NAMESPACE(Dev)




CSerialDev::CSerialDev(void)
{
	m_bInit = false;
	memset(m_szCom, 0, sizeof(m_szCom));
	m_nBaud	= 0;
	m_nParity = 0;
	m_nDataBits	= 0;
	m_nStopBits	= 0;
}


CSerialDev::~CSerialDev(void)
{
}

int CSerialDev::InitDevice(IN const char *szDevContent)
{
	E_Status_t nRet = SUCCESS;	

	CheckNullPtr(szDevContent);
	const char *pBuf = szDevContent;

	Format::SerialDevData_t *pSerialDevData = (Format::SerialDevData_t*)pBuf;
	sprintf_s(m_szCom, sizeof(pSerialDevData->szCom), pSerialDevData->szCom);
	m_nBaud = pSerialDevData->nBaud;
	m_nParity = pSerialDevData->nParity;
	m_nDataBits = pSerialDevData->nDataBits;
	m_nStopBits = pSerialDevData->nStopBits;

	//调用设备接口
	bool bRet = serialcom.InitPort(m_szCom, m_nBaud, m_nParity, m_nDataBits, m_nStopBits);
	if (!bRet) return DISCONNECT;

	m_bInit = true;

	return nRet;
}

int CSerialDev::OpenDevice()
{
	E_Status_t nRet = SUCCESS;
	//if (!m_bInit)
	//{
		bool bRet = serialcom.InitPort(m_szCom, m_nBaud, m_nParity, m_nDataBits, m_nStopBits);
		if (!bRet) return DISCONNECT;
	//}
	return nRet;
}

int CSerialDev::CloseDevice()
{
	E_Status_t nRet = SUCCESS;

	bool bRet = serialcom.ClosePort();
	if (!bRet) return FAIL;
	return nRet;
}


int CSerialDev::WriteDevice(IN const char *pData, IN int nSize, IN int nTimeOut)
{
	E_Status_t nRet = SUCCESS;

	bool bRet = serialcom.WriteToPort(pData, nSize, nTimeOut);
	if (!bRet) return WRITE_DEVICE_FAIL;

	return nRet;
}

int CSerialDev::ReadDevice(OUT char **pData, IN int nSize, IN int nTimeOut)
{
	int nRet = serialcom.ReadFromPort(pData, nSize, nTimeOut);
	return nRet;
}

int CSerialDev::FreeMemory(IN char **pData)
{
	DELARR(pData);
	return SUCCESS;
}



__END_NAMESPACE(Dev)


