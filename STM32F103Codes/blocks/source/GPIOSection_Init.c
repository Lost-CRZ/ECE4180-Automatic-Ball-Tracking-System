#include <stm32f10x.h>                  // Device header

void GPIOA_Init(uint16_t Pin,GPIOMode_TypeDef Mode) //Area, Pin, Mode
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE); //Select clock to enable
	
	GPIO_InitTypeDef GPIO_InitStruture;
	
	GPIO_InitStruture.GPIO_Mode=Mode; //Select mode
	GPIO_InitStruture.GPIO_Pin=Pin; //Select pin
	GPIO_InitStruture.GPIO_Speed=GPIO_Speed_50MHz; //Select GPIO speed
	
	GPIO_Init(GPIOA,&GPIO_InitStruture);//Enable GPIOA
	
}

void GPIOB_Init(uint16_t Pin,GPIOMode_TypeDef Mode)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE); //Select clock to enable
	
	GPIO_InitTypeDef GPIO_InitStruture;
	
	GPIO_InitStruture.GPIO_Mode=Mode; //Select mode
	GPIO_InitStruture.GPIO_Pin=Pin; //Select pin
	GPIO_InitStruture.GPIO_Speed=GPIO_Speed_50MHz; //Select GPIO speed
	
	GPIO_Init(GPIOB,&GPIO_InitStruture);//Enable GPIOA
}

void GPIOC_Init(uint16_t Pin,GPIOMode_TypeDef Mode)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE); //Select clock to enable
	
	GPIO_InitTypeDef GPIO_InitStruture;
	
	GPIO_InitStruture.GPIO_Mode=Mode; //Select mode
	GPIO_InitStruture.GPIO_Pin=Pin; //Select pin
	GPIO_InitStruture.GPIO_Speed=GPIO_Speed_50MHz; //Select GPIO speed
	
	GPIO_Init(GPIOC,&GPIO_InitStruture);//Enable GPIOA
}

