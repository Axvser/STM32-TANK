#include "system.h"

#ifndef __ESP01S_H
#define __ESP01S_H

#define ESP01S_USART USART3
#define ESP01S_IRQChannel USART3_IRQn

#define ESP01S_DEBUG 1
#define ESP01S_DEBUG_USART USART1

/// @brief ESP01S�����ݴ���ص�
/// @param data ����ָ��
extern void ESP01S_DataHandler(uint16_t data);

/// @brief ��ESP01S���ʹ�����Ϣ
/// @param str ��Ϣ
void ESP01S_Send(const char *str);

/// @brief ��ʼ��ESP01Sģ��
/// @param baud ������
void ESP01S_Init(uint32_t baud);

/// @brief ��ʼ��ESP01S
void ESP01S_RST(void);

/// @brief ����ESP01S
void ESP01S_RESTORE(void);

/// @brief �趨ESP01S�Ĳ�����
/// @param baud ������
void ESP01S_UART_CUR(uint32_t baud);

/// @brief �趨ESP01S�Ĺ���ģʽ
/// @param mode 0 ��ʾStation
void ESP01S_CWMODE(uint8_t mode);

/// @brief �Ƿ���DHCP
/// @param operate 0 ��ʾ����
/// @param mode 0 ��ʾStation
void ESP01S_CWDHCP(uint8_t operate, uint8_t mode);

/// @brief ������ָ��WIFI
/// @param ssid WIFI����
/// @param pwd WIFI����
void ESP01S_CWJAP(const char *ssid, const char *pwd, const char *bssid, uint32_t timeout);

/// @brief ����AP
/// @param ip IP��ַ
/// @param gateway ����
/// @param netmask ��������
void ESP01S_CIPAP(const char *ip, const char *gateway, const char *netmask);

/// @brief ����͸��ģʽ
/// @param mode 0 ��ʾ�ر�
void ESP01S_CIPMODE(uint8_t mode);

/// @brief �Ƿ����������
/// @param allowMulti 0 ��ʾ������
void ESP01S_CIPMUX(uint8_t allowMulti);

/// @brief ���÷�����
/// @param mode 0/1 �ر�/����������
/// @param param2 mode0/mode1 �Ƿ�ر����пͻ���/�˿ں�
/// @param type ͨѶ���� [ TCP��TCPv6��SSL��SSLv6 ]
/// @param caEnable �Ƿ�����CA��֤
/// @param port ������Щ�˿� [ 0. ���� | 1. station | 2. AP | 3. ��̫�� ]
void ESP01S_CIPSERVER(uint8_t mode, uint8_t param2, const char *type, uint8_t caEnable, uint8_t port);

/// @brief �������ݵ�ָ������
/// @param linkID ����ID��0-4��
/// @param useTransparentMode �Ƿ�ʹ��͸��ģʽ
/// @param msg Ҫ���͵�����
void ESP01S_CIPSEND(uint8_t linkID, const char *msg);

#endif
