#include "debug.h"

#pragma import(__use_no_semihosting_swi)
struct __FILE
{
	int handle;
};
FILE __stdout;
FILE __stdin;
int fputc(int ch, FILE *f)
{
	while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
		;
	USART_SendData(USART1, ch);
	return ch;
}
void _sys_exit(int return_code)
{
label:
	goto label;
}
void _ttywrch(int ch)
{

}

void USART1_Debug_WriteLine(const char *str)
{
	const char *p = str;
	while (p && *p != '\0')
	{
		while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
			;
		USART_SendData(USART1, *p);
		p++;
	}
}

void USART1_Debug_Init(uint32_t baud)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	// 1. ����ʱ�ӣ�USART1��GPIOA����APB2�����ϣ�����ͬʱ����[4](@ref)
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);

	// 2. ����GPIO����
	// TX (PA9) ����Ϊ�����������
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	// RX (PA10) ����Ϊ��������[2,4](@ref)
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	// 3. ����USART��������
	USART_InitStructure.USART_BaudRate = baud; // ���� 115200
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART1, &USART_InitStructure);

	// 4. (��ѡ)�����ж�
	// ʹ�ܽ����ж�
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	// ����NVIC
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	// 5. ʹ��USART1
	USART_Cmd(USART1, ENABLE);
}

void USART1_IRQHandler(void)
{
	if (USART_GetITStatus(USART1, USART_IT_RXNE) == SET)
	{
		uint16_t tmp = USART_ReceiveData(USART1);
		while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
			;
		USART_SendData(USART1, tmp);
	}
}
