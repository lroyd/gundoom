/*******************************************************************************
	> File Name: 
	> Author: lroyd
	> Mail: htzhangxmu@163.com
	> Created Time: 
 *******************************************************************************/
#include <sys/time.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

#include "usr_timer.h"
//#include "log_file.h"

#define TRUE 1
#define FALSE 0
/*******************************************************************************
*		define macro variable
********************************************************************************/
#define TIMER_STATIC

#define TIMER_CMP(ts1, ts2) \
	((ts1.tv_sec!=ts2.tv_sec)?(ts1.tv_sec-ts2.tv_sec):(ts1.tv_nsec-ts2.tv_nsec))

#define TIMER_CMP_SEC(ts1, ts2) \
	(ts1.tv_sec-ts2.tv_sec)


/*******************************************************************************
*		define struct variable
********************************************************************************/	
typedef int (TIMER_HANDLER)(int, void *);

typedef struct _tagLTimerNode 
{
#define TIMER_TASK_Q_SIZE_MAX		(20)
	int					m_ucTTId;
	char				m_fIsValid;
#define TIMER_TASK_NAME_SIZE_MAX	(31)	
	unsigned char		m_aucName[TIMER_TASK_NAME_SIZE_MAX + 1];
	struct timespec		ts;
	int					m_iSetTick;
	TIMER_HANDLER		*pHandler;
	void 				*pVar;
#ifndef TIMER_STATIC	
	struct _tagLTimerNode *pNext;
#endif	
} T_TimerNode, *PT_TimerLinkList;

static int timer_count = 0;	//当前动态分配的个数，无上限
static PT_TimerLinkList g_tTimerList;
static T_TimerNode g_tTimerTaskQueue[TIMER_TASK_Q_SIZE_MAX];

