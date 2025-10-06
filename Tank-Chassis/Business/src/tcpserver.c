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

    // 1. 恢复出厂设置
    ESP01S_RST();
    delay_ms(2000);

    // 2. 设置为AP模式
    ESP01S_CWMODE(2);
    delay_ms(1000);

    // 3. 配置AP参数
    ESP01S_CWSAP(tcp->ssid, tcp->pwd, 6, 3);
    delay_ms(1000);

    // 4. 关闭DHCP以允许IPV4静态地址
    ESP01S_CWDHCP(0, 0);
    delay_ms(1000);

    // 5. 设置AP的IP地址
    ESP01S_CIPAP(tcp->ip);
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

    printf("[AP] TCP Service Builded ...\r\n");
}

void TcpServer_Build_STA(void *context)
{
    TcpServerContext *tcp = (TcpServerContext *)context;
    if (tcp == 0)
        return;

    printf("[STA] Start TCP Service Building ...\r\n");

    // 1. 恢复出厂设置
    ESP01S_RST();
    delay_ms(2000);

    // 2. 修改设备名称
    ESP01S_CWHOSTNAME("TANK-CH");
    delay_ms(1000);

    // 3. 设置为STA模式
    ESP01S_CWMODE(1); // 修改为STA模式
    delay_ms(1000);

    // 4. 开启DHCP
    ESP01S_CWDHCP(1, 1);
    delay_ms(1000);

    // 5. 连接到指定的WiFi网络
    ESP01S_CWJAP(tcp->ssid, tcp->pwd);
    delay_ms(4000);

    // 6. 启用多连接
    ESP01S_CIPMUX(1);
    delay_ms(1000);

    // 7. 创建TCP服务器
    ESP01S_CIPSERVER(1, tcp->port); // 启用服务器，指定端口
    delay_ms(1000);

    printf("[STA] TCP Service Builded ...\r\n");
}
