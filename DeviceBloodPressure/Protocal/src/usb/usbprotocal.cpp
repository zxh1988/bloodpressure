#include "StdAfx.h"
#include "usbprotocal.h"
#include "datatype.h"
#include "acfdatahdr.h"



__BEGIN_NAMESPACE(Protocal)


UsbProtocal::UsbProtocal(void)
{
	//设备初始化
	Dev::CDevFactory devfactory;
	Dev::E_DevInterfaceType_t eDevType = Dev::DEV_USB_TYPE;
	m_pUSbDev = devfactory.CreateDevInterface(eDevType);
}


UsbProtocal::~UsbProtocal(void)
{
	DELPTR(m_pUSbDev);
}

int UsbProtocal::InitDevice(IN const char *szDevContent)
{
	return m_pUSbDev->InitDevice((char*)szDevContent);
}

int UsbProtocal::OpenDevice()
{
	E_Status_t eRet = SUCCESS;
	eRet = (E_Status_t)m_pUSbDev->OpenDevice();
	CheckStatus(eRet);	
}

int UsbProtocal::CloseDevice()
{
	return m_pUSbDev->CloseDevice();
}

int UsbProtocal::WriteDivice(const char *pData, IN int nSize, IN int nTimeOut)
{
	return m_pUSbDev->WriteDevice(pData, nSize, nTimeOut);
}

int UsbProtocal::ReadDivice(char **pData, IN int &nSize, IN int nTimeOut)
{
	return m_pUSbDev->ReadDevice(pData, nSize, nTimeOut);
}

int UsbProtocal::FreeMemory(IN char **pData)
{
	return m_pUSbDev->FreeMemory(pData);
}


int UsbProtocal::ParseProtocal(IN const char *pData, IN int nType, OUT char *&pParsedData, OUT int &nLen)
{
	E_Status_t nRet = UNKNOW_STATUS;
	switch(nType)
	{
	case Format::ACF_DEV_INIT:
		//设备初始化
		nRet = (E_Status_t)InitDevice(pData);
		break;
	case Format::OPEN_DEVICE:
		nRet = (E_Status_t)OpenDevice();  
		break;
	case Format::CLOSE_DEVICE:
		nRet = (E_Status_t)CloseDevice();
		break;
	case Format::ACF_GET_RECORD_DATA:	//读RRCD数据
		nRet = (E_Status_t)ReadData(&pParsedData, nLen);
		break;
	case Format::ACF_MEASURE_MODE:	//设置时间测量模式
		nRet = (E_Status_t)ParseMessageTimeProtocal(pData, pParsedData);
		break;
	default:
		break;
	}

	return nRet;
}

int UsbProtocal::ParseMessageTimeProtocal(IN const char *pData, OUT char *&pProtocalData)
{
	E_Status_t eRet = UNKNOW_STATUS;
	Parser::SET *pSet = (Parser::SET *)pData;

	//清除盒子RRCD数据
	eRet = (E_Status_t)ClearRecord();
	CheckStatus(eRet);

	//mode
	if (0 == pSet->nMode)
	{}
	else if(1 == pSet->nMode)		//1——按时间表测量
	{
		//1、write id
		eRet = (E_Status_t)SetId(pSet->nID);
		CheckStatus(eRet);

		//2、write auto table
		eRet = (E_Status_t)SetAutoTable();
		CheckStatus(eRet);

		//3、write atuo table mode
		for (int i = 0; i < 6; i++)
		{
			Parser::AUTOTABLE *pAutoTable = NULL;
			pAutoTable = &(pSet->AutoTable[i]);
			eRet = (E_Status_t)SetAutoTableEntry(i, pAutoTable->nHour, pAutoTable->nMin, pAutoTable->nInterval);
			CheckStatus(eRet);
		}

		//4、write date
		eRet = (E_Status_t)SetDate(pSet->CurSysTime.nYear,pSet->CurSysTime.nMon,pSet->CurSysTime.nDay);

		//5、write time
		eRet = (E_Status_t)SetTime(pSet->CurSysTime.nHour, pSet->CurSysTime.nMin);
	}
	else if(2 == pSet->nMode)		//2——自动
	{
		eRet = (E_Status_t)SetId(pSet->nID);
		CheckStatus(eRet);

		eRet = (E_Status_t)SetAuto();
		CheckStatus(eRet);

		eRet = (E_Status_t)SetDate(pSet->CurSysTime.nYear,pSet->CurSysTime.nMon,pSet->CurSysTime.nDay);
		CheckStatus(eRet);

		eRet = (E_Status_t)SetTime(pSet->CurSysTime.nHour, pSet->CurSysTime.nMin);
		CheckStatus(eRet);
	}
	else if(3 == pSet->nMode)		//3——手动时间间隔,自动测试
	{
		//1 set id
		eRet = (E_Status_t)SetId(pSet->nID);
		CheckStatus(eRet);

		//2 set manual
		eRet = (E_Status_t)SetManual(pSet->HandleIntervalMode.nDay, pSet->HandleIntervalMode.nNight);
		CheckStatus(eRet);

		//3 set date
		eRet = (E_Status_t)SetDate(pSet->CurSysTime.nYear,pSet->CurSysTime.nMon,pSet->CurSysTime.nDay);
		CheckStatus(eRet);

		//4 set time
		eRet = (E_Status_t)SetTime(pSet->CurSysTime.nHour, pSet->CurSysTime.nMin);
		CheckStatus(eRet);
	}
	else if(4 == pSet->nMode)
	{}
	else
	{
		return FAIL;
	}
	return eRet;
}

