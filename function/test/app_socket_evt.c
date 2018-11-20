/*************************************************************************
	> File Name: app_gen_evt.c
	> Author: lroyd
	> Mail: 
	> Created Time: 2016年01月27日 星期三 12时37分39秒
 ************************************************************************/
#include "app_gen_private.h"



void AppNetConnSocketCfm(uint16_t status)
{
    MAKE_EVENT(APP_NET_CONN_SER_CFM);    //此时分配的是 栈空间
    msg.status = status;

	APP_GEN_LOG(LOG_TRACE, LOG_GEN_CFM, "AppNetConnSocketCfm status = %s", status?"OK":"NOK");
	AppSendGenMsg( APP_NET_CONN_SER_CFM, &msg);
}
