#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <errno.h>
#include <sys/times.h>

#include <pthread.h>
#include <semaphore.h>

#include "gki_init.h"



#define WAKE_LOCK_ID "wake_lock"

#define __likely(cond)  __builtin_expect(!!(cond),1)
#define __unlikely(cond)  __builtin_expect(!!(cond),0)

#define pthread_cond_timedwait_monotonic	pthread_cond_timedwait

#define NANOSEC_PER_MILLISEC (1000000)
#define NSEC_PER_SEC (1000*NANOSEC_PER_MILLISEC)

/* works only for 1ms to 1000ms heart beat ranges */
#define LINUX_SEC (1000/TICKS_PER_SEC)

typedef struct
{
    UINT8 task_id;          /* GKI task id */
    TASKPTR task_entry;     /* Task entry function*/
    void    *params;          /* Extra params to pass to task entry function 原来 uint32_t */
} gki_pthread_info_t;

/*****************************************************************************
*
***  Static variables
*
*******************************************************************************/
INT32 g_GkiTimerWakeLockOn = 0;
gki_pthread_info_t gki_pthread_info[GKI_MAX_TASKS];

#ifdef NO_GKI_RUN_RETURN
static pthread_t            timer_thread_id = 0;
static INT32                  shutdown_timer = 0;
#endif

tGKI_CB   gki_cb;

static int acquire_wake_lock(int lock, const char* id){return 0;}
static int release_wake_lock(const char* id){return 0;}
/*****************************************************************************
*** Function        gki_task_entry
*** Description     GKI pthread callback
*** Returns         void
********************************************************************************/
void gki_task_entry(void *params)
{
    /* 32bit */
    gki_pthread_info_t *p_pthread_info = ((gki_pthread_info_t *)(params));
    //gki_pthread_info_t *p_pthread_info = ((gki_pthread_info_t *)((unsigned long)params));
   
    gki_cb.os.thread_id[p_pthread_info->task_id] = pthread_self();
    //prctl(PR_SET_NAME, (unsigned long)gki_cb.com.OSTName[p_pthread_info->task_id], 0, 0, 0);
    
    GKI_LOGD("%s task_id=%i [%s] starting", __func__,p_pthread_info->task_id,\
             gki_cb.com.OSTName[p_pthread_info->task_id]);

    /* Call the actual thread entry point */
    (p_pthread_info->task_entry)(&(p_pthread_info->params));
    /* 64bit 系统使用 */
    //(p_pthread_info->task_entry)(((long *)&(p_pthread_info->params)));
    
    GKI_LOGD("%s task_id=%i [%s] terminating", __func__, p_pthread_info->task_id,\
            gki_cb.com.OSTName[p_pthread_info->task_id]);

    pthread_exit(0);    /* GKI tasks have no return value */
}
/* end android */
/*****************************************************************************/


void GKI_init(void)
{
    pthread_mutexattr_t attr;
    tGKI_OS             *p_os;

    memset (&gki_cb, 0, sizeof (gki_cb));
    
    gki_buffer_init();
    gki_timers_init();
    gki_cb.com.OSTicks = (uint32_t)times(0);

    p_os = &gki_cb.os;

    pthread_mutexattr_init(&attr);
    pthread_mutex_init(&p_os->GKI_mutex, &attr);
	
#if (GKI_DEBUG == TRUE)
    pthread_mutex_init(&p_os->GKI_trace_mutex, NULL);
#endif
    /* time  */
    p_os->no_timer_suspend = GKI_TIMER_TICK_RUN_COND;
    pthread_mutex_init(&p_os->gki_timer_mutex, NULL);
#ifndef NO_GKI_RUN_RETURN
    pthread_cond_init(&p_os->gki_timer_cond, NULL);
#endif
}