int UsbProtocal::ReadData(OUT char **pData, OUT int &nSize)
{
	E_Status_t eRet = UNKNOW_STATUS;

	nSize = 0;
	char *rxBuf = NULL;

	eRet = (E_Status_t)ReadDivice(&rxBuf, nSize, 1000);
	CheckStatus(eRet);

	membuffer data;
	membuffer_init(&data);

	//parse rxbuf
	eRet = (E_Status_t)ParseRxBuf(&data, rxBuf, strlen(rxBuf));
	CheckStatus(eRet);

	*pData = new char[data.length+1];
	*(*pData + data.length) = '\0';
	if (NULL != pData)
	{
		memcpy(*pData, data.buf, data.length);
	}
	nSize = data.length;

	membuffer_destroy(&data);

	return eRet;
}

int UsbProtocal::ParseRxBuf(OUT membuffer *pdata, IN const char *prxBuf, OUT int nSize)
{
	E_Status_t eRet = UNKNOW_STATUS;

	if (NULL == prxBuf) return FAIL;

	const char *cursor = prxBuf;

	char *pCmd = NULL;
	int nLen = 0;
	eRet = (E_Status_t)GetTokenString(pCmd, nLen, prxBuf);
	CheckStatus(eRet);

	//parse cmd
	cursor = cursor + nLen + 2;
	eRet = (E_Status_t)ParseData(pdata, pCmd, cursor, nSize - nLen);
	CheckStatus(eRet);

	DELARR(pCmd);
	return eRet;
}


int UsbProtocal::GetTokenString(OUT char *&pBuf, OUT int &nSize, IN const char *prxBuf)
{
	if (NULL == prxBuf) return FAIL;

	int nLen = 0;
	const char *cursor = prxBuf;
	char c = *cursor;
	bool bFound = false;
	while(c)
	{
		if (c == TOKCHAR_LF)
		{
			cursor += 1;
			c = *cursor;
			if (c == TOKCHAR_CR)
			{		
				cursor += 1;
				nLen = cursor - prxBuf - 2;
				pBuf = new char[nLen+1];
				if (NULL != pBuf)
				{
					*(pBuf + nLen) = '\0';
					memcpy(pBuf, prxBuf, nLen);
				}	
				bFound =  true;
				break;
			}
		}
		else
		{
			cursor += 1;
			c = *cursor;
		}
	}
	nSize = nLen;
	return (!bFound) ? FAIL : SUCCESS;

}



