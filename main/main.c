#include <stdio.h>


#include "app_gen.h"
#include "app_code_if.h"




/*************************************************************************
 *                       
 ************************************************************************/
//type	0:cfm	1:ind
void app_gen_callback(uint8_t type, uint16_t opcode, void *p_data)
{
	printf("app_gen_callback : type = %d, opcode %#06x\r\n", type, opcode);
	
	if (type == 0)
	{
		
	}
	else if (type ==1)
	{
		switch (opcode)
		{
			case APP_GEN_LINK_CHANGE_IND:	//网线的热拔插
			{
				APP_GEN_LINK_CHANGE_IND_T *msg = (APP_GEN_LINK_CHANGE_IND_T *)p_data;
				
				printf("net link change state = [%s]\r\n", msg->state?"up":"down");
				
				break;
			}			
			
			
			case APP_GEN_DEV_RCV_DATA_IND:
			{
				APP_GEN_DEV_RCV_DATA_IND_T *msg = (APP_GEN_DEV_RCV_DATA_IND_T *)p_data;
				
				printf("fd = %d, len = %d, data = %s\r\n", msg->fd, msg->len, msg->p_buf);
				
				break;
			}
			
			case APP_GEN_DEV_FD_ADD_IND:
			{
				APP_GEN_DEV_FD_ADD_IND_T *msg = (APP_GEN_DEV_FD_ADD_IND_T *)p_data;
				
				printf("add fd = %d, mfd = %d\r\n", msg->fd, msg->mfd);
				
				AppNettcpServerSendMsg(msg->fd, "hello b", 7);
				
				break;
			}
			case APP_GEN_DEV_FD_DEL_IND:
			{
				APP_GEN_DEV_FD_DEL_IND_T *msg = (APP_GEN_DEV_FD_DEL_IND_T *)p_data;
				
				printf("del fd = %d\r\n", msg->fd);
				
				break;
			}			
			
		}
	}
	
}


/* callback list */
const handlerCbFunc app_handler_cb[] = 
{
    app_gen_callback,             
    NULL,             
    NULL,             
    NULL			              
};

int uart_fd = -1;	

int tfd = -1;
int ufd = -1;
int cfd = -1;
int main(int argc, char **argv) 
{

	AppRegisterHandleCb(&app_handler_cb);

	AppDevicePowerReq(1);
	sleep(2);

	//AppNetConnLinkSocket("fuck", 9527);

#if 0	
	//uart 测试
	
	//AppDeviceUartOpen("/dev/ttyUSB0", 115200, &uart_fd);	// dev/ttyAMA2
	AppDeviceUartOpen("/dev/ttyAMA2", 9600, &uart_fd);	//底层串口波特率 写死的9600
	
	printf("uart_fd = %d\r\n",uart_fd);
	
	AppDeviceUartSend(uart_fd, "test", 4);
	AppDeviceUartClose(uart_fd);
#endif	

	//AppNetLinkGetState();


	AppNettcpServerOpen("0.0.0.0", 60000, &tfd);	//创建本地 TCP server
	//sleep(1);
	//AppNetudpServerOpen("0.0.0.0", 60000, &ufd);	//创建本地 UDP server
	//sleep(1);
	
	//AppTcpClientOpen("192.168.1.164", 60000, &cfd);
	//AppTcpClientOpen("0.0.0.0", 9527, &cfd);
	//AppUdpClientOpen("192.168.1.164", 60000, &cfd);
	while(1)
	{
		//AppDeviceUartSend(uart_fd, "test", 4);
		
		//AppNettcpServerSendMsg(ufd, "tcp test", 8);
		//AppNetudpServerSendMsg(ufd, "udp test", 8);
		//AppTcpClientSendMsg(cfd, "tcp client test", 15);
		//AppUdpClientSendMsg(cfd, "udp client test", 15);
		
		sleep(1);
	}
	
    return 0;
}










