#include "system.h"
#include "esp01s.h"

#ifndef __TCPSERVER_H
#define __TCPSERVER_H

typedef struct
{
    char ssid[32];
    char pwd[32];
    char ip[32];
    char mask[32];
    uint16_t port;
} TcpServerContext;

/// @brief ��ʼ��TCP������
/// @param ssid �ȵ�����
/// @param pwd �ȵ�����
/// @param ip ��������ַ
/// @param mask ��������
/// @param port �������˿�
TcpServerContext TcpServer_Init(const char *ssid, const char *pwd, const char *ip, const char *mask, uint16_t port);

/// @brief ����TCP������
void TcpServer_Build(void *context);

#endif
