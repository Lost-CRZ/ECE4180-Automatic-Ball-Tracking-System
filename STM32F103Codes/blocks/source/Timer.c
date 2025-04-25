#include <stm32f10x.h>             // Device header

void Clock_Init()
{
/************************************************************/
    /*Select clock signal source*/
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1,ENABLE); //Enable APB1 clock

    TIM_InternalClockConfig(TIM1); //Enable internal clock 2


/************************************************************/
    /*Configure basic parameters of timer unit*/
    TIM_TimeBaseInitTypeDef TimeBaseInit;

    TimeBaseInit.TIM_ClockDivision=TIM_CKD_DIV1; //Determine sampling frequency, perform signal filtering
    TimeBaseInit.TIM_CounterMode= TIM_CounterMode_Up; //Determine counting mode
    TimeBaseInit.TIM_Period=10-1; //Reload value ARR
    TimeBaseInit.TIM_Prescaler=7200-1; //Prescaler value PSC
    TimeBaseInit.TIM_RepetitionCounter=0; //Define how many overflows before interrupt
    TIM_TimeBaseInit(TIM1,&TimeBaseInit);

/************************************************************/
    /*Configure interrupt control*/
    TIM_ITConfig(TIM1,TIM_IT_Update,ENABLE);

/************************************************************/
    /*Configure NVIC*/
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

    NVIC_InitTypeDef NVIC_InitStruct;
    NVIC_InitStruct.NVIC_IRQChannel=TIM1_UP_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=1;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority=1;

    NVIC_Init(&NVIC_InitStruct);

/************************************************************/
    /*Start timer*/
    TIM_ClearFlag(TIM1, TIM_FLAG_Update);
    TIM_Cmd(TIM1,ENABLE);
}
