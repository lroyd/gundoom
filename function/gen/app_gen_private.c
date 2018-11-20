/*************************************************************************
	> File Name: app_gen_private.c
	> Author: lroyd
	> Mail: 
	> Created Time: 2016年01月27日 星期三 12时39分37秒
 ************************************************************************/
#include "app_gen_private.h"

#include "gki.h"
#include "os_sched.h"
#include "os_declare.h"
#include "message.h"
#include "bta_hal.h"



T_HostMangerInfo *g_ptHostMangerInfo = NULL;

#define BTU_STACK_SIZE (0)
#define BTU_TASK_STR    ((int8_t *)"BTU_SCHED")

uint32_t SchedStack[(BTU_STACK_SIZE + 3)/4];

uint32_t getGenCmdHandle(void)
{
    return APP_GEN_TASK;
}

uint32_t getGenEvtHandle(void)
{
    return APP_GEN_TASK;
}

uint32_t getGenStackHandle(void)
{
    return STACK_TASK;
}


NEWF_STATE getHostState(void)
{
    return g_ptHostMangerInfo->emState;
}


void setHostState(NEWF_STATE state)
{
	APP_GEN_LOG(LOG_DEBUG, LOG_GEN_CMD, "state is %d", state);
    g_ptHostMangerInfo->emState = state;
}


uint8_t sendMessage(uint16_t taskId, uint16_t id, void *msg)
{  
    TSK_INTER_SEND_MSG(taskId, id, msg);
    return 0;
}

/************************************************************************
* Name: SysPowerOn 
* Descriptions:直接启动，过渡启动（有中间态）
* Param:
* Note:
* **********************************************************************/
uint32_t SysPowerOn(void)
{
	uint8_t ret = 0;
	
	GKI_init();
	//.初始化statck（底层设备）
	stack_init();			//+++ 同步的话需要加判断，异步的不需要
	//.profile层使用的线程池
	//InitThreadPool(THREAD_INIT_FLG_SING);
	
	SchedInit();	//初始化所有task init
	//.thread->schedLoop 开启调度线程

	setHostState(NEWF_ST_INITIALIZING);
	
    ret |= GKI_create_task((TASKPTR)SchedLoop, \
                            BTU_TASK, BTU_TASK_STR, \
                            (uint16_t *)((uint8_t *)SchedStack + BTU_STACK_SIZE),\
                            sizeof(SchedStack));
	
	//.thread->time
	GKI_run(0);
	
    if(ret)
    {
        APP_GEN_LOG(LOG_ERROR, LOG_GEN_CMD, "call power on error!!");
    }	

    return 0;
}


/************************************************************************
* Name: SysPowerOff 
* Descriptions:
* Param:
* Note:
* **********************************************************************/
uint32_t SysPowerOff(void)
{
	
    return 0;
}


/************************************************************************
* Name: 
* Descriptions:
* Param:
* Note:
* **********************************************************************/
int GenNetLinkStart(int *out_fd)
{
	MAKE_HAL_MSG_IN(HAL_OP_DEV_CREATE);

	in_msg.type		= HW_DEV_NET_MONITOR;
	in_msg.port		= 0;
	in_msg.profile	= HAL_INVALID_PROFLE;
	in_msg.protocol	= PRO_DATA_CUSTOM;	
	in_msg.path		= "default";
	
	
	MAKE_HAL_MSG_OUT(HAL_OP_DEV_CREATE);
	
	HAL_IOCTL(HAL_OP_DEV_CREATE, (void *)&in_msg, (void *)&out_msg);
	
	if (out_fd) *out_fd = out_msg.fd;
	
	return 0;
}

int GenNetLinkGetState(int *out_state)
{
	
	
	
	
}
/************************************************************************
* Name: 
* Descriptions:
* Param:
* Note:
* **********************************************************************/
int GenUartOpen(uint8_t *path, int port, int *out_fd)
{
	MAKE_HAL_MSG_IN(HAL_OP_DEV_CREATE);

	in_msg.type		= HW_DEV_UART;
	in_msg.port		= port;
	in_msg.profile	= HAL_INVALID_PROFLE;
	in_msg.protocol	= PRO_DATA_CUSTOM;
	in_msg.path		= path;
	
	MAKE_HAL_MSG_OUT(HAL_OP_DEV_CREATE);
	
	HAL_IOCTL(HAL_OP_DEV_CREATE, (void *)&in_msg, (void *)&out_msg);
	
	if (out_fd) *out_fd = out_msg.fd;
	
	return 0;
}

int GenUartSendMsg(int fd, char *buf, int len)
{
	return HAL_XMIT_MSG(fd, buf, len);
}

int GenUartClose(int fd)
{
	int ufd = fd;
	HAL_IOCTL(HAL_OP_DEV_DESTROY, (void *)&ufd, NULL);
	return 0;
}

/************************************************************************
* Name: 
* Descriptions:创建net server
* Param:
* Note:
* **********************************************************************/
int GenNetDeviceOpen(int type, uint8_t *path, int port, int *out_fd)
{
	MAKE_HAL_MSG_IN(HAL_OP_DEV_CREATE);

	in_msg.type		= type;
	in_msg.port		= port;
	in_msg.profile	= HAL_INVALID_PROFLE;
	in_msg.protocol	= PRO_DATA_CUSTOM;
	in_msg.path		= path;
	
	//printf("xxxxxxx %d, %d\r\n", type, in_msg.type);
	
	MAKE_HAL_MSG_OUT(HAL_OP_DEV_CREATE);
	
	HAL_IOCTL(HAL_OP_DEV_CREATE, (void *)&in_msg, (void *)&out_msg);
	
	if (out_fd) *out_fd = out_msg.fd;
	
	return 0;
}


int GenNetDeviceClose(int fd)
{
	int sfd = fd;
	HAL_IOCTL(HAL_OP_DEV_DESTROY, (void *)&sfd, NULL);
	return 0;
}
//根据fd发送数据
int GenNetDeviceSendByFd(int fd, char *p_buf, int len)
{
	return HAL_XMIT_MSG(fd, p_buf, len);
}





