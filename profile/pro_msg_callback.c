/*************************************************************************
	> File Name: pro_socket_cmd.c
	> Author: lroyd.H
	> Mail: 
	> Created Time: 2016年02月02日 星期二 13时16分51秒
 ************************************************************************/
#include "message.h"
#include "os_declare.h"


//回调cfm
uint8_t ProSendMsgCfm(EventId opcode, void *msg)
{
    TSK_INTER_SEND_MSG(APP_GEN_TASK, opcode, msg);
    return 0;
}


//回调ind
