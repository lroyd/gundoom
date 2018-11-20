/*******************************************************************************
	> File Name: hash_fd.c
	> Author: lroyd
	> Mail: htzhangxmu@163.com
	> Created Time: 
 *******************************************************************************/
#include <stdio.h>
#include <string.h>
#include <fcntl.h>

#include "common.h"
#include "hash_fd.h"
#include "hash.h"

/************************************************************************
* 			LOG	Setting
************************************************************************/
#define LOG_TAG_STR			("HASH")
#define LOG_TAG_LEVE		LOG_TRACE

#ifndef GHSUA_CONSOLE_LOG	
	/* 格式：input(xxxxxx) ->[tag] xxxxxx */
	#define HASH_LOG(level, format, arg...)	if(level < LOG_TAG_LEVE){printf("[%s]"format"", LOG_TAG_STR, ##arg);printf("\r\n");}
#else
	#include "log_file.h"
	/*L0 格式：input(xxxxxx) ->[time][tag] xxxxxx */
	/*L1 格式：input(xxxxxx) ->[time][tag][func:line] xxxxxx */
	#define HASH_LOG(level, format, arg...)	if(level < LOG_TAG_LEVE){syslog_wrapper_L0(level, "[%s]"format"", LOG_TAG_STR, ##arg);}		
#endif
/************************************************************************
* 			Hash Table
************************************************************************/
typedef struct 
{
	int				usr_cnt;	//使用者计数
	//TPLock			mutex;		//锁
	hash_table_t	*absl_hash_table;
}hash_fd_info_t;
/************************************************************************
* 			
************************************************************************/
static hash_fd_info_t	hash_fd_info = {0};






/************************************************************************
* Name: 		
* Descriptions:
* Parameter:		
* Return:     
* **********************************************************************/
static int hash_linear_find_key(void *p_data, KEY_TYPE *out_key)
{
	T_HashData *usr_data = (T_HashData *)p_data;
	*(int *)out_key = usr_data->fd;
	return 0;
}

/************************************************************************
* Name: 		
* Descriptions:
* Parameter:		
* Return:     
* **********************************************************************/
static int hash_linear_check_value(void *p_data, KEY_TYPE key)
{
	T_HashData *table_data = (T_HashData *)p_data;
	if (table_data->fd == key)
	{
		return 1;
	}
	return 0;
}


hash_strategy_t hash_action = {
	sizeof(T_HashData),
    hash_linear_find_key,
	hash_linear_policy,
    hash_linear_check_value
};


/************************************************************************
* Name: 		
* Descriptions:
* Parameter:		
* Return:     
* **********************************************************************/
GH_DEF(int)
HashFdInit(void)
{
	hash_fd_info_t	*hash = &hash_fd_info;
	
	hash->usr_cnt++;
	if (!hash->absl_hash_table)
	{
		hash->absl_hash_table = create_hash_table(&hash_action);	//唯一创建
	}	
	
	return 0;
}


GH_DEF(int)
HashFdDeinit(void)
{
	hash_fd_info_t	*hash = &hash_fd_info;
	hash->usr_cnt--;
	//所有人都释放才能释放
	if (hash->usr_cnt <= 0)
	{
		if (hash->absl_hash_table)
		{
			destroy_hash_table(hash->absl_hash_table);
		}
	}
	
	return 0;
}


GH_DEF(int)
HashInsertData(void *p_data)
{
	hash_fd_info_t	*hash = &hash_fd_info;
	
	insert_data_into_hash(hash->absl_hash_table, p_data);	
	
	return 0;
}


GH_DEF(int)
HashDeleteData(int fd)
{
	hash_fd_info_t	*hash = &hash_fd_info;

	delete_data_from_hash(hash->absl_hash_table, fd);	
	return 0;
}


GH_DEF(int)
HashGetEntityInfo(int fd, void **out_entity)
{
	hash_fd_info_t	*hash = &hash_fd_info;
	
	hash_node_t *node = find_data_in_hash(hash->absl_hash_table, fd);
	if(!node)
	{
		//没有找到对应的fd
		printf("HashGetEntityInfo fd = %d, not found node\r\n", fd);
		return -1;		
	}
		
	if (out_entity)
	{
		*out_entity		= ((T_HashData *)node->data)->entity;	
	}
	return 0;
}








