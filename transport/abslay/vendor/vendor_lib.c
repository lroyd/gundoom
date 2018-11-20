/*******************************************************************************
	> File Name: 
	> Author: lroyd
	> Mail: htzhangxmu@163.com
	> Created Time: 
 *******************************************************************************/
#include <stdio.h>
#include <string.h>
#include <fcntl.h>

#include "coerrno.h"
#include "abslay.h"
#include "vendor_lib.h"
#include "userial_vendor.h"
#include "net_server_vendor.h"
#include "net_client_vendor.h"
#include "net_monitor_vendor.h"

#include "hw_debug.h"
/************************************************************************
*
************************************************************************/
const T_HWDevInterface	*g_tDeviceTable[HW_DEV_BUTT] = {	
	&hw_net_server_interface,		
	&hw_net_client_interface,	
	&hw_monitor_interface,
	&hw_userial_interface,		
	NULL
};

/************************************************************************
* Name: 		
* Descriptions:
* Parameter:		
* Return:     
* **********************************************************************/
static int HWStatusHandle(HW_EVT_STATUS _emState, HW_DEV_TYPE _emDevType, int _iLocalFd, int _iServerFd, char *_pucAddr, int _iPort)
{
	TRANS_RESULT iRet = TRANS_RESULT_FAIL;
	int tmpServiceFd = -1;
	
	//找到回调
	T_DevInfo		*pThis		= NULL;
	T_EntityInfo	*pEntity	= NULL;
	switch(_emState)
	{
		case HW_EVT_ADD_SOCK:	//最后回调
		{	
			//根据mfd得到ser类型
			//找到对应server信息
			HashGetEntityInfo(_iServerFd, (void *)&pEntity);	//不是当前连接子fd
			_ERROR_RETURN(pEntity, -1);
			pThis = pEntity->in_pDevice;
			if (_iServerFd != _iLocalFd)
			{
				//fd != -1 有tcp新连接
				//创建实体
				T_EntityInfo *pSubEntity = (T_EntityInfo *)malloc(sizeof(T_EntityInfo));	
				pSubEntity->m_iDevFd		= _iLocalFd;
				pSubEntity->m_iDevType	= _emDevType|pEntity->m_iDevType;
				pSubEntity->m_iServiceFd		= _iServerFd;
				tmpServiceFd = _iServerFd;
				//目的地址
				memcpy(pSubEntity->to.path, pEntity->to.path, strlen(pEntity->to.path) + 1); //来自服务器
				pSubEntity->to.port	= pEntity->to.port;
				//源地址 
				strcpy(pSubEntity->from.path, _pucAddr); 	//来自回调
				pSubEntity->from.port	= _iPort;		

				pSubEntity->m_iProfile		= pEntity->m_iProfile;
				pSubEntity->m_iProtocol	= pEntity->m_iProtocol;
				
				if (AbslayAddItem(pThis, pSubEntity))
				{
					pThis = NULL;
					free(pSubEntity);
				}
				else
				{
					//VendorPrintEntity(pSubEntity);
					log_out(LOG_DEBUG, "add new tcp client socket fd = %d, mfd = %d, to = %s:%d, from = %s:%d", 
										_iLocalFd, 
										tmpServiceFd,
										pSubEntity->to.path, pSubEntity->to.port,
										pSubEntity->from.path, pSubEntity->from.port);
					iRet = TRANS_RESULT_SUCCESS;					
				}

			}
			else
			{
				//ucp新连接，不创建实体，只修改服务器to地址！！！是服务器
				strcpy(pEntity->to.path, _pucAddr);
				pEntity->to.port	= _iPort;
				log_out(LOG_DEBUG, "add new udp client socket fd = %d, from = %s:%d", _iLocalFd, pEntity->to.path, pEntity->to.port);
				//不回调
				pThis = NULL;
				iRet = TRANS_RESULT_SUCCESS;
			}
			//由fd可以找到，回调用户层
			if (pThis)
			{
				(pThis->if_pTransport)->status_cb(_emState, _iLocalFd, tmpServiceFd, iRet);
			}
			break;
		}	
		case HW_EVT_DEL_SOCK:	
		{
			HashGetEntityInfo(_iLocalFd, (void *)&pEntity);
			_ERROR_RETURN(pEntity, -1);
			pThis = pEntity->in_pDevice;
			tmpServiceFd = pEntity->m_iServiceFd;

			log_out(LOG_DEBUG, "del client socket fd = %d, mfd = %d, to = %s:%d, from = %s:%d",
										_iLocalFd, 
										tmpServiceFd,
										pEntity->to.path, pEntity->to.port,
										pEntity->from.path, pEntity->from.port);			

			iRet = TRANS_RESULT_SUCCESS;

			//先回调，再删除
			if (pThis)
			{
				(pThis->if_pTransport)->status_cb(_emState, _iLocalFd, tmpServiceFd, iRet);
			}
			
			if(AbslayDeleteItem(_iLocalFd, pEntity))
			{
				log_out(LOG_WARNING, "fd %d, absl delete item error", _iLocalFd);
				return -1;
			}			
			
			if(pEntity)
			{
				free(pEntity);
			}			
			break;
		}
		case HW_EVT_LINK_UP:	
		case HW_EVT_LINK_DOWN:	
		{
			HashGetEntityInfo(_iLocalFd, (void *)&pEntity);			
			_ERROR_RETURN(pEntity, -1);
			pThis = pEntity->in_pDevice;
			iRet = TRANS_RESULT_SUCCESS;
			if (pThis)
			{
				(pThis->if_pTransport)->status_cb(_emState, _iLocalFd, tmpServiceFd, iRet);
			}			
			break;
		}
	}

	//hook(xx);

	return 0;
}


