/*************************************************
  Copyright (C), 2015-2020, 理邦精密仪器股份有限公司
  @File name:	
  @Author:   	xianhui zhang
  @Date:     	2016-09-02
  @Description:  
*************************************************/

#ifndef HDR_H
#define HDR_H

typedef enum E_STATUS
{
	UNKNOW_STATUS	= -1,
	SUCCESS,				//操作成功
	FAIL,					//操作失败
	NO_RECORD_DATA,			//盒子无记录数据
	EXCEPTION,				//操作设备异常
	DISCONNECT,				//设备占用或不存在
	TIMEOUT,				//读设备超时
	WRITE_DEVICE_FAIL,		//写设备失败
	READ_DEVICE_FAIL,		//读设备失败
	JSON_FORMAT_ERROR,		//JSON格式错误
	OUT_OF_MEMORY			//内存不够

}E_Status_t;






#endif

