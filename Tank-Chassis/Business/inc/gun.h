#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_tim.h"
#include "bitband.h"

#ifndef __GUN_H
#define __GUN_H

/// @brief 攻击模块上下文
/// @param arr TIM自动重装值
/// @param ah 水平欧拉转角[0.360]
/// @param av 垂直欧拉转角[45,135]
/// @param fire 火力大小[0,1]
typedef struct
{
    uint16_t arr;
    double ah;
    double av;
    double fire;
} GunContext;

/// @brief 攻击模块初始化
/// @param arr 自动重装值
/// @param psc 时钟预分频数
/// @return 攻击模块上下文
GunContext Gun_Init(uint16_t arr, uint16_t psc);

/// @brief 更新攻击模块状态
/// @param context 攻击模块上下文
void Gun_Update(void *context);

#endif
