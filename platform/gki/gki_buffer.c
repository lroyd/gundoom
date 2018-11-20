#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <errno.h>
#include <sys/times.h>


#include "gki_init.h"


#if (GKI_NUM_TOTAL_BUF_POOLS > 16)
#error Number of pools out of range (16 Max)!
#endif

/*******************************************************************************
**
** Function         gki_add_to_pool_list
**
** Description      Adds pool to the pool list which is arranged in the
**                  order of size
**
** Returns          void
**
*******************************************************************************/
static void gki_add_to_pool_list(UINT8 pool_id)
{

    INT32 i, j;
    tGKI_COM_CB *p_cb = &gki_cb.com;

     /* Find the position where the specified pool should be inserted into the list */
    for(i=0; i < p_cb->curr_total_no_of_pools; i++)
    {

        if(p_cb->freeq[pool_id].size <= p_cb->freeq[ p_cb->pool_list[i] ].size)
            break;
    }

    /* Insert the new buffer pool ID into the list of pools */
    for(j = p_cb->curr_total_no_of_pools; j > i; j--)
    {
        p_cb->pool_list[j] = p_cb->pool_list[j-1];
    }

    p_cb->pool_list[i] = pool_id;

    return;
}

/*******************************************************************************
**
** Function         gki_remove_from_pool_list
**
** Description      Removes pool from the pool list. Called when a pool is deleted
**
** Returns          void
**
*******************************************************************************/
static void gki_remove_from_pool_list(UINT8 pool_id)
{
    tGKI_COM_CB *p_cb = &gki_cb.com;
    UINT8 i;

    for(i=0; i < p_cb->curr_total_no_of_pools; i++)
    {
        if(pool_id == p_cb->pool_list[i])
            break;
    }

    while (i < (p_cb->curr_total_no_of_pools - 1))
    {
        p_cb->pool_list[i] = p_cb->pool_list[i+1];
        i++;
    }

    return;
}

/*******************************************************************************
* Name: 
* Descriptions:
* Parameter:	
* Return:	
* *****************************************************************************/
static void gki_init_free_queue(UINT8 _ucId, UINT16 _usSize, UINT16 _usTotal, void *p_mem)
{
	UINT16        i;
    UINT16        act_size;
    BUFFER_HDR_T    *hdr;
    BUFFER_HDR_T    *hdr1 = NULL;
    uint32_t        *magic;
    uint32_t        tempsize = _usSize;
    tGKI_COM_CB     *p_cb = &gki_cb.com;

    tempsize = (uint32_t)ALIGN_POOL(_usSize);
    act_size = (UINT16)(tempsize + BUFFER_PADDING_SIZE);

    if(p_mem)
    {
        p_cb->pool_start[_ucId] = (UINT8 *)p_mem;
        p_cb->pool_end[_ucId]   = (UINT8 *)p_mem + (act_size * _usTotal);
    }


    p_cb->pool_size[_ucId]  = act_size;

    p_cb->freeq[_ucId].size      = (UINT16) tempsize;
    p_cb->freeq[_ucId].total     = _usTotal;
    p_cb->freeq[_ucId].cur_cnt   = 0;
    p_cb->freeq[_ucId].max_cnt   = 0;

    /* Initialize  index table */
// btla-specific ++
    if(p_mem)
    {
        hdr = (BUFFER_HDR_T *)p_mem;
        p_cb->freeq[_ucId].p_first = hdr;
        for (i = 0; i < _usTotal; i++)
        {
            hdr->task_id = GKI_INVALID_TASK;
            hdr->q_id    = _ucId;
            hdr->status  = BUF_STATUS_FREE;
            magic        = (uint32_t *)((UINT8 *)hdr + BUFFER_HDR_SIZE + tempsize);
            *magic       = MAGIC_NO;
            hdr1         = hdr;
            hdr          = (BUFFER_HDR_T *)((UINT8 *)hdr + act_size);
            hdr1->p_next = hdr;
        }
        hdr1->p_next = NULL;
        p_cb->freeq[_ucId].p_last = hdr1;
    }
// btla-specific --
    return;
}
/*******************************************************************************
* Name: 
* Descriptions:
* Parameter:	
* Return:	
* *****************************************************************************/
#ifdef GKI_USE_DEFERED_ALLOC_BUF_POOLS
static UINT8 gki_alloc_free_queue(UINT8 _ucId)
{
    FREE_QUEUE_T  *Q;
    tGKI_COM_CB *p_cb = &gki_cb.com;
    GKI_LOGD("gki_alloc_free_queue in, _ucId:%d ",(INT32)_ucId );

    Q = &p_cb->freeq[p_cb->pool_list[_ucId]];

    if(Q->p_first == 0)
    {
        void* p_mem = GKI_os_malloc((Q->size + BUFFER_PADDING_SIZE) * Q->total,"..");
        if(p_mem)
        {
            //re-initialize the queue with allocated memory
            GKI_LOGD("%s gki_alloc_free_queue calling  gki_init_free_queue, id:%d  size:%d, totol:%d", __func__, _ucId, Q->size, Q->total);
            gki_init_free_queue(_ucId, Q->size, Q->total, p_mem);
            GKI_LOGD("%s gki_alloc_free_queue ret OK, id:%d  size:%d, totol:%d", __func__, _ucId, Q->size, Q->total);
            return TRUE;
        }
        GKI_LOGE("%s gki_alloc_free_queue: Not enough memory",__func__);
    }
    GKI_LOGE("%s gki_alloc_free_queue out failed, id:%d",__func__, _ucId);
    return FALSE;
}

