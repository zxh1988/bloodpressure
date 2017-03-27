#include "StdAfx.h"
#include "protocalfactory.h"
#include "serialprotocal.h"
#include "usbprotocal.h"


__BEGIN_NAMESPACE(Protocal)




IProtocal *CProtocalFactory::CreateProtocal(E_Protocal_Type_t eType)
{
	IProtocal *pProtocal = NULL;

	if (SERIAL_DATA_TYPE == eType)
	{
		pProtocal = new CSerialProtocal();
	}
	else if (USB_DATA_TYPE == eType)
	{
		pProtocal = new UsbProtocal();
	}
	else
	{}

	return pProtocal;
}


__END_NAMESPACE(Protocal)
