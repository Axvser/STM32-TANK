#ifndef _BITBAND_H
#define _BITBAND_H

#include "stm32f10x.h"

#define BITBAND(addr, bitnum) ((addr & 0xF0000000)+0x02000000+((addr & 0x00FFFFFF)<<5)+(bitnum<<2))

#define MEM_ADDR(addr) *((volatile uint32_t *)(addr))

#define BIT_ADDR(addr, bitnum) MEM_ADDR(BITBAND(addr, bitnum))

#define GPIOA_ODR_Addr    (GPIOA_BASE + 0x0C)  // F103��ODRƫ����0x0C
#define GPIOA_IDR_Addr    (GPIOA_BASE + 0x08)  // F103��IDRƫ����0x08

#define GPIOB_ODR_Addr    (GPIOB_BASE + 0x0C)
#define GPIOB_IDR_Addr    (GPIOB_BASE + 0x08)

#define GPIOC_ODR_Addr    (GPIOC_BASE + 0x0C)
#define GPIOC_IDR_Addr    (GPIOC_BASE + 0x08)

#define GPIOD_ODR_Addr    (GPIOD_BASE + 0x0C)
#define GPIOD_IDR_Addr    (GPIOD_BASE + 0x08)

#define GPIOE_ODR_Addr    (GPIOE_BASE + 0x0C)
#define GPIOE_IDR_Addr    (GPIOE_BASE + 0x08)

#define PAout(n)   BIT_ADDR(GPIOA_ODR_Addr, n)  // GPIOA���
#define PAin(n)    BIT_ADDR(GPIOA_IDR_Addr, n)  // GPIOA����

#define PBout(n)   BIT_ADDR(GPIOB_ODR_Addr, n)  // GPIOB���
#define PBin(n)    BIT_ADDR(GPIOB_IDR_Addr, n)  // GPIOB����

#define PCout(n)   BIT_ADDR(GPIOC_ODR_Addr, n)  // GPIOC���
#define PCin(n)    BIT_ADDR(GPIOC_IDR_Addr, n)  // GPIOC����

#define PDout(n)   BIT_ADDR(GPIOD_ODR_Addr, n)  // GPIOD���
#define PDin(n)    BIT_ADDR(GPIOD_IDR_Addr, n)  // GPIOD����

#define PEout(n)   BIT_ADDR(GPIOE_ODR_Addr, n)  // GPIOE���
#define PEin(n)    BIT_ADDR(GPIOE_IDR_Addr, n)  // GPIOE����

#endif
