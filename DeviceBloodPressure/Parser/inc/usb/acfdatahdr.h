/*************************************************
  Copyright (C), 2015-2020, 理邦精密仪器股份有限公司
  @File name:	
  @Author:   	xianhui zhang
  @Date:     	2016-09-02
  @Description:  
*************************************************/
#ifndef ACF_DATA_HDR_H
#define ACF_DATA_HDR_H
#include <vector>
#include <iostream>

__BEGIN_NAMESPACE(Parser)


#pragma pack(push,1)
//1——按时间表测量
typedef struct tag_AUTOTABLE
{
	int nHour;		//时
	int nMin;		//分
	int nInterval;	//间隔
}AUTOTABLE, *pAUTOTABLE;
typedef std::vector<AUTOTABLE> AUTOTABLEVEC;

//2——自动
typedef struct tag_AUTO
{
	int nDay;	//默认白天15min一次
	int nNight;	//默认晚上30min一次

}AUTO, *pAUTO;

//3——手动时间间隔,自动测试
typedef struct tag_HANDLEINTERVAL
{
	int nDay;	//默认白天30min一次
	int nNight;	//默认晚上60min一次

}HANDLEINTERVAL, *pHANDLEINTERVAL;

typedef struct tag_CURSYSTIME
{
	int nYear;
	int nMon;
	int nDay;
	int nHour;
	int nMin;
	int nSec;

}CURSYSTIME, *pCURSYSTIME;



typedef struct tag_SET
{
	char szCmd[MAX_LEN];
	int nID;	//记录盒ID
	int nMode;	//工作模式：0——手动，1——按时间表测量，2——自动，白天15分钟晚上30分钟测一次，3——手动时间间隔,自动测试，4——测量压力模式
	AUTOTABLE AutoTable[6];
	/*AUTOTABLEVEC AutoTableModeVec;		//1——按时间表测量*/
	AUTO AutoMode;						//2——自动
	HANDLEINTERVAL HandleIntervalMode;	//3——手动时间间隔,自动测试
	int nTime;  //定时时间
	CURSYSTIME CurSysTime;
	//	USBUINT nAddr;
	//	USBUINT nData;

}SET, *pSET;

//sdt cmd
typedef struct tag_SDT
{
	char szCmd[MAX_LEN];
	int nYear;
	int nMon;
	int nDay;

}SDT, *pSDT;

//st cmd
typedef struct tag_ST
{
	char szCmd[MAX_LEN];
	int nHour;
	int nMin;

}ST, *pST;

#pragma pack(pop,1)


__END_NAMESPACE(Parser)


#endif




