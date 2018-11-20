#ifndef __USR_EPOLL_H__
#define __USR_EPOLL_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "common.h"


#define EPOLL_EVENT_NUM_MAX		(16)

GH_DECL(int) uepoll_create(int *);
GH_DECL(int) uepoll_add(int , int );
GH_DECL(int) uepoll_del(int , int );





#ifdef __cplusplus
}
#endif

#endif
