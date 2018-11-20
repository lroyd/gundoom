/*************************************************************************
	> File Name: pro_socket_cmd.c
	> Author: lroyd.H
	> Mail: 
	> Created Time: 2016年02月02日 星期二 13时16分51秒
 ************************************************************************/
#include "pro_socket_action.h"


uint8_t ProNetConnectServer(uint8_t *server_ip, uint8_t server_port)
{
	//socket open 
	
	printf("cao nima....... %s, %d\r\n", server_ip, server_port);
	
	ProNetConnectServerCfm(1);
	return 0;
}