/*****************************************************************************/
void GKI_init_task_state(UINT8 task_id, UINT8 *taskname)
{
    gki_cb.com.OSRdyTbl[task_id]    = TASK_READY;
    gki_cb.com.OSTName[task_id]     = taskname;
    gki_cb.com.OSWaitTmr[task_id]   = 0;
    gki_cb.com.OSWaitEvt[task_id]   = 0;

    /* Initialize mutex and condition variable objects for events and timeouts */
    pthread_mutex_init(&gki_cb.os.thread_evt_mutex[task_id], NULL);
    pthread_cond_init (&gki_cb.os.thread_evt_cond[task_id], NULL);
    pthread_mutex_init(&gki_cb.os.thread_timeout_mutex[task_id], NULL);
    pthread_cond_init (&gki_cb.os.thread_timeout_cond[task_id], NULL);
    
    sem_init(&gki_cb.os.thread_sem[task_id], 0, 0);
    
}

/* avoid the heap and heap-using library functions supplied by arm */

void *GKI_os_malloc (uint32_t size)
{
    return (malloc(size));
}


void GKI_os_free (void *p_mem)
{
    if(p_mem != NULL)
        free(p_mem);
    return;
}


UINT8 GKI_suspend_task (UINT8 task_id)
{
    GKI_LOGD("%s %d done", __func__,task_id);

    return (GKI_SUCCESS);
}

UINT8 GKI_resume_task (UINT8 task_id)
{
    GKI_LOGD("%s %d done", __func__, task_id);

    return (GKI_SUCCESS);
}

void GKI_sched_lock(void)
{
  //GKI_LOGD("%s ",__func__);
    return;
}

void GKI_sched_unlock(void)
{
  //GKI_LOGD("%s ",__func__);
    return;
}

void GKI_enable (void)
{
   //GKI_LOGD("%s ",__func__);
    pthread_mutex_unlock(&gki_cb.os.GKI_mutex);

    return;
}

void GKI_disable (void)
{
	//GKI_LOGD("%s ",__func__);
    pthread_mutex_lock(&gki_cb.os.GKI_mutex);

    return;
}

void GKI_exit_task (UINT8 task_id)
{
    GKI_disable();
    gki_cb.com.OSRdyTbl[task_id] = TASK_DEAD;

    /* Destroy mutex and condition variable objects */
    pthread_mutex_destroy(&gki_cb.os.thread_evt_mutex[task_id]);
    pthread_cond_destroy (&gki_cb.os.thread_evt_cond[task_id]);
    //pthread_mutex_destroy(&gki_cb.os.thread_timeout_mutex[task_id]);
    //pthread_cond_destroy (&gki_cb.os.thread_timeout_cond[task_id]);

    GKI_enable();

    //GKI_send_event(task_id, EVENT_MASK(GKI_SHUTDOWN_EVT));

    GKI_LOGD("%s %d done", __func__,task_id);
    return;
}
/*******************************************************************************
**
** Function         GKI_get_taskid
**
** Description      This function gets the currently running task ID.
**
** Returns          task ID
**
** NOTE             The Broadcom upper stack and profiles may run as a single task.
**                  If you only have one GKI task, then you can hard-code this
**                  function to return a '1'. Otherwise, you should have some
**                  OS-specific method to determine the current task.
**
*******************************************************************************/
int8_t GKI_get_taskid (void)
{
    INT32 i;

    pthread_t thread_id = pthread_self( );

   //GKI_LOGD("%s thread_id %x", __func__,(INT32)thread_id);

    for (i = 0; i < GKI_MAX_TASKS; i++) 
    {
        if (gki_cb.os.thread_id[i] == thread_id) 
        {
            GKI_LOGD("%s [TRUST] %x %d done",__func__, thread_id, i);
            return(i);
        }
    }

    //GKI_LOGD("%s task id = -1",__func__);

    return(-1);
}


void GKI_set_taskid (UINT8 _ucId)
{
    pthread_t thread_id = pthread_self();

    gki_cb.os.thread_id[_ucId] = thread_id;

   //GKI_LOGD("%s thread_id %x",__func__,(INT32)thread_id);
}

