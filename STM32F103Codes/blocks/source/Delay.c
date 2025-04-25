#include <stm32f10x.h>             // Device header

/**
  * @brief  Microsecond level delay
  * @param  xus Delay duration, range: 0~233015
  * @retval None
  */
void Delay_us(uint32_t xus)
{
	SysTick->LOAD = 72 * xus;				//Set timer reload value
	SysTick->VAL = 0x00;					//Clear current count value
	SysTick->CTRL = 0x00000005;				//Set clock source to HCLK, start timer
	while(!(SysTick->CTRL & 0x00010000));	//Wait for count to reach 0
	SysTick->CTRL = 0x00000004;				//Turn off timer
}

/**
  * @brief  Millisecond level delay
  * @param  xms Delay duration, range: 0~4294967295
  * @retval None
  */
void Delay_ms(uint32_t xms)
{
	while(xms--)
	{
		Delay_us(1000);
	}
}
 
/**
  * @brief  Second level delay
  * @param  xs Delay duration, range: 0~4294967295
  * @retval None
  */
void Delay_s(uint32_t xs)
{
	while(xs--)
	{
		Delay_ms(1000);
	}
}

