/*******************************************************************************
	> File Name: 
	> Author: lroyd
	> Mail: htzhangxmu@163.com
	> Created Time: 
 *******************************************************************************/
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/time.h>

#include "uepoll.h"
#include "coerrno.h"




/************************************************************************
* 			LOG	Setting
************************************************************************/
#define LOG_TAG_STR			("EP")
#define LOG_TAG_LEVE		LOG_WARNING

#ifndef GHSUA_CONSOLE_LOG	
	/* 格式：input(xxxxxx) ->[tag] xxxxxx */
	#define LOGOUT(level, format, arg...)	if(level < LOG_TAG_LEVE){printf("[%s]"format"", LOG_TAG_STR, ##arg);printf("\r\n");}
#else
	#include "log_file.h"
	/*L0 格式：input(xxxxxx) ->[time][tag] xxxxxx */
	/*L1 格式：input(xxxxxx) ->[time][tag][func:line] xxxxxx */
	#define LOGOUT(level, format, arg...)	if(level < LOG_TAG_LEVE){syslog_wrapper_L0(level, "[%s]"format"", LOG_TAG_STR, ##arg);}		
#endif

/************************************************************************
* Name: 		
* Descriptions:
* Parameter:		
* Return:     
* **********************************************************************/
GH_DEF(int)
uepoll_create(int *_efd)
{
	*_efd = -1;
	*_efd = epoll_create(EPOLL_EVENT_NUM_MAX); 
	if(*_efd < 0)
	{
		LOGOUT(LOG_ERROR, "epoll_create error");
		return GH_EINVALIDOP;		
	}	

	return GH_SUCCESS;
}

/************************************************************************
* Name: 		
* Descriptions:
* Parameter:		
* Return:     
* **********************************************************************/
GH_DEF(int)
uepoll_add(int _efd, int _fd)
{
	_ERROR_RETURN(_efd >=0 && _fd >= 0, GH_EINVAL);
	
	struct epoll_event ev;
	ev.data.fd = _fd;
	ev.events = (EPOLLIN|EPOLLET);
	
	if(epoll_ctl(_efd, EPOLL_CTL_ADD, _fd, &ev) == -1)
	{
		LOGOUT(LOG_ERROR, "epoll_ctl add error");
		return GH_EINVALIDOP;
	}
	
	return GH_SUCCESS;
}
/************************************************************************
* Name: 		
* Descriptions:
* Parameter:		
* Return:     
* **********************************************************************/
GH_DEF(int)
uepoll_del(int _efd, int _fd) 
{
	_ERROR_RETURN(_efd >=0 && _fd >= 0, GH_EINVAL);
	
	if(epoll_ctl(_efd, EPOLL_CTL_DEL, _fd, NULL) == -1)
	{
		LOGOUT(LOG_ERROR, "epoll_ctl del error");
		return GH_EINVALIDOP;
	}
	
	return GH_SUCCESS;
}


