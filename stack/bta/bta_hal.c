#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <assert.h>
#include <signal.h>
#include <time.h>


#include "common.h"
#include "hci_droid.h"
#include "hci_lib.h"
#include "bta_hal.h"
#include "message.h"

static T_HciInfrtface *hc_if = NULL;	//hci_droid.c中定义

static const T_HciCallback hc_callbacks;


static void stack_in_hw_init(void)
{
    if ( (hc_if = (T_HciInfrtface *) getHciInterface()) == NULL)	//
    {

    }

}

static void stack_hci_enable(void)
{
    if (hc_if)
    {
        int result = hc_if->init(&hc_callbacks);


	
        //hc_if->preload(0, NULL);		//调用hci_droid.c 中的preload函数
    }
}

static void stack_hci_disable(void)
{

    if (hc_if)
    {
        hc_if->cleanup();
		

    }
}
/******************************************************************************
**
** Function         stack_init
**
** Description      Internal helper function for chip hardware init
**
** Returns          None
**
******************************************************************************/
void stack_init(void)
{

    stack_in_hw_init();
    

    stack_hci_enable();

}

void stack_deinit(void)
{
    stack_hci_disable();
}

int stack_hci_send (int fd, uint8_t *p_msg, int len)
{
	return hc_if->transmit_buf(fd, p_msg, len);
}


void stack_hci_ioctl(int op, void *in_param, void *out_param)
{
	switch(op)
	{
		case HAL_OP_DEV_CREATE:
		{
			HAL_OP_DEV_CREATE_IN_T *in = (HAL_OP_DEV_CREATE_IN_T *)in_param;
			HAL_OP_DEV_CREATE_OUT_T *out = (HAL_OP_DEV_CREATE_OUT_T *)out_param;

			hc_if->create(in->type, in->path, in->port, in->profile, in->protocol, &out->fd);
			break;
		}
		case HAL_OP_DEV_DESTROY:
		{
			int fd = *(int *)in_param;
			hc_if->destroy(fd);
			break;
		}		
		
		
	}

        


}



/*****************************************************************************
**
**   libbt-hci Callback Functions
**
*****************************************************************************/
static void preload_cb(TRANSAC transac, T_HciPreloadResult result)
{
	//由hci_droid.c 中的hc_worker_thread回调
    //启动成功，返回gen层设置stack_state_ready（解锁命令发送）+++
	
	
	
}

//单字节
static char *alloc(int size)
{
    return ((char *)GKI_getbuf(size));
}

static int dealloc(TRANSAC transac, char *p_buf)
{
	if (transac)
		GKI_freebuf(transac);
    return 0;
}

static int data_ind(TRANSAC transac)
{
	HCI_HDR *p_buf = (HCI_HDR *)transac;
	//printf("444444444444 %d, 0x%.4x\r\n", p_buf->layer_profile, p_buf->event);
	TSK_INTER_SEND_MSG(p_buf->layer_profile, p_buf->event, (void *)p_buf);		//
    return 0;
}


static int tx_result(TRANSAC transac, char *p_buf, T_HciTransmitResult result)
{



    return 0;
}

/*****************************************************************************
**   The libbt-hci Callback Functions Table
*****************************************************************************/
static const T_HciCallback hc_callbacks = 
{
    sizeof(T_HciCallback),
    preload_cb,
    alloc,
    dealloc,
    data_ind,
    tx_result,
};


