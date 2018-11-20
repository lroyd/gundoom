/*************************************************************************
	> File Name: app_gen_cmd.c
	> Author: lroyd
	> Mail: 
	> Created Time: 
 ************************************************************************/
#include "app_gen_private.h"


/************************************************************************
* Name: 		AppNetConnLinkSocket
* Descriptions: 
* Param:		
* Note:
* **********************************************************************/
uint8_t AppNetConnLinkSocket(uint8_t *server_ip, uint8_t server_port)
{
	APP_GEN_LOG(LOG_TRACE, LOG_GEN_CMD, "ip = %s, port = %d", server_ip, server_port);
	
	MAKE_MESSAGE_AFTER_POWERON(APP_NET_CONN_SER_REQ);
	memcpy(msg->server_ip, server_ip, strlen(server_ip)+1);
	msg->server_port = server_port;

	sendMessage(getGenCmdHandle(), APP_NET_CONN_SER_REQ, msg);

	return 0;
}