/*******************************************************************************
**
** Function         GKI_wait
**
** Description      This function is called by tasks to wait for a specific
**                  event or set of events. The task may specify the duration
**                  that it wants to wait for, or 0 if infinite.
**
** Parameters:      flag -    (input) the event or set of events to wait for
**                  timeout - (input) the duration that the task wants to wait
**                                    for the specific events (in system ticks)
**
**
** Returns          the event mask of received events or zero if timeout
**
*******************************************************************************/
UINT16 GKI_wait (UINT16 flag, uint32_t timeout)
{
    UINT16 evt;
    UINT8 rtask;

	struct timespec abstime = { 0, 0 };

    int sec;
    int nano_sec;
    rtask = GKI_get_taskid();

    GKI_LOGD("%s in %d %x %d",__func__, (INT32)rtask, (INT32)flag, (INT32)timeout);

    //gki_cb.com.OSWaitForEvt[rtask] = flag;

    /* protect OSWaitEvt[rtask] from modification from an other thread */
    pthread_mutex_lock(&gki_cb.os.thread_evt_mutex[rtask]);

    if (!(gki_cb.com.OSWaitEvt[rtask] & flag))
    {
    	/*  没有东西才进来 */
        if (timeout)
        {
            clock_gettime(CLOCK_MONOTONIC, &abstime);

            /* add timeout */
            sec = timeout / 1000;
            nano_sec = (timeout % 1000) * NANOSEC_PER_MILLISEC;
            abstime.tv_nsec += nano_sec;
            if (abstime.tv_nsec > NSEC_PER_SEC)
            {
                abstime.tv_sec += (abstime.tv_nsec / NSEC_PER_SEC);
                abstime.tv_nsec = abstime.tv_nsec % NSEC_PER_SEC;
            }
            abstime.tv_sec += sec;

            pthread_cond_timedwait_monotonic(&gki_cb.os.thread_evt_cond[rtask],
                    &gki_cb.os.thread_evt_mutex[rtask], &abstime);
        }
        else
        {
            pthread_cond_wait(&gki_cb.os.thread_evt_cond[rtask], &gki_cb.os.thread_evt_mutex[rtask]);
        }

        /* TODO: check, this is probably neither not needed depending on phtread_cond_wait() implmentation,
         e.g. it looks like it is implemented as a counter in which case multiple cond_signal
         should NOT be lost! */

        /* we are waking up after waiting for some events, so refresh variables
           no need to call GKI_disable() here as we know that we will have some events as we've been waking
           up after condition pending or timeout */

        if (gki_cb.com.OSTaskQFirst[rtask][0])
            gki_cb.com.OSWaitEvt[rtask] |= TASK_MBOX_0_EVT_MASK;
        if (gki_cb.com.OSTaskQFirst[rtask][1])
            gki_cb.com.OSWaitEvt[rtask] |= TASK_MBOX_1_EVT_MASK;
        if (gki_cb.com.OSTaskQFirst[rtask][2])
            gki_cb.com.OSWaitEvt[rtask] |= TASK_MBOX_2_EVT_MASK;
        if (gki_cb.com.OSTaskQFirst[rtask][3])
            gki_cb.com.OSWaitEvt[rtask] |= TASK_MBOX_3_EVT_MASK;

        if (gki_cb.com.OSRdyTbl[rtask] == TASK_DEAD)
        {
            gki_cb.com.OSWaitEvt[rtask] = 0;
            /* unlock thread_evt_mutex as pthread_cond_wait() does auto lock when cond is met */
            pthread_mutex_unlock(&gki_cb.os.thread_evt_mutex[rtask]);
            return (EVENT_MASK(GKI_SHUTDOWN_EVT));
        }
    }

    /* Clear the wait for event mask */
    //gki_cb.com.OSWaitForEvt[rtask] = 0;

    /* Return only those bits which user wants... */
    evt = gki_cb.com.OSWaitEvt[rtask] & flag;

    /* Clear only those bits which user wants... */
    gki_cb.com.OSWaitEvt[rtask] &= ~flag;

    /* unlock thread_evt_mutex as pthread_cond_wait() does auto lock mutex when cond is met */
    pthread_mutex_unlock(&gki_cb.os.thread_evt_mutex[rtask]);

    GKI_LOGD("%s out %d %x %d %x done",__func__, (INT32)rtask, (INT32)flag, (INT32)timeout, (INT32)evt);
    return (evt);
}


