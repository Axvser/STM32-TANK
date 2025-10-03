#include "system.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_tim.h"
#include "tb6612fng.h"

#ifndef __MOTOR_H
#define __MOTOR_H

/// @brief ���������
/// @param arr TIM�Զ���װֵ
/// @param vl ����ٶ�[-1,1]
/// @param vr �Ҳ��ٶ�[-1,1]
typedef struct
{
    uint16_t arr;
    double vl;
    double vr;
} MotorContext;

/// @brief �����ʼ��
/// @param arr �Զ���װֵ
/// @param psc ʱ��Ԥ��Ƶ��
/// @return ���������
MotorContext Motor_Init(uint16_t arr, uint16_t psc);

/// @brief ���µ��״̬
/// @param context ���������
void Motor_Update(void *context);

#endif
