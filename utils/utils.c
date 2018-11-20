/*******************************************************************************
	> File Name: 
	> Author: lroyd
	> Mail: htzhangxmu@163.com
	> Created Time: 
 *******************************************************************************/
#include <errno.h>
#include <pthread.h>

#include "utils.h"

#define UTILS_LOCKS_CNT_MAX	(1)		//锁最大个数




static pthread_mutex_t utils_mutex[UTILS_LOCKS_CNT_MAX];


void utils_init(void)
{
	int i;
    for(i=0; i<UTILS_LOCKS_CNT_MAX; i++)
	{
		pthread_mutex_init(&utils_mutex[i], NULL);
	}
}

void utils_cleanup(int lockid)
{
	//将队列下的全部清除
	
	
}


void utils_queue_init (BUFFER_Q *p_q)
{
    p_q->p_first = p_q->p_last = NULL;
    p_q->count = 0;
}


unsigned short utils_get_count (BUFFER_Q *p_q)
{
    return p_q->count;
}


void utils_enqueue (int lockid, BUFFER_Q *p_q, void *p_buf)
{
    HC_BUFFER_HDR_T    *p_hdr;

    p_hdr = (HC_BUFFER_HDR_T *) ((unsigned char *) p_buf - HC_BUFFER_HDR_SIZE);

	pthread_mutex_lock(&utils_mutex[lockid]);

    if (p_q->p_last)
    {
        HC_BUFFER_HDR_T *p_last_hdr = (HC_BUFFER_HDR_T *)((unsigned char *)p_q->p_last - HC_BUFFER_HDR_SIZE);
        p_last_hdr->p_next = p_hdr;
    }
    else
    {
        p_q->p_first = p_buf;
    }
    p_q->p_last = p_buf;
    p_q->count++;

    p_hdr->p_next = NULL;	
	
	pthread_mutex_unlock(&utils_mutex[lockid]);
}

void *utils_dequeue (int lockid, BUFFER_Q *p_q)
{
    pthread_mutex_lock(&utils_mutex[lockid]);
    void* p_buf =  utils_dequeue_unlocked(p_q);
    pthread_mutex_unlock(&utils_mutex[lockid]);
    return p_buf;
}

void *utils_dequeue_unlocked (BUFFER_Q *p_q)
{
    HC_BUFFER_HDR_T    *p_hdr = NULL;


    if (!p_q || !p_q->count)
    {
        return (NULL);
    }

    p_hdr=(HC_BUFFER_HDR_T *)((unsigned char *)p_q->p_first - HC_BUFFER_HDR_SIZE);

    if (p_hdr->p_next)
        p_q->p_first = ((unsigned char *)p_hdr->p_next + HC_BUFFER_HDR_SIZE);
    else
    {
        p_q->p_first = NULL;
        p_q->p_last  = NULL;
    }

    p_q->count--;

    p_hdr->p_next = NULL;
    return ((unsigned char *)p_hdr + HC_BUFFER_HDR_SIZE);
}

void *utils_getnext (void *p_buf)
{
    HC_BUFFER_HDR_T    *p_hdr;

    p_hdr = (HC_BUFFER_HDR_T *) ((unsigned char *) p_buf - HC_BUFFER_HDR_SIZE);

    if (p_hdr->p_next)
        return ((unsigned char *)p_hdr->p_next + HC_BUFFER_HDR_SIZE);
    else
        return (NULL);
}

void *utils_remove_from_queue (int lockid, BUFFER_Q *p_q, void *p_buf)
{
    pthread_mutex_lock(&utils_mutex[lockid]);
    p_buf = utils_remove_from_queue_unlocked(p_q, p_buf);
    pthread_mutex_unlock(&utils_mutex[lockid]);
    return p_buf;
}

void *utils_remove_from_queue_unlocked (BUFFER_Q *p_q, void *p_buf)
{
    HC_BUFFER_HDR_T    *p_prev;
    HC_BUFFER_HDR_T    *p_buf_hdr;


    if (p_buf == p_q->p_first)
    {
        return (utils_dequeue_unlocked (p_q));
    }

    p_buf_hdr = (HC_BUFFER_HDR_T *)((unsigned char *)p_buf - HC_BUFFER_HDR_SIZE);
    p_prev=(HC_BUFFER_HDR_T *)((unsigned char *)p_q->p_first-HC_BUFFER_HDR_SIZE);

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

            return (p_buf);
        }
    }
    return (NULL);
}

void utils_delay (unsigned int timeout)
{
    struct timespec delay;
    int err;

    delay.tv_sec = timeout / 1000;
    delay.tv_nsec = 1000 * 1000 * (timeout%1000);

    /* [u]sleep can't be used because it uses SIGALRM */
    do {
        err = nanosleep(&delay, &delay);
    } while (err < 0 && errno ==EINTR);
}

void utils_lock (int lockid)
{
    pthread_mutex_lock(&utils_mutex[lockid]);
}

void utils_unlock (int lockid)
{
    pthread_mutex_unlock(&utils_mutex[lockid]);
}

