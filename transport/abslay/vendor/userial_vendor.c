/*******************************************************************************
	> File Name: 
	> Author: lroyd
	> Mail: htzhangxmu@163.com
	> Created Time: 
 *******************************************************************************/
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <termios.h>



#include "hw_type.h"
#include "hw_debug.h"


/************************************************************************
* Name: 		
* Descriptions:
* Parameter:		
* Return:     
* **********************************************************************/
static int userial_open(int dev_type, const char *path, int port)
{
	int ufd = open(path, O_RDWR | O_NOCTTY);
	if(ufd < 0)
	{
		printf("open %s failed\r\n",path);
		return -1;
	}
	
	struct termios options; 
	tcgetattr( ufd,&options);
    cfmakeraw(&options);
    options.c_cflag |= (CRTSCTS | 0);
    tcsetattr(ufd, TCSANOW, &options);
	
	tcflush(ufd, TCIOFLUSH);
	tcsetattr(ufd, TCSANOW, &options);
	tcflush(ufd, TCIOFLUSH);
	
	cfsetispeed(&options, B9600);   //B115200		B9600
	cfsetospeed(&options, B9600);	
	tcsetattr(ufd, TCSANOW, &options);	
	
	return ufd;
}

/************************************************************************
* Name: 		
* Descriptions:
* Parameter:		
* Return:     
* **********************************************************************/
static int userial_close(int fd)
{
	return close(fd);
}

/************************************************************************
* Name: 		
* Descriptions:
* Parameter:		
* Return:     
* **********************************************************************/
static int userial_write(int type, int fd, char *path, int port, char *p_buf, int len)
{
    int ret, total = 0;
	
	//printf("111111111 fd = %d, len = %d, data = %s\r\n",fd, len, p_buf);
    while(len != 0)
    {
        ret = write(fd, p_buf + total, len);
        total += ret;
        len -= ret;
    }
	
    return total;		
}

/************************************************************************
* Name: 		
* Descriptions:
* Parameter:		
* Return:     
* **********************************************************************/
static int userial_read(int type, int fd, char *p_buf, int len)
{
	printf("000000000000000\r\n");
	
	int ret = -1;
	ret = read(fd, p_buf, len);

	return ret;
}

const T_HWDevInterface hw_userial_interface = 
{
    sizeof(T_HWDevInterface),
	NULL,
	
    userial_open,
    userial_close,
	NULL,			//cong
	
	NULL,
	
	userial_read,
	userial_write
};