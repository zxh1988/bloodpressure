/*************************************************
  Copyright (C), 2015-2020, 理邦精密仪器股份有限公司
  @File name:	
  @Author:   	xianhui zhang
  @Date:     	2016-09-02
  @Description:  
*************************************************/
#ifndef LIBBPDEV_INTERFACE_H
#define LIBBPDEV_INTERFACE_H
#include "hdr.h"


#ifdef LIBBPDEVEXPORT                                                 
#define BPDEVEXPORT extern "C" __declspec(dllexport)
#else
#define BPDEVEXPORT extern "C" __declspec(dllimport)
#endif



#define INOUT                    
#define IN                       
#define OUT 




//////////////////////////////////////////////////////////////////////////
/// @function [BpInitDevice]
/// @brief [初始化设备]
///
/// @param [in] szDevContent	[设备内容:
///	HID USB设备JSON格式： {"TAG":"Usb","Param":{wVID:"xxx",wPID:"xxx"}}；
/// 串口设备JSON格式：{"TAG":"Serial","Param":{Com："名称"，nBaud："波特率"，parity："奇偶检验位"，DataBit："数据位"，StopBits:"停止位"}}  
///                             ]
/// @return						[int 类型]
//////////////////////////////////////////////////////////////////////////
BPDEVEXPORT int BpInitDevice(IN const char *szDevContent);

//////////////////////////////////////////////////////////////////////////
/// @function [BpOpenDevice]
/// @brief [打开设备]
///
/// @return						[int 类型]
//////////////////////////////////////////////////////////////////////////
BPDEVEXPORT int BpOpenDevice();

//////////////////////////////////////////////////////////////////////////
/// @function [BpCloseDevice]
/// @brief [关闭设备]
///
/// @return				[int 类型]
//////////////////////////////////////////////////////////////////////////
BPDEVEXPORT int BpCloseDevice();

/// @function [BpWriteDevice]
/// @brief [写数据到设备]
///
/// @param [in] pData	[数据内容]
/// @param [in] nSize	[数据长度]
/// @param [OUT] pResultData	[返回的结果数据，无结果返回值为空]
/// @param [OUT] nResultSize	[返回的结果数据大小，无结果返回值为0]
/// @return				[int 类型]
//////////////////////////////////////////////////////////////////////////
BPDEVEXPORT int BpWriteDevice(IN const char *pData, IN int nSize, \
	OUT char **pResultData,  OUT int &nResultSize);

//////////////////////////////////////////////////////////////////////////
/// @function [BpReadDevice]
/// @brief [读设备]
///
/// @param [OUT] pData	[数据内容]
/// @param [OUT] nSize	[数据长度]
/// @return				[int 类型]
//////////////////////////////////////////////////////////////////////////
BPDEVEXPORT int BpReadDevice(OUT char **pData, OUT int &nSize);


//////////////////////////////////////////////////////////////////////////
/// @function [BpFreeMemory]
/// @brief [读设备]
///
/// @param [in] pData	[数据内容]
/// @return				[int 类型]
//////////////////////////////////////////////////////////////////////////
BPDEVEXPORT int BpFreeMemory(IN char **pData);




#endif



