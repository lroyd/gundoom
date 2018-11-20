/*************************************************************************
	> File Name: app_gen_ind_handle.c
	> Author: lroyd
	> Mail: 
	> Created Time: 2016年01月27日 星期三 12时38分57秒
 ************************************************************************/
#include "app_gen_private.h"
#include "pro_code_if.h"
#include "pmem.h"
#include "bta_hal.h"
/************************************************************************
 *     
 ***********************************************************************/
static void FuncRecvDataInd(void *p_msg)
{
	HCI_HDR *p_hdr = (HCI_HDR *)p_msg;
	//printf("xxxxxxxxxxxxxxxxxxxx rcv data = %s, len = %d\r\n", (char *)(p_hdr + 1), p_hdr->len);
	AppGenDevRecvDataInd(p_hdr->local, p_hdr->len, (char *)(p_hdr + 1));
}

static void FuncFdAttachInd(void *p_msg)
{
	HCI_HDR *p_hdr = (HCI_HDR *)p_msg;

	AppGenDevFdAttachInd(p_hdr->local, p_hdr->attache);
}

static void FuncFdSeparateInd(void *p_msg)
{
	HCI_HDR *p_hdr = (HCI_HDR *)p_msg;

	AppGenDevFdSeparateInd(p_hdr->local);
}

static void FuncLinkChangeInd(void *p_msg)
{
	int change = 0;
	HCI_HDR *p_hdr = (HCI_HDR *)p_msg;
	if ((p_hdr->event&(~PRO_CHK_MSK)) == PRO_IND_LINK_UP)
	{
		change = 1;
	}

	AppGenLinkChangeInd(change);
}
/************************************************************************
 *      1.直通底层的数据ind	2.pro 层过来的通知事件或数据
 ***********************************************************************/
void FuncGenIndHandle(void** data, EventId event, void* msg)
{
	//printf("ind event 0x%.4x\r\n", event);
    switch(event&(~PRO_CHK_MSK))
    {
		case PRO_IND_RCV_DATA:	//
		{
			FuncRecvDataInd(msg);
			break;
		}
		case PRO_IND_FD_ATTCH:	//
		{
			FuncFdAttachInd(msg);
			break;
		}		
		case PRO_IND_FD_SEQ:	//
		{
			FuncFdSeparateInd(msg);
			break;
		}
		
		case PRO_IND_LINK_UP:
		case PRO_IND_LINK_DOWN:
		{
			FuncLinkChangeInd(msg);
			break;			
		}
		
		
        default:  
			APP_GEN_LOG(LOG_ERROR, LOG_GEN_IND, "Unknown event %.4X", event);
            break;
    }
	
	if(msg)
	{
		BtPmemFree(msg);
	}
}


