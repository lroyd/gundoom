/*************************************************************************
	> File Name: app_gen_cmd.c
	> Author: lroyd
	> Mail: 
	> Created Time: 
 ************************************************************************/
#include "app_gen_private.h"
#include "hw_type.h"

/************************************************************************
* Name: 
* Descriptions:同步操作
* Param:		
* Note:
* **********************************************************************/
uint8_t AppDevicePowerReq(char sw)
{
    switch(getHostState())
	{
		case NEWF_ST_OFF:
		{
			if(sw == 1)
			{
				APP_GEN_LOG(LOG_TRACE, LOG_GEN_CMD, "starting the system");
				SysPowerOn();
			}
			break;
		}
		case NEWF_ST_INITIALIZING:
		{
			APP_GEN_LOG(LOG_WARNING, LOG_GEN_CMD, "starting the system, please wait a moment...");
			break;			
		}
		case NEWF_ST_READY:	
		{
			if(sw == 0)
			{
				APP_GEN_LOG(LOG_TRACE, LOG_GEN_CMD, "ready to close the system....");
				SysPowerOff();
			}
			break;			
		}
		case NEWF_ST_CLOSING:	
		{
			//关闭是否有中间态
			
			
			
			
			break;			
		}		
	}
	
    return 0;
}

/************************************************************************
* Name: 
* Descriptions:获取link状态
* Param:		
* Note:
* **********************************************************************/
int AppNetLinkGetState(void)
{


	return 0;
}

//直接设备打开
/************************************************************************
* Name: 
* Descriptions:直接打开串口设备（同步操作）
* Param:		
* Note:
* **********************************************************************/
int AppDeviceUartOpen(uint8_t *path, int port, int *out_fd)
{
	_ERROR_RETURN(path != NULL, -1);
#if 0	
	if (getHostState() != NEWF_ST_READY)
	{
		APP_GEN_LOG(LOG_ERROR, LOG_GEN_CMD, "the system is not open. Please turn on the system first!! Usage: AppDevicePowerReq(1)");
		return -1;
	}
#endif	
	APP_GEN_LOG(LOG_TRACE, LOG_GEN_CMD, "uart open = [%s:%d]", path, port);
	
	GenUartOpen(path, port, out_fd);
	

	return 0;
}
//直接底层发送
int AppDeviceUartSend(int fd, char *buf, int len)
{
	_ERROR_RETURN(fd != -1 && buf, -1);

	APP_GEN_LOG(LOG_TRACE, LOG_GEN_CMD, "uart fd = [%d] send data = %s, len = %d", fd, buf, len);

	GenUartSendMsg(fd, buf, len);
	
	return 0;
}

int AppDeviceUartClose(int fd)
{
	_ERROR_RETURN(fd != -1, -1);

	APP_GEN_LOG(LOG_TRACE, LOG_GEN_CMD, "uart fd = [%d] close", fd);
	
	GenUartClose(fd);
	return 0;
}

/************************************************************************
* Name: 
* Descriptions:tcp server 只是监听，通信依靠附属fd
* Param:		
* Note:
* **********************************************************************/
int AppNettcpServerOpen(uint8_t *path, int port, int *out_fd)
{
	_ERROR_RETURN(path != NULL, -1);

	APP_GEN_LOG(LOG_TRACE, LOG_GEN_CMD, "tcp server open = [%s:%d]", path, port);
	
	GenNetDeviceOpen(HW_DEV_NET_SER|NET_TYPE_TCP, path, port, out_fd);
	

	return 0;
}

int AppNettcpServerClose(int fd)
{
	_ERROR_RETURN(fd != -1, -1);

	APP_GEN_LOG(LOG_TRACE, LOG_GEN_CMD, "tcp server fd = [%d] close", fd);
	
	GenNetDeviceClose(fd);
	return 0;
}

