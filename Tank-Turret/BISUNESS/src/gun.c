#include "gun.h"

GunContext Gun_Init(uint16_t arr, uint16_t psc)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_0; //不同IO口对应不同的通道！！！！！
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	TIM_InternalClockConfig(TIM2);
	
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStructure.TIM_Period = arr;		//ARR
	TIM_TimeBaseInitStructure.TIM_Prescaler = psc;		//PSC
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStructure);
	
	TIM_OCInitTypeDef TIM_OCInitStructure;
	TIM_OCStructInit(&TIM_OCInitStructure);
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 0;		//CCR
    TIM_OC1Init(TIM2, &TIM_OCInitStructure);//通道1
	TIM_OC2Init(TIM2, &TIM_OCInitStructure);//通道2
   
	
	TIM_Cmd(TIM2, ENABLE);

	return (GunContext){arr, 0, 90, 0};
}

void Gun_Update(void *context)
{
	GunContext *gun = (GunContext *)context;
	if (0 != gun)
	{
		TIM_SetCompare1(TIM2, (uint32_t)(gun->av / 360 * gun->arr));
		TIM_SetCompare2(TIM2, (uint32_t)(gun->fire * gun->arr));
	}
}
