/*******************************************************************************
	> File Name: 
	> Author: lroyd
	> Mail: htzhangxmu@163.com
	> Created Time: 
 *******************************************************************************/
#include <sys/prctl.h>
#include <time.h>
#include <signal.h>
#include <assert.h>
#include <pthread.h>

#include "common.h"
#include "hci_droid.h"
#include "abslay.h"
#include "utils.h"
#include "hci_debug.h"
#include "bta_hal.h"




#define USE_ABSL_LAY	(1)


typedef struct
{
    pthread_t       m_iTTid;
    pthread_mutex_t mutex;
    pthread_cond_t  cond;
    uint8_t         epilog_timer_created;
    timer_t         epilog_timer_id;	
} T_HciMangerInfo;

static T_HciMangerInfo g_tHciMangerInfo;
static volatile uint8_t lib_running = 0;
static volatile uint16_t ready_events = 0;
static volatile uint8_t tx_cmd_pkts_pending = FALSE;
/******************************************************************************
**  
******************************************************************************/
T_HciCallback *g_tHciCallbacks = NULL;
volatile uint8_t fwcfg_acked;

extern T_TransportOp vnd_callbacks;

void *device = NULL;	//唯一
BUFFER_Q		tx_q;
BUFFER_Q		rx_q;	//底层接收队列

/* Vendor epilog process timeout period  */
#ifndef EPILOG_TIMEOUT_MS
#define EPILOG_TIMEOUT_MS 3000  // 3 seconds
#endif

void bthc_signal_event(uint16_t event)
{
    pthread_mutex_lock(&g_tHciMangerInfo.mutex);
    ready_events |= event;
    pthread_cond_signal(&g_tHciMangerInfo.cond);
    pthread_mutex_unlock(&g_tHciMangerInfo.mutex);
}

static void epilog_wait_timeout(union sigval arg)
{
    HCI_LOG(LOG_INFO, "...epilog_wait_timeout...");
    bthc_signal_event(HC_EVENT_EXIT);
}

static void epilog_wait_timer(void)
{
	
    int status;
    struct itimerspec ts;
    struct sigevent se;
    uint32_t timeout_ms = EPILOG_TIMEOUT_MS;

    se.sigev_notify = SIGEV_THREAD;
    se.sigev_value.sival_ptr = &g_tHciMangerInfo.epilog_timer_id;
    se.sigev_notify_function = epilog_wait_timeout;
    se.sigev_notify_attributes = NULL;

    status = timer_create(CLOCK_MONOTONIC, &se, &g_tHciMangerInfo.epilog_timer_id);

    if (status == 0)
    {
        g_tHciMangerInfo.epilog_timer_created = 1;
        ts.it_value.tv_sec = timeout_ms/1000;
        ts.it_value.tv_nsec = 1000000*(timeout_ms%1000);
        ts.it_interval.tv_sec = 0;
        ts.it_interval.tv_nsec = 0;

        status = timer_settime(g_tHciMangerInfo.epilog_timer_id, 0, &ts, 0);
        if (status == -1)
            HCI_LOG(LOG_INFO, "Failed to fire epilog watchdog timer");
    }
    else
    {
        HCI_LOG(LOG_INFO, "Failed to create epilog watchdog timer");
        g_tHciMangerInfo.epilog_timer_created = 0;
    }

}


static void *hc_worker_thread(void *arg)
{
    uint16_t events;

    HCI_LOG(LOG_INFO, "hc_worker_thread started");

    tx_cmd_pkts_pending = FALSE;

    while (lib_running)
    {
        pthread_mutex_lock(&g_tHciMangerInfo.mutex);
        while (ready_events == 0)
        {
            pthread_cond_wait(&g_tHciMangerInfo.cond, &g_tHciMangerInfo.mutex);
        }
        events = ready_events;
        ready_events = 0;
        pthread_mutex_unlock(&g_tHciMangerInfo.mutex);

        if (events & HC_EVENT_RX)
        {
			HCI_LOG(LOG_INFO, "HC_EVENT_RX.........\r\n");
			
			//从队列里取出来发往对应任务
			HCI_HDR *p_hdr = utils_dequeue(0, &rx_q);
			if (p_hdr)
			{
				//有数据才回调
				g_tHciCallbacks->data_ind(p_hdr);
			}
        }

        if (events & HC_EVENT_PRELOAD)
        {

        }

        if (events & HC_EVENT_POSTLOAD)
        {

        }

        if (events & HC_EVENT_TX)
        {
			HCI_LOG(LOG_INFO, "HC_EVENT_TX.........\r\n");
			
        }

        if (events & HC_EVENT_EPILOG)
        {
			HCI_LOG(LOG_INFO, "HC_EVENT_EPILOG.........\r\n");
#if 0			
			//通知底层收尾
			if (bt_vnd_if)
                bt_vnd_if->op(BT_VND_OP_EPILOG, NULL);
            else
                break;  // equivalent to HC_EVENT_EXIT		
#endif			
        }

        if (events & HC_EVENT_EXIT)
            break;
    }

    HCI_LOG(LOG_INFO, "hc_worker_thread exiting");
    lib_running = 0;

    pthread_exit(NULL);

    return NULL;    // compiler friendly
}
/*****************************************************************************
**
**   HOST/CONTROLLER INTERFACE LIBRARY FUNCTIONS
**
*****************************************************************************/
static INT32 init(T_HciCallback* p_cb)
{
	INT32 ret = -1;
	HCI_LOG(LOG_INFO, "....int");

	fwcfg_acked = TRUE;//FALSE;
	
	//1.初始化队列锁
	utils_init(); 
	utils_queue_init(&tx_q);
	utils_queue_init(&rx_q);
	
	//2.挂在回调
	g_tHciCallbacks = p_cb;
	ret = AbslayCreate(&vnd_callbacks, &device);	//vnd_callbacks在hw_link.c中定义里面也用到hc_cbacks
	
	//3.创建线程
    pthread_attr_t thread_attr;
    struct sched_param param;
    int policy, result;
	
    lib_running = 1;
    ready_events = 0;
    pthread_mutex_init(&g_tHciMangerInfo.mutex, NULL);
    pthread_cond_init(&g_tHciMangerInfo.cond, NULL);
    pthread_attr_init(&thread_attr);

    if (pthread_create(&g_tHciMangerInfo.m_iTTid, &thread_attr, hc_worker_thread, NULL) != 0)
    {
        //ALOGE("pthread_create failed!");
        lib_running = 0;
        return -1;
    }

    if(pthread_getschedparam(g_tHciMangerInfo.m_iTTid, &policy, &param)==0)
    {
        policy = BTHC_LINUX_BASE_POLICY;
        result = pthread_setschedparam(g_tHciMangerInfo.m_iTTid, policy, &param);
        if (result != 0)
        {
            printf("libbt-hci init: pthread_setschedparam failed (%s)", strerror(result));
        }
    }	
	
	
	
    return ret;
}