/* GKI_shutdown 时调用 */
void gki_dealloc_free_queue(void)
{
    UINT8   i;
    tGKI_COM_CB *p_cb = &gki_cb.com;

    for (i=0; i < p_cb->curr_total_no_of_pools; i++)
    {
        if ( 0 < p_cb->freeq[i].max_cnt )
        {
            GKI_os_free(p_cb->pool_start[i],"..");

            p_cb->freeq[i].cur_cnt   = 0;
            p_cb->freeq[i].max_cnt   = 0;
            p_cb->freeq[i].p_first   = NULL;
            p_cb->freeq[i].p_last    = NULL;

            p_cb->pool_start[i] = NULL;
            p_cb->pool_end[i]   = NULL;
            p_cb->pool_size[i]  = 0;
        }
    }
}
#endif
/*****************************************************************************/
void gki_buffer_init(void)
{
    UINT8 i,tt,mb;
    tGKI_COM_CB *p_cb = &gki_cb.com;
    
    /* Initialize mailboxes */
    for(tt=0;tt<GKI_MAX_TASKS;tt++)
    {
        for(mb = 0; mb < NUM_TASK_MBOX; mb++)
        {
            p_cb->OSTaskQFirst[tt][mb] = NULL;
            p_cb->OSTaskQLast[tt][mb]  = NULL;
        }
    }
    
    for(tt=0; tt < GKI_NUM_TOTAL_BUF_POOLS; tt++)
    {
        p_cb->pool_start[tt]    = NULL;    
        p_cb->pool_end[tt]      = NULL;    
        p_cb->pool_size[tt]     = 0;
        
        p_cb->freeq[tt].p_first  = 0;
        p_cb->freeq[tt].p_last   = 0;
        p_cb->freeq[tt].size     = 0;
        p_cb->freeq[tt].total    = 0;
        p_cb->freeq[tt].cur_cnt  = 0;
        p_cb->freeq[tt].max_cnt  = 0;
    }

    p_cb->pool_access_mask = GKI_DEF_BUFPOOL_PERM_MASK;
	
#if (!defined GKI_USE_DEFERED_ALLOC_BUF_POOLS && (GKI_USE_DYNAMIC_BUFFERS == TRUE))

#if (GKI_NUM_FIXED_BUF_POOLS > 0)
    p_cb->bufpool0 = (UINT8 *)GKI_os_malloc((GKI_BUF0_SIZE + BUFFER_PADDING_SIZE)*GKI_BUF0_MAX);
#endif

#if (GKI_NUM_FIXED_BUF_POOLS > 1)
    p_cb->bufpool1 = (UINT8 *)GKI_os_malloc((GKI_BUF1_SIZE + BUFFER_PADDING_SIZE)*GKI_BUF1_MAX);
#endif

#if (GKI_NUM_FIXED_BUF_POOLS > 2)
    p_cb->bufpool2 = (UINT8 *)GKI_os_malloc((GKI_BUF2_SIZE + BUFFER_PADDING_SIZE)*GKI_BUF2_MAX);
#endif

#if (GKI_NUM_FIXED_BUF_POOLS > 3)
    p_cb->bufpool3 = (UINT8 *)GKI_os_malloc((GKI_BUF3_SIZE + BUFFER_PADDING_SIZE)*GKI_BUF3_MAX);
#endif

#if (GKI_NUM_FIXED_BUF_POOLS > 4)
    p_cb->bufpool4 = (UINT8 *)GKI_os_malloc ((GKI_BUF4_SIZE + BUFFER_PADDING_SIZE) * GKI_BUF4_MAX);
#endif

#if (GKI_NUM_FIXED_BUF_POOLS > 5)
    p_cb->bufpool5 = (UINT8 *)GKI_os_malloc ((GKI_BUF5_SIZE + BUFFER_PADDING_SIZE) * GKI_BUF5_MAX);
#endif

#if (GKI_NUM_FIXED_BUF_POOLS > 6)
    p_cb->bufpool6 = (UINT8 *)GKI_os_malloc ((GKI_BUF6_SIZE + BUFFER_PADDING_SIZE) * GKI_BUF6_MAX);
#endif

#if (GKI_NUM_FIXED_BUF_POOLS > 7)
    p_cb->bufpool7 = (UINT8 *)GKI_os_malloc ((GKI_BUF7_SIZE + BUFFER_PADDING_SIZE) * GKI_BUF7_MAX);
#endif

#if (GKI_NUM_FIXED_BUF_POOLS > 8)
    p_cb->bufpool8 = (UINT8 *)GKI_os_malloc ((GKI_BUF8_SIZE + BUFFER_PADDING_SIZE) * GKI_BUF8_MAX);
#endif

#if (GKI_NUM_FIXED_BUF_POOLS > 9)
    p_cb->bufpool9 = (UINT8 *)GKI_os_malloc ((GKI_BUF9_SIZE + BUFFER_PADDING_SIZE) * GKI_BUF9_MAX);
#endif

#if (GKI_NUM_FIXED_BUF_POOLS > 10)
    p_cb->bufpool10 = (UINT8 *)GKI_os_malloc ((GKI_BUF10_SIZE + BUFFER_PADDING_SIZE) * GKI_BUF10_MAX);
#endif

#if (GKI_NUM_FIXED_BUF_POOLS > 11)
    p_cb->bufpool11 = (UINT8 *)GKI_os_malloc ((GKI_BUF11_SIZE + BUFFER_PADDING_SIZE) * GKI_BUF11_MAX);
#endif

#if (GKI_NUM_FIXED_BUF_POOLS > 12)
    p_cb->bufpool12 = (UINT8 *)GKI_os_malloc ((GKI_BUF12_SIZE + BUFFER_PADDING_SIZE) * GKI_BUF12_MAX);
#endif

#if (GKI_NUM_FIXED_BUF_POOLS > 13)
    p_cb->bufpool13 = (UINT8 *)GKI_os_malloc ((GKI_BUF13_SIZE + BUFFER_PADDING_SIZE) * GKI_BUF13_MAX);
#endif

#if (GKI_NUM_FIXED_BUF_POOLS > 14)
    p_cb->bufpool14 = (UINT8 *)GKI_os_malloc ((GKI_BUF14_SIZE + BUFFER_PADDING_SIZE) * GKI_BUF14_MAX);
#endif

#if (GKI_NUM_FIXED_BUF_POOLS > 15)
    p_cb->bufpool15 = (UINT8 *)GKI_os_malloc ((GKI_BUF15_SIZE + BUFFER_PADDING_SIZE) * GKI_BUF15_MAX);
#endif

#endif

#if (GKI_NUM_FIXED_BUF_POOLS > 0)
    gki_init_free_queue(0, GKI_BUF0_SIZE, GKI_BUF0_MAX, p_cb->bufpool0);
#endif    

#if (GKI_NUM_FIXED_BUF_POOLS > 1)
    gki_init_free_queue(1, GKI_BUF1_SIZE, GKI_BUF1_MAX, p_cb->bufpool1);
#endif    

#if (GKI_NUM_FIXED_BUF_POOLS > 2)
    gki_init_free_queue(2, GKI_BUF2_SIZE, GKI_BUF2_MAX, p_cb->bufpool2);
#endif    

#if (GKI_NUM_FIXED_BUF_POOLS > 3)
    gki_init_free_queue(3, GKI_BUF3_SIZE, GKI_BUF3_MAX, p_cb->bufpool3);
#endif    

#if (GKI_NUM_FIXED_BUF_POOLS > 4)
    gki_init_free_queue(4, GKI_BUF4_SIZE, GKI_BUF4_MAX, p_cb->bufpool4);
#endif

#if (GKI_NUM_FIXED_BUF_POOLS > 5)
    gki_init_free_queue(5, GKI_BUF5_SIZE, GKI_BUF5_MAX, p_cb->bufpool5);
#endif

#if (GKI_NUM_FIXED_BUF_POOLS > 6)
    gki_init_free_queue(6, GKI_BUF6_SIZE, GKI_BUF6_MAX, p_cb->bufpool6);
#endif

#if (GKI_NUM_FIXED_BUF_POOLS > 7)
    gki_init_free_queue(7, GKI_BUF7_SIZE, GKI_BUF7_MAX, p_cb->bufpool7);
#endif

#if (GKI_NUM_FIXED_BUF_POOLS > 8)
    gki_init_free_queue(8, GKI_BUF8_SIZE, GKI_BUF8_MAX, p_cb->bufpool8);
#endif

#if (GKI_NUM_FIXED_BUF_POOLS > 9)
    gki_init_free_queue(9, GKI_BUF9_SIZE, GKI_BUF9_MAX, p_cb->bufpool9);
#endif

#if (GKI_NUM_FIXED_BUF_POOLS > 10)
    gki_init_free_queue(10, GKI_BUF10_SIZE, GKI_BUF10_MAX, p_cb->bufpool10);
#endif

#if (GKI_NUM_FIXED_BUF_POOLS > 11)
    gki_init_free_queue(11, GKI_BUF11_SIZE, GKI_BUF11_MAX, p_cb->bufpool11);
#endif

#if (GKI_NUM_FIXED_BUF_POOLS > 12)
    gki_init_free_queue(12, GKI_BUF12_SIZE, GKI_BUF12_MAX, p_cb->bufpool12);
#endif

#if (GKI_NUM_FIXED_BUF_POOLS > 13)
    gki_init_free_queue(13, GKI_BUF13_SIZE, GKI_BUF13_MAX, p_cb->bufpool13);
#endif

#if (GKI_NUM_FIXED_BUF_POOLS > 14)
    gki_init_free_queue(14, GKI_BUF14_SIZE, GKI_BUF14_MAX, p_cb->bufpool14);
#endif

#if (GKI_NUM_FIXED_BUF_POOLS > 15)
    gki_init_free_queue(15, GKI_BUF15_SIZE, GKI_BUF15_MAX, p_cb->bufpool15);
#endif

    /* add pools to pool_list which is arranged */
    for(i=0; i < GKI_NUM_FIXED_BUF_POOLS; i++)
    {
        p_cb->pool_list[i] = i;
    }

    p_cb->curr_total_no_of_pools = GKI_NUM_FIXED_BUF_POOLS;
}


