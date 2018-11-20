/*************************************************************************
	> File Name: 
	> Author: lroyd
	> Mail: htzhangxmu@163.com
	> Created Time: 
 ************************************************************************/
#ifndef __HCI_DEBUG_H__
#define __HCI_DEBUG_H__


/************************************************************************
* 			LOG	Setting
************************************************************************/
#include "common.h"

#define LOG_TAG_STR			("HCI")
#define LOG_TAG_LEVE		LOG_DEBUG		//LOG_TRACE

#ifndef GHSUA_CONSOLE_LOG	
	/* 格式：input(xxxxxx) ->[tag] xxxxxx */
	#define HCI_LOG(level, format, arg...)	if(level < LOG_TAG_LEVE){printf("[%s]"format"", LOG_TAG_STR, ##arg);printf("\r\n");}
#else
	#include "log_file.h"
	/*L0 格式：input(xxxxxx) ->[time][tag] xxxxxx */
	/*L1 格式：input(xxxxxx) ->[time][tag][func:line] xxxxxx */
	#define HCI_LOG(level, format, arg...)	if(level < LOG_TAG_LEVE){syslog_wrapper_L1(level, "[%s]"format"", LOG_TAG_STR, ##arg);}		
#endif





#endif


