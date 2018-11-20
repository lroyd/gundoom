/*************************************************************************
	> File Name: app_gen_msg_callback.c
	> Author: lroyd
	> Mail: 
	> Created Time: 2016年01月27日 星期三 12时41分32秒
 ************************************************************************/
#include "app_gen_debug.h"
#include "app_gen_private.h"


static handlerCbFunc callBackGenList[APP_GROUP_ID_MAX] = {NULL};			//gen层回调



uint8_t AppRegisterHandleCb(handlerCbFunc fun[])
{

	int i;
    if(NEWF_ST_OFF != getHostState())
    {
        return 1;
    }

	for(i=0;i<APP_GROUP_ID_MAX;i++)
	{
		callBackGenList[i] = fun[i];

	}

	return 0;
}

/*  */
static uint8_t AppSendCommonMsg(uint8_t gid, uint8_t type, uint16_t opcode, void *msg)
{
    if(callBackGenList[gid])
    {
        callBackGenList[gid](type, opcode, msg);
        return 1;
    }
    return 0;
}
//type	0:cfm	1:ind
uint8_t AppSendGenMsg(uint16_t opcode, void *msg)
{
	//命令挂起检测
    if(isRes(opcode) && (GENEVENTE(g_ptHostMangerInfo->pendingCmd) == opcode))
    {
        g_ptHostMangerInfo->pendingCmd = APP_INVALID_OPCODE;
    }
    else
    {
		//EVT_LOGE("%s err: pendingCmd = %#006x,opcode = %#006x",__FUNCTION__, g_ptHostMangerInfo->pendingCmd, opcode);
    }
    /* 返回到应用层callback */
    return AppSendCommonMsg(APP_GEN_GROUP_ID, 0, opcode, msg);
}

uint8_t AppSendGenIndMsg(uint16_t opcode, void *msg)
{
    return AppSendCommonMsg(APP_GEN_GROUP_ID, 1, opcode, msg);
}