/*******************************************************************************
**
** Function         GKI_read_mbox
**
** Description      Called by applications to read a buffer from one of
**                  the task mailboxes.  A task can only read its own mailbox.
**
** Parameters:      mbox  - (input) mailbox ID to read (0, 1, 2, or 3)
**
** Returns          NULL if the mailbox was empty, else the address of a buffer
**
*******************************************************************************/
void *GKI_read_mbox (UINT8 mbox)
{
    UINT8           task_id = GKI_get_taskid();
    void            *p_buf = NULL;
    BUFFER_HDR_T    *p_hdr;

    if ((task_id >= GKI_MAX_TASKS) || (mbox >= NUM_TASK_MBOX))
	{
		
		return (NULL);
	}
        
    GKI_disable();

    if (gki_cb.com.OSTaskQFirst[task_id][mbox])
    {
        p_hdr = gki_cb.com.OSTaskQFirst[task_id][mbox];
        gki_cb.com.OSTaskQFirst[task_id][mbox] = p_hdr->p_next;
        	
        p_hdr->p_next = NULL;
        p_hdr->status = BUF_STATUS_UNLINKED;
        
	  p_buf = (UINT8 *)p_hdr + BUFFER_HDR_SIZE;
    }

    GKI_enable();

    return (p_buf);
}

