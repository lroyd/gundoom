/*************************************************************************
	> File Name: errno.c
	> Author: lroyd
	> Mail: htzhangxmu@163.com
	> Created Time: 
*************************************************************************/
#include "coerrno.h"


#ifndef GH_MAX_ERR_MSG_HANDLER
	#define GH_MAX_ERR_MSG_HANDLER   10
#endif

/* Error message handler. */
static unsigned g_iErrMsgHndCnt = 0;
static struct _tagErrMsgHnd
{
    int			m_iStartAddr;
    int	    	m_iEndAddr;
    void	  	(*pUsrErrHandle)(int, char*, size_t);

} g_tErrMsgHndTable[GH_MAX_ERR_MSG_HANDLER];


static const struct 
{
    int			m_iCode;
    const char *m_strMsg;
} g_tCommErrMsg[] = 
{
    GH_BUILD_ERR(GH_EUNKNOWN,      "Unknown Error" ),
    GH_BUILD_ERR(GH_EPENDING,      "Pending operation" ),
    GH_BUILD_ERR(GH_ETOOMANYCONN,  "Too many connecting sockets" ),
    GH_BUILD_ERR(GH_EINVAL,        "Invalid value or argument" ),
    GH_BUILD_ERR(GH_ENAMETOOLONG,  "Name too long" ),
    GH_BUILD_ERR(GH_ENOTFOUND,     "Not found" ),
    GH_BUILD_ERR(GH_ENOMEM,        "Not enough memory" ),
    GH_BUILD_ERR(GH_EBUG,          "BUG DETECTED!" ),
    GH_BUILD_ERR(GH_ETIMEDOUT,     "Operation timed out" ),
    GH_BUILD_ERR(GH_ETOOMANY,      "Too many objects of the specified type"),
    GH_BUILD_ERR(GH_EBUSY,         "Object is busy"),
    GH_BUILD_ERR(GH_ENOTSUP,	   "Option/operation is not supported"),
    GH_BUILD_ERR(GH_EINVALIDOP,	   "Invalid operation"),
    GH_BUILD_ERR(GH_ECANCELLED,    "Operation cancelled"),
    GH_BUILD_ERR(GH_EEXISTS,       "Object already exists" ),
    GH_BUILD_ERR(GH_EEOF,	   	"End of file" ),
    GH_BUILD_ERR(GH_ETOOBIG,	   "Size is too big"),
    GH_BUILD_ERR(GH_ERESOLVE,	   "gethostbyname() has returned error"),
    GH_BUILD_ERR(GH_ETOOSMALL,	   "Size is too short"),
    GH_BUILD_ERR(GH_EIGNORED,	   "Ignored"),
    GH_BUILD_ERR(GH_EIPV6NOTSUP,   "IPv6 is not supported"),
    GH_BUILD_ERR(GH_EAFNOTSUP,	   "Unsupported address family"),
    GH_BUILD_ERR(GH_EGONE,	   "Object no longer exists"),
    GH_BUILD_ERR(GH_ESOCKETSTOP,   "Socket is in bad state"),
	GH_BUILD_ERR(GH_EFULL,   	"Object is full")
};


#define IN_RANGE(val,start,end)	    ((val)>=(start) && (val)<(end))


static int comm_error(int _iCode, char *_pBuf, size_t _iLen)
{
    int iLen;
    unsigned i;
    for (i=0; i<sizeof(g_tCommErrMsg)/sizeof(g_tCommErrMsg[0]); ++i) 
	{
        if (g_tCommErrMsg[i].m_iCode == _iCode) 
		{
            size_t iLen2 = strlen(g_tCommErrMsg[i].m_strMsg);
            if (iLen2 >= _iLen) iLen2 = _iLen-1;
			memcpy(_pBuf, g_tCommErrMsg[i].m_strMsg, iLen2);
            _pBuf[iLen2] = '\0';
            return (int)iLen2;
        }
    }
}


