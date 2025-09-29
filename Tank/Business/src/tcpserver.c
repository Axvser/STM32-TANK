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

// 修正后的TcpServer_Build函数（替换文档21中的实现）
void TcpServer_Build(void *context)
{
    printf("进入TCP建立\r\n");

    TcpServerContext *tcp = (TcpServerContext *)context;
    if (tcp == 0) return;

    printf("开始TCP建立\r\n");

    // 1. 恢复出厂设置
    ESP01S_RST();
    delay_ms(2000);
    
    // 2. 设置为AP模式
    ESP01S_CWMODE(2);
    delay_ms(1000);
    
    // 3. 配置AP参数
    ESP01S_CWSAP(tcp->ssid, tcp->pwd, 6, 3); // 通道6，WPA2_PSK加密
    delay_ms(1000);
    
    // 4. 启用DHCP服务器
    ESP01S_CWDHCP(1, 1); // 模式1(AP)，启用DHCP
    delay_ms(1000);
    
    // 5. 设置AP的IP地址（可选，但建议设置）
    ESP01S_CIPAP(tcp->ip, tcp->ip, tcp->mask);
    delay_ms(1000);
    
    // 6. 启用多连接
    ESP01S_CIPMUX(1);
    delay_ms(1000);
    
    // 7. 创建TCP服务器
    ESP01S_CIPSERVER(1, tcp->port); // 启用服务器，指定端口
    delay_ms(1000);
    
    // 8. 查询当前配置确认
    ESP01S_Send("AT+CIFSR\r\n"); // 查询IP地址
    delay_ms(500);
    ESP01S_Send("AT+CIPSERVER?\r\n"); // 查询服务器状态
    delay_ms(500);
}
