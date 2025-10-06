#include "stm32f4xx.h"
#include "string.h"
#include "stdarg.h"

#ifndef __CONVERTER_H
#define __CONVERTER_H

#define DEBUG_CONVERTER 0

extern void ProcessTankValues(double values[], uint8_t count);

double StrToDouble(const char *str);

void ParseTankData(uint16_t data);

#endif