/*******************************************************************************
**
** Function         GKI_get_buf_size
**
** Description      查询当前缓冲区的大小
**
** Parameters       p_buf - (input) address of the beginning of a buffer.
**
** Returns          the size of the buffer
**
*******************************************************************************/
UINT16 GKI_get_buf_size (void *p_buf)
{
    BUFFER_HDR_T    *p_hdr;

    p_hdr = (BUFFER_HDR_T *)((UINT8 *) p_buf - BUFFER_HDR_SIZE);
#if 0
    if ((uint32_t)p_hdr & 1)
        return (0);
#endif
    if (p_hdr->q_id < GKI_NUM_TOTAL_BUF_POOLS)
    {
        return (gki_cb.com.freeq[p_hdr->q_id].size);
    }

    return (0);
}

/*******************************************************************************
**
** Function         gki_chk_buf_damage
**
** Description      Called internally by OSS to check for buffer corruption.
**
** Returns          TRUE if there is a problem, else FALSE
**
*******************************************************************************/
BOOLEAN gki_chk_buf_damage(void *p_buf)
{
#if (GKI_ENABLE_BUF_CORRUPTION_CHECK == TRUE)

    UINT32 *magic;
    magic  = (UINT32 *)((UINT8 *) p_buf + GKI_get_buf_size(p_buf));

    if ((UINT32)magic & 1)  //magic 是 地址?
        return (TRUE);

    if (*magic == MAGIC_NO)
        return (FALSE);

    return (TRUE);

#else
    return (FALSE);
#endif
}
/*******************************************************************************
**
** Function         GKI_send_msg
**
** Description      Called by applications to send a buffer to a task
**
** Returns          Nothing
**
*******************************************************************************/
void GKI_send_msg (UINT8 task_id, UINT8 mbox, void *msg)
{
    BUFFER_HDR_T *p_hdr;
	BUFFER_HDR_T *p_tmp;
    tGKI_COM_CB *p_cb = &gki_cb.com;
    /* If task non-existant or not started, drop buffer */
    GKI_LOGD("%s task_id = 0x%.4X, mbox = 0x%.4X, state = %d, opcode = 0x%.4X",__func__,task_id, mbox, p_cb->OSRdyTbl[task_id] , *((INT32*)msg));
    if ((task_id >= GKI_MAX_TASKS) || (mbox >= NUM_TASK_MBOX) || (p_cb->OSRdyTbl[task_id] == TASK_DEAD))
    {
        GKI_LOGE("%s sending to unknown dest",__func__);
        /***********    需要删除，为编译通过先注掉   *****************************/
        GKI_freebuf(msg);
        return;
    }
		
    p_hdr = (BUFFER_HDR_T *) ((UINT8 *) msg - BUFFER_HDR_SIZE);

    if (p_hdr->status != BUF_STATUS_UNLINKED)
    {
	    GKI_LOGE("%s send - buffer linked",__func__);
        return;
    }
    GKI_disable();
		/* 扩展：队列保存 */
    if (p_cb->OSTaskQFirst[task_id][mbox])
	{
        p_tmp = p_cb->OSTaskQFirst[task_id][mbox];
        /* 队列里面有数据  追加*/
        while(p_tmp->p_next)
        {
            p_tmp = p_tmp->p_next;				
        }
        
	    p_tmp->p_next = p_hdr;
        
	}
    else
	{
        p_cb->OSTaskQFirst[task_id][mbox] = p_hdr;
	}

    p_hdr->p_next = NULL;
    p_hdr->status = BUF_STATUS_QUEUED;
    p_hdr->task_id = task_id;		
		
    GKI_enable();

    GKI_send_event(task_id, (UINT16)EVENT_MASK(mbox));

    return;
}

