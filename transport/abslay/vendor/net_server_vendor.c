/*******************************************************************************
	> File Name: 
	> Author: lroyd
	> Mail: htzhangxmu@163.com
	> Created Time: 
 *******************************************************************************/
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/prctl.h>
#include <sys/un.h>


#include "hw_type.h"
#include "hw_debug.h"
/************************************************************************
* 			
************************************************************************/
#define	NET_SER_LISTEN_SIZE_MAX	(10)

#define UNIX_DOMAIN "/tmp/UNIX.domain"

/************************************************************************
* 			
************************************************************************/
static hw_status_cb status_cb = NULL;




static int tcp_create(const char *path, int port)
{
	int fd = socket(AF_INET, SOCK_STREAM, 0);
	int enable = 1;
	setsockopt(fd, SOL_SOCKET, SO_REUSEADDR,(void *)&enable, sizeof (enable));

    struct sockaddr_in sockaddr;
    sockaddr.sin_family = AF_INET;
    sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);	//server 不判定path，默认0.0.0.0
    sockaddr.sin_port = htons(port);

    if(bind(fd,(struct sockaddr *)&sockaddr,sizeof(sockaddr))== -1)
    {
		log_out(LOG_ERROR, "tcp server bind error");
        return -1;
    }
 
    if(listen(fd, NET_SER_LISTEN_SIZE_MAX) == -1)
    {
		log_out(LOG_ERROR, "tcp server listen error");
        return -1;
    }	
	log_out(LOG_TRACE, "tcp server listen fd = %d, path = %s, port = %d", fd, path, port);
	return fd;
}

static int udp_create(const char *path, int port)
{
	int fd = socket(AF_INET, SOCK_DGRAM, 0);
	
	int enable = 1;
	setsockopt(fd, SOL_SOCKET, SO_REUSEADDR,(void *)&enable, sizeof (enable));

    struct sockaddr_in sockaddr;
    sockaddr.sin_family = AF_INET;
    sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);	//server 不判定path，默认0.0.0.0
    sockaddr.sin_port = htons(port);

    if(bind(fd,(struct sockaddr *)&sockaddr,sizeof(sockaddr))== -1)
    {
		log_out(LOG_ERROR, "udp server bind error");
        return -1;
    }	
	log_out(LOG_TRACE, "udp server listen fd = %d, path = %s, port = %d", fd, path, port);
	return fd;
}

static int unix_create(const char *path, int port)
{
	int fd = socket(AF_UNIX, SOCK_STREAM, 0);
	struct sockaddr_un sockaddr;
	sockaddr.sun_family = AF_UNIX;
	strcpy(sockaddr.sun_path,UNIX_DOMAIN);
	
	unlink(UNIX_DOMAIN);
	
    if(bind(fd,(struct sockaddr *)&sockaddr,sizeof(sockaddr)) <0 )
	{
		log_out(LOG_ERROR, "unix_create bind error");
		unlink(UNIX_DOMAIN);
        return -1;
    }
    if(listen(fd,	NET_SER_LISTEN_SIZE_MAX) < 0)
	{
		log_out(LOG_ERROR, "unix_create listen error");
		unlink(UNIX_DOMAIN);
        return -1;
    }	
	log_out(LOG_TRACE, "unix_create fd = %d, path = %s, port = %d", fd, path, port);
	return fd;
}

/************************************************************************
* Name: 		
* Descriptions:
* Parameter:		
* Return:     
* **********************************************************************/
static int net_server_init(hw_status_cb p_cb)
{
	status_cb = p_cb;
	return 0;
}

/************************************************************************
* Name: 		
* Descriptions:
* Parameter:		
* Return:     
* **********************************************************************/
static int net_server_open(int dev_type, const char *path, int port)
{
	int fd = -1;	
	switch(SUB_DEV_MSK(dev_type))
	{
		case 0:	//tcp
			fd = tcp_create(path, port);
			break;
		case 1:	//udp
			fd = udp_create(path, port);
			break;		
		case 2:	//unx
			fd = unix_create(path, port);
			break;		
		default:
			log_out(LOG_ERROR, "net_server_open:error type");
	}
	
	return fd;
}