//同步操作
//必须依附于已经存在的fd
int AppNettcpServerSendMsg(int fd, char *p_data, int len)
{
	_ERROR_RETURN(fd != -1, -1);

	APP_GEN_LOG(LOG_TRACE, LOG_GEN_CMD, "tcp server send fd = [%d], len = [%d], data = [%s]", fd, len, p_data);
	
	GenNetDeviceSendByFd(fd, p_data, len);
	return 0;
}
/************************************************************************
* Name: 
* Descriptions:udp server 的发送只发送最后一次记录者
* Param:		
* Note:
* **********************************************************************/
int AppNetudpServerOpen(uint8_t *path, int port, int *out_fd)
{
	_ERROR_RETURN(path != NULL, -1);
	
	APP_GEN_LOG(LOG_TRACE, LOG_GEN_CMD, "udp server open = [%s:%d]", path, port);
	
	GenNetDeviceOpen(HW_DEV_NET_SER|NET_TYPE_UDP, path, port, out_fd);

	return 0;
}

int AppNetudpServerClose(int fd)
{
	_ERROR_RETURN(fd != -1, -1);

	APP_GEN_LOG(LOG_TRACE, LOG_GEN_CMD, "udp server fd = [%d] close", fd);
	
	GenNetDeviceClose(fd);
	return 0;
}
//udp server 的发送只发送最后一次记录者
int AppNetudpServerSendMsg(int fd, char *p_data, int len)
{
	_ERROR_RETURN(fd != -1, -1);

	APP_GEN_LOG(LOG_TRACE, LOG_GEN_CMD, "udp server send fd = [%d], len = [%d], data = [%s]", fd, len, p_data);
	
	GenNetDeviceSendByFd(fd, p_data, len);
	return 0;
}

/************************************************************************
* Name: 
* Descriptions:
* Param:		
* Note:
* **********************************************************************/
int AppTcpClientOpen(uint8_t *path, int port, int *out_fd)
{
	_ERROR_RETURN(path != NULL, -1);
	
	APP_GEN_LOG(LOG_TRACE, LOG_GEN_CMD, "tcp client open = [%s:%d]", path, port);
	
	GenNetDeviceOpen(HW_DEV_NET_CLI|NET_TYPE_TCP, path, port, out_fd);
	
	return 0;
}

int AppTcpClientClose(int fd)
{
	_ERROR_RETURN(fd != -1, -1);

	APP_GEN_LOG(LOG_TRACE, LOG_GEN_CMD, "tcp client fd = [%d] close", fd);
	
	GenNetDeviceClose(fd);
	return 0;
}

int AppTcpClientSendMsg(int fd, char *p_data, int len)
{
	_ERROR_RETURN(fd != -1, -1);

	APP_GEN_LOG(LOG_TRACE, LOG_GEN_CMD, "tcp server send fd = [%d], len = [%d], data = [%s]", fd, len, p_data);
	
	GenNetDeviceSendByFd(fd, p_data, len);
	return 0;
}
/************************************************************************
* Name: 
* Descriptions:
* Param:		
* Note:
* **********************************************************************/
int AppUdpClientOpen(uint8_t *path, int port, int *out_fd)
{
	_ERROR_RETURN(path != NULL, -1);
	
	APP_GEN_LOG(LOG_TRACE, LOG_GEN_CMD, "udp client open = [%s:%d]", path, port);
	
	GenNetDeviceOpen(HW_DEV_NET_CLI|NET_TYPE_UDP, path, port, out_fd);
	
	return 0;
}

int AppUdpClientClose(int fd)
{
	_ERROR_RETURN(fd != -1, -1);

	APP_GEN_LOG(LOG_TRACE, LOG_GEN_CMD, "udp client fd = [%d] close", fd);
	
	GenNetDeviceClose(fd);
	return 0;
}

int AppUdpClientSendMsg(int fd, char *p_data, int len)
{
	_ERROR_RETURN(fd != -1, -1);

	APP_GEN_LOG(LOG_TRACE, LOG_GEN_CMD, "udp server send fd = [%d], len = [%d], data = [%s]", fd, len, p_data);
	
	GenNetDeviceSendByFd(fd, p_data, len);
	return 0;
}

/************************************************************************
* Name: 
* Descriptions:
* Param:		
* Note:
* **********************************************************************/
//永久修改：interface	动态/静态
int AppNetworkInterface()
{
	
	return 0;
}
//临时修改 IP，mask，gw
int AppNetworkSetConfig()
{
	
	return 0;
}

int AppNetworkGetConfig()
{
	
	return 0;
}

//mac
int AppNetworkSetMac()
{
	
	return 0;
}

int AppNetworkGetMac()
{
	
	return 0;
}











