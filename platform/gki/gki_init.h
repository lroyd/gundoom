#ifndef GKI_INT_H
#define GKI_INT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "gki_common.h"
#include <pthread.h>
#include <sys/prctl.h>


#ifndef GKI_LINUX_BASE_PRIORITY
#define GKI_LINUX_BASE_PRIORITY 30
#endif


#define GKI_SCHED_NORMAL    0
#define GKI_SCHED_FIFO      1
#define GKI_SCHED_RR        2
#ifndef GKI_LINUX_BASE_POLICY
#define GKI_LINUX_BASE_POLICY GKI_SCHED_NORMAL
#endif


#ifndef GKI_LINUX_TIMER_POLICY
#define GKI_LINUX_TIMER_POLICY GKI_SCHED_FIFO
#endif


#ifndef GKI_LINUX_TIMER_TICK_PRIORITY
#define GKI_LINUX_TIMER_TICK_PRIORITY GKI_LINUX_BASE_PRIORITY+2
#endif


#ifndef GKI_LINUX_AV_TIMER_PRIORITY
#define GKI_LINUX_AV_TIMER_PRIORITY GKI_LINUX_BASE_PRIORITY+3
#endif


#ifndef GKI_LINUX_DEFAULT_NICE_INC
#define GKI_LINUX_DEFAULT_NICE_INC -7
#endif

typedef struct
{
    /* 事件锁 */
    pthread_mutex_t     GKI_mutex; 
    pthread_t           thread_id[GKI_MAX_TASKS];
    pthread_mutex_t     thread_evt_mutex[GKI_MAX_TASKS];
    pthread_cond_t      thread_evt_cond[GKI_MAX_TASKS];
    /* 定时器锁 */
    pthread_mutex_t     thread_timeout_mutex[GKI_MAX_TASKS];
    pthread_cond_t      thread_timeout_cond[GKI_MAX_TASKS];
   
    /* timer */
    int                 no_timer_suspend;  /* 1:no suspend, 0:stop calling GKI_timer_update() */
    pthread_mutex_t     gki_timer_mutex;
    pthread_cond_t      gki_timer_cond;
    /* 信号量同步 */
    sem_t               thread_sem[GKI_MAX_TASKS];
 #if (GKI_DEBUG == TRUE)
    pthread_mutex_t     GKI_trace_mutex;
#endif   
} tGKI_OS;

#define GKI_TIMER_TICK_RUN_COND     (1)
#define GKI_TIMER_TICK_STOP_COND    (0)



//GKI_API extern void gki_system_tick_start_stop_cback(uint8_t start);



typedef struct
{
    tGKI_OS     os;
    tGKI_COM_CB com;
} tGKI_CB;

GKI_API extern tGKI_CB gki_cb;




#ifdef __cplusplus
}
#endif


#endif