/*******************************************************************************
**
** Function         GKI_init_q
**
** Description      Called by an application to initialize a buffer queue.
**
** Returns          void
**
*******************************************************************************/
void GKI_init_q (BUFFER_Q *p_q)
{
    p_q->p_first = p_q->p_last = NULL;
    p_q->count = 0;

    return;
}



/*******************************************************************************
**
** Function         GKI_enqueue
**
** Description      Enqueue a buffer at the tail of the queue
**
** Parameters:      p_q  -  (input) pointer to a queue.
**                  p_buf - (input) address of the buffer to enqueue
**
** Returns          void
**
*******************************************************************************/
void GKI_enqueue (BUFFER_Q *p_q, void *p_buf)
{
    BUFFER_HDR_T    *p_hdr;

#if (GKI_ENABLE_BUF_CORRUPTION_CHECK == TRUE)
    if (gki_chk_buf_damage(p_buf))
    {
        //GKI_LOGE("%s Enqueue - Buffer corrupted",__func__);
        return;
    }
#endif

    p_hdr = (BUFFER_HDR_T *) ((UINT8 *) p_buf - BUFFER_HDR_SIZE);

    if (p_hdr->status != BUF_STATUS_UNLINKED)
    {
        //GKI_exception(GKI_ERROR_ENQUEUE_BUF_LINKED, "Eneueue - buf already linked");
        return;
    }

    GKI_disable();

    /* Since the queue is exposed (C vs C++), keep the pointers in exposed format */
    if (p_q->p_last)
    {
        BUFFER_HDR_T *p_last_hdr = (BUFFER_HDR_T *)((UINT8 *)p_q->p_last - BUFFER_HDR_SIZE);
        p_last_hdr->p_next = p_hdr;
    }
    else
    {
    	/* ????????? */
        p_q->p_first = p_buf;
    }
    p_q->p_last = p_buf;
    p_q->count++;

    p_hdr->p_next = NULL;
    p_hdr->status = BUF_STATUS_QUEUED;

    GKI_enable();

    return;
}


/*******************************************************************************
**
** Function         GKI_enqueue_head
**
** Description      Enqueue a buffer at the head of the queue
**
** Parameters:      p_q  -  (input) pointer to a queue.
**                  p_buf - (input) address of the buffer to enqueue
**
** Returns          void
**
*******************************************************************************/
void GKI_enqueue_head (BUFFER_Q *p_q, void *p_buf)
{
    BUFFER_HDR_T    *p_hdr;

#if (GKI_ENABLE_BUF_CORRUPTION_CHECK == TRUE)
    if (gki_chk_buf_damage(p_buf))
    {
        GKI_LOGD("%s Enqueue - Buffer corrupted",__func__);
        return;
    }
#endif

    p_hdr = (BUFFER_HDR_T *) ((UINT8 *) p_buf - BUFFER_HDR_SIZE);

    if (p_hdr->status != BUF_STATUS_UNLINKED)
    {
        //GKI_exception(GKI_ERROR_ENQUEUE_BUF_LINKED, "Eneueue head - buf already linked");
        return;
    }

    GKI_disable();

    if (p_q->p_first)
    {
        p_hdr->p_next = (BUFFER_HDR_T *)((UINT8 *)p_q->p_first - BUFFER_HDR_SIZE);
        p_q->p_first = p_buf;
    }
    else
    {
        p_q->p_first = p_buf;
        p_q->p_last  = p_buf;
        p_hdr->p_next = NULL;
    }
    p_q->count++;

    p_hdr->status = BUF_STATUS_QUEUED;

    GKI_enable();

    return;
}


