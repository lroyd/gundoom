/*************************************************************************
	> File Name: 
	> Author: lroyd
	> Mail: htzhangxmu@163.com
	> Created Time: 
 ************************************************************************/
#ifndef __VND_LIB_H__
#define __VND_LIB_H__

#include <stdint.h>
#include <sys/cdefs.h>
#include <sys/types.h>

#include "hw_type.h"

#include "uran.h"		//使用event框架

#define	PATH_LEN_SIZE_MAX	(24)

typedef struct 
{
	char	path[PATH_LEN_SIZE_MAX];
	int		port;	
}T_AddressInfo;

typedef struct 
{
	char path[PATH_LEN_SIZE_MAX];
	int	port;
}T_VendorOpenParam;

typedef struct 
{
	int		fd;
	T_AddressInfo	dst;
	int		len;
	char	*buf;
}T_VendorSendParam;



typedef enum{
    VND_OP_CTRL = 0,
    VND_OP_OPEN,
    VND_OP_CLOSE,
    VND_OP_FW_CFG,
    VND_OP_EPILOG,
	VND_OP_READ,
	VND_OP_WRITE,
	VND_OP_XMIT,		//单发
}VND_OPCODE_CMD;


typedef struct 
{
    size_t	size;
    int		(*init)(void);	
    int		(*op)(HW_DEV_TYPE, VND_OPCODE_CMD, void *, void **);
    void	(*cleanup)(void);
}T_VenderOp;

//回调实例唯一
typedef struct
{
	T_EvtMangerInfo	*pManger;
	T_TransportOp	*if_pTransport;		//对上用户层
	T_VenderOp		*if_pVendor;	//对下vender层
}T_DevInfo;	



typedef struct 
{
	T_EventInfo		in_tEvent;
	int				m_iDevFd;			//设备fd
	int				m_iDevType;			//设备类型
	int				m_iServiceFd;		//设备附属fd
	T_AddressInfo	to;					//目的地址,特殊意义书写格式保留
	T_AddressInfo	from;				//源地址，特殊意义书写格式保留
	//组合系统使用
	int				m_iProfile;			//profile id
	int				m_iProtocol;		//数据协议检查（简易）
	T_DevInfo		*in_pDevice;
}T_EntityInfo;	//底层实体信息



extern void VendorPrintEntity(T_EntityInfo *entity);

extern T_VenderOp VENDOR_LIB_INTERFACE;


#endif


