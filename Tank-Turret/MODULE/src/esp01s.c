#include "esp01s.h"

void ESP01S_Send(const char *str)
{
    const char *p = str;
    while (p && *p != '\0')
    {
        while (USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET)
            ;
        USART_SendData(USART3, *p);
        p++;
    }
}

void USART3_IRQHandler(void)
{
    if (USART_GetITStatus(USART3, USART_IT_RXNE) == SET)
    {
        uint16_t tmp = USART_ReceiveData(USART3);
        ESP01S_DataHandler(tmp);
        while (USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET)
            ;
#if ESP01S_DEBUG
        USART_SendData(USART1, tmp);
#endif
    }
}

void ESP01S_Init(uint32_t baud)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    // 1. 开启时钟：特别注意！USART3在APB1，GPIOB在APB2，必须分开使能[7](@ref)
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE); // 先开GPIOB时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);                      // 再开USART3时钟[7](@ref)

    // 2. 配置GPIO引脚 (与USART1相同)
    // TX (PB10)
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    // RX (PB11)
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // 3. 配置USART工作参数 (与USART1相同)
    USART_InitStructure.USART_BaudRate = baud;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART3, &USART_InitStructure);

    // 4. (可选)配置中断
    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn; // 中断通道不同
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    // 5. 使能USART3
    USART_Cmd(USART3, ENABLE);
}

void ESP01S_RST(void)
{
    ESP01S_Send("AT+RST\r\n");
}

void ESP01S_RESTORE(void)
{
    ESP01S_Send("AT+RESTORE\r\n");
}

void ESP01S_UART_CUR(uint32_t baud)
{
    char buffer[64] = {'\0'};
    sprintf(buffer, "AT+UART_CUR=%d\r\n", baud);
    ESP01S_Send(buffer);
}

void ESP01S_CWMODE(uint8_t mode)
{
    char buffer[32] = {'\0'};
    sprintf(buffer, "AT+CWMODE=%d\r\n", mode);
    ESP01S_Send(buffer);
}

void ESP01S_CWDHCP(uint8_t mode, uint8_t enable)
{
    char buffer[32] = {'\0'};
    sprintf(buffer, "AT+CWDHCP=%d,%d\r\n", mode, enable);
    ESP01S_Send(buffer);
}

void ESP01S_CWSAP(const char *ssid, const char *pwd, uint8_t channel, uint8_t ecn)
{
    char buffer[128] = {'\0'};
    // 构造AT指令，例如：AT+CWSAP="My_ESP","12345678",5,3\r\n
    sprintf(buffer, "AT+CWSAP=\"%s\",\"%s\",%d,%d\r\n", ssid, pwd, channel, ecn);
    ESP01S_Send(buffer);
}

void ESP01S_CWJAP(const char *ssid, const char *pwd)
{
    char buffer[128] = {'\0'};
    sprintf(buffer, "AT+CWJAP=\"%s\",\"%s\"\r\n", ssid, pwd);
    ESP01S_Send(buffer);
}

void ESP01S_CIPSTA(const char *ip, const char *gateway, const char *netmask)
{
    char buffer[128] = {'\0'};
    sprintf(buffer, "AT+CIPSTA=\"%s\",\"%s\",\"%s\"\r\n", ip, gateway, netmask);
    ESP01S_Send(buffer);
}

void ESP01S_CIPAP(const char *ip, const char *gateway, const char *netmask)
{
    char buffer[128] = {'\0'};
    sprintf(buffer, "AT+CIPAP=\"%s\",\"%s\",\"%s\"\r\n", ip, gateway, netmask);
    ESP01S_Send(buffer);
}

void ESP01S_CIPMODE(uint8_t mode)
{
    char buffer[32] = {'\0'};
    sprintf(buffer, "AT+CIPMODE=%d\r\n", mode);
    ESP01S_Send(buffer);
}

void ESP01S_CIPMUX(uint8_t allowMulti)
{
    char buffer[32] = {'\0'};
    sprintf(buffer, "AT+CIPMUX=%d\r\n", allowMulti);
    ESP01S_Send(buffer);
}

void ESP01S_CIPSERVER(uint8_t mode, uint16_t port)
{
    char buffer[32] = {'\0'};
    if (mode == 1)
    {
        sprintf(buffer, "AT+CIPSERVER=%d,%d\r\n", mode, port);
    }
    else
    {
        sprintf(buffer, "AT+CIPSERVER=%d\r\n", mode);
    }
    ESP01S_Send(buffer);
}

void ESP01S_CIPSEND(uint8_t linkID, const char *msg)
{
    char buffer[32] = {'\0'};

    // 先发送AT+CIPSEND指令指定数据长度
    if (linkID == 0)
    {
        sprintf(buffer, "AT+CIPSEND=%d\r\n", strlen(msg));
    }
    else
    {
        sprintf(buffer, "AT+CIPSEND=%d,%d\r\n", linkID, strlen(msg));
    }

    ESP01S_Send(buffer);

    // 发送实际数据
    ESP01S_Send(msg);
}
