#ifndef __HASH_TBL_H__
#define __HASH_TBL_H__

#ifdef __cplusplus
extern "C" {
#endif


#define HASH_DATA_TYPE_INT


#ifdef HASH_DATA_TYPE_INT
	typedef int	KEY_TYPE;		//key类型：int
	extern unsigned int hash_linear_policy(KEY_TYPE key);
#else
	typedef char *	KEY_TYPE;		//key类型：char *
	extern unsigned int hash_nolinear_policy(KEY_TYPE key);
#endif
	



#define HASH_TABLE_SIZE_MAX		(10)		

//定义哈希表和基本数据节点
typedef struct _tag_node
{
    void *data;
    struct _tag_node *next;
}hash_node_t;

typedef struct _tag_hash_table
{
    hash_node_t *value[HASH_TABLE_SIZE_MAX];
}hash_table_t;

typedef int (*find_key)(void *, KEY_TYPE *);		//1.void *用户数据	2.out:找到的key
typedef unsigned int (*policy)(KEY_TYPE);					//由key产生hash表的下标   返回下标
typedef int (*check_value)(void *, KEY_TYPE);		//1.表里的数据	2.要验证的key


typedef struct {
	int				size;		//用户数据大小
	find_key		find;		//找出用户数据的key
	policy 			key2index;	//key转换数组下标的函数
	check_value		check;		//返回 1:相等，0：不相等
	
}hash_strategy_t;







extern hash_table_t *create_hash_table(hash_strategy_t *);
extern void destroy_hash_table(hash_table_t *);

extern hash_node_t *find_data_in_hash(hash_table_t *, KEY_TYPE);
extern int insert_data_into_hash(hash_table_t *, void *);
extern int delete_data_from_hash(hash_table_t *, KEY_TYPE);


#ifdef __cplusplus
}
#endif

#endif
