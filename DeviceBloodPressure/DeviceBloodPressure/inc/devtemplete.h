#pragma once
#include "typedef.h"

#include "devfactory.h"
#include "idevinterfacetype.h"


#define IN
#define OUT


__BEGIN_NAMESPACE(Dev)



template<class T>
class CDevTemplete
{
public:
	CDevTemplete(void):m_pIDev(NULL)
	{
	};
	~CDevTemplete(void)
	{
		DELPTR(m_pIDev);
	};

	int InitDevice(T *pDevData)
	{
		E_Status_t nRet = UNKNOW_STATUS;
		if (sizeof(Format::SerialDevData_t) == sizeof(T))
		{
			Format::SerialDevData_t *pSerialDevData = pDevData;

			//Dev::CDevFactory devfactory;
			//Dev::E_DevInterfaceType_t eDevType = Dev::DEV_SERIAL_TYPE;
			//m_pIDev = devfactory.CreateDevInterface(eDevType);

			////调用设备接口
			//nRet = (E_Status_t)m_pIDev->OpenSerialDevice(pSerialDevData->szCom,\
			//pSerialDevData->nBaud, pSerialDevData->nParity,\
			//pSerialDevData->nDataBits, pSerialDevData->nStopBits);

			CSerialCom.InitPort(pSerialDevData->szCom,\
				pSerialDevData->nBaud, pSerialDevData->nParity,\
				pSerialDevData->nDataBits, pSerialDevData->nStopBits);

		}
		else if (sizeof(Format::UsbDevData_t) == sizeof(T))
		{
		}
		else
		{}
		return nRet;
	}
	int OpenDevice();
	int CloseDevice();
	int WriteDevice(IN const char *pData, IN int nSize, IN int nTimeOut = 10000);//timeout= 10s
	int ReadDevice(OUT char **pData, OUT int &nSize, IN int nTimeOut = 10000);
	int FreeMemory(IN char **pData);

private:
	Dev::IDevInterfaceType *m_pIDev;


};



__END_NAMESPACE(Dev)