/*******************************************************************************
* Name: 
* Descriptions:
* Parameter:	
* Return:	timer fd >=0 / error -1 
* *****************************************************************************/
#ifndef TIMER_STATIC
/* 动态分配fd 从1开始*/
int USR_TimerNew(int (_Handler)(int, void *), void *_pVar, const char *_pucName, int _iSecs) 
{
	int ucTTId = 0;
	PT_TimerLinkList pList = g_tTimerList;
	
	if (NULL == pList) 
		return ERR_TIMER_LINK_LIST_INIT;
	
	while(NULL != pList->pNext) 
	{
		/* 找到最后一个节点 */
		ucTTId++;
		pList = pList->pNext;
	}
	
	if (NULL == pList->pNext) 
	{
		pList->pNext = malloc(sizeof(T_TimerNode));
		++timer_count;
		////syslog_wrapper(LOG_DEBUG, "malloc %d", timer_count);
		if (NULL == pList->pNext) 
		{
			////syslog_wrapper(LOG_ERROR, "add error");
			return ERR_TIMER_NO_MEMORY;
		}
		pList->pNext->pNext = NULL;
	}
	
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	ts.tv_sec	+= _iSecs;
	pList->pNext->m_ucTTId			= ucTTId;
	pList->pNext->m_iSetTick	= _iSecs;
	pList->pNext->m_fIsValid	= TRUE;
	pList->pNext->ts			= ts;
	pList->pNext->pHandler		= _Handler;
	pList->pNext->pVar			= _pVar;
	
	if (_pucName && strlen(_pucName) <= TIMER_TASK_NAME_SIZE_MAX)
	{
		memcpy(pList->pNext->m_aucName, _pucName, strlen(_pucName) + 1);
	}		
	
	////syslog_wrapper(LOG_DEBUG, "add timer fd = %d, set secs = %d, name = %s", ucTTId, _iSecs, _pucName);
	return ((ucTTId < TIMER_TASK_Q_SIZE_MAX) ? ucTTId : ERR_TIMER_TTID_FULL);
}
#else
int USR_TimerNew(int (_Handler)(int, void *), void *_pVar, const char *_pucName, int _iSecs) 
{
	int ucTTId = 0;
	for (ucTTId = 0; ucTTId < TIMER_TASK_Q_SIZE_MAX; ucTTId++)
	{
		if (g_tTimerTaskQueue[ ucTTId ].m_fIsValid == FALSE)
		{
			struct timespec ts;
			clock_gettime(CLOCK_MONOTONIC, &ts);
			ts.tv_sec	+= _iSecs;
			g_tTimerTaskQueue[ucTTId].m_ucTTId			= ucTTId;
			g_tTimerTaskQueue[ucTTId].m_iSetTick	= _iSecs;
			g_tTimerTaskQueue[ucTTId].m_fIsValid	= TRUE;
			g_tTimerTaskQueue[ucTTId].ts			= ts;
			g_tTimerTaskQueue[ucTTId].pHandler	= _Handler;
			g_tTimerTaskQueue[ucTTId].pVar		= _pVar;

			if (_pucName && strlen(_pucName) <= TIMER_TASK_NAME_SIZE_MAX)
			{
				memcpy(g_tTimerTaskQueue[ucTTId].m_aucName, _pucName, strlen(_pucName) + 1);
			}
			////syslog_wrapper(LOG_DEBUG, "add timer fd = %d, set secs = %d, name = %s", ucTTId, _iSecs, _pucName);
			break;
		}
	}

	return ((ucTTId < TIMER_TASK_Q_SIZE_MAX) ? ucTTId : ERR_TIMER_TTID_FULL);
}
#endif
/*******************************************************************************
* Name: 
* Descriptions:
* Parameter:	
* Return:	
* *****************************************************************************/
#ifndef TIMER_STATIC
int USR_TimerDel(int _ucTTId) 
{
	PT_TimerLinkList pList = g_tTimerList;
	if(NULL == pList) 
		return ERR_TIMER_LINK_LIST_INIT;
	
	while(NULL != pList->pNext) 
	{
		if(_ucTTId == pList->pNext->m_ucTTId) 
		{
			pList->pNext->m_fIsValid = FALSE; //由handler延迟释放
			////syslog_wrapper(LOG_DEBUG, "del timer fd = %d", _ucTTId);
			break;
		}
		pList = pList->pNext;
	}
	return ERR_TIMER_OK;
}
#else
int USR_TimerDel(int _ucTTId) 
{
	if(_ucTTId >= TIMER_TASK_Q_SIZE_MAX || _ucTTId < 0) 
		return ERR_TIMER_TTID_FULL;
	
	g_tTimerTaskQueue[_ucTTId].m_fIsValid = FALSE;
	
	return ERR_TIMER_OK;
}
#endif
/*******************************************************************************
* Name: 
* Descriptions:修改定时器，如果存在直接修改，不存在直接创建
* Parameter:	
* Return:	
* *****************************************************************************/
#ifndef TIMER_STATIC
int USR_TimerMod(int _ucTTId, int (_Handler)(int, void *), void *_pVar, const char *_pucName, int _iSecs) 
{
	PT_TimerLinkList pList = g_tTimerList;
	
	if (NULL == pList) 
		return ERR_TIMER_LINK_LIST_INIT;
	
	while(NULL != pList->pNext) 
	{
		if(_ucTTId == pList->pNext->m_ucTTId) 
		{
			break;
		}
		pList = pList->pNext;
	}

	if (NULL == pList->pNext) 
	{
		pList->pNext = malloc(sizeof(T_TimerNode));
		++timer_count;
		////syslog_wrapper(LOG_DEBUG, "malloc %d", timer_count);
		if (NULL == pList->pNext) 
		{
			////syslog_wrapper(LOG_ERROR, "add error");
			return ERR_TIMER_NO_MEMORY;
		}
		pList->pNext->pNext = NULL;
	}
	
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	ts.tv_sec	+= _iSecs;
	pList->pNext->m_ucTTId			= _ucTTId;
	pList->pNext->m_iSetTick	= _iSecs;
	pList->pNext->m_fIsValid	= TRUE;
	pList->pNext->ts			= ts;
	pList->pNext->pHandler		= _Handler;
	pList->pNext->pVar			= _pVar;
	
	if (_pucName && strlen(_pucName) <= TIMER_TASK_NAME_SIZE_MAX)
	{
		memcpy(pList->pNext->m_aucName, _pucName, strlen(_pucName) + 1);
	}		
	
	//syslog_wrapper(LOG_DEBUG, "mod timer fd = %d, set secs = %d, name = %s", _ucTTId, _iSecs, _pucName);
	return ERR_TIMER_OK;
}
#else
int USR_TimerMod(int _ucTTId, int (_Handler)(int, void *), void *_pVar, const char *_pucName, int _iSecs) 
{
	if(_ucTTId >= TIMER_TASK_Q_SIZE_MAX || _ucTTId < 0) 
		return ERR_TIMER_TTID_FULL;
	
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	ts.tv_sec	+= _iSecs;
	g_tTimerTaskQueue[_ucTTId].m_ucTTId		= _ucTTId;
	g_tTimerTaskQueue[_ucTTId].m_iSetTick	= _iSecs;
	g_tTimerTaskQueue[_ucTTId].m_fIsValid	= TRUE;
	g_tTimerTaskQueue[_ucTTId].ts			= ts;
	g_tTimerTaskQueue[_ucTTId].pHandler		= _Handler;
	g_tTimerTaskQueue[_ucTTId].pVar			= _pVar;

	if (_pucName && strlen(_pucName) <= TIMER_TASK_NAME_SIZE_MAX)
	{
		memcpy(g_tTimerTaskQueue[_ucTTId].m_aucName, _pucName, strlen(_pucName) + 1);
	}
	//syslog_wrapper(LOG_DEBUG, "mod timer fd = %d, set secs = %d, name = %s", _ucTTId, _iSecs, _pucName);	
	
	return ERR_TIMER_OK;
}
#endif
/*******************************************************************************
* Name: 
* Descriptions:
* Parameter:	
* Return:	
* *****************************************************************************/
#ifndef TIMER_STATIC
int USR_TimerHandler() 
{
	PT_TimerLinkList pList = g_tTimerList;
	if(NULL == pList) 
		return ERR_TIMER_LINK_LIST_INIT;
	T_TimerNode *pCurrNode;
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	while(NULL != pList->pNext) 
	{
		if(TIMER_CMP(ts, pList->pNext->ts)>0) 
		{
			if(TRUE == pList->pNext->m_fIsValid) 
			{
				if(USR_TIMER_LOOP == pList->pNext->pHandler(pList->pNext->m_ucTTId, pList->pNext->pVar)) 
				{
					pList->pNext->ts.tv_sec = ts.tv_sec+pList->pNext->m_iSetTick;
					pList->pNext->ts.tv_nsec = ts.tv_nsec;
					pList->pNext->m_fIsValid = TRUE;
				}
				else
				{
					pList->pNext->m_fIsValid = FALSE;
				}
				//syslog_wrapper(LOG_DEBUG, "after handler event %d", pList->pNext->m_ucTTId);
			}
			if(FALSE == pList->pNext->m_fIsValid) 
			{
				--timer_count;
				//syslog_wrapper(LOG_DEBUG, "free %d", timer_count);
				pCurrNode = pList->pNext;
				pList->pNext = pList->pNext->pNext;
				free(pCurrNode);
				if(NULL == pList->pNext) 
					break;
			}
		}
		pList = pList->pNext;
	}
	return ERR_TIMER_OK;
}
#else
int USR_TimerHandler() 
{
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	int ucTTId;
	for(ucTTId = 0; ucTTId < TIMER_TASK_Q_SIZE_MAX; ucTTId++) 
	{
		if(TRUE == g_tTimerTaskQueue[ucTTId].m_fIsValid && TIMER_CMP(ts, g_tTimerTaskQueue[ucTTId].ts) > 0) 
		{
			if(USR_TIMER_LOOP != g_tTimerTaskQueue[ucTTId].pHandler(ucTTId, g_tTimerTaskQueue[ucTTId].pVar)) 
			{
				g_tTimerTaskQueue[ucTTId].m_fIsValid = FALSE;
			} 
			else 
			{
				g_tTimerTaskQueue[ucTTId].ts.tv_sec = ts.tv_sec+g_tTimerTaskQueue[ucTTId].m_iSetTick;
				g_tTimerTaskQueue[ucTTId].ts.tv_nsec = ts.tv_nsec;
			}
		}
	}
	return ERR_TIMER_OK;
}
#endif
/*******************************************************************************
* Name: 
* Descriptions:
* Parameter:	
* Return:	
* *****************************************************************************/
#ifndef TIMER_STATIC
int USR_TimerInit() 
{
	/* 动态分配的第一位置不用 */
	g_tTimerList = malloc(sizeof(T_TimerNode));
	if(NULL == g_tTimerList) 
	{
		//syslog_wrapper(LOG_ERROR, "init error");
		return ERR_TIMER_NO_MEMORY;
	}
	g_tTimerList->pNext = NULL;
	//syslog_wrapper(LOG_DEBUG, "init ok");
	return ERR_TIMER_OK;
}
#else
int USR_TimerInit() 
{
	int ucTTId;
	for(ucTTId = 0; ucTTId < TIMER_TASK_Q_SIZE_MAX; ucTTId++) 
	{
		g_tTimerTaskQueue[ucTTId].m_fIsValid = FALSE;
	}
	return ERR_TIMER_OK;
}
#endif
/*******************************************************************************
* Name: 
* Descriptions:
* Parameter:	
* Return:	
* *****************************************************************************/
#ifndef TIMER_STATIC
int USR_TimerDeinit() 
{
	PT_TimerLinkList pList = g_tTimerList;
	
	if (NULL == pList) 
		return ERR_TIMER_LINK_LIST_INIT;
	
	while(NULL != pList->pNext) 
	{
		if (TRUE == pList->pNext->m_fIsValid) 
		{
			pList->pNext->m_fIsValid = FALSE;
		}
		pList = pList->pNext;
	}
	return ERR_TIMER_OK;
}
#else
int USR_TimerDeinit() 
{
	int ucTTId;
	//syslog_wrapper(LOG_DEBUG, "deinit ok");
	for(ucTTId = 0; ucTTId < TIMER_TASK_Q_SIZE_MAX; ucTTId++) 
	{
		g_tTimerTaskQueue[ucTTId].m_fIsValid = FALSE;
	}
	return ERR_TIMER_OK;
}
#endif
/*******************************************************************************
* Name: 
* Descriptions:
* Parameter:	
* Return:	
* *****************************************************************************/
#ifndef TIMER_STATIC
int USR_TimerPrint() 
{
	
	return ERR_TIMER_OK;
}
#else
int USR_TimerPrint() 
{
	int ucTTId, ucNum = 1;
	//syslog_wrapper(LOG_DEBUG, "========================================================");
	for(ucTTId = 0; ucTTId < TIMER_TASK_Q_SIZE_MAX; ucTTId++) 
	{
		if (TRUE == g_tTimerTaskQueue[ucTTId].m_fIsValid)
		{
#if 0			
			syslog_wrapper(LOG_DEBUG, "%d. TTId = %d ,Set Ticks = %d, Name = %s, Handler = 0x%08x, Var = 0x%08x", 
			ucNum, g_tTimerTaskQueue[ucTTId].m_ucTTId, 
			g_tTimerTaskQueue[ucTTId].m_iSetTick,
			&g_tTimerTaskQueue[ucTTId].m_aucName[0],
			g_tTimerTaskQueue[ucTTId].pHandler,
			g_tTimerTaskQueue[ucTTId].pVar);
#endif			
			ucNum++;
		}
	}
	//syslog_wrapper(LOG_DEBUG, "========================================================");
	return ERR_TIMER_OK;
}
#endif
/*******************************************************************************
* Name: 
* Descriptions:
* Parameter:	
* Return:	
* *****************************************************************************/
#ifndef TIMER_STATIC
int USR_TimerQuery(int _ucTTId) 
{

	return ERR_TIMER_OK;
}
#else
int USR_TimerQuery(int _ucTTId) 
{
#if 0	
	syslog_wrapper(LOG_DEBUG, "TTId = %d ,Set Ticks = %d, Name = %s, Handler = 0x%08x, Var = 0x%08x", 
	g_tTimerTaskQueue[_ucTTId].m_ucTTId, 
	g_tTimerTaskQueue[_ucTTId].m_iSetTick,
	&g_tTimerTaskQueue[_ucTTId].m_aucName[0],
	g_tTimerTaskQueue[_ucTTId].pHandler,
	g_tTimerTaskQueue[_ucTTId].pVar);
#endif	
	return ERR_TIMER_OK;
}
#endif

