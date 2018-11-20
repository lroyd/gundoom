/*************************************************************************
	> File Name: abslay.h
	> Author: lroyd
	> Mail: htzhangxmu@163.com
	> Created Time: 
 ************************************************************************/
#ifndef __ABSLAY_H__
#define __ABSLAY_H__


#include "common.h"
#include "hw_type.h"









GH_DECL(int) AbslayCreate(T_TransportOp *, void **);	
GH_DECL(int) AbslayDestroy(void *);

/****************************** 设备操作层 ********************************************/
GH_DECL(int) AbslayDevOpen(void *, HW_DEV_TYPE, const char *, int, int *);	
GH_DECL(int) AbslayDevClose(int);

GH_DECL(int) AbslayDevSend(int, char *, int);
GH_DECL(int) AbslayDevSendAddress(HW_DEV_TYPE, const char *, int, char *, int);
//打印fd的实体信息
GH_DECL(int) AbslayPrintInfo(int);

//组合系统使用
GH_DECL(int) AbslaySetProfile(int fd, int profile, int protocol);
GH_DECL(int) AbslayGetProfile(int, int *, int *);



#endif


