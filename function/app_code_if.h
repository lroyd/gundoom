/*************************************************************************
	> File Name: app_gen_private.h
	> Author: lroyd
	> Mail: 
	> Created Time: 2016年01月27日 星期三 12时39分41秒
 ************************************************************************/
#ifndef __APP_CODE_IF_H__
#define __APP_CODE_IF_H__

#ifdef __cplusplus
extern "C"{
#endif

#include "common.h"


/* define the invalid opcode value */
#define APP_INVALID_OPCODE  (0xFFFF)
#define isCmd(x)    ((!(x&0x8000)) && (!(x&0X4000)))
#define isRes(x)    (x&0x8000)


#define APP_GEN_GROUP_ID_OFFSET (0<<8)
#define GENCOMMAND(x)           (x|APP_GEN_GROUP_ID_OFFSET)
#define GENINDICATE(x)          (x|APP_GEN_GROUP_ID_OFFSET|0x4000)
#define GENEVENTE(x)            (x|APP_GEN_GROUP_ID_OFFSET|0x8000)

/*************************************** GEN [req:cfm]****************************************/
//对应的命令 req-cfm
typedef struct
{
    uint8_t  server_ip[31 + 1];  //\0
    uint8_t	 server_port;
} APP_NET_CONN_SER_REQ_T;

typedef struct
{
    uint16_t status;
}APP_NET_CONN_SER_CFM_T;

//之所以都放在一起是因为不能重复

#define APP_NET_CONN_SER_REQ			GENCOMMAND(0x0007)			
#define APP_NET_CONN_SER_CFM			GENEVENTE(APP_NET_CONN_SER_REQ)



/*************************************** GEN [ind]****************************************/
/********************** DEVICE设备相关 ********************/
//一个命令对应一个结构体
typedef struct
{
    int		fd;
	int		len;
	char	*p_buf;	//具体数据
}APP_GEN_DEV_RCV_DATA_IND_T;
#define APP_GEN_DEV_RCV_DATA_IND		GENINDICATE(0x0000)		//底层设备数据接收

//设备fd增加
typedef struct
{
    int		fd;	
	int		mfd;
}APP_GEN_DEV_FD_ADD_IND_T;
#define APP_GEN_DEV_FD_ADD_IND			GENINDICATE(0x0001)		
//设备fd删除
typedef struct
{
    int		fd;	
}APP_GEN_DEV_FD_DEL_IND_T;
#define APP_GEN_DEV_FD_DEL_IND			GENINDICATE(0x0002)		

//netlink	
typedef struct
{
    int		state;	//1:up	0:down
}APP_GEN_LINK_CHANGE_IND_T;
#define APP_GEN_LINK_CHANGE_IND			GENINDICATE(0x0003)	




#ifdef __cplusplus
}
#endif
#endif
