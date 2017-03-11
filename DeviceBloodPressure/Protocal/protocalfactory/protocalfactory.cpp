#include "StdAfx.h"
#include "protocalfactory.h"
#include "serialprotocal.h"



__BEGIN_NAMESPACE(Protocal)



CProtocalFactory::CProtocalFactory(void)
{
}


CProtocalFactory::~CProtocalFactory(void)
{
}


IProtocal *CProtocalFactory::CreateProtocal(E_Protocal_Type_t eType)
{
	IProtocal *pProtocal = NULL;

	if (SERIAL_DATA_TYPE == eType)
	{
		pProtocal = new CSerialProtocal();
	}
	else if (USB_DATA_TYPE == eType)
	{
		//pProtocal = new CUsbDev();
	}
	else
	{}

	return pProtocal;
}


__END_NAMESPACE(Parser)
