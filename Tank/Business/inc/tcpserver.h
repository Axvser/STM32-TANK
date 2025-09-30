#include "system.h"
#include "esp01s.h"

#ifndef __TCPSERVER_H
#define __TCPSERVER_H

typedef struct
{
    char ssid[32];
    char pwd[32];
    char ip[32];
    char gw[32];
    char mask[32];
    uint16_t port;
} TcpServerContext;

/// @brief 初始化TCP服务器
/// @param ssid 热点名称
/// @param pwd 热点密码
/// @param ip 服务器地址
/// @param gw 网关
/// @param mask 子网掩码
/// @param port 服务器端口
TcpServerContext TcpServer_Init(const char *ssid, const char *pwd, const char *ip, const char *gw, const char *mask, uint16_t port);

/// @brief 建立TCP服务器于AP模式下
void TcpServer_Build_AP(void *context);

/// @brief 建立TCP服务器于STA模式下
void TcpServer_Build_STA(void *context);

#endif