int UsbProtocal::ParseData(OUT membuffer *pdata, IN const char *cmd, IN const char *cursor, OUT int nSize)
{
	E_Status_t eRet = UNKNOW_STATUS;

	if (NULL == cmd || NULL == cursor) return FAIL;

	if (0 == strnicmp(cmd, "rrcd", strlen("rrcd")))
	{
		eRet = (E_Status_t)ParseRrcdCmdData(pdata, cursor, nSize);
	}
	else if (0 == strnicmp(cmd, "rset", strlen("rset")))
	{
		//eRet = (E_Status_t)ParseRsetCmdData(pdata, cursor, nSize);
	}
	else if (0 == strnicmp(cmd, "rda", strlen("rda")))	//parse have problem
	{
	}
	else if (0 == strnicmp(cmd, "rdf", strlen("rdf")))	
	{
	}
	else if (0 == strnicmp(cmd, "ver", strlen("rdf")))	
	{
	}
	else if (0 == strnicmp(cmd, "set", strlen("set")))
	{
		char *pCmd = NULL;
		int nLen = 0;
		eRet = (E_Status_t)GetTokenString(pCmd, nLen, cursor);

		membuffer_append(pdata, pCmd, strlen(pCmd));
		delete[] pCmd;
	}
	else if (0 == strnicmp(cmd, "sdt", strlen("sdt")))
	{
		char *pCmd = NULL;
		int nLen = 0;
		eRet = (E_Status_t)GetTokenString(pCmd, nLen, cursor);

		membuffer_append(pdata, pCmd, strlen(pCmd));
		delete[] pCmd;
	}
	else if (0 == strnicmp(cmd, "st", strlen("st")))
	{
		char *pCmd = NULL;
		int nLen = 0;
		eRet = (E_Status_t)GetTokenString(pCmd, nLen, cursor);

		membuffer_append(pdata, pCmd, strlen(pCmd));
		delete[] pCmd;
	}
	else if (0 == strnicmp(cmd, "clr", strlen("clr")))
	{
		char *pCmd = NULL;
		int nLen = 0;
		eRet = (E_Status_t)GetTokenString(pCmd, nLen, cursor);

		membuffer_append(pdata, pCmd, strlen(pCmd));
		delete[] pCmd;
	}
	else
	{
		return FAIL;
	}
	return eRet;
}


int UsbProtocal::ParseRrcdCmdData(OUT membuffer *pdata, IN const char *cursor, OUT int nSize)
{
	E_Status_t eRet = UNKNOW_STATUS;
	if (NULL == cursor) return FAIL;

	const char *cur = cursor;
	//1、parse record id
	char *szId = NULL;
	int nIdLen = 0;
	eRet = (E_Status_t)GetTokenString(szId, nIdLen, cur);
	CheckStatus(eRet);
	cur = cur + nIdLen;

	//hex convert to int
	int nVal = HexToASCII(szId);
	DELARR(szId);

	Format::acf_record_data_t recorddata = { 0 };
	recorddata.nId = (nVal & 0x00ff) << 8;		//高8位值
	recorddata.nId |= (nVal & 0xff00) >> 8;		//低8位值

	//3、parse data
	parse_status_t status;
	memptr token;
	token_type_t tok_type;

	int nRecordSum = 0;
	eRet = (E_Status_t)GetRecordDataSum(cur, nRecordSum);
	CheckStatus(eRet);

	int nRecordLen = nRecordSum * sizeof(Format::acf_record_data_t);
	char *p = new char[nRecordLen + 1];
	*(p + nRecordLen) = '\0';

	Format::acf_record_data_t *pRecordData = (Format::acf_record_data_t*)p;

	int nType = 0;
	while(NULL != *cur)
	{			
		status = ScannerGetToken(cur, &token, &tok_type);
		if (status != (parse_status_t)PARSE_OK) 
		{
			return FAIL;
		}

		char cInt = 0;
		uint64_t ulValue = 0;
		switch (tok_type) 
		{
			case TT_IDENTIFIER:				
				for(uint16_t usIdx = 0; token.buf[usIdx] && (usIdx < token.length); usIdx++)
				{					
					if(-1 == (cInt = HexToInt(token.buf[usIdx])))
						return FAIL;
					ulValue = ulValue << 4;
					ulValue |= cInt;
				}
				eRet = (E_Status_t)SetRecordDataStruct(nType, ulValue, pRecordData);
				CheckStatus(eRet);
				nType++;
				break;
			case TT_WHITESPACE:
				break;			
			case TT_CRLF:
				break;
			case TT_CTRL:
				break;
			case TT_SEPARATOR:
				if (',' == *token.buf)
				{	
					pRecordData++;
					nType = 0;
				}
				break;	
			case TT_QUOTEDSTRING:
				break;
			default:
				break;
		}
		cur += token.length;
	}
	return eRet;
}