GH_DEF(int) 
ErrnoRegisterHandlers(int _iStartAddr, int _iSpaceSize, ERROR_CALLBACK _pHnd)
{
    unsigned i;

    _ERROR_RETURN(_iStartAddr && _iSpaceSize && _pHnd, GH_EINVAL);
    _ERROR_RETURN(g_iErrMsgHndCnt < _ARRAY_SIZE(g_tErrMsgHndTable), GH_ETOOMANY);
    _ERROR_RETURN(_iStartAddr >= GH_ERRNO_START_USER, GH_EEXISTS);

    /* Check that no existing handler has covered the specified range. */
    for (i=0; i<g_iErrMsgHndCnt; ++i) 
	{
		if (IN_RANGE(_iStartAddr, g_tErrMsgHndTable[i].m_iStartAddr, g_tErrMsgHndTable[i].m_iEndAddr) || 
			IN_RANGE(_iStartAddr + _iSpaceSize - 1, g_tErrMsgHndTable[i].m_iStartAddr, g_tErrMsgHndTable[i].m_iEndAddr))
		{
			if (g_tErrMsgHndTable[i].m_iStartAddr == _iStartAddr && 
				g_tErrMsgHndTable[i].m_iEndAddr == (_iStartAddr+_iSpaceSize) && 
				g_tErrMsgHndTable[i].pUsrErrHandle == _pHnd)
			{
				/* The same range and handler has already been registered */
				return GH_SUCCESS;
			}

			return GH_EEXISTS;
		}
    }

    /* Register the handler. */
    g_tErrMsgHndTable[g_iErrMsgHndCnt].m_iStartAddr = _iStartAddr;
    g_tErrMsgHndTable[g_iErrMsgHndCnt].m_iEndAddr = _iStartAddr + _iSpaceSize;
    g_tErrMsgHndTable[g_iErrMsgHndCnt].pUsrErrHandle = _pHnd;

    ++g_iErrMsgHndCnt;

    return GH_SUCCESS;
}

GH_DEF(void)
ErrnoClearHandlers(void)
{
    g_iErrMsgHndCnt = 0;
	memset(&g_tErrMsgHndTable[0], 0, sizeof(g_tErrMsgHndTable));
}

GH_DEF(void) 
ErrnoGetMessage(int _iCode, char *_pBuf, size_t _iLen )
{
    int iLen = -1;

    _ASSERT(_pBuf && _iLen);

    if (_iCode == GH_SUCCESS) 
    {
		//成功
		iLen = snprintf(_pBuf, _iLen, "Success");
    }
    else if (_iCode < GH_ERRNO_START + GH_ERRNO_SPACE_SIZE) 
    {
		//未知
        iLen = snprintf(_pBuf, _iLen, "unknown error %d", _iCode);
    } 
    else if (_iCode < GH_ERRNO_START_STATUS + GH_ERRNO_SPACE_SIZE)
    {
		//comm层
        iLen = comm_error(_iCode, _pBuf, _iLen);
    } 
    else if (_iCode < GH_ERRNO_START_SYS + GH_ERRNO_SPACE_SIZE) 
    {
		//system层
        //iLen = platform_strerror(GH_STATUS_TO_OS(_iCode), _pBuf, _iLen);
    } 
    else 
    {
		//用户自定义层
		unsigned i;

		/* Find user handler to get the error message. */
		for (i=0; i<g_iErrMsgHndCnt; ++i) 
		{
		    if (IN_RANGE(_iCode, g_tErrMsgHndTable[i].m_iStartAddr, g_tErrMsgHndTable[i].m_iEndAddr)) 
		    {
				return (*g_tErrMsgHndTable[i].pUsrErrHandle)(_iCode, _pBuf, _iLen);
		    }
		}

		/* Handler not found! */
		iLen = snprintf( _pBuf, _iLen, "not found user handler : unknown error %d", _iCode);
    }

    if (iLen < 1 || iLen >= (int)_iLen) 
    {
		iLen = (int)(_iLen - 1);
		_pBuf[iLen] = '\0';
    }

    return;
}














