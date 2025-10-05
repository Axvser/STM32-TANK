#include "tcpserver.h"

TcpServerContext TcpServer_Init(const char *ssid, const char *pwd, const char *ip, const char *gw, const char *mask, uint16_t port)
{
    TcpServerContext context;
    strcpy(context.ssid, ssid);
    strcpy(context.pwd, pwd);
    strcpy(context.ip, ip);
    strcpy(context.gw, gw);
    strcpy(context.mask, mask);
    context.port = port;
    return context;
}

void TcpServer_Build_AP(void *context)
{
    TcpServerContext *tcp = (TcpServerContext *)context;
    if (tcp == 0)
        return;

    printf("[AP] Start TCP Service Building ...\r\n");

    // 1. �ָ���������
    ESP01S_RST();
    delay_ms(2000);

    // 2. ����ΪAPģʽ
    ESP01S_CWMODE(2);
    delay_ms(1000);

    // 3. ����AP����
    ESP01S_CWSAP(tcp->ssid, tcp->pwd, 6, 3); // ͨ��6��WPA2_PSK����
    delay_ms(1000);

    // 4. ����DHCP������
    ESP01S_CWDHCP(1, 1); // ģʽ1(AP)������DHCP
    delay_ms(1000);

    // 5. ����AP��IP��ַ����ѡ�����������ã�
    ESP01S_CIPAP(tcp->ip, tcp->ip, tcp->mask);
    delay_ms(1000);

    // 6. ���ö�����
    ESP01S_CIPMUX(1);
    delay_ms(1000);

    // 7. ����TCP������
    ESP01S_CIPSERVER(1, tcp->port); // ���÷�������ָ���˿�
    delay_ms(1000);

    // 8. ��ѯ��ǰ����ȷ��
    ESP01S_Send("AT+CIFSR\r\n"); // ��ѯIP��ַ
    delay_ms(500);
    ESP01S_Send("AT+CIPSERVER?\r\n"); // ��ѯ������״̬
    delay_ms(500);

    printf("[AP] TCP Service Builded ...\r\n");
}

void TcpServer_Build_STA(void *context)
{
    TcpServerContext *tcp = (TcpServerContext *)context;
    if (tcp == 0)
        return;

    printf("[STA] Start TCP Service Building ...\r\n");

    // 1. �ָ���������
    ESP01S_RST();
    delay_ms(2000);

    // 2. ����ΪSTAģʽ���ͻ���ģʽ��
    ESP01S_CWMODE(1); // �޸�ΪSTAģʽ
    delay_ms(1000);

    // 4. ����DHCP�ͻ��ˣ���·������ȡIP��
    ESP01S_CWDHCP(1, 1); // ģʽ0(STA)������DHCP
    delay_ms(1000);

    // 3. ���ӵ�ָ����WiFi����
    ESP01S_CWJAP(tcp->ssid, tcp->pwd); // ʹ��CWJAP����·����
    delay_ms(3000);                    // ������Ҫ����ʱ��

    // 5. �����Ҫ��̬IP��ʹ��CIPSTA������CIPAP
    ESP01S_CIPSTA(tcp->ip, tcp->gw, tcp->mask);
    delay_ms(1000);

    // 6. ���ö�����
    ESP01S_CIPMUX(1);
    delay_ms(1000);

    // 7. ����TCP������
    ESP01S_CIPSERVER(1, tcp->port); // ���÷�������ָ���˿�
    delay_ms(1000);

    // 8. ��ѯ��ǰ����ȷ��
    ESP01S_Send("AT+CIFSR\r\n"); // ��ѯIP��ַ����·������ȡ�ģ�
    delay_ms(500);
    ESP01S_Send("AT+CIPSERVER?\r\n"); // ��ѯ������״̬
    delay_ms(500);
    ESP01S_Send("AT+CWJAP?\r\n"); // ��ѯWiFi����״̬
    delay_ms(500);

    printf("[STA] TCP Service Builded ...\r\n");
}