/*******************************************************************************
**
** Function         GKI_send_event
**
** Description      This function is called by tasks to send events to other
**                  tasks. Tasks can also send events to themselves.
**
** Parameters:      task_id -  (input) The id of the task to which the event has to
**                  be sent
**                  event   -  (input) The event that has to be sent
**
**
** Returns          GKI_SUCCESS if all OK, else GKI_FAILURE
**
*******************************************************************************/
UINT8 GKI_send_event (UINT8 task_id, UINT16 event)
{
    /* use efficient coding to avoid pipeline stalls */
    if (task_id < GKI_MAX_TASKS)
    {
        /* protect OSWaitEvt[task_id] from manipulation in GKI_wait() */
        pthread_mutex_lock(&gki_cb.os.thread_evt_mutex[task_id]);
        
        /* Set the event bit */
        gki_cb.com.OSWaitEvt[task_id] |= event;
        
        pthread_cond_signal(&gki_cb.os.thread_evt_cond[task_id]);
        
        pthread_mutex_unlock(&gki_cb.os.thread_evt_mutex[task_id]);
        
        GKI_LOGD("%s %d %x done",__func__, task_id, event);
        return ( GKI_SUCCESS );
    }
    GKI_LOGD("%s FAILED!!",__func__);
    return (GKI_FAILURE);
}


/*******************************************************************************
*** 
*** Function         GKI_create_task
***
*** Description      This function is called to create a new OSS task.
***
*** Parameters:      task_entry  - (input) pointer to the entry function of the task
***                  task_id     - (input) Task id is mapped to priority
***                  taskname    - (input) name given to the task
***                  stack       - (input) pointer to the top of the stack (highest memory location)
***                  stacksize   - (input) size of the stack allocated for the task 
***
*** Returns          GKI_SUCCESS if all OK, GKI_FAILURE if any problem
***
*** NOTE             This function take some parameters that may not be needed
***                  by your particular OS. They are here for compatabilit
***                  of the function prototype.
***
********************************************************************************/
UINT8 GKI_create_task (TASKPTR task_entry, \
                        UINT8 task_id, \
                        int8_t *taskname,\
                        UINT16 *stack, \
                        UINT16 stacksize )
{
    UINT16 i;
    UINT8 *p;

    struct sched_param param;
    INT32 policy, ret = 0;
    pthread_attr_t attr1;

    if (task_id >= GKI_MAX_TASKS)
    {
        GKI_LOGE("%s Error! task ID > max task allowed",__func__);
        return (GKI_FAILURE);
    }

    gki_cb.com.OSRdyTbl[task_id]    = TASK_READY;
    gki_cb.com.OSTName[task_id]     = taskname;
    gki_cb.com.OSWaitTmr[task_id]   = 0;
    gki_cb.com.OSWaitEvt[task_id]   = 0;
    
	
	gki_cb.com.OSStack[task_id]		= (UINT8 *)stack;
	gki_cb.com.OSStackSize[task_id]	= stacksize;
	
    pthread_mutex_init(&gki_cb.os.thread_evt_mutex[task_id], NULL);
    pthread_cond_init (&gki_cb.os.thread_evt_cond[task_id], NULL);
    pthread_mutex_init(&gki_cb.os.thread_timeout_mutex[task_id], NULL);
    pthread_cond_init (&gki_cb.os.thread_timeout_cond[task_id], NULL);

    pthread_attr_init(&attr1);
#if ( FALSE == GKI_PTHREAD_JOINABLE  )
    pthread_attr_setdetachstate(&attr1, PTHREAD_CREATE_DETACHED);
    GKI_LOGD("%s %i", __func__,task_id);
#else
    GKI_LOGD("%s JOINABLE task %i",__func__, task_id);
#endif
    
    gki_pthread_info[task_id].task_id = task_id;
    gki_pthread_info[task_id].task_entry = task_entry;
    gki_pthread_info[task_id].params = 0;
    
    ret = pthread_create( &gki_cb.os.thread_id[task_id],\
                            &attr1,\
                            (void *)gki_task_entry,\
                            &gki_pthread_info[task_id]);
    if (ret != 0)
    {
        GKI_LOGD("%s pthread_create failed(%d), %s!",__func__, ret, taskname);
        return GKI_FAILURE;
    }
    
    if(pthread_getschedparam(gki_cb.os.thread_id[task_id], &policy, &param)==0)
    {
#if (GKI_LINUX_BASE_POLICY!=GKI_SCHED_NORMAL)
#if defined(PBS_SQL_TASK)
        if (task_id == PBS_SQL_TASK)
        {
            GKI_LOGD("%s PBS SQL lowest priority task",__func__);
            policy = SCHED_NORMAL;
        }
        else
#endif
#endif
        {
            /* check if define in gki_int.h is correct for this compile environment! */
            policy = GKI_LINUX_BASE_POLICY;
#if (GKI_LINUX_BASE_POLICY!=GKI_SCHED_NORMAL)
            param.sched_priority = GKI_LINUX_BASE_PRIORITY - task_id - 2;
#endif
        }
        pthread_setschedparam(gki_cb.os.thread_id[task_id], policy, &param);
    }
    return (GKI_SUCCESS);
}


