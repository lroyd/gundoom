#include <os_sched.h>

#include "gki.h"
#include "btu_declare.h"
#include "os_conf.h"


typedef struct
{
    uint16_t  taskId;
    uint16_t  opcode;
    void*   msg;
}TaskInternalMsg;

extern taskEntry LocalTasks[INVALID_TASK_ID];

static uint8_t SchedLoopRunning = TRUE;

static void SchedTaskInit(void)
{
    uint8_t i = 0;

    for (i = 0; i < INVALID_TASK_ID; i++)
    {
        if (LocalTasks[i].init != NULL)
        {
            LocalTasks[i].init(&LocalTasks[i].data);
        }
    }
	printf("SchedTaskInit.....\r\n");
}

static void SchedTaskDeinit(void)
{
    uint8_t i = 0;

    for (; i < INVALID_TASK_ID; i++)
    {
        if (LocalTasks[i].deinit != NULL)
        {
            LocalTasks[i].deinit(&LocalTasks[i].data);
        }
    }
}

void SchedInit(void)
{
    SchedLoopRunning = TRUE;

    SchedTaskInit();

}

void SchedDeinit(void)
{
     SchedLoopRunning = FALSE;
	 
     SchedTaskDeinit();

}



void SchedLoop(uint32_t parameter)
{
    uint16_t event;
    uint8_t  handled;
    //GKI_set_taskid(0);

	//printf("SchedLoop start.....\r\n");
    while ( SchedLoopRunning )
    {
        event = GKI_wait(0xFFFF, 0);
        
		/* btu */
        if (event & TASK_MBOX_0_EVT_MASK)
        {
            TaskInternalMsg* interMsg = NULL;
            while ((interMsg = (TaskInternalMsg*)GKI_read_mbox(TASK_MBOX_0)) != NULL)
            {
                uint16_t taskId = interMsg->taskId;
                if (taskId < INVALID_TASK_ID && LocalTasks[taskId].handle != NULL)
                {
                    LocalTasks[taskId].handle(&LocalTasks[taskId].data, interMsg->opcode, interMsg->msg);
                }
                GKI_freebuf(interMsg);  //释放的是TaskInternalMsg头信息，具体的interMsg->msg是由task内部自己释放
            }		
        }       
		
		/* 任务之间 */
        if (event & TASK_MBOX_1_EVT_MASK)
        {
            TaskInternalMsg* interMsg = NULL;
            while ((interMsg = (TaskInternalMsg*)GKI_read_mbox(TASK_MBOX_1)) != NULL)
            {
                uint16_t taskId = interMsg->taskId;
                if (taskId < INVALID_TASK_ID && LocalTasks[taskId].handle != NULL)
                {
                    LocalTasks[taskId].handle(&LocalTasks[taskId].data, interMsg->opcode, interMsg->msg);
                }
                GKI_freebuf(interMsg);  
            }
        }     

        if (event & TASK_MBOX_2_EVT_MASK)
        {

	
        }


        if(event & TIMER_0_EVT_MASK)
        {

        }
        
        if(event & TIMER_1_EVT_MASK)
        {
            /* 用户定时器 每10ms进入一次 */    
            //HandleTimer();
        }
        
    }
    //printf("SchedLoop EXIT.....\r\n");
    return;
}


void TaskSendMessage(uint16_t taskId, uint16_t opcode, void* msg)
{
    TaskInternalMsg *prim = (TaskInternalMsg *)GKI_getbuf(sizeof(TaskInternalMsg));

	if (prim==NULL)
	{
		//ALOGE("fuck.....................");
	}

    prim->taskId = taskId;
    prim->opcode = opcode;
    prim->msg    = msg;
	
    //printf("MSG: %x - %s",prim, prim->msg);
    /* 发送的 */
    if(TASK_FUNCTION(taskId))
    {
        /* 发送给Stack */
        GKI_send_msg(BTU_TASK, TASK_MBOX_0, prim); 
    }
    else
    {
        /* 任务之间的发送 */
        GKI_send_msg(BTU_TASK, TASK_MBOX_1, prim);   
    }

}



