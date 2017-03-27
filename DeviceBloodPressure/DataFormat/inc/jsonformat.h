#pragma once
#include "iformat.h"
#include "datatype.h"

//parser factory
#include "iparserinterface.h"
#include "parserfactory.h"


#ifdef WIN32
#define strncasecmp strnicmp
#else
/* Other systems have strncasecmp */
#endif


__BEGIN_NAMESPACE(Format)




//template<class T>
class CJsonFormat : public IFormat
{
public:
	CJsonFormat(void):m_pSerialParser(NULL){};
	~CJsonFormat(void)
	{
		DELPTR(m_pSerialParser);
	};


public:
	int ParseFormat(IN const char *pData, OUT int &nType, OUT char *&pFormatData);
	int CreateFormat(IN const char *pData, IN int nLen, \
		IN int nType, OUT char **pFormatData,\
		OUT int &FormatLen);


protected:
	int CreateFromatRecordData(IN const char *pData, IN int nLen,\
		OUT char **pFormatData,	OUT int &FormatLen);
	int CreateACFRecordData(IN const char *pData, IN int nLen,\
		OUT char **pFormatData,	OUT int &FormatLen);



private:
	CDataType<SerialDevData_t> m_SerialDevType;
	CDataType<UsbDevData_t> m_UsbDevType;

	Parser::IParserInterface *m_pSerialParser;

};


__END_NAMESPACE(Format)