/*******************************************************************************
**
** Function         GKI_task_self_cleanup
**
** Description      This function is used in the case when the calling thread
**                  is exiting itself. The GKI_destroy_task function can not be
**                  used in this case due to the pthread_join call. The function
**                  cleans up GKI control block associated to the terminating
**                  thread.
**
** Parameters:      task_id     - (input) Task id is used for sanity check to
**                                 make sure the calling thread is in the right
**                                 context.
**
** Returns          None
**
*******************************************************************************/
void GKI_task_self_cleanup(UINT8 task_id)
{
    UINT8 my_task_id = GKI_get_taskid();

    if (task_id != my_task_id)
    {
        GKI_LOGE("%s Wrong context - current task %d is not the given task id %d",\
                      __func__, my_task_id, task_id);
        return;
    }

    if (gki_cb.com.OSRdyTbl[task_id] != TASK_DEAD)
    {
        /* paranoi settings, make sure that we do not execute any mailbox events */
        gki_cb.com.OSWaitEvt[task_id] &= ~(TASK_MBOX_0_EVT_MASK|TASK_MBOX_1_EVT_MASK|
                                            TASK_MBOX_2_EVT_MASK|TASK_MBOX_3_EVT_MASK|
                                            TASK_MBOX_4_EVT_MASK);

#if (GKI_NUM_TIMERS > 0)
        gki_cb.com.OSTaskTmr0R[task_id] = 0;
        gki_cb.com.OSTaskTmr0 [task_id] = 0;
#endif

#if (GKI_NUM_TIMERS > 1)
        gki_cb.com.OSTaskTmr1R[task_id] = 0;
        gki_cb.com.OSTaskTmr1 [task_id] = 0;
#endif

#if (GKI_NUM_TIMERS > 2)
        gki_cb.com.OSTaskTmr2R[task_id] = 0;
        gki_cb.com.OSTaskTmr2 [task_id] = 0;
#endif

#if (GKI_NUM_TIMERS > 3)
        gki_cb.com.OSTaskTmr3R[task_id] = 0;
        gki_cb.com.OSTaskTmr3 [task_id] = 0;
#endif

#if (GKI_NUM_TIMERS > 4)
        gki_cb.com.OSTaskTmr4R[task_id] = 0;
        gki_cb.com.OSTaskTmr4 [task_id] = 0;
#endif

        GKI_exit_task(task_id);

        /* Calling pthread_detach here to mark the thread as detached.
           Once the thread terminates, the system can reclaim its resources
           without waiting for another thread to join with.
        */
        pthread_detach(gki_cb.os.thread_id[task_id]);
    }
}

/*******************************************************************************
**
** Function         GKI_shutdown
**
** Description      shutdowns the GKI tasks/threads in from max task id to 0 and frees
**                  pthread resources!
**                  IMPORTANT: in case of join method, GKI_shutdown must be called outside
**                  a GKI thread context!
**
** Returns          void
**
*******************************************************************************/

