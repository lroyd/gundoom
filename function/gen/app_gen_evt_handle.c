/*************************************************************************
	> File Name: app_gen_evt_handle.c
	> Author: lroyd
	> Mail: 
	> Created Time: 2016年01月27日 星期三 12时38分57秒
 ************************************************************************/

#include "app_gen_private.h"
#include "pro_code_if.h"
#include "pmem.h"

/************************************************************************
 *     
 ***********************************************************************/
static void FuncNetConnServerCfm(void *p_msg)
{
   AppNetConnSocketCfm(1);
}


/************************************************************************
 *      gen处理函数,包括 PRO_DM 层反上来的设备 
 ***********************************************************************/
void FuncGenEvtHandle(void** data, EventId event, void* msg)
{
    switch(event)
    {
		case PRO_NET_CONN_SER_CFM:
		{
			FuncNetConnServerCfm(msg);
			break;
		}


        default:  
			APP_GEN_LOG(LOG_ERROR, LOG_GEN_CFM, "Unknown event %.4X", event);
            break;
    }
	
	if(msg)
	{
		BtPmemFree(msg);
	}
}


