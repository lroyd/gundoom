#ifndef OS_CFG_H
#define OS_CFG_H

#include "os_declare.h"
#include "btu_declare.h"

#include "app_gen_declare.h"
#include "pro_socket_task_declare.h"






//任务的顺序要和TASK_ID_ADD的一致
/**************************************************************************************************/
SCP_BEGIN_TASK_DEFINE

SCP_IMPLEMENT_TASK(STACK_TASK	, STACK_TASK_INIT	, STACK_TASK_HANDLE		, STACK_TASK_DEINIT)

SCP_IMPLEMENT_TASK(PRO_SOCKET_TASK, PRO_SOCKET_INIT	, PRO_SOCKET_HANDLE	, PRO_SOCKET_DEINIT)




SCP_IMPLEMENT_TASK(APP_GEN_TASK	, APP_GEN_INIT		, APP_GEN_HANDLE		, APP_GEN_DEINIT)

SCP_END_TASK_DEFINE	
/**************************************************************************************************/











#endif