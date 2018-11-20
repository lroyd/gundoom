/*************************************************************************
	> File Name: hash_fd.h
	> Author: lroyd
	> Mail: htzhangxmu@163.com
	> Created Time: 
 ************************************************************************/
#ifndef __HASH_FD_H__
#define __HASH_FD_H__



typedef struct _tag_fd
{
	int fd;		//设备打开的df
	//void *device;	//device_info_t 实例(malloc)
	void *entity;	//底层实体，里面有个fd，重复(malloc)
}T_HashData;


GH_DECL(int) HashFdInit(void);
GH_DECL(int) HashFdDeinit(void);
GH_DECL(int) HashInsertData(void *);
GH_DECL(int) HashDeleteData(int);
GH_DECL(int) HashGetEntityInfo(int , void **);


#endif


