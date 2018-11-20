/*************************************************************************
	> File Name: errno.h
	> Author: lroyd
	> Mail: htzhangxmu@163.com
	> Created Time: 
 ************************************************************************/
#ifndef _COMM_ERRNO_H_
#define _COMM_ERRNO_H_

#include "common.h" 


#ifndef GH_BUILD_ERR
	#define GH_BUILD_ERR(code,msg) { code, msg " (" #code ")" }
#endif


/************************************************************************
* 			Error Code Definition
* **********************************************************************/
#define GH_ERRNO_START			20000
#define GH_ERRNO_SPACE_SIZE		50000
#define GH_ERRNO_START_STATUS	(GH_ERRNO_START + GH_ERRNO_SPACE_SIZE)			//comm code		:70001 - 119000
#define GH_ERRNO_START_SYS		(GH_ERRNO_START_STATUS + GH_ERRNO_SPACE_SIZE)	//system code	:120000 - 169000
/* 需要用户自行处理 */
#define GH_ERRNO_START_USER		(GH_ERRNO_START_SYS + GH_ERRNO_SPACE_SIZE)		//usr code		:170000 - 219000


#define GH_SUCCESS				(0)
//comm code		:70001 - 119000
#define GH_EUNKNOWN				(GH_ERRNO_START_STATUS + 1)	// 70001 Unknown error has been reported
#define GH_EPENDING				(GH_ERRNO_START_STATUS + 2)	// 70002 The operation is pending and will be completed later.
#define GH_ETOOMANYCONN			(GH_ERRNO_START_STATUS + 3)	// 70003 Too many connecting sockets.
#define GH_EINVAL	    		(GH_ERRNO_START_STATUS + 4)	// 70004 Invalid argument.
#define GH_ENAMETOOLONG	    	(GH_ERRNO_START_STATUS + 5)	// 70005 Name too long (eg. hostname too long).
#define GH_ENOTFOUND	    	(GH_ERRNO_START_STATUS + 6)	// 70006 Not found.
#define GH_ENOMEM	    		(GH_ERRNO_START_STATUS + 7)	// 70007 Not enough memory.
#define GH_EBUG             	(GH_ERRNO_START_STATUS + 8)	// 70008 Bug detected!
#define GH_ETIMEDOUT        	(GH_ERRNO_START_STATUS + 9)	// 70009 Operation timed out.
#define GH_ETOOMANY         	(GH_ERRNO_START_STATUS + 10)// 70010 Too many objects.
#define GH_EBUSY            	(GH_ERRNO_START_STATUS + 11)// 70011 Object is busy.
#define GH_ENOTSUP	    		(GH_ERRNO_START_STATUS + 12)// 70012 The specified option is not supported.
#define GH_EINVALIDOP	    	(GH_ERRNO_START_STATUS + 13)// 70013 Invalid operation.
#define GH_ECANCELLED	    	(GH_ERRNO_START_STATUS + 14)// 70014 Operation is cancelled.
#define GH_EEXISTS          	(GH_ERRNO_START_STATUS + 15)// 70015 Object already exists.
#define GH_EEOF		    		(GH_ERRNO_START_STATUS + 16)// 70016 End of file.
#define GH_ETOOBIG	    		(GH_ERRNO_START_STATUS + 17)// 70017 Size is too big.
#define GH_ERESOLVE	    		(GH_ERRNO_START_STATUS + 18)// 70018 
#define GH_ETOOSMALL	    	(GH_ERRNO_START_STATUS + 19)// 70019 Size is too small.
#define GH_EIGNORED	    		(GH_ERRNO_START_STATUS + 20)// 70020 Ignored
#define GH_EIPV6NOTSUP	    	(GH_ERRNO_START_STATUS + 21)// 70021 IPv6 is not supported
#define GH_EAFNOTSUP	    	(GH_ERRNO_START_STATUS + 22)// 70022 Unsupported address family
#define GH_EGONE	    		(GH_ERRNO_START_STATUS + 23)// 70023 Object no longer exists
#define GH_ESOCKETSTOP	    	(GH_ERRNO_START_STATUS + 24)// 70024 Socket is stopped
#define GH_EFULL            	(GH_ERRNO_START_STATUS + 25)// 70025 Object is full.

typedef void (*ERROR_CALLBACK)(int , char *, size_t);

GH_DECL(int)	ErrnoRegisterHandlers(int, int, ERROR_CALLBACK);
GH_DECL(void)	ErrnoClearHandlers(void);
GH_DEF(void)	ErrnoGetMessage(int, char *, size_t);




#endif


