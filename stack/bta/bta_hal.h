#ifndef __STACK_INIT_H__
#define __STACK_INIT_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "hw_type.h"
#include "os_declare.h"
#include "pro_code_if.h"

#define HAL_INVALID_PROFLE  APP_GEN_TASK//(0xFFFF)

//同步构造，不需要malloc
#define MAKE_HAL_MSG_IN(op)     op##_IN_T in_msg
#define MAKE_HAL_MSG_OUT(op)    op##_OUT_T out_msg


typedef struct
{
	uint8_t		layer_profile;	//profile
	uint8_t		layer_protocol;	//protocol
    uint16_t	event;
    uint16_t	offset;
	int			attache;		//mfd	
	int			local;			//fd	
	int			len;			//数据长度
} HCI_HDR;


void stack_init(void);
void stack_deinit(void);

int stack_hci_send (int , uint8_t *p_msg, int len);




//ioctl
#define HAL_OP_DEV_CREATE		(0x00000001)	//同步命令	
typedef struct
{
	int		type;
	int		port;
	int		profile;	//是否绑定， 不绑定：HAL_INVALID_PROFLE
	int		protocol;
    char	*path;  //同步方式不用分配的方式直接引用？
	
}HAL_OP_DEV_CREATE_IN_T;

typedef struct
{
	int fd;
}HAL_OP_DEV_CREATE_OUT_T;

#define HAL_OP_DEV_DESTROY		(0x00000002)	//同步命令	




void stack_hci_ioctl(int op, void *in_param, void *out_param);






//提供给外层使用重定义名
#define	HAL_IOCTL(p, p1, p2)		stack_hci_ioctl(p, p1, p2)
#define	HAL_XMIT_MSG(p1, p2, p3)	stack_hci_send(p1, p2, p3)




#ifdef __cplusplus
}
#endif


#endif
