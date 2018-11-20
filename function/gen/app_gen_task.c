/*************************************************************************
	> File Name: app_gen_task.c
	> Author: lroyd
	> Mail: 
	> Created Time: 2016年01月27日 星期三 14时41分52秒
 ************************************************************************/
#include "app_gen_private.h"
#include "pro_code_if.h"

void FuncGenInit(void** p_data)
{
    g_ptHostMangerInfo = malloc(sizeof(T_HostMangerInfo));
    memset(g_ptHostMangerInfo, 0, sizeof(T_HostMangerInfo));
    
    /* 初始化赋值 */
    g_ptHostMangerInfo->emState		= NEWF_ST_OFF;
    g_ptHostMangerInfo->pendingCmd	= APP_INVALID_OPCODE;
}


void FuncGenHandle(void** data, EventId event, void *msg)
{
    switch(event & PRO_CHK_MSK)
    {
		case PRO_CMD_MSK:	//gen cmd 层过来的
			FuncGenCmdHandle(data, event, msg);
			break;

		case PRO_IND_MSK:	//1.直通底层的数据ind	2.pro 层过来的通知事件或数据
			FuncGenIndHandle(data, event, msg);
			break;

		case PRO_CFM_MSK:	//pro 层过来的cfm
			FuncGenEvtHandle(data, event, msg);
			break;

    }
}

void FuncGenDeinit(void** data)
{

}

