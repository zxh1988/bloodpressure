#pragma once
#include "typedef.h"


__BEGIN_NAMESPACE(Dev)




typedef enum E_DevInterfaceType
{
	UNKNOW_DEV_TYPE		= -1,
	DEV_SERIAL_TYPE,
	DEV_USB_TYPE

}E_DevInterfaceType_t;



class IDevInterfaceType;


class CDevFactory
{
public:
	CDevFactory(void){};
	~CDevFactory(void){};

public:
	IDevInterfaceType *CreateDevInterface(E_DevInterfaceType_t eType);
	
};


__END_NAMESPACE(Dev)
