/*******************************************************************************
	> File Name: 
	> Author: lroyd
	> Mail: htzhangxmu@163.com
	> Created Time: 
 *******************************************************************************/
#include <stdio.h>
#include <string.h>
#include <fcntl.h>


#include "hw_type.h"
#include "hci_lib.h"
#include "abslay.h"
#include "utils.h"
#include "pro_code_if.h"
#include "bta_hal.h"

extern T_HciCallback *g_tHciCallbacks;	//g_tHciCallbacks	:bta_hal.c中定义
//p_hci_if	：hci_droid 层传进来的，传输协议层回调
extern BUFFER_Q rx_q;

//提供给absl层使用的
static void* alloc(int _iSize)
{
	//  上层分配
	void *pHdr = NULL;
    if (g_tHciCallbacks)
        pHdr = g_tHciCallbacks->alloc(_iSize);
    return (pHdr);
}
 
static void dealloc(void *_pData)
{
    if (g_tHciCallbacks)
        g_tHciCallbacks->dealloc((TRANSAC) _pData, NULL);
}

static int xmit_cb(int _iOpcode, void *_pData, tINT_CMD_CBACK _pCallbck)
{

    return 0;
}

//fd：当前使用
static int rcv_cb(int _iFd, int _iServerFd, void *_pData, int _iLen)
{
	//组合模式底层数据直接入队列，不用拷贝数据，发送bthc_signal_event(HC_EVENT_RX); 让HCI调用协议层解析数据
	int iProfile = 0, iProtocol;
	AbslayGetProfile(_iFd, &iProfile, &iProtocol);
	if (!iProfile)
	{
		printf("rerror profile！！！！\r\n");
		return 0;		
	}	
	
	HCI_HDR *pHdr = NULL;
    if (g_tHciCallbacks)
	{
		//1.malloc（需要队 HDR）
		pHdr = (HCI_HDR *)g_tHciCallbacks->alloc(sizeof(HCI_HDR) + _iLen);	//注意不要超过最大buffer
		if (!pHdr)
		{
			printf("recv data not enough\r\n");
			return 0;
		}
	}

	pHdr->event				= PRO_IND_MSK|PRO_IND_RCV_DATA;		//task内部判断的子事件
	pHdr->layer_profile		= iProfile;
	pHdr->layer_protocol	= iProtocol;
	pHdr->attache			= _iServerFd;
	pHdr->local				= _iFd;
	pHdr->len				= _iLen;
	//BtMemCpy
	memcpy((char *)(pHdr + 1), (char *)_pData, _iLen);
	
	//2.入队列
	utils_enqueue(0, &rx_q, pHdr);	//使用0号锁
	printf(".........usr rcv fd = %d, mfd = %d, data = %s, len = %d\r\n", _iFd, _iServerFd, (char *)(pHdr + 1), _iLen);
	
	//3.发送信号
	bthc_signal_event(HC_EVENT_RX);
	
	return 0;
}



//HW_EVT_STATUS： HW_EVT_ADD_SOCK = 2， HW_EVT_DEL_SOCK = 3
static void net_cb(HW_EVT_STATUS _emState, int _iFd, int _iServerFd, TRANS_RESULT _emRet)
{
	if (_emRet == TRANS_RESULT_FAIL)
	{
		//状态更新失败
		//printf("@@@@@@@@@@@@@@@@ net_cb ret = error!!!!\r\n", ret);
		return;
	}
	int iProfile = 0, iProtocol;
	AbslayGetProfile(_iFd, &iProfile, &iProtocol);
	if (!iProfile)
	{
		printf("error profile！！！！\r\n");
		return;		
	}	
	
	HCI_HDR *pHdr = (HCI_HDR *)g_tHciCallbacks->alloc(sizeof(HCI_HDR));
	pHdr->event			= PRO_IND_MSK;		
	pHdr->layer_profile	= iProfile;
	pHdr->layer_protocol	= iProtocol;
	pHdr->attache			= _iServerFd;
	pHdr->local			= _iFd;	
	//和hw_type.h中定义的HW_EVT_STATUS对应
	switch (_emState)
	{
		case HW_EVT_ADD_SOCK:
		{
			pHdr->event |= PRO_IND_FD_ATTCH;		
			break;
		}
		case HW_EVT_DEL_SOCK:
		{
			pHdr->event |= PRO_IND_FD_SEQ;	
			break;
		}		

		case HW_EVT_LINK_UP:
		{
			pHdr->event |= PRO_IND_LINK_UP;	
			break;
		}		
		case HW_EVT_LINK_DOWN:
		{
			pHdr->event |= PRO_IND_LINK_DOWN;	
			break;
		}		
		
		
		default:
			g_tHciCallbacks->dealloc((TRANSAC) pHdr, NULL);
			return;
	}

	utils_enqueue(0, &rx_q, pHdr);
	bthc_signal_event(HC_EVENT_RX);
	
	return;
}

//传给absl使用的
const T_TransportOp vnd_callbacks = {
    sizeof(T_TransportOp),
    NULL,
    alloc,
    dealloc,
    xmit_cb,
	rcv_cb,
    NULL,
	net_cb	
};


