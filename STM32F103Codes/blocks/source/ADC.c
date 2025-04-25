#include "stm32f10x.h"                  // Device header
#include "GPIOSection_Init.h"
#include "Delay.h"


// Tilt  Analog in PA0
// Pan Analog in PA1
// Button in PA2

void DMA1_ADC1_Init(uint32_t MemoAddress) //ADC compares 12-bit to data register 16-bit, register address is 32-bit
{
	
/*************************************************************************/
	//Enable peripheral clocks simultaneously
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,ENABLE);
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);//Enable ADC clock 72M/6=12MHz
	
/*************************************************************************/
	//Configure GPIO parameters
	GPIO_InitTypeDef GPIO_InitStruture;
	
	GPIO_InitStruture.GPIO_Mode=GPIO_Mode_AIN;//Select mode
	GPIO_InitStruture.GPIO_Pin=GPIO_Pin_0|GPIO_Pin_1; //Select pins
	GPIO_InitStruture.GPIO_Speed=GPIO_Speed_50MHz; //Select GPIO speed
	GPIO_Init(GPIOA,&GPIO_InitStruture);//Configure GPIO parameters
	
	
/*************************************************************************/
	//Configure DMA parameters
	DMA_InitTypeDef DMA_InitStruct;
	
	DMA_InitStruct.DMA_PeripheralBaseAddr=(uint32_t)&ADC1->DR; //Peripheral base address
	DMA_InitStruct.DMA_PeripheralDataSize=DMA_PeripheralDataSize_HalfWord;//Peripheral data width
	DMA_InitStruct.DMA_PeripheralInc=DMA_PeripheralInc_Disable;//Whether peripheral pointer increments
	DMA_InitStruct.DMA_MemoryBaseAddr=MemoAddress; //Memory address
	DMA_InitStruct.DMA_MemoryDataSize=DMA_MemoryDataSize_HalfWord; //Memory data width
	DMA_InitStruct.DMA_MemoryInc=DMA_MemoryInc_Enable ;//Whether memory pointer increments
	DMA_InitStruct.DMA_DIR=DMA_DIR_PeripheralSRC  ;//Transfer direction SRC: peripheral as source DIS: peripheral as destination
	DMA_InitStruct.DMA_BufferSize=2; //Transfer counter, transfers n times, decrements each time, stops at 0
	DMA_InitStruct.DMA_Mode=DMA_Mode_Circular;//DMA mode, whether to auto-reload
	DMA_InitStruct.DMA_M2M=DMA_M2M_Disable;//Software trigger or hardware trigger, 0 is hardware trigger, 1 is software trigger
	DMA_InitStruct.DMA_Priority=DMA_Priority_Medium;//Priority
	DMA_Init(DMA1_Channel1,&DMA_InitStruct);
	
	
		
/*************************************************************************/
	//Configure ADC parameters
	
	 //Specify parameters for each channel
	ADC_RegularChannelConfig(ADC1,ADC_Channel_0,1,ADC_SampleTime_239Cycles5);
	ADC_RegularChannelConfig(ADC1,ADC_Channel_1,2,ADC_SampleTime_239Cycles5); 

		
	ADC_InitTypeDef ADC_InitStruct;
	
	ADC_InitStruct.ADC_Mode=ADC_Mode_Independent; //Independent working mode
	ADC_InitStruct.ADC_ExternalTrigConv=ADC_ExternalTrigConv_None; //Software trigger mode
	ADC_InitStruct.ADC_DataAlign=ADC_DataAlign_Right; //Data right-aligned
	ADC_InitStruct.ADC_NbrOfChannel=2; //Number of conversion channels is 4
	ADC_InitStruct.ADC_ContinuousConvMode=ENABLE ; //Whether to trigger continuously
	ADC_InitStruct.ADC_ScanConvMode=ENABLE; //Whether to scan
	ADC_Init(ADC1,&ADC_InitStruct);
	
	ADC_DMACmd(ADC1,ENABLE); //Enable DMA data transfer


/*************************************************************************/
	//First enable DMA to wait, then enable ADC debugging
	
	DMA_Cmd(DMA1_Channel1,ENABLE);
	
	ADC_Cmd(ADC1,ENABLE);
	
	//Calibrate ADC
  ADC_ResetCalibration(ADC1);
  while(ADC_GetResetCalibrationStatus(ADC1)==SET); //Initially set to 1=SET, reset complete set to 0=RESET, Set means calibration complete, software sets to 1, hardware clears to 0
  ADC_StartCalibration(ADC1);
  while(ADC_GetCalibrationStatus(ADC1)==SET);

  ADC_SoftwareStartConvCmd(ADC1, ENABLE);//Finally software triggers ADC to implement ADC DMA automatic coordination for continuous loading of values
	
}


void ButtonDetectInit(void){
	GPIOA_Init(GPIO_Pin_2,GPIO_Mode_IPU); //Initialize Pin1 and Pin0 in B area with pull-up input mode
}

uint8_t CheckButton(void){
	
	if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_2)==0)
		{
			Delay_ms(50);
			while(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_2)==0);
			Delay_ms(50);
			return 0;
		}
	else{
			return 1;
		}
}



