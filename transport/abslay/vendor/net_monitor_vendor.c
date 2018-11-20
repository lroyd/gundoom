/*******************************************************************************
	> File Name: 
	> Author: lroyd
	> Mail: htzhangxmu@163.com
	> Created Time: 
 *******************************************************************************/
#include <string.h> 
#include <unistd.h>
#include <stdlib.h>  
#include <stdio.h>  
#include <asm/types.h> 
#include <sys/types.h>  
#include <sys/socket.h>  
#include <sys/ioctl.h>  
#include <linux/if.h>  
#include <linux/netlink.h>  
#include <linux/rtnetlink.h> 

#include "hw_type.h"
#include "hw_debug.h"

/************************************************************************
* 注意:全局唯一
* **********************************************************************/
#define MONITOR_RECV_SIZE			(1024)  


int		link_fd = -1;

static hw_status_cb status_cb = NULL;
/************************************************************************
* Name: 		
* Descriptions:
* Parameter:		
* Return:     
* **********************************************************************/
static int monitor_init(hw_status_cb p_cb)
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
static int monitor_open(int dev_type, const char *path, int port)
{
	if (link_fd != -1)
	{
		//已经打开过了
		goto EXIT;
	}
	int len = MONITOR_RECV_SIZE;
	struct sockaddr_nl		addr;  
	//.创建sock
	link_fd = socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE);  
    setsockopt(link_fd, SOL_SOCKET, SO_RCVBUF, &len, sizeof(len));  
    addr.nl_family = AF_NETLINK;  
    addr.nl_groups = RTNLGRP_LINK;  
    bind(link_fd, (struct sockaddr*)&addr, sizeof(addr)); 
	printf("netlink start fd = %d ......\r\n", link_fd);
EXIT:	
	return link_fd;
}

/************************************************************************
* Name: 		
* Descriptions:
* Parameter:		
* Return:     
* **********************************************************************/
static int monitor_close(int fd)
{
	close(fd);
	link_fd = -1;
	return 0;
}


/************************************************************************
* Name: 		
* Descriptions:
* Parameter:		
* Return:     
* **********************************************************************/
static int monitor_read(int type, int fd, char *p_buf, int _len)
{
	int read_size = 0, link_len, notice = 0, change;
	struct ifinfomsg	*ifinfo;  
	struct nlmsghdr		*nh;
	struct rtattr		*attr;  
	char 	buf[MONITOR_RECV_SIZE];

	read_size = read(link_fd, buf, MONITOR_RECV_SIZE);
	for (nh = (struct nlmsghdr *) buf; NLMSG_OK(nh, read_size); nh = NLMSG_NEXT(nh, read_size)) 
	{
		switch (nh->nlmsg_type) 
		{
			default:
				/*收到些奇怪的信息*/
				printf("nh->nlmsg_type = %d\n", nh->nlmsg_type);
				break;
			case NLMSG_DONE:
			case NLMSG_ERROR:
				printf("111111111\n");
				break;
			case RTM_NEWLINK:
			case RTM_DELLINK:
				notice = 1;
				ifinfo = NLMSG_DATA(nh);  
				printf("%u: %s", ifinfo->ifi_index, (ifinfo->ifi_flags & IFF_LOWER_UP) ? "up" : "down" );  
				change = (ifinfo->ifi_flags & IFF_LOWER_UP)? HW_EVT_LINK_UP : HW_EVT_LINK_DOWN;
				
				//获取网卡信息
				attr = (struct rtattr*)(((char*)nh) + NLMSG_SPACE(sizeof(*ifinfo)));  
				link_len = nh->nlmsg_len - NLMSG_SPACE(sizeof(*ifinfo));  
				for (; RTA_OK(attr, link_len); attr = RTA_NEXT(attr, link_len))  
				{  
					if (attr->rta_type == IFLA_IFNAME)  
					{  
						printf(" %s", (char*)RTA_DATA(attr));  
						break;  
					}  
				}  
				printf("\n"); 				

				break;
			case RTM_NEWADDR:
			case RTM_DELADDR:
				printf("333333333\n");
				break;
			case RTM_NEWROUTE:
			case RTM_DELROUTE:
				printf("22222222\n");

				
				break;			
			
			
			
		}

	}

	if(notice && status_cb)
	{
		status_cb(change, 
					HW_DEV_BUTT, link_fd, -1, NULL, 0); 	
	}	
	
	return 0;	//不需要回调数据层
}

const T_HWDevInterface hw_monitor_interface = 
{
    sizeof(T_HWDevInterface),
	monitor_init,
    monitor_open,
    monitor_close,
	NULL,			
	NULL,
	monitor_read,
	NULL
};