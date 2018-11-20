/*************************************************************************
	> File Name: 
	> Author: lroyd
	> Mail: htzhangxmu@163.com
	> Created Time: 
 ************************************************************************/
#ifndef __HW_TYPE_H__
#define __HW_TYPE_H__


#include "common.h"


/************************************************************************
* 			
************************************************************************/
//底层硬件使用
#define	DEV_READ_BUF_SIZE		(64)	//设备底层默认读取大小（不能太大，注意栈空间）：用户修改，根据接收协议



#define	MAJOR_DEV_MSK	(0x0000FFFF)
#define	SUB_DEV_MSK(a)	(a>>16)

#define	NET_SOCK_CONN_TIMEOUT	(5)	//tcp 连接默认时长

typedef enum 
{
#define	NET_TYPE_TCP	(0x00000000)
#define	NET_TYPE_UDP	(0x00010000)
#define	NET_TYPE_UNX	(0x00020000)	
    HW_DEV_NET_SER = 0,		//默认tcp，作为server不会主动关闭附着的fd，都是由cli自己控制断开
	HW_DEV_NET_CLI,			
	HW_DEV_NET_MONITOR,		//网络监控设备
	HW_DEV_UART,			//串口	
	HW_DEV_NOR,				//通用设备(需要open，epoll监听操作的)
	HW_DEV_BUTT
}HW_DEV_TYPE;



/************************************************************************
* 			
************************************************************************/
typedef enum {
	HW_EVT_LINK_DOWN = 0,	
	HW_EVT_LINK_UP,
	
	HW_EVT_ADD_SOCK,		//网络fd附着
	HW_EVT_DEL_SOCK,
	
}HW_EVT_STATUS;

typedef int (*hw_status_cb)(HW_EVT_STATUS, HW_DEV_TYPE, int, int, char *, int);



typedef struct 
{
    size_t	size;
	int		(*init)(hw_status_cb);
	int 	(*open)(int, const char *, int);
	int		(*close)(int);
	int		(*config)(int);
	int		(*epiolg)(int);
	int		(*read)(int, int, char *, int);
	int		(*write)(int, int, char *, int, char *, int);//tcp有内置dst地址，为了兼容udp使用
}T_HWDevInterface;






//usr应用层使用
/************************************************************************
* 			
************************************************************************/
typedef enum {
    TRANS_RESULT_SUCCESS = 0,
    TRANS_RESULT_FAIL,
}TRANS_RESULT;


typedef void (*cfg_result_cb)(TRANS_RESULT);
typedef void* (*malloc_cb)(int);
typedef void (*mdealloc_cb)(void *);
typedef void (*tINT_CMD_CBACK)(void *);
typedef int (*cmd_xmit_cb)(int , void *, tINT_CMD_CBACK);
typedef int (*acl_rcv_cb)(int , int, void *, int);	

typedef void (*net_status_cb)(HW_EVT_STATUS, int, int, TRANS_RESULT);  //1.子fd	2.主fd	

typedef struct {
    size_t         size;

    cfg_result_cb  fwcfg_cb;

    malloc_cb   alloc;

    mdealloc_cb dealloc;

    cmd_xmit_cb xmit_cb;

	acl_rcv_cb	rcv_cb;
	
    cfg_result_cb epilog_cb;
	
	net_status_cb status_cb;
	
}T_TransportOp;




#endif


