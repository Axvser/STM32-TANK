#include "tcpserver.h"

TcpServerContext TcpServer_Init(const char *ssid, const char *pwd, const char *ip, const char *mask, uint16_t port)
{
    TcpServerContext context;
    strcpy(context.ssid, ssid);
    strcpy(context.pwd, pwd);
    strcpy(context.ip, ip);
    strcpy(context.mask, mask);
    context.port = port;
    return context;
}

// �������TcpServer_Build�������滻�ĵ�21�е�ʵ�֣�
void TcpServer_Build(void *context)
{
    printf("����TCP����\r\n");

    TcpServerContext *tcp = (TcpServerContext *)context;
    if (tcp == 0) return;

    printf("��ʼTCP����\r\n");

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
}
