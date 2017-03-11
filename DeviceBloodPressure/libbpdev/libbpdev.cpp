// libbpdev.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "libbpdevinterface.h"
#include "devserver.h"


DevServer::CDevServer *p_gDevServer = NULL;



int BpInitDevice(IN const char *szDevContent)
{
	return p_gDevServer->GetInstance()->InitDevice(szDevContent);
}


int BpOpenDevice()
{
	return p_gDevServer->GetInstance()->OpenDevice();
}

int BpCloseDevice()
{
	return p_gDevServer->GetInstance()->CloseDevice();
}

int BpWriteDevice(IN const char *pData, IN int nSize, \
	OUT char **pResultData,  OUT int &nResultSize)
{
	return p_gDevServer->GetInstance()->WriteDevice(pData,nSize,pResultData,nResultSize);
}

int BpReadDevice(OUT char **pData, OUT int &nSize)
{
	return p_gDevServer->GetInstance()->ReadDevice(pData,nSize);
}

int BpFreeMemory(IN char **pData)
{
	return p_gDevServer->GetInstance()->FreeMemory(pData);
}





