#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_usart.h"
#include "stdio.h"
#include "string.h"

#ifndef __DEBUG_H
#define __DEBUG_H

void USART1_Debug_Init(uint32_t baud);

void USART1_Debug_WriteLine(const char *str);

#endif