/*******************************************************************************
**
** Function         GKI_dequeue
**
** Description      Dequeues a buffer from the head of a queue
**
** Parameters:      p_q  - (input) pointer to a queue.
**
** Returns          NULL if queue is empty, else buffer
**
*******************************************************************************/
void *GKI_dequeue (BUFFER_Q *p_q)
{
    BUFFER_HDR_T    *p_hdr;

    GKI_disable();

    if (!p_q || !p_q->count)
    {
        GKI_enable();
        return (NULL);
    }

    p_hdr = (BUFFER_HDR_T *)((UINT8 *)p_q->p_first - BUFFER_HDR_SIZE);

    /* Keep buffers such that GKI header is invisible
    */
    if (p_hdr->p_next)
        p_q->p_first = ((UINT8 *)p_hdr->p_next + BUFFER_HDR_SIZE);
    else
    {
        p_q->p_first = NULL;
        p_q->p_last  = NULL;
    }

    p_q->count--;

    p_hdr->p_next = NULL;
    p_hdr->status = BUF_STATUS_UNLINKED;

    GKI_enable();

    return ((UINT8 *)p_hdr + BUFFER_HDR_SIZE);
}


/*******************************************************************************
**
** Function         GKI_remove_from_queue
**
** Description      Dequeue a buffer from the middle of the queue
**
** Parameters:      p_q  - (input) pointer to a queue.
**                  p_buf - (input) address of the buffer to enqueue
**
** Returns          NULL if queue is empty, else buffer
**
*******************************************************************************/
void *GKI_remove_from_queue (BUFFER_Q *p_q, void *p_buf)
{
    BUFFER_HDR_T    *p_prev;
    BUFFER_HDR_T    *p_buf_hdr;

    GKI_disable();

    if (p_buf == p_q->p_first)
    {
        GKI_enable();
        return (GKI_dequeue (p_q));
    }

    p_buf_hdr = (BUFFER_HDR_T *)((UINT8 *)p_buf - BUFFER_HDR_SIZE);
    p_prev    = (BUFFER_HDR_T *)((UINT8 *)p_q->p_first - BUFFER_HDR_SIZE);

    for ( ; p_prev; p_prev = p_prev->p_next)
    {
        /* If the previous points to this one, move the pointers around */
        if (p_prev->p_next == p_buf_hdr)
        {
            p_prev->p_next = p_buf_hdr->p_next;

            /* If we are removing the last guy in the queue, update p_last */
            if (p_buf == p_q->p_last)
                p_q->p_last = p_prev + 1;

            /* One less in the queue */
            p_q->count--;

            /* The buffer is now unlinked */
            p_buf_hdr->p_next = NULL;
            p_buf_hdr->status = BUF_STATUS_UNLINKED;

            GKI_enable();
            return (p_buf);
        }
    }

    GKI_enable();
    return (NULL);
}


/*******************************************************************************
**
** Function         GKI_getfirst
**
** Description      Return a pointer to the first buffer in a queue
**
** Parameters:      p_q  - (input) pointer to a queue.
**
** Returns          NULL if queue is empty, else buffer address
**
*******************************************************************************/
void *GKI_getfirst (BUFFER_Q *p_q)
{
    return (p_q->p_first);
}


/*******************************************************************************
**
** Function         GKI_getlast
**
** Description      Return a pointer to the last buffer in a queue
**
** Parameters:      p_q  - (input) pointer to a queue.
**
** Returns          NULL if queue is empty, else buffer address
**
*******************************************************************************/
void *GKI_getlast (BUFFER_Q *p_q)
{
    return (p_q->p_last);
}

/*******************************************************************************
**
** Function         GKI_getnext
**
** Description      Return a pointer to the next buffer in a queue
**
** Parameters:      p_buf  - (input) pointer to the buffer to find the next one from.
**
** Returns          NULL if no more buffers in the queue, else next buffer address
**
*******************************************************************************/
void *GKI_getnext (void *p_buf)
{
    BUFFER_HDR_T    *p_hdr;

    p_hdr = (BUFFER_HDR_T *) ((UINT8 *) p_buf - BUFFER_HDR_SIZE);

    if (p_hdr->p_next)
        return ((UINT8 *)p_hdr->p_next + BUFFER_HDR_SIZE);
    else
        return (NULL);
}



