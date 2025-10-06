#include "system.h"

#ifndef __ESP01S_H
#define __ESP01S_H

#define ESP01S_DEBUG 0

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

/// @brief 设置STA下的设备名
/// @param name 名字
void ESP01S_CWHOSTNAME(const char *name);

/// @brief 修改设备mac
/// @param mac 设备mac
void ESP01S_CIPSTAMAC_CUR(const char *mac);

/// @brief 设定ESP01S的波特率
/// @param baud 波特率
void ESP01S_UART_CUR(uint32_t baud);

/// @brief 设定ESP01S的工作模式
/// @param mode 1 表示Station
void ESP01S_CWMODE(uint8_t mode);

/// @brief 设置是否启用DHCP
/// @param mode 模式 (0:Station, 1:SoftAP)
/// @param enable 是否启用 (0:禁用, 1:启用)
void ESP01S_CWDHCP(uint8_t mode, uint8_t enable);

/// @brief  设置ESP-01S的AP模式参数（SSID、密码等）
/// @param  ssid: 想要设置的Wi-Fi名称
/// @param  pwd: 密码，至少8个字符
/// @param  channel: 信道（1-13）
/// @param  ecn: 加密方式 (0:OPEN, 2:WPA_PSK, 3:WPA2_PSK, 4:WPA_WPA2_PSK)
void ESP01S_CWSAP(const char *ssid, const char *pwd, uint8_t channel, uint8_t ecn);

/// @brief 连接至指定WIFI
/// @param ssid WIFI名称
/// @param pwd WIFI密码
void ESP01S_CWJAP(const char *ssid, const char *pwd);

/// @brief 配置STA
/// @param ip IP地址
void ESP01S_CIPSTA(const char *ip);

/// @brief 配置AP
/// @param ip IP地址
void ESP01S_CIPAP(const char *ip);

/// @brief 配置透传模式
/// @param mode 0 表示关闭
void ESP01S_CIPMODE(uint8_t mode);

/// @brief 是否允许多连接
/// @param allowMulti 0 表示单连接
void ESP01S_CIPMUX(uint8_t allowMulti);

/// @brief 配置服务器
/// @param mode 模式
/// @param port 端口
void ESP01S_CIPSERVER(uint8_t mode, uint16_t port);

/// @brief 发送数据到指定连接
/// @param linkID 连接ID（0-4）
/// @param useTransparentMode 是否使用透传模式
/// @param msg 要发送的数据
void ESP01S_CIPSEND(uint8_t linkID, const char *msg);

#endif