/************************************************************************
* Name: 		
* Descriptions:
* Parameter:		
* Return:     
* **********************************************************************/
static int net_server_close(int fd)
{
	//注意 如果是 server fd close 是否手动关闭下属子fd，还是会自动关闭子fd？？？
	log_out(LOG_TRACE, "net server close fd = %d", fd);
	return close(fd);
}

/************************************************************************
* Name: 		
* Descriptions:
* Parameter:		
* Return:     
* **********************************************************************/
static int net_server_write(int type, int fd, char *path, int port, char *p_buf, int len)
{
	int ret = 0;
	//tcp server 应该没有发送 她的发送都是依赖于子fd
	
	
	
	if (SUB_DEV_MSK(type) == 1)
	{
		//udp server 具有发送，给最后一个连接的fd，内置地址
		struct sockaddr_in sockaddr;
		sockaddr.sin_family = AF_INET;
		sockaddr.sin_addr.s_addr = inet_addr(path);
		sockaddr.sin_port = htons(port);
		ret = sendto(fd, p_buf, len, 0, (struct sockaddr *)&sockaddr, sizeof(sockaddr));	
		log_out(LOG_TRACE, "udp server send fd %d, path %s:%d, data %s, len %d",fd, path, port, p_buf, len);
	}

    return ret;		
}

//tcp server read函数，只是用来接收新连接用，没有用户数据
//udp server 
/************************************************************************
* Name: 		
* Descriptions:
* Parameter:		
* Return:     
* **********************************************************************/
static int net_server_read(int type, int fd, char *p_buf, int len)
{
	int ret = -1, read_len = 0, conn_fd = -1;
	char addr[24] = {0};
	int port = 0;
	struct sockaddr_in sockaddr;
	socklen_t length = sizeof(sockaddr);
	
	switch(SUB_DEV_MSK(type))
	{
		case 0:	//tcp
		{
			//有tcp/unix客户端连进来
			conn_fd = accept(fd, (struct sockaddr*)&sockaddr, &length);		
			strcpy(addr, inet_ntoa(sockaddr.sin_addr));
			port = ntohs(sockaddr.sin_port);
			log_out(LOG_TRACE, "accept tcp client fd = %d, mfd = %d, from = %s:%d, status_cb = 0x%04x", 
												conn_fd, 
												fd, 
												addr, port, 
												status_cb);
			if(status_cb)
			{
				ret = status_cb(HW_EVT_ADD_SOCK, 
								HW_DEV_NET_CLI, 
								conn_fd, 
								fd, 
								addr,
								port); 
			}
			read_len = 0;			
			break;
		}
		case 1:	//udp
		{
			//udp 直接及接受
			//需要把地址信息-》to
			read_len = recvfrom(fd, p_buf, len, 0, (struct sockaddr*)&sockaddr, &length);
			strcpy(addr, inet_ntoa(sockaddr.sin_addr));
			port = ntohs(sockaddr.sin_port);	
			log_out(LOG_TRACE, "accept udp client fd = %d, mfd = %d, from = %s:%d, data = %s, len = %d, status_cb = 0x%04x", 
												fd, 
												-1, 
												addr, port, 
												p_buf, read_len, 
												status_cb);	
			if(status_cb)
			{
				ret = status_cb(HW_EVT_ADD_SOCK, 
								HW_DEV_NET_CLI, 
								fd, 
								fd,
								addr,
								port); 

			}	
			
			break;	
		}
				
		case 2:	//unx
			
			break;		
		default:
			log_out(LOG_ERROR, "net server type error");
	}	

	return read_len;	//返回值决定是否回调用户层	
}


const T_HWDevInterface hw_net_server_interface = 
{
    sizeof(T_HWDevInterface),
	net_server_init,
	
    net_server_open,
    net_server_close,
	NULL,		
	NULL,
	
	net_server_read,
	net_server_write  
};