/************************************************************************
* Name: 		
* Descriptions:
* Parameter:		
* Return:     
* **********************************************************************/
static int init(void)
{
	//初始化所有底层init函数
	int i;
	for(i=0; i<HW_DEV_BUTT; i++)
	{
		if (g_tDeviceTable[i] && g_tDeviceTable[i]->init)
		{
			g_tDeviceTable[i]->init(HWStatusHandle);
		}
	}
    return 0;
}

/************************************************************************
* Name: 		
* Descriptions:
* Parameter:		
* Return:     
* **********************************************************************/
static int op(HW_DEV_TYPE _emDevType, VND_OPCODE_CMD _emOpcode, void *_vParam, void **_p2OutEntity)
{
	int iRet = -1;
	_ERROR_RETURN(g_tDeviceTable[_emDevType & MAJOR_DEV_MSK], -1);
	
    switch(_emOpcode)
    {
        case VND_OP_CTRL:
		{
			break;
		}
            
			
		case VND_OP_OPEN:
		{
			int tmpFd;
			T_EntityInfo *pEntity = (T_EntityInfo *)malloc(sizeof(T_EntityInfo));	
			_ERROR_RETURN(pEntity, -1);
			
			T_VendorOpenParam *pOpen = (T_VendorOpenParam *)_vParam;
			tmpFd = g_tDeviceTable[_emDevType & MAJOR_DEV_MSK]->open(_emDevType, pOpen->path, pOpen->port);
			_ERROR_RETURN(tmpFd >= 0, -1);
			
			pEntity->m_iDevFd		= tmpFd;
			pEntity->m_iDevType	= _emDevType;
			pEntity->m_iServiceFd	= -1;
			//目的地址
			memcpy(pEntity->to.path, pOpen->path, strlen(pOpen->path) + 1); //拷贝结束符
			pEntity->to.port	= pOpen->port;
			//源地址
			memcpy(pEntity->from.path, pOpen->path, strlen(pOpen->path) + 1); 	//拷贝结束符
			pEntity->from.port	= pOpen->port;			
			
			iRet = tmpFd;
			*_p2OutEntity = pEntity;
			break;			
		}
		case VND_OP_CLOSE:
		{
			int tmpFd = *(int *)_vParam;
			g_tDeviceTable[_emDevType & MAJOR_DEV_MSK]->close(tmpFd);
			//注意半关闭的情况（目前没考虑）
			T_EntityInfo *pEntity = *_p2OutEntity;
			if (pEntity)
			{
				free(pEntity);
			}
			iRet = 0;
			break;			
		}
		
		case VND_OP_READ:
		{
			int tmpFd = *(int *)_vParam;
			char buf[DEV_READ_BUF_SIZE] = {0};

			T_EntityInfo	*pEntity = NULL;
			
			HashGetEntityInfo(tmpFd, (void *)&pEntity);	
			_ERROR_RETURN(pEntity, -1);
			
			if (g_tDeviceTable[pEntity->m_iDevType & MAJOR_DEV_MSK]->read)
			{
				iRet = g_tDeviceTable[pEntity->m_iDevType & MAJOR_DEV_MSK]->read(pEntity->m_iDevType, tmpFd, buf, sizeof(buf));
				if (iRet)
				{
					//回调用户层
					T_TransportOp *pTransCb = pEntity->in_pDevice->if_pTransport;
					if (pTransCb)
					{
						(pTransCb)->rcv_cb(tmpFd, pEntity->m_iServiceFd, buf, iRet);
					}					
				}				
			}
			break;	
		}
		case VND_OP_WRITE:
		{			
			T_VendorSendParam *pSend = (T_VendorSendParam *)_vParam;
			g_tDeviceTable[_emDevType & MAJOR_DEV_MSK]->write(_emDevType, 
														pSend->fd, 
														pSend->dst.path, 
														pSend->dst.port, 
														pSend->buf, 
														pSend->len);
			break;			
		}	
		case VND_OP_XMIT:
		{
			T_VendorSendParam *pSend = (T_VendorSendParam *)_vParam;
			//1.open
			int tmpFd = g_tDeviceTable[_emDevType & MAJOR_DEV_MSK]->open(_emDevType, pSend->dst.path, pSend->dst.port);
			_ERROR_RETURN(tmpFd >= 0, -1);
			
			//2.write
			g_tDeviceTable[_emDevType & MAJOR_DEV_MSK]->write(_emDevType, 
														tmpFd, 
														pSend->dst.path, 
														pSend->dst.port, 
														pSend->buf, 
														pSend->len);			
			
			//3.close
			g_tDeviceTable[_emDevType & MAJOR_DEV_MSK]->close(tmpFd);
			iRet = 0;
			break;
		}
	}	

EXIT:	
	
	return iRet;
}


/************************************************************************
* Name: 		
* Descriptions:
* Parameter:		
* Return:     
* **********************************************************************/
static void cleanup( void )
{
	int i;
	for(i=0; i<HW_DEV_BUTT; i++)
	{
		if (g_tDeviceTable[i] && g_tDeviceTable[i]->epiolg)
		{
			g_tDeviceTable[i]->epiolg(0);
		}
	}
	
	return;
}

/************************************************************************
* Name: 		
* Descriptions:
* Parameter:		
* Return:     
* **********************************************************************/
void VendorPrintEntity(T_EntityInfo *pEntity)
{	
	log_out(LOG_DEBUG, "entity : type = 0x%04d, fd = %d, mfd = %d, to = %s:%d, from = %s:%d", 
																	pEntity->m_iDevType,
																	pEntity->m_iDevFd, pEntity->m_iServiceFd,
																	pEntity->to.path, pEntity->to.port,
																	pEntity->from.path, pEntity->from.port);
	return;
}


T_VenderOp VENDOR_LIB_INTERFACE = 
{
    sizeof(T_VenderOp),
    init,
    op,
    cleanup
};













