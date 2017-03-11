#pragma once
#include "typedef.h"



__BEGIN_NAMESPACE(Parser)


typedef enum E_ParseDataType
{
	UNKNOW_TYPE		= -1,
	SERIAL_DATA_TYPE,
	USB_DATA_TYPE

}E_ParseDataType_t;




class IParserInterface;

class CParserFactory
{
public:
	CParserFactory(void){};		
	~CParserFactory(void);

public:
	IParserInterface *CreateParser(E_ParseDataType eType);
	IParserInterface *GetInstance(E_ParseDataType eType);

private:
	
	static IParserInterface *m_pgInstance;


};


__END_NAMESPACE(Parser)
