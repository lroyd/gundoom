/******************************************************************
**
**
******************************************************************/
#include "os_declare.h"
#include "btu_declare.h"



tBTU_CB   btu_cb = {0}; 


/******************************************************************
*			任务初始化
******************************************************************/
void btu_task_init(void** data)
{ 

}

/******************************************************************
*			主任务处理
*			注意 释放
******************************************************************/
void btu_task(void** data, EventId event, void* p_data)
{
	syslog_wrapper_L0(LOG_ERROR, "btu_task event = 0x%04x, data = %s", event, p_data);
	int ret  =0;

	switch(event)
	{
		case EVT_TO_TIMER_MSK:		//使用定时器的时候发送
		{
			/* user timer event 100ms */
			//btu_process_timer(event); 
			break;
		}

		default:
			syslog_wrapper_L0(LOG_ERROR, "btu_task unknown event = 0x%04x, data = %s",event, p_data);
			break;
	}	
	
	if (p_data)
	{
		GKI_freebuf(p_data);  
	}
	
}
/******************************************************************
*			任务析构
******************************************************************/
void btu_task_deinit(void** data)
{
	
}


