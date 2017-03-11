#pragma once
#include "idevinterfacetype.h"


__BEGIN_NAMESPACE(Dev)




class CUsbDev :
	public IDevInterfaceType
{
public:
	CUsbDev(void);
	~CUsbDev(void);

public:
	int InitDevice(IN const char *szDevContent, IN int nType = 0);
	int OpenDevice();
	int CloseDevice();
	int WriteDevice(IN const char *pData, IN int nSize, IN int nTimeOut = 10000);
	int ReadDevice(OUT char **pData, OUT int &nSize, IN int nTimeOut = 10000);
	int FreeMemory(IN char **pData);

};


__END_NAMESPACE(Dev)

