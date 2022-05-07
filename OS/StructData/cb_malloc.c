/**
* FileName:   cb_malloc.c
* CreateDate: 2022-05-06 23:24:57
* Author:     ticks
* Email:      2938384958@qq.com
* Des:
*/
#include "cb_malloc.h"

// 分配内存
void* cb_malloc(int size)
{
    void* ptr = malloc(size);
    if (!ptr){
        perror("malloc: ");
        exit(1);
    }
    return ptr;
}

void* cb_zalloc(int size)
{
    void* ptr = malloc(size);
    if (!ptr){
        perror("malloc: ");
        exit(1);
    }
    memset(ptr, 0, size);
    return ptr;
}

void* cb_realloc(void* old_ptr, int new_size)
{
    void* ptr = realloc(old_ptr, new_size);
    if (!ptr){
        perror("malloc: ");
        exit(1);
    }
    return ptr;
}

// 释放内存
void cb_free(void** ptr)
{
    void* p = *ptr;
    if (p){
        free(p);
    }
    (*ptr) = NULL;
}