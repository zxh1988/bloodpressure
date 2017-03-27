#include "StdAfx.h"
#include "formatfactory.h"
#include "jsonformat.h"


__BEGIN_NAMESPACE(Format)






IFormat *CFormatFactory::CreateFormat(E_FormatType_t eType)
{
	IFormat *pFormat = NULL;

	if (JSON_FORMAT_TYPE == eType)
	{
		pFormat = new CJsonFormat();
	}
	else if (XML_FORMAT_TYPE == eType)
	{
	}
	else
	{}

	return pFormat;
}



__END_NAMESPACE(Format)

