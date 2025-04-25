#ifndef __ADC_H
#define __ADC_H
#include <stm32f10x.h>                   // Device header

void DMA1_ADC1_Init(uint32_t MemoAddress);
void ButtonDetectInit(void);
uint8_t CheckButton(void);

#endif
