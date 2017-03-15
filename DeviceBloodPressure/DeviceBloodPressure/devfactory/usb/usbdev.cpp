#include "StdAfx.h"
#include "usbdev.h"

#include "datatype.h"



__BEGIN_NAMESPACE(Dev)



CUsbDev::CUsbDev(void) : m_bInit(false)
{
}


CUsbDev::~CUsbDev(void)
{
}

int CUsbDev::InitDevice(IN const char *szDevContent)
{
	E_Status_t nRet = SUCCESS;	

	CheckNullPtr(szDevContent);
	const char *pBuf = szDevContent;

	Format::UsbDevData_t *pSerialDevData = (Format::UsbDevData_t*)pBuf;
	m_wPID = pSerialDevData->wPID;
	m_wVID = pSerialDevData->wVID;

	//调用设备接口
	bool bRet = m_HidUsb.OpenUsb(m_wVID, m_wPID);
	if (!bRet) return DISCONNECT;

	m_bInit = true;
	return nRet;
}

int CUsbDev::OpenDevice()
{
	E_Status_t nRet = SUCCESS;
	if (!m_bInit)
	{
		bool bRet = m_HidUsb.OpenUsb(m_wVID, m_wPID);
		if (!bRet) return DISCONNECT;
	}
	return nRet;
}

int CUsbDev::CloseDevice()
{
	E_Status_t nRet = SUCCESS;

	bool bRet = m_HidUsb.CloseUsb();
	if (!bRet) return FAIL;
	return nRet;
}


int CUsbDev::WriteDevice(IN const char *pData, IN int nSize, IN int nTimeOut)
{
	E_Status_t nRet = SUCCESS;

	bool bRet = m_HidUsb.WriteToUsb(pData, nSize);
	if (!bRet) return WRITE_DEVICE_FAIL;

	return nRet;
}

int CUsbDev::ReadDevice(OUT char **pData, IN int nSize, IN int nTimeOut)
{
	membuffer rxBuf;
	membuffer_init(&rxBuf);
	int nRet = m_HidUsb.ReadFromUsb(&rxBuf);
	if (NULL == rxBuf.buf) return READ_DEVICE_FAIL;
	
	*pData = new char[rxBuf.length+1];
	*(*pData + rxBuf.length) = '\0';
	if (NULL != pData)
	{
		memcpy(*pData, rxBuf.buf, rxBuf.length);
	}
	nSize = rxBuf.length;

	membuffer_destroy(&rxBuf);
	return nRet;
}


int CUsbDev::FreeMemory(IN char **pData)
{
	DELARR(pData);
	return SUCCESS;
}



__END_NAMESPACE(Dev)


