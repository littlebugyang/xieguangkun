#pragma once
#ifndef __myLog_H_
#define __myLog_H_
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>

#include <iostream>
#include <ratio>
#include <chrono>
#include <mutex>

std::string GetName(char * fileName);

std::string getCurrentSystemTime();

long long getCurrentMs();
// 初始化路径
int LOGINIT(char *path);
// 结束时候调用
int LOGEnd();
// 设置最大日志文件，默认5M, mSize单位是字节
int LOGSetMaxSpace(int mSize);

int logWrite(char * s);

#define  LOGE(...)  {\
    char temp[8192] = {0};      \
    sprintf(temp+strlen(temp), __VA_ARGS__); \
    sprintf(temp + strlen(temp), "\n");    \
    printf("msg...");   \
    logWrite(temp); \
}
// printf("%s", temp);
//sprintf(temp, "%s", getCurrentSystemTime().c_str());  
//sprintf(temp + strlen(temp), " %s %d ", GetName(__FILE__).c_str(), __LINE__);  
#endif