/**
* FileName:   coonsole.h
* CreateDate: 2022-04-30 12:31:25
* Author:     ticks
* Email:      2938384958@qq.com
* Des:        控制台输出相关
*/
#ifndef _COONSOLE_H_
#define _COONSOLE_H_

#include <stdio.h>
#include <stdarg.h>

typedef struct Rgb
{
    unsigned int r:9;
    unsigned int g:9;
    unsigned int b:9;
}Rgb;

#define RGB(r, g, b)    ((Rgb){(r), (g), (b)})

static int print(const char* format, ... )
{
    va_list vl;
    vprintf(format, vl);
}

static int color_print(Rgb rgb, const char* format, ... )
{
    printf("\033[38;2;%u;%u;%um", rgb.r, rgb.g, rgb.b);
    va_list vl;
    vprintf(format, vl);
    printf("\033[0m");
}


#endif //_COONSOLE_H_
