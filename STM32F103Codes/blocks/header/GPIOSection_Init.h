#ifndef _GPIOSection_Init_
#define _GPIOSection_Init_
#include <stm32f10x.h>
void GPIOA_Init(uint16_t Pin,GPIOMode_TypeDef Mode);
void GPIOB_Init(uint16_t Pin,GPIOMode_TypeDef Mode);
void GPIOC_Init(uint16_t Pin,GPIOMode_TypeDef Mode);
#endif
