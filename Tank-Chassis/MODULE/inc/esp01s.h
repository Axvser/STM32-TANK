#include "system.h"

#ifndef __ESP01S_H
#define __ESP01S_H

#define ESP01S_DEBUG 0

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

/// @brief ����STA�µ��豸��
/// @param name ����
void ESP01S_CWHOSTNAME(const char *name);

/// @brief �޸��豸mac
/// @param mac �豸mac
void ESP01S_CIPSTAMAC_CUR(const char *mac);

/// @brief �趨ESP01S�Ĳ�����
/// @param baud ������
void ESP01S_UART_CUR(uint32_t baud);

/// @brief �趨ESP01S�Ĺ���ģʽ
/// @param mode 1 ��ʾStation
void ESP01S_CWMODE(uint8_t mode);

/// @brief �����Ƿ�����DHCP
/// @param mode ģʽ (0:Station, 1:SoftAP)
/// @param enable �Ƿ����� (0:����, 1:����)
void ESP01S_CWDHCP(uint8_t mode, uint8_t enable);

/// @brief  ����ESP-01S��APģʽ������SSID������ȣ�
/// @param  ssid: ��Ҫ���õ�Wi-Fi����
/// @param  pwd: ���룬����8���ַ�
/// @param  channel: �ŵ���1-13��
/// @param  ecn: ���ܷ�ʽ (0:OPEN, 2:WPA_PSK, 3:WPA2_PSK, 4:WPA_WPA2_PSK)
void ESP01S_CWSAP(const char *ssid, const char *pwd, uint8_t channel, uint8_t ecn);

/// @brief ������ָ��WIFI
/// @param ssid WIFI����
/// @param pwd WIFI����
void ESP01S_CWJAP(const char *ssid, const char *pwd);

/// @brief ����STA
/// @param ip IP��ַ
void ESP01S_CIPSTA(const char *ip);

/// @brief ����AP
/// @param ip IP��ַ
void ESP01S_CIPAP(const char *ip);

/// @brief ����͸��ģʽ
/// @param mode 0 ��ʾ�ر�
void ESP01S_CIPMODE(uint8_t mode);

/// @brief �Ƿ����������
/// @param allowMulti 0 ��ʾ������
void ESP01S_CIPMUX(uint8_t allowMulti);

/// @brief ���÷�����
/// @param mode ģʽ
/// @param port �˿�
void ESP01S_CIPSERVER(uint8_t mode, uint16_t port);

/// @brief �������ݵ�ָ������
/// @param linkID ����ID��0-4��
/// @param useTransparentMode �Ƿ�ʹ��͸��ģʽ
/// @param msg Ҫ���͵�����
void ESP01S_CIPSEND(uint8_t linkID, const char *msg);

#endif
