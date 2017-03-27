#pragma once
#include "iparserinterface.h"
#include "typedef.h"
#include "datatype.h"




#include "acfdatahdr.h"


#ifdef WIN32
#define strncasecmp strnicmp
#else
/* Other systems have strncasecmp */
#endif


#define DEVICE_USB					"Usb"			//USB…Ë±∏√˚≥∆


#define WVID						"wVID"
#define WPID						"wPID"



__BEGIN_NAMESPACE(Parser)





class CUsbDataParser : public IParserInterface
{
public:
	CUsbDataParser(void){};
	~CUsbDataParser(void){};

public:
	int Parse(IN const char *pData, IN int &nType, OUT char *&pParsedData);

protected:
	int ParseUsbDevFormat(IN const char *pData, OUT char *&pFormatData);

	int HandleJsonCmd(IN const char *pData, IN const char *pCmd, OUT int &nType, OUT char *&pFormatData);
	int HandleSetCmd(IN const char *pData, IN const char *pCmd, OUT char *&pFormatData);
	int ParseJsonSetCmd(IN const char *pData, OUT SET &pSet);
	

//√¸¡Ó
protected:




};




__END_NAMESPACE(Parser)




