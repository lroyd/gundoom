#ifndef __MESSAGE_H__
#define __MESSAGE_H__

/* ���û�ʹ�õ� */

#ifdef __cplusplus
extern "C" {
#endif

#include "common.h"
#include "gki.h"
#include "btu_declare.h"





//�첽���죬��Ҫmalloc
#define MAKE_MESSAGE(TYPE)     \
    TYPE##_T* msg = NULL;               \
    msg = (TYPE##_T*)((uint8_t *)GKI_getbuf(sizeof(TYPE##_T)))


#define MAKE_EVENT(TYPE) TYPE##_T msg; memset(&msg, 0, sizeof(TYPE##_T))	//��Ҫ�����֣�����malloc
	
	
#define MAKE_MESSAGE_AFTER_POWERON MAKE_MESSAGE	
	
	
#define TSK_INTER_SEND_MSG(m1, m2, m3)	TaskSendMessage(m1, m2, m3)







#ifdef __cplusplus
}
#endif

#endif
