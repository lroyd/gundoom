#include <stdio.h>

#include "hw_type.h"

#include "abslay.h"



static int rcv_cb(int fd, int mfd, void *p_buf, int len)
{
	printf("..........usr rcv fd = %d, mfd = %d, data = %s, len = %d\r\n", fd, mfd, p_buf, len);
	
	//AbslayDevSend(fd, "riririr", 8);
	
	return 0;
}

static void status_cb(HW_EVT_STATUS state, int fd, int mfd, TRANS_RESULT ret)
{
	printf("net_status_cb state = %d, fd = %d, mfd = %d, ret = %d\r\n", state, fd, mfd, ret);
	
	return;
}


T_TransportOp usr_callbacks = {
    sizeof(T_TransportOp),
    NULL,
    NULL,
    NULL,
    NULL,
	rcv_cb,
    NULL,
	status_cb
};

int ufd = -1;
int tcp_fd = -1;
int tcp_fd2 = -1;
int tcp_fd3 = -1;

void *device = NULL;

























void main(int argc, char *argv[])
{
	int ret,i;
	AbslayCreate(&usr_callbacks, &device);

#if 1	
	//串口测试
	AbslayDevOpen(device, HW_DEV_UART, "/dev/ttyUSB0", 9600, &ufd);
	printf("ufd = %d\r\n", ufd);
	
	AbslayDevSend(ufd, "riririr", 7);
	
	//absl_print_info(ufd);

#endif	
	
#if 0	
	//tcp serv测试
	//ret = AbslayDevOpen(device, HW_DEV_NET_SER, "0.0.0.0", 10086, &tcp_fd);  //HW_DEV_NET_SER|NET_TYPE_UDP
	//printf("tcp_fd = %d,ret = %d\r\n", tcp_fd, ret);
	//AbslayDevOpen(device, HW_DEV_NET_SER|NET_TYPE_UDP, "0.0.0.0", 10087, &tcp_fd2);	

	//client测试
	//tcp/udp 长连接，手动断开
	char buf[64] = {0};
	ret = AbslayDevOpen(device, HW_DEV_NET_CLI, "192.168.1.164", 60000, &tcp_fd3);
	printf("tcp_fd3  = %d,ret = %d\r\n", tcp_fd3, ret);
	for(i=0;i<10;i++)
	{
		snprintf(buf, 64, "fuck you %d",i);
		AbslayDevSend(tcp_fd3, buf, strlen(buf));
		sleep(1);
	}
	AbslayDevClose(tcp_fd3);
	
	//短连接，发送完直接断开
	//sleep(2);
	//AbslayDevSendAddress(HW_DEV_NET_CLI|NET_TYPE_UDP, "192.168.1.164", 60001, "this is test", 13);	//tcp/udp
#endif
	
	while(1)
	{
		
		sleep(1);
	}
	
	return;
}