int UsbProtocal::SetRecordDataStruct(int nType, int nVal, Format::acf_record_data_t *&pRecordData)
{
	E_Status_t eRet = SUCCESS;
	switch(nType)
	{
		case ID_TYPE:
			pRecordData->nId = nVal;
			break;
		case SYS_TYPE:
			pRecordData->nSys = nVal;
			break;
		case DIA_TYPE:
			pRecordData->nDia = nVal;
			break;
		case RATE_TYPE:
			pRecordData->nRate = nVal;
			break;
		case YEAR_TYPE:
			pRecordData->nYear = nVal;
			break;
		case MON_TYPE:
			pRecordData->nMon = nVal;
			break;
		case DAY_TYPE:
			pRecordData->nDay = nVal;
			break;
		case HOUR_TYPE:
			pRecordData->nHour = nVal;
			break;
		case MIN_TYPE:
			pRecordData->nMin = nVal;
			break;
		case SEC_TYPE:
			pRecordData->nSec = nVal;
			break;
		default:
			eRet = UNKNOW_STATUS;
			break;
	}
	return eRet;
}

int UsbProtocal::GetRecordDataSum(IN const char *cursor, OUT int &nRecordSum)
{
	E_Status_t eRet = SUCCESS;

	const char *cur = cursor;
	nRecordSum = 0;

	parse_status_t status;
	memptr token;
	token_type_t tok_type;

	int nCount = 0;
	while(NULL != *cur)
	{			
		status = ScannerGetToken(cur, &token, &tok_type);
		if (status != (parse_status_t)PARSE_OK) 
		{
			return FAIL;
		}

		char cInt = 0;
		uint64_t ulValue = 0;
		switch (tok_type) 
		{
		case TT_IDENTIFIER:				
			break;
		case TT_WHITESPACE:
			break;			
		case TT_CRLF:
			break;
		case TT_CTRL:
			break;
		case TT_SEPARATOR:
			nCount++;
			break;	
		case TT_QUOTEDSTRING:
			break;
		default:
			break;
		}
		cur += token.length;
	}
	nRecordSum = nCount;
	return eRet;
}

int UsbProtocal::SetId(int id)
{
	E_Status_t eRet = UNKNOW_STATUS;
	//write low 8 bytes
	eRet = (E_Status_t)Set(0, (id & 0x00FF));
	CheckStatus(eRet);

	//read data
	char *pData = NULL;
	int nLen = 0;
	eRet = (E_Status_t)ReadData(&pData, nLen);
	CheckStatus(eRet);
	DELARR(pData);

	//write high 8 bytes
	eRet = (E_Status_t)Set(1, (id & 0xFF00) >> 8);
	CheckStatus(eRet);

	//read data
	nLen = 0;
	eRet = (E_Status_t)ReadData(&pData, nLen);
	CheckStatus(eRet);
	DELARR(pData);

	return eRet;
}

int UsbProtocal::SetAuto()
{
	// 2--自动，白天15分钟晚上30分钟测一次
	//write
	E_Status_t eRet = (E_Status_t)Set(2, 2);
	CheckStatus(eRet);
	//read
	char *pData = NULL;
	int nLen = 0;
	eRet = (E_Status_t)ReadData(&pData, nLen);
	CheckStatus(eRet);
	DELARR(pData);

	return eRet;
}

int UsbProtocal::SetManual(int intervalDay, int intervalNight)
{
	// 3--手动时间间隔,自动测试
	//1、
	E_Status_t eRet = (E_Status_t)Set(3, intervalDay % 256);
	CheckStatus(eRet);
	char *pData = NULL;
	int nLen = 0;
	eRet = (E_Status_t)ReadData(&pData, nLen);
	CheckStatus(eRet);
	DELARR(pData);

	//2、
	eRet = (E_Status_t)Set(4, intervalNight % 256);
	CheckStatus(eRet);
	pData = NULL;
	nLen = 0;
	eRet = (E_Status_t)ReadData(&pData, nLen);
	CheckStatus(eRet);
	DELARR(pData);

	//3、
	eRet = (E_Status_t)Set(2, 3);
	CheckStatus(eRet);

	pData = NULL;
	nLen = 0;
	eRet = (E_Status_t)ReadData(&pData, nLen);
	CheckStatus(eRet);
	DELARR(pData);

	return eRet;
}

