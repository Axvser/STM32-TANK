#include "system.h"

#ifndef __ESP01S_H
#define __ESP01S_H

#define ESP01S_USART USART3
#define ESP01S_IRQChannel USART3_IRQn

#define ESP01S_DEBUG 1
#define ESP01S_DEBUG_USART USART1

/// @brief ESP01S的数据处理回调
/// @param data 数据指针
extern void ESP01S_DataHandler(uint16_t data);

/// @brief 向ESP01S发送串口消息
/// @param str 消息
void ESP01S_Send(const char *str);

/// @brief 初始化ESP01S模块
/// @param baud 波特率
void ESP01S_Init(uint32_t baud);

/// @brief 初始化ESP01S
void ESP01S_RST(void);

/// @brief 重置ESP01S
void ESP01S_RESTORE(void);

/// @brief 设定ESP01S的波特率
/// @param baud 波特率
void ESP01S_UART_CUR(uint32_t baud);

/// @brief 设定ESP01S的工作模式
/// @param mode 0 表示Station
void ESP01S_CWMODE(uint8_t mode);

/// @brief 是否开启DHCP
/// @param operate 0 表示禁用
/// @param mode 0 表示Station
void ESP01S_CWDHCP(uint8_t operate, uint8_t mode);

/// @brief 连接至指定WIFI
/// @param ssid WIFI名称
/// @param pwd WIFI密码
void ESP01S_CWJAP(const char *ssid, const char *pwd, const char *bssid, uint32_t timeout);

/// @brief 配置AP
/// @param ip IP地址
/// @param gateway 网关
/// @param netmask 子网掩码
void ESP01S_CIPAP(const char *ip, const char *gateway, const char *netmask);

/// @brief 配置透传模式
/// @param mode 0 表示关闭
void ESP01S_CIPMODE(uint8_t mode);

/// @brief 是否允许多连接
/// @param allowMulti 0 表示单连接
void ESP01S_CIPMUX(uint8_t allowMulti);

/// @brief 配置服务器
/// @param mode 0/1 关闭/建立服务器
/// @param param2 mode0/mode1 是否关闭所有客户端/端口号
/// @param type 通讯类型 [ TCP，TCPv6，SSL，SSLv6 ]
/// @param caEnable 是否启用CA认证
/// @param port 监听哪些端口 [ 0. 所有 | 1. station | 2. AP | 3. 以太网 ]
void ESP01S_CIPSERVER(uint8_t mode, uint8_t param2, const char *type, uint8_t caEnable, uint8_t port);

/// @brief 发送数据到指定连接
/// @param linkID 连接ID（0-4）
/// @param useTransparentMode 是否使用透传模式
/// @param msg 要发送的数据
void ESP01S_CIPSEND(uint8_t linkID, const char *msg);

#endif
