/******************************************************************
**
**
******************************************************************/
#include "os_declare.h"
#include "btu_declare.h"



tBTU_CB   btu_cb = {0}; 


/******************************************************************
*			�����ʼ��
******************************************************************/
void btu_task_init(void** data)
{ 

}

/******************************************************************
*			��������
*			ע�� �ͷ�
******************************************************************/
void btu_task(void** data, EventId event, void* p_data)
{
	syslog_wrapper_L0(LOG_ERROR, "btu_task event = 0x%04x, data = %s", event, p_data);
	int ret  =0;

	switch(event)
	{
		case EVT_TO_TIMER_MSK:		//ʹ�ö�ʱ����ʱ����
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
*			��������
******************************************************************/
void btu_task_deinit(void** data)
{
	
}


