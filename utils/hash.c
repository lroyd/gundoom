/*************************************************************************
	> File Name: hash_table.c
	> Author: 
	> Mail: 
	> Created Time: Thu 13 Sep 2018 01:03:35 AM PDT
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h> 
#include <string.h> 

#include "hash.h"




hash_strategy_t *p_hash_stgy = NULL;

/*******************************************************************************
* Name: 
* Descriptions:
* Parameter:	
* Return:	
* *****************************************************************************/
#ifdef HASH_DATA_TYPE_INT
unsigned int hash_linear_policy(KEY_TYPE key)
{
	return key;
}
#else
//！！字符串必须有结束符
unsigned int hash_nolinear_policy(KEY_TYPE key)
{
    unsigned int index = 0;
	char *str = key;
	
    while (*str) 
	{
        index = (index << 5) + index + *str++;
    }
	
    return index;
}	
#endif

/*******************************************************************************
* Name: 
* Descriptions:
* Parameter:	
* Return:	
* *****************************************************************************/
hash_table_t *create_hash_table(hash_strategy_t *p_hash)
{
    hash_table_t* p_table = (hash_table_t*)malloc(sizeof(hash_table_t));
	if (p_table && p_hash)
	{
		memset(p_table, 0, sizeof(hash_table_t));
	}
	else
	{
		return NULL;
	}
    
	//初始化哈希函数
	p_hash_stgy = p_hash;
	
    return p_table;
}


void destroy_hash_table(hash_table_t *p_hash)
{
	if (p_hash)
	{
		free(p_hash);
		p_hash_stgy = NULL;
	}
}
/*******************************************************************************
* Name: 
* Descriptions:
* Parameter:	
* Return:	
* *****************************************************************************/
hash_node_t *find_data_in_hash(hash_table_t* p_table, KEY_TYPE key)
{
    hash_node_t* p_node;
    if(!p_table)
	{
		return NULL;
	}

	//数组下标头地址
	if(!(p_node = p_table->value[p_hash_stgy->key2index(key) % HASH_TABLE_SIZE_MAX]))	
	{
		//头地址没找到
		//printf("0000000000000\r\n");
		return NULL;
	}
	//头地址开始遍历,子节点是否相等
    while(p_node)
	{
		if (p_hash_stgy->check(p_node->data, key))
		{
			//相等
			//printf("=========\r\n");
			return p_node;
		}
        p_node = p_node->next;
    }
	//printf("1111111111\r\n");
    return NULL;
}


/*******************************************************************************
* Name: 
* Descriptions:
* Parameter:	
* Return:	
* *****************************************************************************/
typedef struct _tag_data
{
	int fd;
	int type;
	char name[32];
}hash_data_t;	//测试使用


int insert_data_into_hash(hash_table_t *p_table, void *new_data)
{
    hash_node_t* p_node;
    if(!p_table)	
	{
		return -1;
	}
	
	//1.找出新数据的key
	KEY_TYPE key;
	p_hash_stgy->find(new_data, &key);
	
	int index = p_hash_stgy->key2index(key);
	//printf("insert %s, index = %d\r\n", key, index%HASH_TABLE_SIZE_MAX);
    if(!(p_table->value[index % HASH_TABLE_SIZE_MAX]))	
	{
		//没有对应的index，则头新插入
        p_node = (hash_node_t*)malloc(sizeof(hash_node_t) + p_hash_stgy->size);
		if (!p_node)
		{
			return -1;
		}
        memset(p_node, 0, sizeof(hash_node_t) + p_hash_stgy->size);
        //copy用户数据
		p_node->data = (char *)p_node + sizeof(hash_node_t);
		memcpy(p_node->data, (char *)new_data, p_hash_stgy->size);
		
		//printf("xxx node 0x%04x, %s\r\n", ((hash_data_t *)p_node->data)->name, ((hash_data_t *)p_node->data)->name);
		//插入头节点
		p_table->value[index % HASH_TABLE_SIZE_MAX] = p_node;
        return 0;
    }
	
	//头位置有数据，链表接入
	//1.是否已经插入过
    if(find_data_in_hash(p_table, key))
	{
		//已经存在,可忽略的错误
		return 0;
	}
	//没有find到数据
	//2.链表子插入
	p_node = p_table->value[index % HASH_TABLE_SIZE_MAX]; //头节点
    while(p_node->next)
	{
		p_node = p_node->next;
	}

    p_node->next = (hash_node_t*)malloc(sizeof(hash_node_t) + p_hash_stgy->size);
	if (!p_node->next)
	{
		return -1;
	}

    memset(p_node->next, 0, sizeof(hash_node_t) + p_hash_stgy->size);
	//copy用户数据
    p_node->next->data = (char *)p_node->next + sizeof(hash_node_t);
	memcpy(p_node->next->data, (char *)new_data, p_hash_stgy->size);
	
    return 0;
}


/*******************************************************************************
* Name: 
* Descriptions:
* Parameter:	
* Return:	
* *****************************************************************************/
int delete_data_from_hash(hash_table_t* p_table, KEY_TYPE key)
{
    hash_node_t* p_head = NULL;
    hash_node_t* p_node = NULL;
    if(!p_table)
	{
		return -1;
	}
	
	//1.是否存在
    if(!(p_node = find_data_in_hash(p_table, key)))
	{
		//不存在
		return -1;
	}

	//2.存在
	//2.1是否是链表头index
	int index = p_hash_stgy->key2index(key);
	if(p_node == p_table->value[index % HASH_TABLE_SIZE_MAX])
	{
		//是表头，直接释放
		p_table->value[index % HASH_TABLE_SIZE_MAX] = p_node->next; //将后面的子节点设置成头地址
        free(p_node);
        return 0;
    }
	//2.2不是链表头，遍历子节点
	p_head = p_table->value[index % HASH_TABLE_SIZE_MAX];
    while(p_node != p_head ->next)
	{
		p_head = p_head->next;
	}
    p_head->next = p_node->next;
	free(p_node);
	
	return 0;
}