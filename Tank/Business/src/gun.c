#include "gun.h"

GunContext Gun_Init(uint16_t arr, uint16_t psc)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;

	// ʱ��ʹ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	// ��ʼ��PC6��PC7Ϊ����ģ���ת��PWMԴ | PB1Ϊ����PWMԴ
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource6, GPIO_AF_TIM8);
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource7, GPIO_AF_TIM8);
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource8, GPIO_AF_TIM8);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8;
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
	TIM_TimeBaseInit(TIM8, &TIM_TimeBaseStructure);

	// ѡ��ʱ��ģʽ:TIM�����ȵ���ģʽ1
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	// �Ƚ����ʹ��
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	// �������:TIM����Ƚϼ��Ը�
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	// ����TIM_OCInitStruct��ָ���Ĳ�����ʼ������TIMx
	TIM_OC1Init(TIM8, &TIM_OCInitStructure);
	TIM_OC2Init(TIM8, &TIM_OCInitStructure);
	TIM_OC3Init(TIM8, &TIM_OCInitStructure);
	// CH1Ԥװ��ʹ��
	TIM_OC1PreloadConfig(TIM8, TIM_OCPreload_Enable);
	TIM_OC2PreloadConfig(TIM8, TIM_OCPreload_Enable);
	TIM_OC3PreloadConfig(TIM8, TIM_OCPreload_Enable);

	TIM_CtrlPWMOutputs(TIM8, ENABLE); // �߼���ʱ�������Ҫ�������
	// ʹ��TIMx��ARR�ϵ�Ԥװ�ؼĴ���
	TIM_ARRPreloadConfig(TIM8, ENABLE);

	// ʹ��TIM8
	TIM_Cmd(TIM8, ENABLE);

	return (GunContext){arr, 0, 90, 0};
}

void Gun_Update(void *context)
{
	GunContext *gun = (GunContext *)context;
	if (0 == gun)
		return;
	TIM_SetCompare1(TIM8, (uint32_t)(gun->ah / 360 * gun->arr));
	TIM_SetCompare2(TIM8, (uint32_t)(gun->av / 360 * gun->arr));
	TIM_SetCompare3(TIM8, (uint32_t)(gun->fire * gun->arr));
}
