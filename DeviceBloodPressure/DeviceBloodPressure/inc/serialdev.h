#pragma once
#include "idevinterfacetype.h"
#include "SerialCom.h"
#include "formatfactory.h"




#define MIN_LEN			32
#define MAX_LEN			512




__BEGIN_NAMESPACE(Dev)
	

class CSerialDev : public IDevInterfaceType
{
public:
	CSerialDev(void);
	~CSerialDev(void);

public:
	int InitDevice(IN const char *szDevContent);
	int OpenDevice();
	int CloseDevice();
	int WriteDevice(IN const char *pData, IN int nSize, IN int nTimeOut = 1000);//timeout= 10s
	int ReadDevice(OUT char **pData, IN int nSize = 1024, IN int nTimeOut = 1000);
	int FreeMemory(IN char **pData);

private:
	CSerialCom serialcom;
	
	bool m_bInit;
	char m_szCom[MIN_LEN];
	int	m_nBaud;
	char m_nParity;
	int m_nDataBits;
	int m_nStopBits;


};




__END_NAMESPACE(Dev)






