/*************************************************************************
	> File Name: app_gen_ind.c
	> Author: lroyd
	> Mail: 
	> Created Time: 2016年01月27日 星期三 12时37分39秒
 ************************************************************************/
#include "app_gen_private.h"

void AppGenDevRecvDataInd(int fd, int len, uint8_t *p_data)
{
    MAKE_EVENT(APP_GEN_DEV_RCV_DATA_IND);    //此时分配的是 栈空间
    msg.fd		= fd;
	msg.len		= len;
	msg.p_buf	= p_data;
	APP_GEN_LOG(LOG_TRACE, LOG_GEN_IND, "AppGenDevRecvDataInd fd = %d, len = %d, data = %s", msg.fd, msg.len, msg.p_buf);
	AppSendGenIndMsg(APP_GEN_DEV_RCV_DATA_IND, &msg);
}

void AppGenDevFdAttachInd(int fd, int mfd)
{
    MAKE_EVENT(APP_GEN_DEV_FD_ADD_IND);    
    msg.fd		= fd;
	msg.mfd		= mfd;
	APP_GEN_LOG(LOG_TRACE, LOG_GEN_IND, "AppGenDevFdAttachInd fd = %d, mfd = %d", msg.fd, msg.mfd);
	AppSendGenIndMsg(APP_GEN_DEV_FD_ADD_IND, &msg);
}

void AppGenDevFdSeparateInd(int fd)
{
    MAKE_EVENT(APP_GEN_DEV_FD_DEL_IND);    
    msg.fd		= fd;
	APP_GEN_LOG(LOG_TRACE, LOG_GEN_IND, "AppGenDevFdSeparateInd fd = %d", msg.fd);
	AppSendGenIndMsg(APP_GEN_DEV_FD_DEL_IND, &msg);
}

//1:up	0:down
void AppGenLinkChangeInd(int state)
{
    MAKE_EVENT(APP_GEN_LINK_CHANGE_IND);    
    msg.state		= state;
	APP_GEN_LOG(LOG_TRACE, LOG_GEN_IND, "AppGenLinkChangeInd state = %s", msg.state?"up":"down");
	AppSendGenIndMsg(APP_GEN_LINK_CHANGE_IND, &msg);
}