/*************************************************************************
	> File Name: pro_socket_if.h
	> Author: lroyd
	> Mail: 
	> Created Time: 2016年02月01日 星期一 17时35分41秒
 ************************************************************************/

#ifndef _PRO_CODE_IF_H
#define _PRO_CODE_IF_H

#ifdef __cplusplus
extern "C" {
#endif



//pro层的相关通用宏

#define  PRO_CHK_MSK 	(0xFF00)
#define  PRO_CMD_MSK 	(0x0000)
#define  PRO_CFM_MSK 	(0x8000)
#define  PRO_IND_MSK 	(0x4000)

//ind相关事件
typedef enum {
	PRO_IND_RCV_DATA = 0x0000,	
	PRO_IND_FD_ATTCH,	//fd附着
	PRO_IND_FD_SEQ,		//fd脱离
	PRO_IND_LINK_UP,
	PRO_IND_LINK_DOWN,
	
}PRO_IND_TYPE;



#define PRO_DM_LOWSTREAM_OPCODE_CFM(x) 	 	(x|PRO_CFM_MSK)
#define PRO_DM_LOWSTREAM_OPCODE_IND(x) 		(x|PRO_IND_MSK)



//profile, protocol
typedef enum {
	PRO_DATA_HEX = 0x00000000,		//变长hex
	PRO_DATA_HEX_LEN,				//固长hex
	PRO_DATA_STR,					//字符串 \r\n结尾
	PRO_DATA_JSON,					//Json
	PRO_DATA_CUSTOM					//自定义，不处理
}PRO_DATA_FORMAT;


/*************************************** PRO 层只定义CFM 就行 ****************************************/

#define PRO_NET_STATE_CONN_EVT                		(0x000B)


typedef struct
{
    uint16_t  status; 
} PRO_NET_CONN_SER_CFM_T;

/* 这里要注意 上面直接数值定义的 不能和下面宏定义的冲突 !!!! */
#define PRO_NET_CONN_SER_CFM             	PRO_DM_LOWSTREAM_OPCODE_CFM(PRO_NET_STATE_CONN_EVT)  
//#define PRO_NET_CONN_SER_IND             	PRO_DM_LOWSTREAM_OPCODE_IND(PRO_NET_STATE_CONN_EVT) 









#ifdef __cplusplus
}
#endif
#endif