static void set_power(INT32 state)
{
	HCI_LOG(LOG_INFO, "....set_power");

}

static void preload(INT32 fd, TRANSAC transac)
{
	HCI_LOG(LOG_INFO, "....preload");
	bthc_signal_event(HC_EVENT_PRELOAD);
}

/* 所有的底层要发送的数据包，都是由底层释放 */
static INT32 transmit_buf(INT32 fd, TRANSAC transac, int len)
{
	
	HCI_LOG(LOG_INFO, "transmit_buf fd = %d, len = %d, data = %s",fd, len, transac);
	
	//测试使用
#ifdef USE_ABSL_LAY	
	AbslayDevSend(fd, transac, len);
#endif	
	
	
	
    //utils_enqueue(&tx_q, (void *) transac);	//如队列必须有头

    //bthc_signal_event(HC_EVENT_TX);	
    return 0;
}

static void cleanup(void)
{
	HCI_LOG(LOG_INFO, "....cleanup");
    if (lib_running)
    {
        if (fwcfg_acked == TRUE)
        {
            epilog_wait_timer();
            bthc_signal_event(HC_EVENT_EPILOG);
        }
        else
        {
            /* add stop hci init for bcsp */
            //p_hci_if->cleanup(HCI_STOP_INIT);
            bthc_signal_event(HC_EVENT_EXIT);
        }
		
        pthread_join(g_tHciMangerInfo.m_iTTid, NULL);

        if (g_tHciMangerInfo.epilog_timer_created == 1)
        {
            timer_delete(g_tHciMangerInfo.epilog_timer_id);
            g_tHciMangerInfo.epilog_timer_created = 0;
        }
    }

    lib_running = 0;

	AbslayDestroy(device);
    utils_cleanup(0);
	fwcfg_acked = FALSE;
    g_tHciCallbacks = NULL;
}


static int create(int type, char *path, int port, int profile, int protocol, int *out_fd)
{
	HCI_LOG(LOG_INFO, "type = %d, path = %s, port = %d", type, path, port);
	//1.open
#ifdef USE_ABSL_LAY	
	AbslayDevOpen(device, type, path, port, out_fd);
#else	
	*out_fd = 5; //测试使用不使用absl框架
#endif
	int fd = *out_fd;
	
	printf("------- %d\r\n",fd);
	
	//2.set profile
	if ((fd != -1)) 
	{
		HCI_LOG(LOG_INFO, "fd = %d set profile 0x%.4x, protocol %d", fd, profile, protocol);	
#ifdef USE_ABSL_LAY	
		AbslaySetProfile(*out_fd, profile, protocol);
#endif		
	}
    return 0;
}

static int destroy(int fd)
{
	HCI_LOG(LOG_INFO, "fd = %d", fd);
#ifdef USE_ABSL_LAY	
	AbslayDevClose(fd);
#endif	
    return 0;
}


static const T_HciInfrtface HCLibInterface = {
    sizeof(T_HciInfrtface),
    init,
    set_power,
    preload,
    transmit_buf,
    //cmd_int,
    cleanup,
	
	create,
	destroy
};


/*******************************************************************************
**
** Function        bt_hc_get_interface
**
** Description     Caller calls this function to get API instance
**
** Returns         API table
**
*******************************************************************************/
const T_HciInfrtface *getHciInterface(void)
{
    return &HCLibInterface;
}
