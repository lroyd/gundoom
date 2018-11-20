#ifndef USR_TIMER_H
#define USR_TIMER_H

#ifdef __cplusplus
extern "C" {
#endif





enum {
	USR_TIMER_ONCE,
	USR_TIMER_LOOP,
};

enum {
	ERR_TIMER_OK = 0,
	ERR_TIMER_LINK_LIST_INIT,
	ERR_TIMER_TTID_FULL,
	ERR_TIMER_NO_MEMORY,
};


/*******************************************************************************
* Name: 
* Descriptions:
* Parameter:	
* Return:	
* *****************************************************************************/
int USR_TimerInit();
/*******************************************************************************
* Name: 
* Descriptions:
* Parameter:	
* Return:	
* *****************************************************************************/
int USR_TimerDeinit();
/*******************************************************************************
* Name: 
* Descriptions:
* Parameter:	
* Return:	
* *****************************************************************************/
int USR_TimerHandler();
/*******************************************************************************
* Name: 
* Descriptions:
* Parameter:	
* Return:	
* *****************************************************************************/
int USR_TimerNew(int (_Handler)(int, void *), void *, const char *, int) ;
/*******************************************************************************
* Name: 
* Descriptions:
* Parameter:	
* Return:	
* *****************************************************************************/
int USR_TimerMod(int, int (_Handler)(int, void *), void *, const char *, int);
/*******************************************************************************
* Name: 
* Descriptions:
* Parameter:	
* Return:	
* *****************************************************************************/
int USR_TimerDel(int);
/*******************************************************************************
* Name: 
* Descriptions:
* Parameter:	
* Return:	
* *****************************************************************************/
int USR_TimerPrint();
/*******************************************************************************
* Name: 
* Descriptions:
* Parameter:	
* Return:	
* *****************************************************************************/
int USR_TimerQuery(int);


#ifdef __cplusplus
}
#endif

#endif
