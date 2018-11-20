/*************************************************************************
	> File Name: app_gen_cmd.h
	> Author: lroyd
	> Mail: 
	> Created Time: 2016年01月27日 星期三 12时37分32秒
 ************************************************************************/
#ifndef __APP_GEN_H__
#define __APP_GEN_H__

#ifdef __cplusplus
extern "C"{
#endif

#include "common.h"



//cfm_t 添加





typedef enum
{
	APP_GEN_GROUP_ID = 0x00,	//命令CFM
	APP_SQLITE_GROUP_ID,		//数据库操作
	APP_MEDIA_GROUP_ID, 		//媒体服务：rtsp,rtmp,p2p,sip
	APP_xxxx_GROUP_ID, 			//预留
	APP_GROUP_ID_MAX
}app_group_id;


typedef void (*handlerCbFunc)(uint8_t type, uint16_t opcode, void *p_data);

extern uint8_t AppDevicePowerReq(char sw);


extern uint8_t AppNetConnLinkSocket(uint8_t *server_ip, uint8_t server_port);



#ifdef __cplusplus
}
#endif
#endif
