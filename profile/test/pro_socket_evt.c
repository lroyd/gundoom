/*************************************************************************
	> File Name: pro_socket_cmd.c
	> Author: lroyd.H
	> Mail: 
	> Created Time: 2016年02月02日 星期二 13时16分51秒
 ************************************************************************/
#include "pro_socket_action.h"
#include "message.h"



uint8_t ProNetConnectServerCfm(uint8_t status)
{
    MAKE_MESSAGE(PRO_NET_CONN_SER_CFM); 
    
    msg->status = status;
    return ProSendMsgCfm(PRO_NET_CONN_SER_CFM, msg);
}
