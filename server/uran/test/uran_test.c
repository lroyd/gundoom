#include "common.h"
#include "uran.h"
#include "uran_debug.h"

#define TIMER_MAIN   (2*1000)	// 1000ms单位

static INT32 MainTimerFun(void *_pThis)
{
	PT_EventInfo pThis = _pThis;
	pThis->m_emType		= EVENT_TIMER;
	pThis->m_iEventFD	= TIMER_MAIN;
	
	UranRegister(pThis, pThis->m_pManger);  
	
	printf("MainTimerFun...\r\n");
   	return 0;
}

/*******************************************************************************
*		用户定时器测试
********************************************************************************/
typedef struct _tagUserInfo
{    
	T_EventInfo	in_tEvent;
	INT32		m_iCnt;
}T_UserInfo;

T_UserInfo g_pUserEvt;

INT32 SubTimerFun(void *_pThis)
{
	T_UserInfo *pThis = _pThis;
	INT32 iCnt = pThis->m_iCnt++;
	if(pThis->m_iCnt == 5)
	{
		UranCancel(_pThis);
	}
	else
	{
		pThis->in_tEvent.m_emType	= EVENT_TIMER;
		pThis->in_tEvent.m_iEventFD	= 1000;	//1s
		
		UranRegister(pThis, pThis->in_tEvent.m_pManger); 		
	}
 
	printf("cnt = %d\r\n", iCnt);
   	return 0;
}


/*******************************************************************************
*		
********************************************************************************/
static void testCleanup(void *_pArg)
{
	printf("xxxxxx %s\r\n", _pArg);
	return;
}

INT32 main(INT32 argc, char **argv) 
{
	void *pMaiManger = NULL;
	
	UranMangerCreate(MANGER_ROLE_MASTER, 0, MainTimerFun, TIMER_MAIN, "test", &pMaiManger); //MainTimerFun, TIMER_MAIN
	UranMangerSetCleanup(pMaiManger, testCleanup, "fuck");
/********************************************************************************/	
	/* 测试：注册用户定时器 */

	g_pUserEvt.in_tEvent.m_emType		= EVENT_TIMER;
	g_pUserEvt.in_tEvent.m_iEventFD		= 1000;	//1s
	g_pUserEvt.in_tEvent.m_Handle		= SubTimerFun;
	g_pUserEvt.m_iCnt = 0;
	UranRegister(&g_pUserEvt, pMaiManger);

/********************************************************************************/	
	sleep(3);
	UranMangerDestroy(pMaiManger);
	while(1)
	{
		sleep(1);
	}
 
    return 0;
}














