/*************************************************
  Copyright (C), 2015-2020, 理邦精密仪器股份有限公司
  @File name:	
  @Author:   	xianhui zhang
  @Date:     	2016-09-02
  @Description:  
*************************************************/
#ifndef ISDEVERVER_H
#define ISDEVERVER_H
#include "typedef.h"

#define IN
#define OUT

__BEGIN_NAMESPACE(DevServer)


class IDevServer
{
public:
	IDevServer(){};
	virtual ~IDevServer(){};


public:
	//////////////////////////////////////////////////////////////////////////
	/// @function [InitDevice]
	/// @brief [初始化设备]
	///
	/// @param [in] szDevContent	[设备内容:
	///	HID USB设备JSON格式： {"TAG":"Usb","Param":{wVID:"xxx",wPID:"xxx"}}；
	/// 串口设备JSON格式：{"TAG":"Serial","Param":{Com："名称"，nBaud："波特率"，parity："奇偶检验位"，DataBit："数据位"，StopBits:"停止位"}}  
	///                             ]
	/// @return						[int 类型]
	//////////////////////////////////////////////////////////////////////////
	virtual int InitDevice(IN const char *szDevContent) = 0;

	//////////////////////////////////////////////////////////////////////////
	/// @function [OpenDevice]
	/// @brief [打开设备]
	///
	/// @return						[int 类型]
	//////////////////////////////////////////////////////////////////////////
	virtual int OpenDevice() = 0;

	//////////////////////////////////////////////////////////////////////////
	/// @function [CloseDevice]
	/// @brief [关闭设备]
	///
	/// @return				[int 类型]
	//////////////////////////////////////////////////////////////////////////
	virtual int CloseDevice() = 0;

	/// @function [WriteDevice]
	/// @brief [写数据到设备]
	///
	/// @param [in] pData	[数据内容]
	/// @param [in] nSize	[数据长度]
	/// @param [OUT] pResultData	[返回的结果数据，无结果返回值为空]
	/// @param [OUT] nResultSize	[返回的结果数据大小，无结果返回值为0]
	/// @return				[int 类型]
	//////////////////////////////////////////////////////////////////////////
	virtual int WriteDevice(IN const char *pData, IN int nSize, \
		OUT char **pResultData,  OUT int &nResultSize) = 0;

	//////////////////////////////////////////////////////////////////////////
	/// @function [ReadDevice]
	/// @brief [读设备]
	///
	/// @param [OUT] pData	[数据内容]
	/// @param [OUT] nSize	[数据长度]
	/// @param [in] nTimeOut	[超时时间]
	/// @return				[int 类型]
	//////////////////////////////////////////////////////////////////////////
	virtual int ReadDevice(OUT char **pData, OUT int &nSize) = 0;


	//////////////////////////////////////////////////////////////////////////
	/// @function [FreeMemory]
	/// @brief [读设备]
	///
	/// @param [in] pData	[数据内容]
	/// @return				[int 类型]
	//////////////////////////////////////////////////////////////////////////
	virtual int FreeMemory(IN char **pData) = 0;



};











__END_NAMESPACE(DevServer)


#endif



