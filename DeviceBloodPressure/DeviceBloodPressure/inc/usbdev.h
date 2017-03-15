#pragma once
#include "idevinterfacetype.h"

#include "Usb.h"




__BEGIN_NAMESPACE(Dev)




class CUsbDev :
	public IDevInterfaceType
{
public:
	CUsbDev(void);
	~CUsbDev(void);

public:
	int InitDevice(IN const char *szDevContent);
	int OpenDevice();
	int CloseDevice();
	int WriteDevice(IN const char *pData, IN int nSize, IN int nTimeOut = 10000);
	int ReadDevice(OUT char **pData, IN int nSize, IN int nTimeOut = 10000);
	int FreeMemory(IN char **pData);


private:
	CUsb m_HidUsb;
	int m_wPID;
	int m_wVID;

	bool m_bInit;

};


__END_NAMESPACE(Dev)