/*******************************************************************************
**
** Function         GKI_queue_is_empty
**
** Description      Check the status of a queue.
**
** Parameters:      p_q  - (input) pointer to a queue.
**
** Returns          TRUE if queue is empty, else FALSE
**
*******************************************************************************/
UINT8 GKI_queue_is_empty(BUFFER_Q *p_q)
{
    return ((UINT8) (p_q->count == 0));
}

/*******************************************************************************
**
** Function         GKI_find_buf_start
**
** Description      This function is called with an address inside a buffer,
**                  and returns the start address ofthe buffer.
**
**                  The buffer should be one allocated from one of GKI's pools.
**
** Parameters:      p_user_area - (input) address of anywhere in a GKI buffer.
**
** Returns          void * - Address of the beginning of the specified buffer if successful,
**                          otherwise NULL if unsuccessful
**
*******************************************************************************/
void *GKI_find_buf_start (void *p_user_area)
{
    UINT16       xx, size;
    uint32_t       yy;
    tGKI_COM_CB *p_cb = &gki_cb.com;
    UINT8       *p_ua = (UINT8 *)p_user_area;

    for (xx = 0; xx < GKI_NUM_TOTAL_BUF_POOLS; xx++)
    {
        if ((p_ua > p_cb->pool_start[xx]) && (p_ua < p_cb->pool_end[xx]))
        {
            yy = (uint32_t)(p_ua - p_cb->pool_start[xx]);

            size = p_cb->pool_size[xx];

            yy = (yy / size) * size;

            return ((void *) (p_cb->pool_start[xx] + yy + sizeof(BUFFER_HDR_T)) );
        }
    }

    /* If here, invalid address - not in one of our buffers */
    GKI_LOGE("%s bad addr",__func__);

    return (NULL);
}

/*******************************************************************************
**
** Function         GKI_getbuf
**
** Description      Called by an application to get a free buffer which
**                  is of size greater or equal to the requested size.
**
**                  Note: This routine only takes buffers from public pools.
**                        It will not use any buffers from pools
**                        marked GKI_RESTRICTED_POOL.
**
** Parameters       size - (input) number of bytes needed.
**
** Returns          A pointer to the buffer, or NULL if none available
**
*******************************************************************************/
void *GKI_getbuf_raw (UINT16 size)
{
    UINT8         i;
    FREE_QUEUE_T  *Q;
    BUFFER_HDR_T  *p_hdr;
    tGKI_COM_CB *p_cb = &gki_cb.com;

    if (size == 0)
    {
        //GKI_exception (GKI_ERROR_BUF_SIZE_ZERO, "getbuf: Size is zero");
		GKI_LOGE("%s getbuf: Size is zero",__func__);
        return (NULL);
    }
    //GKI_TRACE("GKI_getbuf_raw : size = %d",size);
    /* Find the first buffer pool that is public that can hold the desired size */
    /* 找到第一个符合大小的缓冲池 */
    for (i=0; i < p_cb->curr_total_no_of_pools; i++)
    {
        if ( size <= p_cb->freeq[p_cb->pool_list[i]].size )
        {
            /* Only look at PUBLIC buffer pools (bypass RESTRICTED pools) */
            if (((UINT16)1 << p_cb->pool_list[i]) & p_cb->pool_access_mask)
                continue;
            else
                break;
        }
    }

    if(i == p_cb->curr_total_no_of_pools)
    {
        //GKI_exception (GKI_ERROR_BUF_SIZE_TOOBIG, "getbuf: Size is too big");
		GKI_LOGE("%s getbuf: Size is too big",__func__);
        return (NULL);
    }

    /* Make sure the buffers aren't disturbed til finished with allocation */
    GKI_disable();

    /* search the public buffer pools that are big enough to hold the size
     * until a free buffer is found */
    for ( ; i < p_cb->curr_total_no_of_pools; i++)
    {
        /* Only look at PUBLIC buffer pools (bypass RESTRICTED pools) */
        if (((UINT16)1 << p_cb->pool_list[i]) & p_cb->pool_access_mask)
            continue;
	/* 第i个缓冲池的空闲链表地址 */
        Q = &p_cb->freeq[p_cb->pool_list[i]];
	/* 空闲还有剩余 */
        if(Q->cur_cnt < Q->total)
        {
// btla-specific ++
        #ifdef GKI_USE_DEFERED_ALLOC_BUF_POOLS
            if(Q->p_first == 0 && gki_alloc_free_queue(i) != TRUE)
                return NULL;
        #endif
// btla-specific --
            p_hdr = Q->p_first;
            Q->p_first = p_hdr->p_next;

            if (!Q->p_first)
                Q->p_last = NULL;

            if(++Q->cur_cnt > Q->max_cnt)
                Q->max_cnt = Q->cur_cnt;

            GKI_enable();

            p_hdr->task_id = GKI_get_taskid();

            p_hdr->status  = BUF_STATUS_UNLINKED;
            p_hdr->p_next  = NULL;
            p_hdr->Type    = 0;

            return ((void *) ((UINT8 *)p_hdr + BUFFER_HDR_SIZE));
        }
	else
	{
		GKI_LOGE("%s ERR:No free position",__func__);
	}
    }

    GKI_enable();

    //GKI_exception (GKI_ERROR_OUT_OF_BUFFERS, "getbuf: out of buffers");
	GKI_LOGE("%s getbuf: out of buffers",__func__);
#if 0
#ifdef BLUETEC_PMEM_DEBUG
    /* Dump all memory */
    GKI_buffer_print_debug();
    assert(0);
#endif
#endif
    return (NULL);
}

