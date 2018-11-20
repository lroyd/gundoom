#ifndef __CONSOLE_LOG__
#define __CONSOLE_LOG__

#ifdef __cplusplus
extern "C"{
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>


#define syslog_wrapper_L0(level, format, arg...)	CONSOLE_##level(format, ##arg);
#define syslog_wrapper_L1(level, format, arg...)	CONSOLE_##level("[%s:%d] "format"", __FUNCTION__, __LINE__, ##arg);



#ifdef __cplusplus
}
#endif

#endif
