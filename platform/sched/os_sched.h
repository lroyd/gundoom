#ifndef __TASK_H__
#define __TASK_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "common.h"


#define SYSTEM_TASK (0)
//#define UPSTREAM(x)     (x)
//#define LOWSTREAM(x)    ((x)|0x8000)

#define UPPER_OPCODE (0)
#define LOWER_OPCODE (1)


#define TASK_FUNCTION(x)    (x == SYSTEM_TASK)




void SchedInit(void);

void SchedLoop(uint32_t parameter);

void SchedDeinit(void);

void SchedSendMessage(uint16_t taskId, uint16_t opcode, void* msg);

void SchedSendbtaMessage(uint16_t taskId, uint16_t opcode, void *msg);

#ifdef __cplusplus
}
#endif
#endif