void GKI_shutdown(void)
{
    UINT8 task_id;

    INT32 result;

#ifdef GKI_USE_DEFERED_ALLOC_BUF_POOLS
    gki_dealloc_free_queue();
#endif
    /* release threads and set as TASK_DEAD. going from low to high priority fixes
     * GKI_exception problem due to btu->hci sleep request events  */
    for (task_id = GKI_MAX_TASKS; task_id > 0; task_id--)
    {
        if (gki_cb.com.OSRdyTbl[task_id - 1] != TASK_DEAD)
        {
            gki_cb.com.OSRdyTbl[task_id - 1] = TASK_DEAD;

            /* paranoi settings, make sure that we do not execute any mailbox events */
            gki_cb.com.OSWaitEvt[task_id-1] &= ~(TASK_MBOX_0_EVT_MASK|TASK_MBOX_1_EVT_MASK|
                                                TASK_MBOX_2_EVT_MASK|TASK_MBOX_3_EVT_MASK);
            GKI_send_event(task_id - 1, EVENT_MASK(GKI_SHUTDOWN_EVT));

            result = pthread_join( gki_cb.os.thread_id[task_id-1], NULL );

            if ( result < 0 )
            {
                GKI_LOGE("%s pthread_join() FAILED: result: %d",__func__, result );
            }
            // GKI_ERROR_LOG( "GKI_shutdown(): task %s dead\n", gki_cb.com.OSTName[task_id]);
            GKI_exit_task(task_id - 1);
        }
    }

    /* Destroy mutex and condition variable objects */
    pthread_mutex_destroy(&gki_cb.os.GKI_mutex);
#if (GKI_DEBUG == TRUE)
    pthread_mutex_destroy(&gki_cb.os.GKI_trace_mutex);
#endif

#ifdef NO_GKI_RUN_RETURN
    shutdown_timer = 1;
#endif

    if (g_GkiTimerWakeLockOn)
    {
        GKI_LOGD("%s GKI_shutdown :  release_wake_lock(brcm_btld)",__func__);
        release_wake_lock(WAKE_LOCK_ID);
        g_GkiTimerWakeLockOn = 0;
    }
}

/*******************************************************************************
 **
 ** Function        gki_system_tick_start_stop_cback
 **
 ** Description     This function runs a task
 **
 ** Parameters:     start: TRUE start system tick (again), FALSE stop
 **
 ** Returns         void
 **
 *********************************************************************************/

void gki_system_tick_start_stop_cback(UINT8 start)
{
    tGKI_OS         *p_os = &gki_cb.os;
    INT32    *p_run_cond = &p_os->no_timer_suspend;
    static INT32 wake_lock_count;

    if ( FALSE == start )
    {
        /* gki_system_tick_start_stop_cback() maybe called even so it was already stopped! */
        if (GKI_TIMER_TICK_RUN_COND == *p_run_cond)
        {

            /* take free mutex to block timer thread */
            pthread_mutex_lock(&p_os->gki_timer_mutex);
            /* this can lead to a race condition. however as we only read this variable in the
             * timer loop we should be fine with this approach. otherwise uncomment below mutexes.
             */
            /* GKI_disable(); */
            *p_run_cond = GKI_TIMER_TICK_STOP_COND;
            /* GKI_enable(); */
#include <sys/times.h>

            GKI_LOGD("%s >>> STOP GKI_timer_update(), wake_lock_count:%d", __func__,--wake_lock_count);
            release_wake_lock(WAKE_LOCK_ID);
            g_GkiTimerWakeLockOn = 0;
        }
    }
    else
    {
        /* restart GKI_timer_update() loop */
        //acquire_wake_lock(PARTIAL_WAKE_LOCK, WAKE_LOCK_ID);

        g_GkiTimerWakeLockOn = 1;
        *p_run_cond = GKI_TIMER_TICK_RUN_COND;

        pthread_mutex_unlock( &p_os->gki_timer_mutex );
        GKI_LOGD("%s >>> START GKI_timer_update(), wake_lock_count:%d",__func__, ++wake_lock_count );
    }
}