int UsbProtocal::SetAutoTable()
{
	E_Status_t eRet = (E_Status_t)Set(2, 1);
	CheckStatus(eRet);

	char *pData = NULL;
	int nLen = 0;
	eRet = (E_Status_t)ReadData(&pData, nLen);
	CheckStatus(eRet);

	DELARR(pData);
	return eRet;
}

int UsbProtocal::SetAutoTableEntry(int index, int hour, int minute, int interval)
{
	//1、write hour
	int address = 5 + index * 3;
	E_Status_t eRet = (E_Status_t)Set(address, hour);
	CheckStatus(eRet);

	char *pData = NULL;
	int nLen = 0;
	eRet = (E_Status_t)ReadData(&pData, nLen);
	CheckStatus(eRet);
	DELARR(pData);

	//2、write min
	eRet = (E_Status_t)Set(address + 1, minute);
	CheckStatus(eRet);

	pData = NULL;
	nLen = 0;
	eRet = (E_Status_t)ReadData(&pData, nLen);
	CheckStatus(eRet);
	DELARR(pData);

	//3、write interval
	eRet = (E_Status_t)Set(address + 2, interval);
	CheckStatus(eRet);

	pData = NULL;
	nLen = 0;
	eRet = (E_Status_t)ReadData(&pData, nLen);
	CheckStatus(eRet);
	DELARR(pData);

	return eRet;
}

int UsbProtocal::SetDate(int nYear, int nMon, int nDay)
{
	E_Status_t eRet = (E_Status_t)SendSetCommand("sdt " + Int2Hex(nYear, 4) + " " + Int2Hex(nMon, 2) + " " + Int2Hex(nDay, 2) + "\n\r");
	CheckStatus(eRet);

	char *pData = NULL;
	int nLen = 0;
	eRet = (E_Status_t)ReadData(&pData, nLen);
	CheckStatus(eRet);
	DELARR(pData);

	return eRet;
}

int UsbProtocal::SetTime(int nHour, int nMin)			//st
{
	E_Status_t eRet = (E_Status_t)SendSetCommand("st " + Int2Hex(nHour, 2) + " " + Int2Hex(nMin, 2) + "\n\r");
	CheckStatus(eRet);

	char *pData = NULL;
	int nLen = 0;
	eRet = (E_Status_t)ReadData(&pData, nLen);
	CheckStatus(eRet);
	DELARR(pData);

	return eRet;
}

int UsbProtocal::SetEmpty(int address)
{
	return SendSetCommand("set " + Int2Hex(address, 2) + " " + " 0" );
}

int UsbProtocal::Set(int address, int value)
{
	return SendSetCommand("set " + Int2Hex(address, 2) + " " + Int2Hex(value, 2) + "\n\r");
}

int UsbProtocal::SendSetCommand(std::string szCmd)
{
	E_Status_t eRet = UNKNOW_STATUS;
	//EnterCriticalSection(&m_csMutex);
	eRet = (E_Status_t)WriteDivice(szCmd.c_str(), szCmd.size());
	//LeaveCriticalSection(&m_csMutex);
	CheckStatus(eRet)
	return eRet;
}

int UsbProtocal::SendReadCommand(std::string szCmd)
{
	E_Status_t eRet = UNKNOW_STATUS;
	//EnterCriticalSection(&m_csMutex);
	eRet = (E_Status_t)WriteDivice(szCmd.c_str(), szCmd.size());
	//LeaveCriticalSection(&m_csMutex);
	CheckStatus(eRet)
	return eRet;
}

int UsbProtocal::Help()
{
	return SendReadCommand("?\n\r");
}

int UsbProtocal::ReadRSet()
{
	return SendReadCommand("rset\n\r");
}

int UsbProtocal::ReadRecord()
{
	return SendReadCommand("rrcd\n\r");
}

int UsbProtocal::ClearRecord()
{
	return SendReadCommand("clr\n\r");
}

std::string UsbProtocal::Int2Hex(int value, int digit)
{
	char szVal[MAX_LEN] = { 0 };
	sprintf_s(szVal, sizeof(szVal), "%X", value);
	return szVal;
}

