#include "delay.h"

void delay_ms(uint32_t ms)
{
	while(ms--)
	{
		SysTick->CTRL = 0; 			// �ر�ϵͳ��ʱ����������üĴ���
		SysTick->LOAD = 168000-1; 	// ���ü���ֵ���������ö�ʱ��ʱ��1ms
		SysTick->VAL = 0; 			// ��յ�ǰֵ���м�����־λ
		SysTick->CTRL =5; 			// ʹ��ϵͳ��ʱ����������ʱ��ԴΪϵͳʱ��168MHz
		while ((SysTick->CTRL & (1<<16))==0);	// �ȴ�ϵͳ��ʱ���������
		SysTick->CTRL = 0; 			// �ر�ϵͳ��ʱ��	
	}
}

void delay_us(uint32_t us)
{
	SysTick->CTRL = 0; 			// �ر�ϵͳ��ʱ����������üĴ���
	SysTick->LOAD = 168*us-1; 	// ���ü���ֵ���������ö�ʱ��ʱ��1us
	SysTick->VAL = 0; 			// ��յ�ǰֵ���м�����־λ
	SysTick->CTRL =5; 			// ʹ��ϵͳ��ʱ����������ʱ��ԴΪϵͳʱ��168MHz
	while ((SysTick->CTRL & (1<<16))==0);	// �ȴ�ϵͳ��ʱ���������
	SysTick->CTRL = 0; 			// �ر�ϵͳ��ʱ��
}
