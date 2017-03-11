/*************************************************
  Copyright (C), 2015-2020, 理邦精密仪器股份有限公司
  @File name:	
  @Author:   	xianhui zhang
  @Date:     	2016-09-02
  @Description:  
*************************************************/
#ifndef TYPEDEF_H
#define TYPEDEF_H
#include "hdr.h"



#define FALSE 0
#define TRUE  1


//////////////////////////////////////////////////////////////////////////
#if defined(__cplusplus)
#define __BEGIN_NAMESPACE(_x) namespace _x {
#define __END_NAMESPACE(_x) };
#define	__BEGIN_DECLS	extern "C" {
#define	__END_DECLS	}
#else
#define __BEGIN_NAMESPACE(_x)
#define __END_NAMESPACE(_x)
#define	__BEGIN_DECLS
#define	__END_DECLS
#endif


#define DELPTR(PPTR) {	\
	if (NULL != PPTR) {delete PPTR; PPTR = NULL;}	\
}

#define DELARR(ARR)	{	\
	if (NULL != ARR) {delete[] ARR; ARR = NULL;}	\
}

#define  CheckNullPtr(PPTR){	\
	if (NULL == PPTR){return FAIL;}	\
}


#define  CheckStatus(RES){	\
	if (SUCCESS != RES) {return RES;}	\
}



#endif



