/*************************************************************************
	> File Name: app_gen_debug.h
	> Author: lroyd
	> Mail: 
	> Created Time: 2016年01月27日 星期三 13时35分05秒
 ************************************************************************/

#ifndef __APP_GEN_DEBUG_H__
#define __APP_GEN_DEBUG_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "common.h"

#define LOG_TAG_STR			("APP_GEN")
#define LOG_GEN_CMD			("CMD")
#define LOG_GEN_CFM			("CFM")
#define LOG_GEN_IND			("IND")

#define LOG_TAG_LEVE		LOG_MAX		//LOG_TRACE

#ifndef GHSUA_CONSOLE_LOG	
	/* 格式：input(xxxxxx) ->[tag] xxxxxx */
	#define APP_GEN_LOG(level, x, format, arg...)	if(level < LOG_TAG_LEVE){printf("[%s:%s]"format"", LOG_TAG_STR, x, ##arg);printf("\r\n");}
#else
	#include "log_file.h"
	/*L0 格式：input(xxxxxx) ->[time][tag] xxxxxx */
	/*L1 格式：input(xxxxxx) ->[time][tag][func:line] xxxxxx */
	#define APP_GEN_LOG(level, x, format, arg...)	if(level < LOG_TAG_LEVE){syslog_wrapper_L0(level, "[%s:%s]"format"", LOG_TAG_STR, x, ##arg);}		
#endif

#ifdef __cplusplus
}
#endif

#endif
