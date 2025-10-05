#include "gun.h"

GunContext Gun_Init(uint16_t arr, uint16_t psc)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;

	// 时钟使能
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	// 初始化PC6和PC7为攻击模块的转向PWM源 | PC8为开火PWM源
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource6, GPIO_AF_TIM3);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;	   // 复用
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; // 高速
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;	   // 推挽
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;   // 无上下拉
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	// 设置在下一个更新事件装入活动的自动重装载寄存器周期的值
	TIM_TimeBaseStructure.TIM_Period = arr;
	// 设置用来作为TIMx时钟频率除数的预分频值
	TIM_TimeBaseStructure.TIM_Prescaler = psc;
	// 设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	// 向上计数模式
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	// 根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

	// 选择定时器模式:TIM脉冲宽度调制模式1
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	// 比较输出使能
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	// 输出极性:TIM输出比较极性高
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	// 根据TIM_OCInitStruct中指定的参数初始化外设TIMx
	TIM_OC1Init(TIM3, &TIM_OCInitStructure);
	// CH1预装载使能
	TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);

	// TIM_CtrlPWMOutputs(TIM3, ENABLE); // 高级定时器输出需要设置这句
	// 使能TIMx在ARR上的预装载寄存器
	TIM_ARRPreloadConfig(TIM3, ENABLE);

	// 使能TIM3
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
