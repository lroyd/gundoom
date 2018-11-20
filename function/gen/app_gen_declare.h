/*************************************************************************
	> File Name: app_gen_task.h
	> Author: lroyd
	> Mail: 
	> Created Time: 2016年01月27日 星期三 14时42分12秒
 ************************************************************************/
#ifndef _APP_GEN_TASK_H
#define _APP_GEN_TASK_H

#ifdef __cplusplus
extern "C" {
#endif


#include "common.h"


void FuncGenInit(void** data);
void FuncGenHandle(void** data, EventId event, void* msg);
void FuncGenDeinit(void** data);

#define APP_GEN_INIT    FuncGenInit
#define APP_GEN_HANDLE  FuncGenHandle
#define APP_GEN_DEINIT  FuncGenDeinit

#ifdef __cplusplus
}
#endif

#endif
