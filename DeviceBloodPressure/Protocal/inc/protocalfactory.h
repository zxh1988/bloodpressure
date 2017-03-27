#pragma once
#include "typedef.h"


__BEGIN_NAMESPACE(Protocal)


typedef enum E_ProtocalType
{
	UNKNOW_TYPE		= -1,
	SERIAL_DATA_TYPE,
	USB_DATA_TYPE

}E_Protocal_Type_t;


class IProtocal;

class CProtocalFactory
{
public:
	CProtocalFactory(void) {};
	~CProtocalFactory(void){};

public:
	IProtocal *CreateProtocal(E_Protocal_Type_t eType);

};



__END_NAMESPACE(Protocal)