int8_t UsbProtocal::HexToInt(char c)
{
	if('0' <= c && c <= '9')
		return c - '0';
	else if ('A' <= c&&c <= 'F')
		return 10 + c-'A';
	else if ('a' <= c && c <= 'f')
		return 10 + c - 'a';
	else 
		return -1;
}

uint64_t UsbProtocal::HexToASCII(const char* szIn)
{
	if(NULL == szIn) return -1;
	//if('0' != szIn[0] || 'x' != szIn[1])
	//{//说明输入的格式串不是十六进制数据
	//	//return atoll(szIn);
	//	return atol(szIn);
	//}
	//16进制串转换成整型
	char cInt;
	uint64_t ulValue = 0;
	//for(uint16_t usIdx = 2; szIn[usIdx]; usIdx++)
	for(uint16_t usIdx = 0; szIn[usIdx]; usIdx++)
	{
		if(-1 == (cInt = HexToInt(szIn[usIdx])))
			return -1;
		ulValue = ulValue << 4;
		ulValue |= cInt;
	}
	return ulValue;
}


parse_status_t UsbProtocal::ScannerGetToken(INOUT const char *scanner, OUT memptr *token, OUT token_type_t *tok_type)
{
	if (NULL == scanner) return PARSE_FAILURE;

	const char *cursor;
	int c;
	token_type_t token_type;
	int got_end_quote;


	cursor = scanner;
	c = *cursor;


	if (IsIdentifierChar(c))
	{
		/* scan identifier */
		token->buf = (char*)cursor++;
		token_type = TT_IDENTIFIER;
		while (IsIdentifierChar(*cursor))
			cursor++;
		/* calc token length */
		token->length = (size_t)cursor - (size_t)token->buf;
	}
	else if (c == ' ' || c == '\t')
	{
		token->buf = (char*)cursor++;
		token_type = TT_WHITESPACE;
		while ((*cursor == ' ' || *cursor == '\t'))
			cursor++;
		token->length = (size_t)cursor - (size_t)token->buf;

	}
	else if (c == TOKCHAR_CR) 
	{
		/* scan CRLF */
		token->buf = (char*)cursor++;
		if (*cursor != TOKCHAR_LF) 
		{
			/* couldn't match CRLF; match as CR */
			token_type = TT_CTRL;	/* ctrl char */
			token->length = (size_t)1;
		}
		else
		{
			/* got CRLF */
			token->length = (size_t)2;
			token_type = TT_CRLF;
			cursor++;
		}
	}
	else if (c == TOKCHAR_LF) /* accept \n as CRLF */
	{
		token->buf = (char*)cursor++;
		token->length = (size_t)1;
		token_type = TT_CRLF;
	}
	else if (c == '"') 
	{
		/* quoted text */
		token->buf = (char*)cursor++;
		token_type = TT_QUOTEDSTRING;
		got_end_quote = FALSE;
		//while (cursor < null_terminator) {
		c = *cursor++;
		if (c == '"') 
		{
			got_end_quote = TRUE;
			//	break;
		} 
		else if (c == '\\') 
		{
			//if (cursor < null_terminator) {
			c = *cursor++;
			/* the char after '\\' could be ANY octet */
			//}
			/* else, while loop handles incomplete buf */
		} 
		else if (IsQdtextChar(c))
		{
			/* just accept char */
		} 
		else
			/* bad quoted text */
			return PARSE_FAILURE;
		//}
		if (got_end_quote)
			token->length = (size_t)cursor - (size_t)token->buf;
		else 
		{	/* incomplete */

			//assert(cursor == null_terminator);
			return PARSE_INCOMPLETE;
		}
	}
	else if (IsSeparatorChar(c)) 
	{
		/* scan separator */
		token->buf = (char*)cursor++;
		token_type = TT_SEPARATOR;
		token->length = (size_t)1;
	}
	else if (IsControlChar(c)) 
	{
		/* scan ctrl char */
		token->buf = (char*)cursor++;
		token_type = TT_CTRL;
		token->length = (size_t)1;
	}
	else
	{
		//error
		return PARSE_FAILURE;
	}

	//scanner->cursor += token->length;	/* move to next token */
	*tok_type = token_type;
	return PARSE_OK;
}



__END_NAMESPACE(Protocal)



