#pragma once



template<class T>
class CFromatTemplete
{
public:
	CFromatTemplete(void) {};
	~CFromatTemplete(void) {};


public:
	int ParseFormat(IN const char *pData, IN int nDevType, OUT T *&pFormatData);
	int CreateFormat(IN const char *pData, OUT T *pFormatData);


};

