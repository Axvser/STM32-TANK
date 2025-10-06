#include "motor.h"

MotorContext Motor_Init(uint16_t arr, uint16_t psc)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;

	// ʱ��ʹ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE | RCC_AHB1Periph_GPIOB, ENABLE);
	// ��ʼ��PE9��PE11ΪTB6612FNG��PWMԴ
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource9, GPIO_AF_TIM1);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource11, GPIO_AF_TIM1);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;	   // ����
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; // ����
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;	   // ����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;   // ��������
	GPIO_Init(GPIOE, &GPIO_InitStructure);
	// ��ʼ��PB10,PB11,PB12,PE15ΪTB6612FNG��ANx��BNx���
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15 | GPIO_Pin_12;
	GPIO_Init(GPIOE, &GPIO_InitStructure);
	PEout(12) = 1;

	// ��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ
	TIM_TimeBaseStructure.TIM_Period = arr;
	// ����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
	TIM_TimeBaseStructure.TIM_Prescaler = psc;
	// ����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	// ���ϼ���ģʽ
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	// ����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);

	// ѡ��ʱ��ģʽ:TIM�����ȵ���ģʽ1
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	// �Ƚ����ʹ��
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	// �������:TIM����Ƚϼ��Ը�
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	// ����TIM_OCInitStruct��ָ���Ĳ�����ʼ������TIMx
	TIM_OC1Init(TIM1, &TIM_OCInitStructure);
	TIM_OC2Init(TIM1, &TIM_OCInitStructure);
	// CH1Ԥװ��ʹ��
	TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);
	TIM_OC2PreloadConfig(TIM1, TIM_OCPreload_Enable);

	TIM_CtrlPWMOutputs(TIM1, ENABLE); // �߼���ʱ�������Ҫ�������
	// ʹ��TIMx��ARR�ϵ�Ԥװ�ؼĴ���
	TIM_ARRPreloadConfig(TIM1, ENABLE);

	// ʹ��TIM1
	TIM_Cmd(TIM1, ENABLE);

	return (MotorContext){arr, 0, 0};
}

void Motor_Update(void *context)
{
	MotorContext *motor = (MotorContext *)context;
	if (0 == motor)
		return;

	// ����Ĵ�
	if (motor->vl > 0)
	{
		TB6612FNG_AN1 = 1;
		TB6612FNG_AN2 = 0;
		TIM_SetCompare1(TIM1, (uint32_t)(motor->vl * motor->arr));
	}
	else if (motor->vl < 0)
	{
		TB6612FNG_AN1 = 0;
		TB6612FNG_AN2 = 1;
		TIM_SetCompare1(TIM1, (uint32_t)(-motor->vl * motor->arr));
	}
	else
	{
		TB6612FNG_AN1 = 0;
		TB6612FNG_AN2 = 0;
		TIM_SetCompare1(TIM1, 0);
	}

	// �Ҳ��Ĵ�
	if (motor->vr > 0)
	{
		TB6612FNG_BN1 = 1;
		TB6612FNG_BN2 = 0;
		TIM_SetCompare2(TIM1, (uint32_t)(motor->vr * motor->arr));
	}
	else if (motor->vr < 0)
	{
		TB6612FNG_BN1 = 0;
		TB6612FNG_BN2 = 1;
		TIM_SetCompare2(TIM1, (uint32_t)(-motor->vr * motor->arr));
	}
	else
	{
		TB6612FNG_BN1 = 0;
		TB6612FNG_BN2 = 0;
		TIM_SetCompare2(TIM1, 0);
	}
}

void Motor_RST(void *context)
{
	MotorContext *motor = (MotorContext *)context;
	if (0 == motor)
		return;

	motor->vl = 0;
	motor->vr = 0;
	Motor_Update(motor);
}
