#ifndef __UTILS_Q_H__
#define __UTILS_Q_H__



typedef struct
{
    void			*p_first;
    void			*p_last;
    unsigned short	count;
} BUFFER_Q;


typedef struct _hc_buffer_hdr{
    struct _hc_buffer_hdr *p_next;
    unsigned char reserved1;
    unsigned char reserved2;
    unsigned char reserved3;
    unsigned char reserved4;
}HC_BUFFER_HDR_T;


#define HC_BUFFER_HDR_SIZE  (sizeof(HC_BUFFER_HDR_T))




void utils_init (void);
void utils_cleanup (int);


void utils_queue_init (BUFFER_Q *);

unsigned short utils_get_count (BUFFER_Q *);

void utils_enqueue (int , BUFFER_Q *, void *);

void *utils_dequeue (int , BUFFER_Q *);

void *utils_dequeue_unlocked (BUFFER_Q *);

void *utils_getnext (void *);

void *utils_remove_from_queue (int , BUFFER_Q *, void *);

void *utils_remove_from_queue_unlocked (BUFFER_Q *, void *);

void utils_delay (unsigned int);

void utils_lock (int);

void utils_unlock (int);

#endif 

