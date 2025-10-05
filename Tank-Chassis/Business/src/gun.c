#include "gun.h"

GunContext Gun_Init(uint16_t arr, uint16_t psc)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;

	// ʱ��ʹ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	// ��ʼ��PC6��PC7Ϊ����ģ���ת��PWMԴ | PC8Ϊ����PWMԴ
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource6, GPIO_AF_TIM3);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;	   // ����
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; // ����
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;	   // ����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;   // ��������
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	// ��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ
	TIM_TimeBaseStructure.TIM_Period = arr;
	// ����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
	TIM_TimeBaseStructure.TIM_Prescaler = psc;
	// ����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	// ���ϼ���ģʽ
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	// ����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

	// ѡ��ʱ��ģʽ:TIM�����ȵ���ģʽ1
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	// �Ƚ����ʹ��
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	// �������:TIM����Ƚϼ��Ը�
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	// ����TIM_OCInitStruct��ָ���Ĳ�����ʼ������TIMx
	TIM_OC1Init(TIM3, &TIM_OCInitStructure);
	// CH1Ԥװ��ʹ��
	TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);

	// TIM_CtrlPWMOutputs(TIM3, ENABLE); // �߼���ʱ�������Ҫ�������
	// ʹ��TIMx��ARR�ϵ�Ԥװ�ؼĴ���
	TIM_ARRPreloadConfig(TIM3, ENABLE);

	// ʹ��TIM3
	TIM_Cmd(TIM3, ENABLE);

	return (GunContext){arr, 0, 90, 0, 0};
}

void Gun_Update(void *context)
{
	GunContext *gun = (GunContext *)context;	
	if(0 != gun)
	{
		TIM_SetCompare1(TIM3, (uint32_t)(gun->ah / 360 * gun->arr));
	}
}
