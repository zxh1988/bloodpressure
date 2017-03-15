#include "StdAfx.h"
#include "devfactory.h"
#include "SerialDev.h"
#include "usbdev.h"




__BEGIN_NAMESPACE(Dev)





IDevInterfaceType *CDevFactory::CreateDevInterface(E_DevInterfaceType_t eType)
{
	IDevInterfaceType *pDevType = NULL;

	if (DEV_SERIAL_TYPE == eType)
	{
		pDevType = new Dev::CSerialDev();
	}
	else if (DEV_USB_TYPE == eType)
	{
		pDevType = new Dev::CUsbDev();
	}
	else
	{}

	return pDevType;
}





__END_NAMESPACE(Dev)


