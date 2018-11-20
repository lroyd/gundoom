#ifndef __OS_DECLARE_H__
#define __OS_DECLARE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "common.h"


typedef void (*taskInit)(void** data);
typedef void (*taskHandle)(void** data, EventId event, void* msg);
typedef void (*taskDeinit)(void** data);

typedef struct
{
    TaskId      id;
    taskInit    init;
    taskHandle  handle;
    taskDeinit  deinit;

    void*       data;
}taskEntry;



#define SCP_BEGIN_TASK_DEFINE \
taskEntry LocalTasks[] = {

#define SCP_IMPLEMENT_TASK(ID,INIT,HANDLE,DEINIT) \
{ID, INIT, HANDLE, DEINIT, NULL},

#define  SCP_END_TASK_DEFINE \
};

#define TASK_ID_ADD(Task0ID, arg...) \
enum TASKID \
{ \
    Task0ID = 0, \
    ##arg , \
    INVALID_TASK_ID \
};

TASK_ID_ADD(STACK_TASK, PRO_SOCKET_TASK, APP_GEN_TASK);




#ifdef __cplusplus
}
#endif
#endif
