#include "StdAfx.h"
#include "usbdev.h"



__BEGIN_NAMESPACE(Dev)



CUsbDev::CUsbDev(void)
{
}


CUsbDev::~CUsbDev(void)
{
}

int CUsbDev::InitDevice(IN const char *szDevContent, IN int nDevType)
{
	return 0;
}

int CUsbDev::OpenDevice()
{
	return 0;
}

int CUsbDev::CloseDevice()
{

	return 0;
}


int CUsbDev::WriteDevice(IN const char *pData, IN int nSize, IN int nTimeOut)
{
	return 0;
}

int CUsbDev::ReadDevice(OUT char **pData, OUT int &nSize, IN int nTimeOut)
{
	return 0;
}


int CUsbDev::FreeMemory(IN char **pData)
{
	return 0;
}



__END_NAMESPACE(Dev)


