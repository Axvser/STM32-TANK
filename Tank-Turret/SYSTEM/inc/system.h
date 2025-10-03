#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_usart.h"
#include "misc.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "bitband.h"
#include "debug.h"
#include "delay.h"
#include "string.h"
#include "stdio.h"
#include "math.h"

#ifndef __SYSTEM_H
#define __SYSTEM_H

#define BAUD_DEF 9600
#define BAUD_LOW 115200
#define BAUD_MIDDLE 460800
#define BAUD_FAST 921600

#endif
