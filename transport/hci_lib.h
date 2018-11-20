#ifndef __HCI_LIB_H__
#define __HCI_LIB_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <sys/cdefs.h>
#include <sys/types.h>

/* Host/Controller lib internal event ID */
#define HC_EVENT_PRELOAD               0x0001
#define HC_EVENT_POSTLOAD              0x0002
#define HC_EVENT_RX                    0x0004
#define HC_EVENT_TX                    0x0008
#define HC_EVENT_LPM_ENABLE            0x0010
#define HC_EVENT_LPM_DISABLE           0x0020
#define HC_EVENT_LPM_WAKE_DEVICE       0x0040
#define HC_EVENT_LPM_ALLOW_SLEEP       0x0080
#define HC_EVENT_LPM_IDLE_TIMEOUT      0x0100
#define HC_EVENT_EXIT                  0x0200
#define HC_EVENT_EPILOG                0x0400

typedef void* TRANSAC;

typedef enum {
    HC_PRELOAD_SUCCESS  = 0x00,    //鍚姩鎴愬姛  0x00
    HC_PRELOAD_CB_ERROR,           //鍥炶皟鏈寕杞?
    HC_PRELOAD_UART_ERROR = 0x10,  //涓插彛澶辫触
    HC_PRELOAD_FULL_ERROR          //涓插彛澶辫触锛屽洖璋冨け璐?
}T_HciPreloadResult;

typedef enum {
    HC_TX_SUCCESS,
    HC_TX_FAIL,
    HC_TX_FRAGMENT
}T_HciTransmitResult;




typedef void (*preload_result_cb)(TRANSAC , T_HciPreloadResult );

typedef char* (*alloc_mem_cb)(int );

typedef int (*dealloc_mem_cb)(TRANSAC , char *);

typedef int (*tx_result_cb)(TRANSAC , char *, T_HciTransmitResult );

typedef int (*data_ind_cb)(TRANSAC );

typedef struct 
{
    size_t         size;
	
    preload_result_cb  preload_cb;

    alloc_mem_cb   alloc;

    dealloc_mem_cb dealloc;

    data_ind_cb data_ind;

    tx_result_cb  tx_result;
} T_HciCallback;


typedef struct 
{
    size_t          size;

    int   (*init)(const T_HciCallback* p_cb);

    void (*set_power)(int state);

    void (*preload)(int fd, TRANSAC transac);	//参数需要修改

    int (*transmit_buf)(int fd, TRANSAC transac, int len);

    //int (*cmd_int)(int , void *, void *);

    void (*cleanup)( void );
	
	//直接命令控制，单函数，要不又要映射一层cmd opcode
	int	(*create)(int type, char *path, int port, int profile, int protocol, int *out_fd);		//open+bind
	int	(*destroy)(int);
	
	
} T_HciInfrtface;



extern const T_HciInfrtface *getHciInterface(void);

#ifdef __cplusplus
}
#endif
#endif
