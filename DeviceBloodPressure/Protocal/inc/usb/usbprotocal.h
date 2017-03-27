#pragma once
#include "iprotocal.h"

//设备
#include "idevinterfacetype.h"
#include "devfactory.h"

#include <iostream>
#include <string>


#include "membuffer.h"
#include <stdint.h>
#include <assert.h>

#include "datatype.h"

#define INOUT
#define IN
#define OUT



using namespace std;


#define TOKCHAR_CR		0xD
#define TOKCHAR_LF		0xA



__BEGIN_NAMESPACE(Protocal)


#pragma pack(push,1)

/* Used to represent different types of tokens in input. */
typedef enum
{
	TT_IDENTIFIER, 
	TT_WHITESPACE, 
	TT_CRLF, 
	TT_CTRL,
	TT_SEPARATOR,
	TT_QUOTEDSTRING,
} token_type_t;

/*! status of parsing */
typedef enum 
{
	/*! msg was parsed successfully. */
	PARSE_SUCCESS = 0,
	/*! need more data to continue. */
	PARSE_INCOMPLETE,
	/*! for responses that don't have length specified. */
	PARSE_INCOMPLETE_ENTITY,
	/*! parse failed; check status code for details. */
	PARSE_FAILURE,
	/*! done partial. */
	PARSE_OK,
	/*! token not matched. */
	PARSE_NO_MATCH,
	/*! private. */
	PARSE_CONTINUE_1
} parse_status_t;


typedef enum {
	ID_TYPE = 0,
	SYS_TYPE,
	DIA_TYPE,
	RATE_TYPE,
	YEAR_TYPE,
	MON_TYPE,
	DAY_TYPE,
	HOUR_TYPE,
	MIN_TYPE,
	SEC_TYPE

}record_data_type_t;




#pragma pack(pop,1)


class UsbProtocal : public IProtocal
{
public:
	UsbProtocal(void);
	~UsbProtocal(void);

public:
	int ParseProtocal(IN const char *pData, IN int nType, OUT char *&pParsedData, OUT int &nLen);

protected:
	int ParseMessageTimeProtocal(IN const char *pData, OUT char *&pProtocalData);


protected:
	int ReadData(OUT char **pData, OUT int &nSize);
	int ParseRxBuf(OUT membuffer *pdata, IN const char *prxBuf, OUT int nSize);
	int GetTokenString(OUT char *&pBuf, OUT int &nSize, IN const char *prxBuf);
	int ParseData(OUT membuffer *pdata, IN const char *cmd, IN const char *cursor, OUT int nSize);

	int ParseRrcdCmdData(OUT membuffer *pdata, IN const char *cursor, OUT int nSize);

	int GetRecordDataSum(IN const char *cursor, OUT int &nRecordSum);
	int SetRecordDataStruct(int nType, int nVal, Format::acf_record_data_t *&pRecordData);

//操作命令
	int SetAuto();	
	int SetManual(int intervalDay, int intervalNight);
	int SetAutoTable();
	int SetAutoTableEntry(int index, int hour, int minute, int interval);
	int SetAutoTableEmpty(int index);

	int SetDate(int nYear, int nMon, int nDay);		//sdt
	int SetTime(int nHour, int nMin);		//st
	int SetEmpty(int address);

	int SetId(int id);
	int Set(int address, int value);
	int SendSetCommand(std::string szCmd);		//write set cmd	
	int SendReadCommand(std::string szCmd);		//write cmd to device
	
	//write cmd
	int Help();
	int ReadRSet();
	int ReadRecord();
	int ClearRecord();			//clear RRCD data

	std::string Int2Hex(int value, int digit);

	uint64_t HexToASCII(const char* szIn);
	int8_t HexToInt(char c);

	parse_status_t ScannerGetToken(INOUT const char *scanner, OUT memptr *token, OUT token_type_t *tok_type);
	
	inline int IsSeparatorChar(IN int c)
	{
		return strchr(" \t()<>@,;:\\\"/[]?={}", c) != 0;
	}

	inline int IsIdentifierChar(IN int c)
	{
		return c >= 32 && c <= 126 && !IsSeparatorChar(c);
	}

	inline int IsControlChar(IN int c)
	{
		return (c >= 0 && c <= 31) || c == 127;
	}

	inline int IsQdtextChar(IN int c)
	{
		assert( c != '"' );
		return
			(c >= 32 && c != 127) ||
			c < 0 ||
			c == TOKCHAR_CR ||
			c == TOKCHAR_LF ||
			c == '\t';
	}

	//操作设备函数
protected:
	int InitDevice(IN const char *szDevContent);
	int OpenDevice();
	int CloseDevice();
	int WriteDivice(const char *pData, IN int nSize, IN int nTimeOut = 1000);
	int ReadDivice(char **pData, IN int &nSize, IN int nTimeOut = 1000);
	int FreeMemory(IN char **pData);


private:
	Dev::IDevInterfaceType *m_pUSbDev;	//设备


};



__END_NAMESPACE(Protocal)