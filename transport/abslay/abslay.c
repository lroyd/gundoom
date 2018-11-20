/*******************************************************************************
	> File Name: abslay.c
	> Author: lroyd
	> Mail: htzhangxmu@163.com
	> Created Time: 
 *******************************************************************************/
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/epoll.h>

#include "coerrno.h"
#include "uepoll.h"
#include "hash_fd.h"
#include "vendor_lib.h"

#include "abslay.h"
#include "hw_debug.h"


/************************************************************************
* 			
************************************************************************/




/************************************************************************
* Name: 		
* Descriptions:
* Parameter:		
* Return:     
* **********************************************************************/
INT32 AbslayConnProcess(void *_pThis)
{
	T_EntityInfo	*pEntity 	= (T_EntityInfo *)_pThis;
	T_VenderOp		*pVndOpFunc = pEntity->in_pDevice->if_pVendor;
	
	if (pVndOpFunc)
	{
		pVndOpFunc->op(pEntity->m_iDevType, VND_OP_READ, (void *)&pEntity->m_iDevFd, NULL);	
	}
	return 0;    
}


GH_DEF(int)
AbslayAddItem(void *_pInDevice, void *_pInEntity)
{
	_ERROR_RETURN(_pInDevice && _pInEntity, -1);
	
	T_DevInfo		*pDevice	= (T_DevInfo *)_pInDevice;
	T_EntityInfo	*pEntity	= (T_EntityInfo *)_pInEntity;

	pEntity->in_tEvent.m_emType		= EVENT_INLT;
	pEntity->in_tEvent.m_iEventFD	= pEntity->m_iDevFd;
	pEntity->in_tEvent.m_Handle		= AbslayConnProcess;
	
	UranRegister(&pEntity->in_tEvent, pDevice->pManger);	
	
	pEntity->in_pDevice = pDevice;
	
	T_HashData tItem;
	tItem.fd		= pEntity->m_iDevFd;
	tItem.entity	= pEntity;
	
	HashInsertData((void *)&tItem);	
	
	return 0;
}

GH_DEF(int)
AbslayDeleteItem(int _iFd, void *_pInEntity)
{
	_ERROR_RETURN(_pInEntity && _iFd >= 0, -1);
	
	T_EntityInfo	*pEntity = (T_EntityInfo *)_pInEntity;

	UranCancel((void *)pEntity);

	HashDeleteData(_iFd);	
	
	return 0;
}

/************************************************************************
* Name: 		
* Descriptions:
* Parameter:		
* Return:     
* **********************************************************************/
static void AbslayCleanup(void *_pArg)
{
	T_DevInfo *pThis = (T_DevInfo *)_pArg;
	if (pThis->if_pVendor)
	{
		pThis->if_pVendor->cleanup();	
	}	
	
	free(pThis);
	
	return;
}

/************************************************************************
* Name: 		
* Descriptions:
* Parameter:		p_cb:本层和vendor层，hw层都可以使用
* Return:     
* **********************************************************************/
GH_DEF(int)
AbslayCreate(T_TransportOp *_pUserCallbck, void **_p2DevInfo)
{
	T_DevInfo *pThis = (T_DevInfo *)malloc(sizeof(T_DevInfo));
	_ERROR_RETURN(pThis, -1);

	int iRet = -1;

	pThis->if_pTransport = _pUserCallbck;		
	
	iRet = UranMangerCreate(MANGER_ROLE_MASTER, 0, NULL, 0, "abslay",(void **)&pThis->pManger);	//注意pManger是malloc出来的，需要释放
	if (iRet == -1)
	{
		free(pThis);
		return -1;
	}
	UranMangerSetCleanup(pThis->pManger, AbslayCleanup, (void *)pThis);
	/* 2. hash fd table*/
	HashFdInit();
	
	/* 3.挂在vendor */
	pThis->if_pVendor = &VENDOR_LIB_INTERFACE;
	
	if (pThis->if_pVendor)
	{
		pThis->if_pVendor->init();	
	}
	else
	{
		log_out(LOG_ERROR,"device open: missing vendor interface, unable to open device!!!");
		//
		free(pThis);
        return -1;		
	}		

	if (_p2DevInfo)
	{
		*_p2DevInfo = pThis;
	}
	
	
	return 0;
}


GH_DEF(int)
AbslayDestroy(void *_pDevInfo)
{
	_ERROR_RETURN(_pDevInfo, -1);
	T_DevInfo *pThis = (T_DevInfo *)_pDevInfo;
	UranMangerDestroy(pThis);
	
	HashFdDeinit();
	return 0;
}

