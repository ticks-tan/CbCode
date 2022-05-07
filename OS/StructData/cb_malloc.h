/**
* FileName:   cb_malloc.h
* CreateDate: 2022-05-06 23:16:55
* Author:     ticks
* Email:      2938384958@qq.com
* Des:        内存管理
*/
#include <malloc.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#ifndef _CB_MALLOC_H_
#define _CB_MALLOC_H_

// 获取变量地址
#define ADDRESS(p) (&(p))

// 分配内存
void* cb_malloc(int size);
void* cb_zalloc(int size);
void* cb_realloc(void* old_ptr, int new_size);
// 释放内存
void cb_free(void** ptr);

#define NEW(type, size) ((type)cb_zalloc(size))
#define DELETE(p)   cb_free(ADDRESS(p))

#endif //_CB_MALLOC_H_
