#pragma once
#include "typedef.h"

#define IN
#define OUT


__BEGIN_NAMESPACE(Format)




class IFormat
{
public:
	IFormat(void) {};
	virtual ~IFormat(void) = 0 {};

public:
	virtual int ParseFormat(IN const char *pData, OUT int &Type, OUT char *&pFormatData) = 0;
	virtual int CreateFormat(IN const char *pData, IN int nLen, \
		IN int nType, OUT char **pFormatData,\
		OUT int &FormatLen) = 0;



};


__END_NAMESPACE(Format)