/************************************************************************
* Name: 		
* Descriptions:
* Parameter:		
* Return:     
* **********************************************************************/
GH_DEF(int)
AbslayDevOpen(void *_pDevInfo, HW_DEV_TYPE _emDevType, const char *_strPath, int _iPort, int *_pOutFd)
{
	_ERROR_RETURN(_pDevInfo, -1);
	
	T_DevInfo	*pThis = _pDevInfo;
	T_VenderOp *pVndOpFunc = pThis->if_pVendor;
	int tmpFd;
	if (_pOutFd)*_pOutFd = -1;

	/* 1.open */
	T_EntityInfo *pEntity = NULL;
	T_VendorOpenParam tOpen;
	memcpy(tOpen.path, _strPath, strlen(_strPath));
	tOpen.port = _iPort;	
	tmpFd = pVndOpFunc->op(_emDevType, VND_OP_OPEN, (void *)&tOpen, (void**)&pEntity);	//VND_DEV_UART_OP_OPEN
	if (tmpFd < 0)
	{
		log_out(LOG_ERROR,"device open: fd %d error, %s:%d", tmpFd, _strPath, _iPort);
		if (!pEntity)
		{
			free(pEntity);
		}
		return -1;
	}

	if (AbslayAddItem(pThis, pEntity))
	{
		//-1：失败
		log_out(LOG_ERROR,"device open:fd = %d, epoll add error!!!", tmpFd);
		//关闭设备
		pVndOpFunc->op(_emDevType, VND_OP_CLOSE, &tmpFd, NULL);
		return -1;		
	}
	
	if (_pOutFd)*_pOutFd = tmpFd;
    return 0;
}
/************************************************************************
* Name: 		
* Descriptions:
* Parameter:		
* Return:     
* **********************************************************************/
GH_DEF(int)
AbslayDevClose(int _iFd)
{
	_ERROR_RETURN(_iFd >= 0, -1);
	
	T_EntityInfo	*pEntity = NULL;
	T_VenderOp		*pVndOpFunc;
	
	HashGetEntityInfo(_iFd, (void *)&pEntity);
	pVndOpFunc = pEntity->in_pDevice->if_pVendor;
	
	_ERROR_RETURN(pEntity && pVndOpFunc, -1);
			
	
	if(AbslayDeleteItem(_iFd, pEntity))
	{
		log_out(LOG_ERROR,"device close: fd %d not found device !!!");
		return -1;		
	}
	
	/* hw close */
	pVndOpFunc->op(pEntity->m_iDevType, VND_OP_CLOSE, &_iFd, (void *)&pEntity);

	
	return 0;
}

GH_DEF(int)
AbslayDevSend(int _iFd, char *_pucData, int _iLen)
{
	_ERROR_RETURN(_iFd >= 0 && _pucData, -1);

	T_EntityInfo	*pEntity = NULL;
	T_VenderOp		*pVndOpFunc;
	HashGetEntityInfo(_iFd, (void *)&pEntity);
	pVndOpFunc = pEntity->in_pDevice->if_pVendor;
	
	_ERROR_RETURN(pEntity && pVndOpFunc, -1);
	
	T_VendorSendParam tSend;
	tSend.fd = _iFd;

	if (pEntity->m_iDevType == HW_DEV_NET_CLI)
	{
		//对于tcp client， 取出from
		strcpy(tSend.dst.path, pEntity->from.path);
		tSend.dst.port = pEntity->from.port;		
	}
	else
	{
		strcpy(tSend.dst.path, pEntity->to.path);
		tSend.dst.port = pEntity->to.port;		
	}
	tSend.len = _iLen;
	tSend.buf = _pucData;

	pVndOpFunc->op(pEntity->m_iDevType, VND_OP_WRITE, (void *)&tSend, NULL);	
	
	return 0;
}

GH_DEF(int)
AbslayDevSendAddress(HW_DEV_TYPE _emDevType, const char *_strPath, int _iPort, char *_pucData, int _iLen)
{
#if 0	
	_ERROR_RETURN(pVndOpFunc, -1);

	T_VendorSendParam tSend;
	strcpy(tSend.dst.path, _strPath);
	tSend.dst.port = _iPort;
	tSend.len = _iLen;
	tSend.buf = _pucData;
	
	pVndOpFunc->op(_emDevType, VND_OP_XMIT, (void *)&tSend, NULL);
#endif	
	return 0;
}

GH_DEF(int)
AbslayPrintInfo(int _iFd)
{
	_ERROR_RETURN(_iFd >= 0, -1);

	T_EntityInfo	*pEntity = NULL;
	
	HashGetEntityInfo(_iFd, (void *)&pEntity);
	_ERROR_RETURN(pEntity, -1);
	
	VendorPrintEntity(pEntity);
	return 0;
}

GH_DEF(int)
AbslaySetProfile(int _iFd, int _iProfile, int _iProtocol)
{
	_ERROR_RETURN(_iFd >= 0, -1);
	
	T_EntityInfo	*pEntity = NULL;
	
	HashGetEntityInfo(_iFd, (void *)&pEntity);
	
	pEntity->m_iProfile		= _iProfile;
	pEntity->m_iProtocol	= _iProtocol;
	
	return 0;
}

GH_DEF(int)
AbslayGetProfile(int _iFd, int *_pOutProfile, int *_pOutProtocol)
{
	_ERROR_RETURN(_iFd >= 0, -1);
	
	T_EntityInfo	*pEntity = NULL;
	
	HashGetEntityInfo(_iFd, (void *)&pEntity);
	_ERROR_RETURN(pEntity, -1);
	
	if(_pOutProfile)
		*_pOutProfile = pEntity->m_iProfile;
	if(_pOutProtocol)
		*_pOutProtocol= pEntity->m_iProtocol;
	
	return 0;
}





