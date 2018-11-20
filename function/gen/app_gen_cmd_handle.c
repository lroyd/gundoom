/*************************************************************************
	> File Name: app_gen_cmd_handle.c
	> Author: lroyd.H
	> QQ: 289021171
	> Created Time: 
 ************************************************************************/
#include "app_gen_private.h"
#include "pro_code_if.h"
#include "pmem.h"




static void FuncNetConnectServerReq(void *p_msg)
{
	APP_NET_CONN_SER_REQ_T *info = (APP_NET_CONN_SER_REQ_T *)p_msg;
	ProNetConnectServer(info->server_ip, info->server_port);
}
/************************************************************************
* Name: 
* Descriptions:
* Param:
* Note:
* **********************************************************************/
void FuncGenCmdHandle(void** data, EventId event, void* msg)
{
    if( isCmd(event) && g_ptHostMangerInfo->pendingCmd != APP_INVALID_OPCODE )
    {
        /* 有命令没有CFM导致挂起,不可以继续发送，释放 */
		APP_GEN_LOG(LOG_ERROR, LOG_GEN_CMD, "busy, pending command is %.4X event = 0x%.4x", g_ptHostMangerInfo->pendingCmd, event);
        if(msg)
		{
			BtPmemFree(msg);
		}
        return;
    }

    if(isCmd(event))
    {
		//挂起当前命令
        g_ptHostMangerInfo->pendingCmd = event;
    }
	APP_GEN_LOG(LOG_DEBUG, LOG_GEN_CMD, "event = %d", event);
    switch(event)
    {
		case APP_NET_CONN_SER_REQ:
		{
			FuncNetConnectServerReq(msg);
			break;
		}
		
        default:
			APP_GEN_LOG(LOG_ERROR, LOG_GEN_CMD, "Unknown event %.4X", event);
            break;
    }

	if(msg)
	{
		BtPmemFree(msg);
	}        
}


