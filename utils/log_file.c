/*******************************************************************************
	> File Name: .h
	> Author: lroyd
	> Mail: htzhangxmu@163.com
	> Created Time: 
 *******************************************************************************/
#include "log_file.h"

//前景色:30（黑色）、31（红色）、32（绿色）、 33（黄色）、34（蓝色）、35（洋红）、36（青色）、37（白色）

#define LOG_BUFFER_SIZE_MAX					(256)

static char buf_time[32];

static char *get_log_time()
{
    struct timeval time;
    struct tm *tmp;

    /* Get Current time */
    gettimeofday(&time, NULL);
    tmp = localtime(&time.tv_sec);
    sprintf(buf_time, "%04d-%02d-%02d %02d:%02d:%02d:%03d", 
						tmp->tm_year + 1900, tmp->tm_mon + 1, tmp->tm_mday,
						tmp->tm_hour, tmp->tm_min, tmp->tm_sec, (int)(time.tv_usec/1000));
    return buf_time;
}


void CONSOLE_LOG_FATAL(const char* _pFormat, ...)
{
	unsigned char aucBuf[LOG_BUFFER_SIZE_MAX] = {0};
	va_list argp;
    va_start(argp, _pFormat);
    vsprintf(aucBuf, _pFormat, argp);
    va_end(argp);	
	
	fprintf(stdout, "[\033[1;33;40m%s\033[m]\033[1;33;40m[FATAL] %s \033[m\n", get_log_time(), aucBuf); //33（黄色）
}
void CONSOLE_LOG_ERROR(const char* _pFormat, ...)
{
	unsigned char aucBuf[LOG_BUFFER_SIZE_MAX] = {0};
	va_list argp;
    va_start(argp, _pFormat);
    vsprintf(aucBuf, _pFormat, argp);
    va_end(argp);	
	fprintf(stdout, "[\033[1;33;40m%s\033[m]\033[1;31;40m[ERROR] %s \033[m\n", get_log_time(), aucBuf); //31（红色）
}
void CONSOLE_LOG_WARNING(const char* _pFormat, ...)
{
	unsigned char aucBuf[LOG_BUFFER_SIZE_MAX] = {0};
	va_list argp;
    va_start(argp, _pFormat);
    vsprintf(aucBuf, _pFormat, argp);
    va_end(argp);
	fprintf(stdout, "[\033[1;33;40m%s\033[m]\033[1;35;40m[WARN ] %s \033[m\n", get_log_time(), aucBuf); //35（洋红）
}
void CONSOLE_LOG_DEBUG(const char* _pFormat, ...)
{
	unsigned char aucBuf[LOG_BUFFER_SIZE_MAX] = {0};
	va_list argp;
    va_start(argp, _pFormat);
    vsprintf(aucBuf, _pFormat, argp);
    va_end(argp);	
	fprintf(stdout, "[\033[1;33;40m%s\033[m]\033[1;36;40m[DEBUG] %s \033[m\n", get_log_time(), aucBuf); //36（青色）
}
void CONSOLE_LOG_INFO(const char* _pFormat, ...)
{
	unsigned char aucBuf[LOG_BUFFER_SIZE_MAX] = {0};
	va_list argp;
    va_start(argp, _pFormat);
    vsprintf(aucBuf, _pFormat, argp);
    va_end(argp);	
	fprintf(stdout, "[\033[1;33;40m%s\033[m]\033[1;32;40m[INFO ] %s\033[m\n", get_log_time(), aucBuf); //32（绿色）
}
void CONSOLE_LOG_TRACE(const char* _pFormat, ...)
{
	unsigned char aucBuf[LOG_BUFFER_SIZE_MAX] = {0};
	va_list argp;
    va_start(argp, _pFormat);
    vsprintf(aucBuf, _pFormat, argp);
    va_end(argp);	
	fprintf(stdout, "[\033[1;33;40m%s\033[m][TRACE] %s\n", get_log_time(), aucBuf); 
}

