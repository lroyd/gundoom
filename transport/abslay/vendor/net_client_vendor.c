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
#include <errno.h>

#include "hw_type.h"
#include "hw_debug.h"
/************************************************************************
* 			
************************************************************************/




/************************************************************************
* 			
************************************************************************/
static hw_status_cb status_cb = NULL;

static int inet_connect(const char *path, int port)
{
	
	
	
	return 0;
}

static int unix_connect(const char *path, int port)
{
	
	
	
	return 0;
}
/************************************************************************
* Name: 		
* Descriptions:
* Parameter:		
* Return:     
* **********************************************************************/
static int net_client_init(hw_status_cb p_cb)
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
//port = 0 表示被动创建的
//默认长连接
//path:目标地址
static int net_client_open(int dev_type, const char *path, int port)
{
	int fd = -1;

	switch(SUB_DEV_MSK(dev_type))
	{
		case 0:	//tcp,提前设置内置地址
			fd = socket(AF_INET,SOCK_STREAM, 0);	//tcp connect阻塞75s+，非阻塞需要使用select
			break;			
		case 1:	//udp
			fd = socket(AF_INET,SOCK_DGRAM, 0);		//udp connect始终返回0
			break;		
		case 2:	//unx
			fd = socket(AF_UNIX,SOCK_STREAM, 0);
			break;		
		default:
			log_out(LOG_ERROR, "tcp client type error");
			return fd;
	}
	//设置非阻塞
	int flags = fcntl(fd, F_GETFL, 0);
	fcntl(fd, F_SETFL, flags|O_NONBLOCK);

	struct sockaddr_in sockaddr; 
	sockaddr.sin_family = AF_INET; 
	sockaddr.sin_port = htons(port); 
	sockaddr.sin_addr.s_addr = inet_addr(path); 
	socklen_t length = sizeof(sockaddr);
	int ret = connect(fd, (struct sockaddr*)&sockaddr, length);
	if (ret == -1)
	{
		if (errno == EINPROGRESS)
		{
			//正在连接
			fd_set wset;
			int optval;
			socklen_t optlen = sizeof(optval);
			struct timeval tv;
			tv.tv_sec = NET_SOCK_CONN_TIMEOUT;
			tv.tv_usec = 0;
			
			/* wait to be available in writing */
			FD_ZERO(&wset);
			FD_SET(fd, &wset);
			ret = select(fd + 1, NULL, &wset, NULL, &tv);
			if (ret <= 0) 
			{
				/* timeout or fail */
				log_out(LOG_ERROR, "net client connect timeout error");
				close(fd);
				return -1;
			}
			/* the connection is established if SO_ERROR and optval are set to 0 */
			ret = getsockopt(fd, SOL_SOCKET, SO_ERROR, (void *)&optval, &optlen);
			if (ret == 0 && optval == 0) 
			{
				log_out(LOG_TRACE, "net tcp client create fd %d",fd);
				return fd;
			} 
			else 
			{
				errno = ECONNREFUSED;
				log_out(LOG_ERROR, "net client connect refused error");
				close(fd);
				return -1;
			}
		}
		else
		{
			log_out(LOG_ERROR, "net client connect sock error");
			close(fd);
			return -1;
		}

	}
	else
	{
		log_out(LOG_TRACE, "net client create fd %d",fd);
	}		

	return fd;
}

/************************************************************************
* Name: 		
* Descriptions:
* Parameter:		
* Return:     
* **********************************************************************/
static int net_client_close(int fd)
{
	log_out(LOG_TRACE, "net client close fd = %d", fd);
	return close(fd);
}

/************************************************************************
* Name: 		
* Descriptions:
* Parameter:		
* Return:     
* **********************************************************************/
//udp 使用sendto 和 目标地址
static int net_client_write(int type, int fd, char *path, int port, char *p_buf, int len)
{
    int ret, total = 0;
	struct sockaddr_in sockaddr;
	switch(SUB_DEV_MSK(type))
	{
		case 0:	//tcp 本身连接有地址记录，直接发送就行
		{
			//大数据发送的写法
			while(len != 0)
			{
				ret = write(fd, p_buf + total, len);
				total += ret;
				len -= ret;
			}	
			log_out(LOG_TRACE, "tcp client send fd = %d, to = %s:%d, data = %s, len = %d", fd, path, port, p_buf, total);	
			break;
		}
		case 1:	//udp 需要附加目的地址
			sockaddr.sin_family = AF_INET;
			sockaddr.sin_addr.s_addr = inet_addr(path);
			sockaddr.sin_port = htons(port);
			sendto(fd, p_buf, len, 0, (struct sockaddr *)&sockaddr, sizeof(sockaddr));
			log_out(LOG_TRACE, "udp client send fd = %d, to = %s:%d, data = %s, len = %d", fd, path, port, p_buf, len);	
			break;		
		case 2:	//unx
			break;		
		default:
			log_out(LOG_ERROR, "net client type error");
	}

    return total;	
}

/************************************************************************
* Name: 		
* Descriptions:
* Parameter:		
* Return:     
* **********************************************************************/
static int net_client_read(int type, int fd, char *p_buf, int len)
{
	int ret = -1;
	
	ret = read(fd, p_buf, len);	
	
	if (!ret)
	{
		//断开
		if (status_cb)
		{
			status_cb(HW_EVT_DEL_SOCK, HW_DEV_NET_CLI, fd, 0, NULL, 0);
		}
	}
	log_out(LOG_TRACE, "net client recv fd = %d, data = %s, read len = %d", fd, p_buf, ret);	
	
	return ret;
}


const T_HWDevInterface hw_net_client_interface = 
{
    sizeof(T_HWDevInterface),
	net_client_init,
	
    net_client_open,
    net_client_close,
	NULL,			
	
	NULL,
	
	net_client_read,
	net_client_write
};