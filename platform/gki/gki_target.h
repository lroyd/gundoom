#ifndef GKI_TARGET_H
#define GKI_TARGET_H

#ifdef __cplusplus
extern "C" {
#endif


#define GKI_DEBUG TRUE

#define GKI_API

/******************************************************************************
**
** Task configuration
**
******************************************************************************/
#define BTU_TASK                0
#define BTIF_TASK               1
#define A2DP_MEDIA_TASK         2
#define GKI_MAX_TASKS           3

/******************************************************************************
**
** Timer configuration
**
******************************************************************************/
#ifndef GKI_NUM_TIMERS
#define GKI_NUM_TIMERS              (5)
#endif

#ifndef TICKS_PER_SEC
#define TICKS_PER_SEC               (100)
#endif

#ifndef GKI_MS_TO_TICKS
#define GKI_MS_TO_TICKS(x)		((x)/(1000/TICKS_PER_SEC))
#endif

#ifndef GKI_SECS_TO_TICKS
#define GKI_SECS_TO_TICKS(x)	((x)*(TICKS_PER_SEC))
#endif

#ifndef GKI_TICKS_TO_MS
#define GKI_TICKS_TO_MS(x)		((x)*1000/TICKS_PER_SEC)
#endif

#ifndef GKI_TICKS_TO_SECS
#define GKI_TICKS_TO_SECS(x)	((x)/TICKS_PER_SEC)
#endif

#ifndef OS_TICKS_PER_SEC
#define OS_TICKS_PER_SEC			(1000)
#endif

#ifndef GKI_OS_TICKS_TO_MS
#define GKI_OS_TICKS_TO_MS(x)		((x)*1000/OS_TICKS_PER_SEC)
#endif

#ifndef GKI_OS_TICKS_TO_SECS
#define GKI_OS_TICKS_TO_SECS(x)		((x)/OS_TICKS_PER_SEC)
#endif

#ifndef GKI_DELAY_STOP_SYS_TICK
#define GKI_DELAY_STOP_SYS_TICK     (10)
#endif

#ifndef GKI_TIMER_LIST_NOPREEMPT
#define GKI_TIMER_LIST_NOPREEMPT		(FALSE)
#endif

/******************************************************************************
**
** Buffer configuration
**
******************************************************************************/
/* TRUE if GKI uses dynamic buffers. */
#ifndef GKI_USE_DYNAMIC_BUFFERS
#define GKI_USE_DYNAMIC_BUFFERS			(TRUE)
#endif

/********************************************************/
#ifndef GKI_BUF0_SIZE
#define GKI_BUF0_SIZE               64	//#define GKI_BUF0_SIZE   (32)
#endif

#ifndef GKI_BUF0_MAX
#define GKI_BUF0_MAX                50	//define GKI_BUF0_MAX    (200)
#endif

#ifndef GKI_POOL_ID_0
#define GKI_POOL_ID_0               0	//#define GKI_POOL_ID_0   (0)
#endif
/********************************************************/
#ifndef GKI_BUF1_SIZE
#define GKI_BUF1_SIZE               288	//#define GKI_BUF1_SIZE   (64)
#endif

#ifndef GKI_BUF1_MAX
#define GKI_BUF1_MAX                26	//#define GKI_BUF1_MAX    (100)
#endif

#ifndef GKI_POOL_ID_1
#define GKI_POOL_ID_1               1	//#define GKI_POOL_ID_1   (1)
#endif
/********************************************************/
#ifndef GKI_BUF2_SIZE
#define GKI_BUF2_SIZE               660	//#define GKI_BUF2_SIZE   (128)
#endif

#ifndef GKI_BUF2_MAX
#define GKI_BUF2_MAX                30	//#define GKI_BUF2_MAX    (50)
#endif

#ifndef GKI_POOL_ID_2
#define GKI_POOL_ID_2               2	//#define GKI_POOL_ID_2   (2)
#endif
/********************************************************/
#ifndef GKI_BUF3_SIZE
#define GKI_BUF3_SIZE               (1024+16)	//#define GKI_BUF3_SIZE   (256)
#endif

#ifndef GKI_BUF3_MAX
#define GKI_BUF3_MAX                20	//#define GKI_BUF3_MAX    (10)
#endif

#ifndef GKI_POOL_ID_3
#define GKI_POOL_ID_3               3	//#define GKI_POOL_ID_3   (3)
#endif
/********************************************************/
#ifndef GKI_MAX_BUF_SIZE
#define GKI_MAX_BUF_SIZE            GKI_BUF3_SIZE
#endif

#ifndef GKI_MAX_BUF_SIZE_POOL_ID
#define GKI_MAX_BUF_SIZE_POOL_ID    GKI_POOL_ID_3
#endif
/********************************************************/
#ifndef GKI_BUF4_SIZE
#define GKI_BUF4_SIZE               (4096+16)
#endif

#ifndef GKI_BUF4_MAX
#define GKI_BUF4_MAX                15
#endif

#ifndef GKI_POOL_ID_4
#define GKI_POOL_ID_4               4
#endif
/********************************************************/

#ifndef GKI_NUM_FIXED_BUF_POOLS
#define GKI_NUM_FIXED_BUF_POOLS     5
#endif

#ifndef GKI_DEF_BUFPOOL_PERM_MASK
#define GKI_DEF_BUFPOOL_PERM_MASK   0xfde0
#endif

#ifndef GKI_NUM_TOTAL_BUF_POOLS
#define GKI_NUM_TOTAL_BUF_POOLS     5
#endif


#ifndef GKI_BUF5_MAX
#define GKI_BUF5_MAX                64
#endif

/* The ID of buffer pool 5. */
#ifndef GKI_POOL_ID_5
#define GKI_POOL_ID_5               5
#endif


#ifndef GKI_BUF5_SIZE
#define GKI_BUF5_SIZE               748
#endif
/********************************************************/

#ifndef GKI_ENABLE_BUF_CORRUPTION_CHECK
#define GKI_ENABLE_BUF_CORRUPTION_CHECK TRUE
#endif


#ifndef GKI_SEVERE
#define GKI_SEVERE(code)
#endif


#ifndef GKI_DEBUG
#define GKI_DEBUG                   FALSE
#endif


#ifndef GKI_MAX_EXCEPTION
#define GKI_MAX_EXCEPTION           8
#endif


#ifndef GKI_MAX_EXCEPTION_MSGLEN
#define GKI_MAX_EXCEPTION_MSGLEN    64
#endif

#ifndef GKI_SEND_MSG_FROM_ISR
#define GKI_SEND_MSG_FROM_ISR    FALSE
#endif



#ifndef GKI_POOL_ID_6
#define GKI_POOL_ID_6               6
#endif

#ifndef GKI_BUF6_SIZE
#define GKI_BUF6_SIZE               268
#endif

#ifndef GKI_BUF6_MAX
#define GKI_BUF6_MAX                60
#endif

#ifndef GKI_POOL_ID_7
#define GKI_POOL_ID_7               7
#endif

#ifndef GKI_BUF7_SIZE
#define GKI_BUF7_SIZE               (10240 + 24)
#endif

#ifndef GKI_BUF7_MAX
#define GKI_BUF7_MAX                2
#endif

#ifndef GKI_POOL_ID_8
#define GKI_POOL_ID_8               8
#endif

#ifndef GKI_BUF8_SIZE
#define GKI_BUF8_SIZE               128
#endif

#ifndef GKI_BUF8_MAX
#define GKI_BUF8_MAX                30
#endif

#ifndef GKI_POOL_ID_9
#define GKI_POOL_ID_9              9
#endif

#ifndef GKI_BUF9_SIZE
#define GKI_BUF9_SIZE            8192
#endif

#ifndef GKI_BUF9_MAX
#define GKI_BUF9_MAX           5
#endif

#ifndef NO_GKI_RUN_RETURN
#define NO_GKI_RUN_RETURN  TRUE
#endif


#ifdef __cplusplus
}
#endif


#endif


