#include "StdAfx.h"
#include "parserfactory.h"
#include "serialdataparser.h"


__BEGIN_NAMESPACE(Parser)


IParserInterface *CParserFactory::m_pgInstance = NULL;


CParserFactory::~CParserFactory(void)
{
	if (NULL != m_pgInstance)
	{
		delete m_pgInstance;
	}
}



IParserInterface *CParserFactory::GetInstance(E_ParseDataType eType)
{	
	if (NULL == m_pgInstance)
	{
		m_pgInstance = CreateParser(eType);
	}	
	return m_pgInstance;
}

IParserInterface *CParserFactory::CreateParser(E_ParseDataType eType)
{
	IParserInterface *pParser = NULL;

	if (SERIAL_DATA_TYPE == eType)
	{
		pParser = new SerialDataParser();
	}
	else if (USB_DATA_TYPE == eType)
	{
		//pParser = new CUsbDev();
	}
	else
	{}

	return pParser;
}


__END_NAMESPACE(Parser)