/*******************************************************************************
**
** Function         GKI_run
**
** Description     
**
** Parameters     
**
** Returns       
**
*******************************************************************************/
void *timer_thread(void *arg)
{
    INT32 timeout_ns = 0;
    struct timespec timeout;
    struct timespec previous = {0,0};
    struct timespec current;

    INT32 err;
    INT32 delta_ns;
    INT32 restart;
    tGKI_OS     *p_os = &gki_cb.os;
    INT32 *p_run_cond = &p_os->no_timer_suspend;

    restart = 1;

    //prctl(PR_SET_NAME, (unsigned long)"gki timer", 0, 0, 0);

    TIM_LOGD("%s start..",__func__);
    while(!shutdown_timer)
    {
        if(*p_run_cond == GKI_TIMER_TICK_STOP_COND)
        {
            //GKI_TRACE("GKI_run lock mutex");
            pthread_mutex_lock(&p_os->gki_timer_mutex);
            //GKI_TRACE("GKI_run unlock mutex");
            pthread_mutex_unlock(&p_os->gki_timer_mutex);
            restart = 1;
        }
        
        clock_gettime(CLOCK_MONOTONIC, &current);
        if(__unlikely(restart))
        {
            restart = 0;
            timeout_ns = (GKI_TICKS_TO_MS(1)*1000000);
        }
        else
        {
            delta_ns  = current.tv_nsec - previous.tv_nsec;
            delta_ns += (current.tv_sec - previous.tv_sec)*1000000000;
            timeout_ns += (GKI_TICKS_TO_MS(1)*1000000) - delta_ns;
        }
        previous = current;
        
        timeout.tv_sec = 0;
        
        if(__unlikely(timeout_ns < ((GKI_TICKS_TO_MS(1)*1000000)*0.1)))
        {
            timeout.tv_nsec = (GKI_TICKS_TO_MS(1)*1000000)*0.1;
            
            if(timeout_ns < GKI_TICKS_TO_MS(-5)*1000000)
            {
                TIM_LOGE("%s tick delayed > 5 slots(%d,%d)--cpu overload?",__func__,
                timeout_ns, GKI_TICKS_TO_MS(-5)*1000000);
            }
        }
        else
        {
            timeout.tv_nsec = timeout_ns;    
        }
        do
        {
            err = nanosleep(&timeout, &timeout);   
        }while(err < 0 && errno == EINTR);
        
        GKI_timer_update(1);
    }

    TIM_LOGD("%s gki:Exiting timer_thread",__func__);

    pthread_exit(NULL);
    return NULL;
}

static void gki_set_timer_scheduling(void)
{
    pid_t               main_pid = getpid();
    struct sched_param  param;
    INT32                 policy;

    policy = sched_getscheduler(main_pid);

    if ( policy != -1 )
    {
        TIM_LOGD("%s scheduler current policy: %d", __func__,policy);

        /* ensure highest priority in the system + 2 to allow space for read threads */
        param.sched_priority = GKI_LINUX_TIMER_TICK_PRIORITY;

        if ( 0!=sched_setscheduler(main_pid, GKI_LINUX_TIMER_POLICY, &param ) )
        {
            TIM_LOGD("%s failed with error: %d", __func__,errno);
        }
    }
    else
    {
        TIM_LOGD("%s failed: %d", __func__,errno);
    }
}

void GKI_run(void *p_task_id)
{
    struct timespec delay;    
    INT32 err;
    volatile INT32 *p_run_cond = &gki_cb.os.no_timer_suspend;

    //gki_set_timer_scheduling();

    GKI_timer_queue_register_callback(gki_system_tick_start_stop_cback);
    GKI_LOGD("%s Start/Stop GKI_timer_update_registered!",__func__);

    pthread_attr_t timer_attr;

    shutdown_timer = 0;

    pthread_attr_init(&timer_attr);

    if(pthread_create(  &timer_thread_id, 
                        &timer_attr,
                        timer_thread,
                        NULL)!=0)
    {
        //GKI_ERR("pthread_create failed to create timer_thread!\n");
        GKI_LOGD("%s pthread_create failed to create timer_thread!",__func__);
        return;
    }
    return;
}


/*****************************************************************************
**
** Function        GKI_freeze
**
** Description     Freeze GKI. Relevant only when NO_GKI_RUN_RETURN is defined
**
** Returns
**
*******************************************************************************/

void GKI_freeze()
{
   shutdown_timer = 1;
   pthread_mutex_unlock( &gki_cb.os.gki_timer_mutex );
   /* Ensure that the timer thread exits */
   pthread_join(timer_thread_id, NULL);
}

