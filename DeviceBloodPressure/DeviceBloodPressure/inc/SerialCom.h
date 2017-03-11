#pragma once
#include <Windows.h>
#include "typedef.h"



__BEGIN_NAMESPACE(Dev)


class CSerialCom
{
public:
	CSerialCom(void);
	~CSerialCom(void);

	bool InitPort(const char *szCom, \
				int nBaud = 19200, \
				const char nparity = 'N', \
				int nDataBits = 8, \
				int nStopBits = 1);   
	int ReadFromPort(char **pBuff,IN int nSize, unsigned int timeout);		//read com data
	bool WriteToPort(const void *pBuff, DWORD size, unsigned int timeout);	//write com data
	
	bool ReadComm(void *pData, int nLength);
	bool WriteComm(void* pData, int nLength);
	bool ClosePort(); 
	void ClearCommBuff();

protected:
	bool OpenPort(const char *szCom);

private:
	 HANDLE m_hcom;  //serial com handle

	 // 串口空闲信号(用于发送和接收操作互斥)
	 HANDLE m_hCommFree;
	 // 串口接收信号
	 HANDLE m_hCommRecv;
	 // 串口接收信号
	 HANDLE m_hCommSend;


};


__END_NAMESPACE(Dev)

