/*************************************************************************
	> File Name: app_gen_private.h
	> Author: lroyd
	> Mail: 
	> Created Time: 2016年01月27日 星期三 12时39分41秒
 ************************************************************************/
#ifndef __APP_GEN_PRIVATE_H__
#define __APP_GEN_PRIVATE_H__

#ifdef __cplusplus
extern "C"{
#endif

#include "common.h"
#include "app_gen.h"
#include "message.h"
#include "app_gen_debug.h"
#include "app_code_if.h"



typedef enum
{
	NEWF_ST_OFF				= 0x00,		//关闭，中间态，epilog回调设置
	NEWF_ST_INITIALIZING,				//开启(过度态)
	NEWF_ST_READY,						//preload回调设置
	NEWF_ST_CLOSING						//关闭(过度态)
}NEWF_STATE;


//gen层管理实体
typedef struct
{
	NEWF_STATE emState;
    uint16_t pendingCmd;   



}T_HostMangerInfo;



extern T_HostMangerInfo *g_ptHostMangerInfo;



uint8_t sendMessage(uint16_t taskId, uint16_t id, void *msg);

uint32_t SysPowerOn(void);
uint32_t SysPowerOff(void);

void setHostState(NEWF_STATE state);
NEWF_STATE getHostState(void);


uint32_t getGenCmdHandle(void);
uint32_t getGenEvtHandle(void);
uint32_t getGenStackHandle(void);






#ifdef __cplusplus
}
#endif
#endif
