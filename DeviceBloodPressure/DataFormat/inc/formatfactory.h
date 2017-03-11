#pragma once
#include "typedef.h"


__BEGIN_NAMESPACE(Format)


typedef enum E_FormatType
{
	UNKNOW_FORMAT_TYPE		= -1,
	JSON_FORMAT_TYPE,
	XML_FORMAT_TYPE,
	PROTOBUF_FORMAT_TYPE

}E_FormatType_t;


class IFormat;

class CFormatFactory
{
public:
	CFormatFactory(void){};
	~CFormatFactory(void){};


public:
	IFormat *CreateFormat(E_FormatType_t eType);



};


__END_NAMESPACE(Format)

