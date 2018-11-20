/*******************************************************************************
	> File Name: 
	> Author: lroyd
	> Mail: htzhangxmu@163.com
	> Created Time: 
 *******************************************************************************/
#ifndef __HCI_DROID_H__
#define __HCI_DROID_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "hci_lib.h"






#define SCHED_NORMAL 0


#ifndef BTHC_LINUX_BASE_POLICY
#define BTHC_LINUX_BASE_POLICY SCHED_NORMAL
#endif




#define HC_BUFFER_HDR_SIZE  (sizeof(HC_BUFFER_HDR_T))



extern T_HciCallback *g_tHciCallbacks;




#ifdef __cplusplus
}
#endif
#endif



