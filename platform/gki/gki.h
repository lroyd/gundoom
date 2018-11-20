#ifndef GKI_H
#define GKI_H

#ifdef __cplusplus
extern "C" {
#endif


#include <pthread.h>
#include <semaphore.h>

#include "common.h"
#include "gki_target.h"

#include "utils.h"
//#include "log_file.h"

#define GKI_LOGD
#define GKI_LOGW
#define GKI_LOGE
#define GKI_LOGI

#define TIM_LOGD
#define TIM_LOGW
#define TIM_LOGE
#define TIM_LOGI


#define GKI_SUCCESS         0x00
#define GKI_FAILURE         0x01
#define GKI_INVALID_TASK    0xF0
#define GKI_INVALID_POOL    0xFF



#define GKI_SCHED_NORMAL    0
#define GKI_SCHED_FIFO      1
#define GKI_SCHED_RR        2
#define GKI_LINUX_BASE_POLICY GKI_SCHED_NORMAL


#define TASK_MBOX_0    (0)
#define TASK_MBOX_1    (1)
#define TASK_MBOX_2    (2)
#define TASK_MBOX_3    (3)

#define TASK_MBOX_0_EVT_MASK   0x0001
#define TASK_MBOX_1_EVT_MASK   0x0002
#define TASK_MBOX_2_EVT_MASK   0x0004
#define TASK_MBOX_3_EVT_MASK   0x0008
#define TASK_MBOX_4_EVT_MASK   0x0010

#define NUM_TASK_MBOX			(3)
//#define BTU_HCI_RCV_MBOX        TASK_MBOX_0
//#define BTU_INPUT_RCV_MBOX      TASK_MBOX_1
//#define BTU_TASK_INNER_MBOX     TASK_MBOX_2



#define MAX_EVENTS              (16)

#define TIMER_0     (0)         //内核使用
#define TIMER_1     (1)         //用户使用
#define TIMER_2     (2)
#define TIMER_3     (3)
#define TIMER_4     (4)


#define TIMER_0_EVT_MASK   0x0010
#define TIMER_1_EVT_MASK   0x0020
#define TIMER_2_EVT_MASK   0x0040
#define TIMER_3_EVT_MASK   0x0080
#define TIMER_4_EVT_MASK   0x0100


#define APPL_EVT_0          8
#define APPL_EVT_1          9
#define APPL_EVT_2          10
#define APPL_EVT_3          11
#define APPL_EVT_4          12
#define APPL_EVT_5          13
#define APPL_EVT_6          14
#define APPL_EVT_7          15

#define EVENT_MASK(evt)       ((uint16_t)(0x0001 << (evt)))

#define GKI_SHUTDOWN_EVT    APPL_EVT_7








/************************************************************************
**  Max Time Queue
**/
#ifndef GKI_MAX_TIMER_QUEUES
#define GKI_MAX_TIMER_QUEUES    3
#endif


typedef void (SYSTEM_TICK_CBACK)(uint8_t);

/* Timer list entry callback type */
typedef void (TIMER_CBACK)(uint16_t , uint8_t, void *);
#define TIMER_PARAM_TYPE    (void *)

/* Define a timer list entry */
typedef struct _tle
{
    struct _tle     *p_next;
    struct _tle     *p_prev;
    TIMER_CBACK     *p_cback;
    void		*hci_cb;
    int32_t         ticks;			//tle timeout_ticks
    void        	*param;
    uint16_t        event;		//tle timeout_type
    //uint16_t	sub_event;	//
    uint8_t         in_use;
}TIMER_LIST_ENT;
/* define a timer list queue */
typedef struct
{
    TIMER_LIST_ENT  *p_first;
    TIMER_LIST_ENT  *p_last;
    int32_t         last_ticks;
}TIMER_LIST_Q;







/********************************************************************
**  Buffer Management Data Structures
*********************************************************************/



/* 初始化调度线程变量 */
typedef void (*TASKPTR)(void *);










GKI_API extern void GKI_enable (void);
GKI_API extern void GKI_disable (void);

GKI_API extern void GKI_init(void);
GKI_API extern void GKI_init_task_state(uint8_t task_id, uint8_t *taskname);
GKI_API extern void *GKI_os_malloc (uint32_t size);
GKI_API extern void GKI_os_free (void *p_mem);


//#define AMXIDEA_PMEM_DEBUG
#ifdef AMXIDEA_PMEM_DEBUG


#else   

GKI_API extern void   *GKI_getbuf_raw (UINT16);
GKI_API extern void    GKI_freebuf_raw (void *);
GKI_API extern void   *GKI_getpoolbuf_raw (UINT8);

//需要走内核调度的
#define GKI_getbuf(sz)      GKI_getbuf_raw((sz))
#define GKI_freebuf(ptr)    GKI_freebuf_raw((ptr))
#define GKI_getpoolbuf(id)  GKI_getpoolbuf_raw((id))

#endif 



GKI_API extern void GKI_set_taskid (uint8_t id);
GKI_API extern uint16_t GKI_wait (uint16_t flag, uint32_t timeout);
GKI_API extern void *GKI_read_mbox (uint8_t mbox);
GKI_API extern void GKI_send_msg (uint8_t task_id, uint8_t mbox, void *msg);
GKI_API extern uint8_t GKI_send_event (uint8_t task_id, uint16_t event);

GKI_API extern void GKI_init_q (BUFFER_Q *p_q);
GKI_API extern void GKI_enqueue (BUFFER_Q *p_q, void *p_buf);
GKI_API extern void GKI_enqueue_head (BUFFER_Q *p_q, void *p_buf);
GKI_API extern void *GKI_dequeue (BUFFER_Q *p_q);
GKI_API extern void *GKI_find_buf_start (void *p_user_area);
GKI_API extern uint8_t GKI_queue_is_empty(BUFFER_Q *p_q);
GKI_API extern void *GKI_getnext (void *p_buf);
GKI_API extern void *GKI_getlast (BUFFER_Q *p_q);
GKI_API extern void *GKI_getfirst (BUFFER_Q *p_q);

GKI_API extern void GKI_task_self_cleanup(uint8_t task_id);
GKI_API extern void GKI_shutdown(void);



GKI_API extern uint8_t GKI_create_task (TASKPTR task_entry, uint8_t task_id, int8_t *taskname, uint16_t *stack, uint16_t stacksize);


/* Timer management
*/
GKI_API extern void    GKI_add_to_timer_list (TIMER_LIST_Q *, TIMER_LIST_ENT  *);
GKI_API extern void    GKI_delay(uint32_t);
GKI_API extern uint32_t  GKI_get_tick_count(void);
GKI_API extern int8_t   *GKI_get_time_stamp(int8_t *);
GKI_API extern void    GKI_init_timer_list (TIMER_LIST_Q *);
GKI_API extern void    GKI_init_timer_list_entry (TIMER_LIST_ENT  *);
GKI_API extern int32_t   GKI_ready_to_sleep (void);
GKI_API extern void    GKI_remove_from_timer_list (TIMER_LIST_Q *, TIMER_LIST_ENT  *);
GKI_API extern void    GKI_start_timer(uint8_t, int32_t, uint8_t);
GKI_API extern void    GKI_stop_timer (uint8_t);
GKI_API extern void    GKI_timer_update(int32_t);
GKI_API extern uint16_t  GKI_update_timer_list (TIMER_LIST_Q *, int32_t);
GKI_API extern uint32_t  GKI_get_remaining_ticks (TIMER_LIST_Q *, TIMER_LIST_ENT  *);
GKI_API extern uint16_t  GKI_wait(uint16_t, uint32_t);

GKI_API extern uint8_t GKI_timer_queue_empty (void);
GKI_API extern void    GKI_timer_queue_register_callback(SYSTEM_TICK_CBACK *);



#ifdef __cplusplus
}
#endif


#endif