/*******************************************************************************
**
** Function         GKI_getpoolbuf
**
** Description      Called by an application to get a free buffer from
**                  a specific buffer pool.
**
**                  Note: If there are no more buffers available from the pool,
**                        the public buffers are searched for an available buffer.
**
** Parameters       pool_id - (input) pool ID to get a buffer out of.
**
** Returns          A pointer to the buffer, or NULL if none available
**
*******************************************************************************/
void *GKI_getpoolbuf_raw (UINT8 pool_id)
{
    FREE_QUEUE_T  *Q;
    BUFFER_HDR_T  *p_hdr;
    tGKI_COM_CB *p_cb = &gki_cb.com;

    if (pool_id >= GKI_NUM_TOTAL_BUF_POOLS)
    {
        //GKI_exception(GKI_ERROR_GETPOOLBUF_BAD_QID, "getpoolbuf bad pool");
        return (NULL);
    }

    /* Make sure the buffers aren't disturbed til finished with allocation */
    GKI_disable();

    Q = &p_cb->freeq[pool_id];
    if(Q->cur_cnt < Q->total)
    {
// btla-specific ++
#ifdef GKI_USE_DEFERED_ALLOC_BUF_POOLS
        if(Q->p_first == 0 && gki_alloc_free_queue(pool_id) != TRUE)
            return NULL;
#endif
// btla-specific --
        p_hdr = Q->p_first;
        Q->p_first = p_hdr->p_next;

        if (!Q->p_first)
            Q->p_last = NULL;

        if(++Q->cur_cnt > Q->max_cnt)
            Q->max_cnt = Q->cur_cnt;

        GKI_enable();


        p_hdr->task_id = GKI_get_taskid();

        p_hdr->status  = BUF_STATUS_UNLINKED;
        p_hdr->p_next  = NULL;
        p_hdr->Type    = 0;

        return ((void *) ((UINT8 *)p_hdr + BUFFER_HDR_SIZE));
    }

    /* If here, no buffers in the specified pool */
    GKI_enable();

    /* try for free buffers in public pools */
    return (GKI_getbuf_raw(p_cb->freeq[pool_id].size));

}

/*******************************************************************************
**
** Function         GKI_freebuf
**
** Description      Called by an application to return a buffer to the free pool.
**
** Parameters       p_buf - (input) address of the beginning of a buffer.
**
** Returns          void
**
*******************************************************************************/
void GKI_freebuf_raw (void *p_buf)
{
    FREE_QUEUE_T    *Q;
    BUFFER_HDR_T    *p_hdr;

#if (GKI_ENABLE_BUF_CORRUPTION_CHECK == TRUE)
    if (!p_buf || gki_chk_buf_damage(p_buf))
    {
        //GKI_exception(GKI_ERROR_BUF_CORRUPTED, "Free - Buf Corrupted");
		GKI_LOGE("%s Free - Buf Corrupted",__func__);
        return;
    }
#endif

    p_hdr = (BUFFER_HDR_T *) ((UINT8 *)p_buf - BUFFER_HDR_SIZE);

    if (p_hdr->status != BUF_STATUS_UNLINKED)
    {
        //GKI_exception(GKI_ERROR_FREEBUF_BUF_LINKED, "Freeing Linked Buf");
		GKI_LOGE("%s Freeing Linked Buf",__func__);
        return;
    }

    if (p_hdr->q_id >= GKI_NUM_TOTAL_BUF_POOLS)
    {
#include <time.h>
        //GKI_exception(GKI_ERROR_FREEBUF_BAD_QID, "Bad Buf QId");
		GKI_LOGE("%s Bad Buf QId",__func__);
        return;
    }

    GKI_disable();

    /*
    ** Release the buffer
    */
    Q  = &gki_cb.com.freeq[p_hdr->q_id];
    if (Q->p_last)
        Q->p_last->p_next = p_hdr;
    else
        Q->p_first = p_hdr;

    Q->p_last      = p_hdr;
    p_hdr->p_next  = NULL;
    p_hdr->status  = BUF_STATUS_FREE;
    p_hdr->task_id = GKI_INVALID_TASK;
    if (Q->cur_cnt > 0)
        Q->cur_cnt--;

    GKI_enable();

    return;
}
