/*************************************************************************
	> File Name: 
	> Author: lroyd.H
	> Mail: 
	> Created Time: 2016年02月02日 星期二 10时40分05秒
 ************************************************************************/

#ifndef _PRO_SOCKET_DECLARE_H
#define _PRO_SOCKET_DECLARE_H

#ifdef __cplusplus
extern "C" {
#endif

void pro_socket_init(void **data);
void pro_socket_handle(void **data, EventId event, void *msg);
void pro_socket_deinit(void **data);

#define PRO_SOCKET_INIT    	pro_socket_init
#define PRO_SOCKET_HANDLE  	pro_socket_handle
#define PRO_SOCKET_DEINIT  	pro_socket_deinit

 
 
#ifdef __cplusplus
}
#endif
#endif
