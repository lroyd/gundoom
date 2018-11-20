/*************************************************************************
	> File Name: stack_declare.h
	> Author: lroyd
	> Mail: 
	> Created Time: 2016年01月27日 星期三 11时41分11秒
 ************************************************************************/
#ifndef _STACK_DECLARE_H
#define _STACK_DECLARE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "common.h"
#include "gki.h"
#include "message.h"
#include "log_file.h"
#include "os_declare.h"

typedef struct
{
    uint16_t    event;
	uint16_t	len;				           
	uint16_t    layer_specific;
	void*		msg;
}BTU_HDR;

#define BTU_HDR_SIZE     (sizeof(BTU_HDR))






/*btu task time event  0x3000 - 0x3001 */
#define EVT_TO_TIMER_MSK            (0x3000)
#define EVT_TO_START_TIMER          (0x3000)
#define EVT_TO_STOP_TIMER           (0x3001)










 /* profile层向往HCI层发（需要定时重传） 必须定义 */
typedef struct
{      
	uint8_t				controller_id;
	BUFFER_Q 			cmd_xmit_q;
	BUFFER_Q 			cmd_cmpl_q;
	TIMER_LIST_ENT 		cmd_cmpl_timer;   //user kcp
	uint16_t		    cmd_window;
	/* 定时器 */
	uint16_t 			event;				
	uint32_t 			ticks;			
}tHCI_CMD_CB;



#define BTU_SEND_MODE_XMIT	(0)	//透传模式，不带头
#define BTU_SEND_MODE_ANS	(1)	//应答模式，带头

typedef struct
{
	uint8_t			sand_mode;
    TIMER_LIST_Q	quick_timer_queue;
    TIMER_LIST_Q	timer_queue;
    tHCI_CMD_CB		hci_cmd_cb[INVALID_TASK_ID];	
} tBTU_CB;

extern  tBTU_CB   btu_cb; 
/**************************************************************************
 *                        函数声明
 **************************************************************************/
void btu_task_init(void** data);
void btu_task(void** data, EventId event, void* msg);
void btu_task_deinit(void** data);

#define STACK_TASK_INIT      btu_task_init
#define STACK_TASK_HANDLE    btu_task
#define STACK_TASK_DEINIT    btu_task_deinit



#ifdef __cplusplus
}
#endif
#endif
