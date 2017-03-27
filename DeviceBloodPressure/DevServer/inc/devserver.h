#pragma once
#include "typedef.h"
#include "idevserver.h"

//格式
#include "iformat.h"
#include "formatfactory.h"

//设备
#include "idevinterfacetype.h"
#include "devfactory.h"

//解析
#include "iparserinterface.h"
#include "parserfactory.h"

//协议
#include "iprotocal.h"
#include "protocalfactory.h"

__BEGIN_NAMESPACE(DevServer)


class IDevServer;


class CDevServer : public IDevServer
{
public:
	static IDevServer *GetInstance();
	CDevServer(void);
	~CDevServer(void);	

public:
	int InitDevice(IN const char *szDevContent);
	int OpenDevice();
	int CloseDevice();
	int WriteDevice(IN const char *pData, IN int nSize, \
		OUT char **pResultData,  OUT int &nResultSize);
	int ReadDevice(OUT char **pData, OUT int &nSize);
	int FreeMemory(IN char **pData);


private:
	
	//Format::IFormat *m_pJsonFormat;	//格式
	int m_nDevType;
	bool m_bDevInit;

	Parser::IParserInterface *m_pSerialParser;		//串口解析
	Protocal::IProtocal *m_pSerialProtocal;			//串口协议解析

	Parser::IParserInterface *m_pUsbParser;			//USB解析
	Protocal::IProtocal *m_pUsbProtocal;			//USB协议解析

	static IDevServer *m_pgInstance;
};


__END_NAMESPACE(DevServer)

