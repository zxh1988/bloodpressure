#pragma once
#include "typedef.h"


#define IN
#define OUT


__BEGIN_NAMESPACE(Parser)


class IParserInterface
{
public:
	IParserInterface(void){};
	virtual ~IParserInterface(void) = 0{};

public:
	virtual int Parse(IN const char *pData, IN int nType, OUT char *&pParsedData) = 0;



};

__END_NAMESPACE(Parser)

