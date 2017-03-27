#pragma once
#include "typedef.h"


#define IN
#define OUT



__BEGIN_NAMESPACE(Protocal)


class IProtocal
{
public:
	IProtocal(void){};
	virtual ~IProtocal(void) = 0{};

public:
	virtual int ParseProtocal(IN const char *pData, IN int nType, OUT char *&pParsedData, OUT int &nLen) = 0;


};



__END_NAMESPACE(Protocal)


