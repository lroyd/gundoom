#ifndef __GKI_COMMON_H__
#define __GKI_COMMON_H__

#ifdef __cplusplus
extern "C" {
#endif


#include "common.h"
#include "gki.h"

#include "utils.h"

#define TASK_DEAD       0   /* b0000 */
#define TASK_READY      1   /* b0001 */
#define TASK_WAIT       2   /* b0010 */
#define TASK_DELAY      4   /* b0100 */
#define TASK_SUSPEND    8   /* b1000 */


/********************************************************************
**  Internal Error codes
*********************************************************************/
#define GKI_ERROR_BUF_CORRUPTED         0xFFFF
#define GKI_ERROR_NOT_BUF_OWNER         0xFFFE
#define GKI_ERROR_FREEBUF_BAD_QID       0xFFFD
#define GKI_ERROR_FREEBUF_BUF_LINKED    0xFFFC
#define GKI_ERROR_SEND_MSG_BAD_DEST     0xFFFB
#define GKI_ERROR_SEND_MSG_BUF_LINKED   0xFFFA
#define GKI_ERROR_ENQUEUE_BUF_LINKED    0xFFF9
#define GKI_ERROR_DELETE_POOL_BAD_QID   0xFFF8
#define GKI_ERROR_BUF_SIZE_TOOBIG       0xFFF7
#define GKI_ERROR_BUF_SIZE_ZERO         0xFFF6
#define GKI_ERROR_ADDR_NOT_IN_BUF       0xFFF5
#define GKI_ERROR_OUT_OF_BUFFERS        0xFFF4
#define GKI_ERROR_GETPOOLBUF_BAD_QID    0xFFF3


typedef struct _buffer_hdr {
    struct _buffer_hdr *p_next;
    unsigned char q_id;
    unsigned char task_id;
    unsigned char status;
    unsigned char Type;
}BUFFER_HDR_T;

typedef struct _free_queue
{
    BUFFER_HDR_T *p_first;
    BUFFER_HDR_T *p_last;

    unsigned short    size;
    unsigned short    total;
    unsigned short    cur_cnt;
    unsigned short    max_cnt;

}FREE_QUEUE_T;


#define ALIGN_POOL(pl_size) ((((pl_size) + 3)/sizeof(uint32_t))* sizeof(uint32_t))
#define BUFFER_HDR_SIZE (sizeof(BUFFER_HDR_T))
#define BUFFER_PADDING_SIZE (sizeof(BUFFER_HDR_T) + sizeof(uint32_t))
#define MAX_USER_BUF_SIZE ((uint16_t)0xffff - BUFFER_PADDING_SIZE)
#define MAGIC_NO (0xDDBADDBA)

#define BUF_STATUS_FREE     0
#define BUF_STATUS_UNLINKED 1
#define BUF_STATUS_QUEUED   2

// btla-specific ++
//#define GKI_USE_DEFERED_ALLOC_BUF_POOLS	//延时分配，init buffer 时不提前分配
// btla-specific --


typedef struct
{
#if (!defined GKI_USE_DEFERED_ALLOC_BUF_POOLS && (GKI_USE_DYNAMIC_BUFFERS == FALSE))



#else	
    /* dynamic buffer user */
#if (GKI_NUM_FIXED_BUF_POOLS > 0) 
    UINT8     *bufpool0;
#endif

#if (GKI_NUM_FIXED_BUF_POOLS > 1) 
    UINT8     *bufpool1;
#endif

#if (GKI_NUM_FIXED_BUF_POOLS > 2) 
    UINT8     *bufpool2;
#endif

#if (GKI_NUM_FIXED_BUF_POOLS > 3) 
    UINT8     *bufpool3;
#endif

#if (GKI_NUM_FIXED_BUF_POOLS > 4)
    UINT8 *bufpool4;
#endif

#if (GKI_NUM_FIXED_BUF_POOLS > 5)
    UINT8 *bufpool5;
#endif

#if (GKI_NUM_FIXED_BUF_POOLS > 6)
    UINT8 *bufpool6;
#endif

#if (GKI_NUM_FIXED_BUF_POOLS > 7)
    UINT8 *bufpool7;
#endif

#if (GKI_NUM_FIXED_BUF_POOLS > 8)
    UINT8 *bufpool8;
#endif

#if (GKI_NUM_FIXED_BUF_POOLS > 9)
    UINT8 *bufpool9;
#endif

#if (GKI_NUM_FIXED_BUF_POOLS > 10)
    UINT8 *bufpool10;
#endif

#if (GKI_NUM_FIXED_BUF_POOLS > 11)
    UINT8 *bufpool11;
#endif

#if (GKI_NUM_FIXED_BUF_POOLS > 12)
    UINT8 *bufpool12;
#endif

#if (GKI_NUM_FIXED_BUF_POOLS > 13)
    UINT8 *bufpool13;
#endif

#if (GKI_NUM_FIXED_BUF_POOLS > 14)
    UINT8 *bufpool14;
#endif

#if (GKI_NUM_FIXED_BUF_POOLS > 15)
    UINT8 *bufpool15;
#endif

#endif

    UINT8  *OSStack[GKI_MAX_TASKS];         /* pointer to beginning of stack */
    UINT16  OSStackSize[GKI_MAX_TASKS];     /* stack size available to each task */

    UINT8   *OSTName[GKI_MAX_TASKS];         /* name of the task */
    UINT8   OSRdyTbl[GKI_MAX_TASKS];        /* current state of the task */
    uint16_t  OSWaitEvt[GKI_MAX_TASKS];       /* events that have to be processed by the task */
    
    uint32_t  OSTicks;                        /* system ticks from start */
    
    /* Timer related variables */
    int32_t   OSTicksTilExp;                 /* 当前到下一次超时的滴答数 */
    uint32_t  OSTicksTilStop;
    int32_t   OSNumOrigTicks;
    uint32_t  OSWaitTmr   [GKI_MAX_TASKS];  /* ticks the task has to wait, for specific events */
    
#if(GKI_NUM_TIMERS > 0) 
    int32_t   OSTaskTmr0[GKI_MAX_TASKS];
    int32_t   OSTaskTmr0R[GKI_MAX_TASKS];
#endif

#if(GKI_NUM_TIMERS > 1) 
    int32_t   OSTaskTmr1[GKI_MAX_TASKS];
    int32_t   OSTaskTmr1R[GKI_MAX_TASKS];
#endif

#if(GKI_NUM_TIMERS > 2) 
    int32_t   OSTaskTmr2[GKI_MAX_TASKS];
    int32_t   OSTaskTmr2R[GKI_MAX_TASKS];
#endif

#if(GKI_NUM_TIMERS > 3) 
    int32_t   OSTaskTmr3[GKI_MAX_TASKS];
    int32_t   OSTaskTmr3R[GKI_MAX_TASKS];
#endif

#if(GKI_NUM_TIMERS > 4) 
    int32_t   OSTaskTmr4[GKI_MAX_TASKS];
    int32_t   OSTaskTmr4R[GKI_MAX_TASKS];
#endif

    /* Buffer related variables
    */
    BUFFER_HDR_T    *OSTaskQFirst[GKI_MAX_TASKS][NUM_TASK_MBOX]; /* array of pointers to the first event in the task mailbox */
    BUFFER_HDR_T    *OSTaskQLast [GKI_MAX_TASKS][NUM_TASK_MBOX]; /* array of pointers to the last event in the task mailbox */

    /* buffer pool management */
    FREE_QUEUE_T    freeq[GKI_NUM_TOTAL_BUF_POOLS];

    uint16_t        pool_buf_size[GKI_NUM_TOTAL_BUF_POOLS];
    uint16_t        pool_max_count[GKI_NUM_TOTAL_BUF_POOLS];
    uint16_t        pool_additions[GKI_NUM_TOTAL_BUF_POOLS];

    /* buffer pool start addresses */
    UINT8         *pool_start[GKI_NUM_TOTAL_BUF_POOLS];
    UINT8         *pool_end[GKI_NUM_TOTAL_BUF_POOLS];
    uint16_t        pool_size[GKI_NUM_TOTAL_BUF_POOLS];

    /* buffer pool access control var */
    void            *p_user_mempool;
    uint16_t        pool_access_mask;
    UINT8         pool_list[GKI_NUM_TOTAL_BUF_POOLS];
    UINT8         curr_total_no_of_pools;

    UINT8         timer_nesting;
    
    /* Time queue arrays */
    TIMER_LIST_Q    *timer_queues[GKI_MAX_TIMER_QUEUES];
    /* system. tick callback */
    SYSTEM_TICK_CBACK *p_tick_cb;
    UINT8         system_tick_running;

} tGKI_COM_CB;






extern BOOLEAN   gki_chk_buf_damage(void *);
extern void	gki_buffer_init (void);
extern void gki_timers_init(void);
extern void gki_adjust_timer_count (int32_t ticks);

#ifdef GKI_USE_DEFERED_ALLOC_BUF_POOLS
extern void      gki_dealloc_free_queue(void);
#endif




/* Debug aids
*/
typedef void  (*FP_PRINT)(char *, ...);

#if (GKI_DEBUG == TRUE)

typedef void  (*PKT_PRINT)(UINT8 *, UINT16);

extern void gki_print_task(FP_PRINT);
extern void gki_print_exception(FP_PRINT);
extern void gki_print_timer(FP_PRINT);
extern void gki_print_stack(FP_PRINT);
extern void gki_print_buffer(FP_PRINT);
extern void gki_print_buffer_statistics(FP_PRINT, INT16);
GKI_API extern void gki_print_used_bufs (FP_PRINT, UINT8);
extern void gki_dump(UINT8 *, UINT16, FP_PRINT);
extern void gki_dump2(UINT16 *, UINT16, FP_PRINT);
extern void gki_dump4(UINT32 *, UINT16, FP_PRINT);

#endif


#ifdef __cplusplus
}
#endif


#endif


