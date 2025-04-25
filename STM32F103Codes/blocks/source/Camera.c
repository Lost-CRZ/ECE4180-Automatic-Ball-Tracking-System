#include <stm32f10x.h>                 // Device header
#include "Camera.h"

#define MCU_TX_Pin GPIO_Pin_9
#define MCU_RX_Pin GPIO_Pin_10
/*
Main controller serial communication:
PA9 -> USART1 TX
PA10 -> USART1 RX
*/

#define Camera_TX_Pin GPIO_Pin_2
#define Camera_RX_Pin GPIO_Pin_3

/*
Camera serial communication:
PA2 -> USART2 TX 
PA3 -> USART2 RX
*/


extern char CameraData[12];
extern uint8_t MCU_Cmd;
	

void MCU_Serial_Init(void)
{
/***************************************************/
	//Enable clock module
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	
/***************************************************/
	//Configure GPIO
	GPIO_InitTypeDef GPIO_InitStruture;
	
	GPIO_InitStruture.GPIO_Mode=GPIO_Mode_AF_PP; //Select mode: Alternate function push-pull mode, serial module takes control of pin high/low level output
	GPIO_InitStruture.GPIO_Pin=MCU_TX_Pin; //Select pin
	GPIO_InitStruture.GPIO_Speed=GPIO_Speed_50MHz; //Select GPIO speed
	GPIO_Init(GPIOA,&GPIO_InitStruture);//Pass GPIOA parameters
	
	GPIO_InitStruture.GPIO_Mode=GPIO_Mode_IN_FLOATING; //Select mode: Floating input mode, pin control is taken over by corresponding serial port TX, note that TX and RX pin modes are different
	GPIO_InitStruture.GPIO_Pin=MCU_RX_Pin; //Select pin
	GPIO_InitStruture.GPIO_Speed=GPIO_Speed_50MHz; //Select GPIO speed
	GPIO_Init(GPIOA,&GPIO_InitStruture);//Pass GPIOA parameters
	
/***************************************************/
	//Configure serial module
	
	USART_InitTypeDef USART_InitStructure;
	USART_InitStructure.USART_BaudRate=115200;  //Baud rate
	USART_InitStructure.USART_HardwareFlowControl=USART_HardwareFlowControl_None; //Data flow setting
	USART_InitStructure.USART_Mode=USART_Mode_Rx|USART_Mode_Tx; //Serial mode setting: read or write
	USART_InitStructure.USART_Parity=USART_Parity_No; //Parity bit
	USART_InitStructure.USART_StopBits=USART_StopBits_1; //Stop bit width
	USART_InitStructure.USART_WordLength=USART_WordLength_8b; //Frame width: 8bits without parity bit, 9 bits with 1 parity bit
	USART_Init(USART1,&USART_InitStructure);
    
/***************************************************/
//	//Configure NVIC
//    USART_ITConfig(USART1,USART_IT_RXNE,ENABLE); //Enable serial port read interrupt
//	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //NVIC grouping method, only one grouping method allowed throughout the program
//	NVIC_InitTypeDef NVIC_InitStructure;
//	NVIC_InitStructure.NVIC_IRQChannel=USART1_IRQn;//Modified bit, configure NVIC receiving line
//	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE; //Enable
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0; //Configure preemption priority
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0;//Configure response priority
//	NVIC_Init(&NVIC_InitStructure);

/***************************************************/
	//Configure DMA: DMA+USART automatically receives large amount of data without occupying CPU resources
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE); //Don't put this clock enable at the very beginning
	USART_DMACmd(USART1,USART_DMAReq_Rx,ENABLE); //Serial read register non-empty trigger
	
	DMA_InitTypeDef DMA_InitStruct; 
	//USART1->DR points to USART1's DR register, & gets register address
	DMA_InitStruct.DMA_PeripheralBaseAddr=(uint32_t)&USART1->DR; //Peripheral base address, read register address
	DMA_InitStruct.DMA_PeripheralDataSize= DMA_PeripheralDataSize_Byte;//Peripheral data width
	DMA_InitStruct.DMA_PeripheralInc=DMA_PeripheralInc_Disable;//Whether peripheral pointer increments
	DMA_InitStruct.DMA_MemoryBaseAddr=(uint32_t)&MCU_Cmd; //Memory address
	DMA_InitStruct.DMA_MemoryDataSize=DMA_MemoryDataSize_Byte; //Memory data width
	DMA_InitStruct.DMA_MemoryInc=DMA_MemoryInc_Enable ;//Whether memory pointer increments
	DMA_InitStruct.DMA_DIR=DMA_DIR_PeripheralSRC  ;//Transfer direction SRC: peripheral as source, DIS: peripheral as destination
	DMA_InitStruct.DMA_BufferSize=1; //Transfer counter, transfers n times, decrements each time, stops at 0
	DMA_InitStruct.DMA_Mode=DMA_Mode_Circular;//DMA mode, whether to auto-reload
	DMA_InitStruct.DMA_M2M=DMA_M2M_Disable;//Software trigger or hardware trigger, 1 is software trigger
	DMA_InitStruct.DMA_Priority=DMA_Priority_Medium;//Priority
	
	
	DMA_Init(DMA1_Channel5,&DMA_InitStruct);//Each hardware trigger has its own corresponding channel number
	DMA_Cmd(DMA1_Channel5,ENABLE);
	
	USART_Cmd(USART1,ENABLE); //Enable serial port

}

void Camera_Serial_Init(void)
{
/***************************************************/
	//Enable clock module
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	
/***************************************************/
	//Configure GPIO
	GPIO_InitTypeDef GPIO_InitStruture;
	
	GPIO_InitStruture.GPIO_Mode=GPIO_Mode_AF_PP; //Select mode: Alternate function push-pull mode, serial module takes control of pin high/low level output
	GPIO_InitStruture.GPIO_Pin=Camera_TX_Pin; //Select pin
	GPIO_InitStruture.GPIO_Speed=GPIO_Speed_50MHz; //Select GPIO speed
	GPIO_Init(GPIOA,&GPIO_InitStruture);//Pass GPIOA parameters
	
	GPIO_InitStruture.GPIO_Mode=GPIO_Mode_IN_FLOATING; //Select mode: Floating input mode, pin control is taken over by corresponding serial port TX, note that TX and RX pin modes are different
	GPIO_InitStruture.GPIO_Pin=Camera_RX_Pin; //Select pin
	GPIO_InitStruture.GPIO_Speed=GPIO_Speed_50MHz; //Select GPIO speed
	GPIO_Init(GPIOA,&GPIO_InitStruture);//Pass GPIOA parameters
	
/***************************************************/
	//Configure serial module
	
	USART_InitTypeDef USART_InitStructure;
	USART_InitStructure.USART_BaudRate=115200;  //Baud rate
	USART_InitStructure.USART_HardwareFlowControl=USART_HardwareFlowControl_None; //Data flow setting
	USART_InitStructure.USART_Mode=USART_Mode_Rx|USART_Mode_Tx; //Serial mode setting: read or write
	USART_InitStructure.USART_Parity=USART_Parity_No; //Parity bit
	USART_InitStructure.USART_StopBits=USART_StopBits_1; //Stop bit width
	USART_InitStructure.USART_WordLength=USART_WordLength_8b; //Frame width: 8bits without parity bit, 9 bits with 1 parity bit
	USART_Init(USART2,&USART_InitStructure);
    
/***************************************************/
//	//Configure NVIC
//    USART_ITConfig(USART1,USART_IT_RXNE,ENABLE); //Enable serial port read interrupt
//	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //NVIC grouping method, only one grouping method allowed throughout the program
//	NVIC_InitTypeDef NVIC_InitStructure;
//	NVIC_InitStructure.NVIC_IRQChannel=USART1_IRQn;//Modified bit, configure NVIC receiving line
//	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE; //Enable
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0; //Configure preemption priority
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0;//Configure response priority
//	NVIC_Init(&NVIC_InitStructure);

/***************************************************/
	//Configure DMA: DMA+USART automatically receives large amount of data without occupying CPU resources
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE); //Don't put this clock enable at the very beginning
	USART_DMACmd(USART2,USART_DMAReq_Rx,ENABLE); //Serial read register non-empty trigger
	
	DMA_InitTypeDef DMA_InitStruct; 
	//USART1->DR points to USART1's DR register, & gets register address
	DMA_InitStruct.DMA_PeripheralBaseAddr=(uint32_t)&USART2->DR; //Peripheral base address, read register address
	DMA_InitStruct.DMA_PeripheralDataSize= DMA_PeripheralDataSize_Byte;//Peripheral data width
	DMA_InitStruct.DMA_PeripheralInc=DMA_PeripheralInc_Disable;//Whether peripheral pointer increments
	DMA_InitStruct.DMA_MemoryBaseAddr=(uint32_t)&CameraData; //Memory address
	DMA_InitStruct.DMA_MemoryDataSize=DMA_MemoryDataSize_Byte; //Memory data width
	DMA_InitStruct.DMA_MemoryInc=DMA_MemoryInc_Enable ;//Whether memory pointer increments
	DMA_InitStruct.DMA_DIR=DMA_DIR_PeripheralSRC  ;//Transfer direction SRC: peripheral as source, DIS: peripheral as destination
	DMA_InitStruct.DMA_BufferSize=12; //Transfer counter, transfers n times, decrements each time, stops at 0
	DMA_InitStruct.DMA_Mode=DMA_Mode_Circular;//DMA mode, whether to auto-reload
	DMA_InitStruct.DMA_M2M=DMA_M2M_Disable;//Software trigger or hardware trigger, 1 is software trigger
	DMA_InitStruct.DMA_Priority=DMA_Priority_Medium;//Priority
	
	
	DMA_Init(DMA1_Channel6,&DMA_InitStruct);//Each hardware trigger has its own corresponding channel number
	DMA_Cmd(DMA1_Channel6,ENABLE);
	
	USART_Cmd(USART2,ENABLE); //Enable serial port

}


void CameraData_Process(Openmv_DataSet_Typedef* Openmv_DataSet)
{

    //!!!!!!!!!!!!!!!1 Note that the variable order here needs to be verified
    Openmv_DataSet->Distance=*((float*)CameraData+0);
    Openmv_DataSet->Phi_Error=*((float*)CameraData+1);
    Openmv_DataSet->Theta_Error=*((float*)CameraData+2);
}







void Serial_SendByte(uint8_t Data) //Send one byte
{
	USART_SendData(USART1,Data); //Assign to DR register, transfer to send shift register for bit-by-bit transmission
	while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==RESET); 
	/*
	Check TX register empty signal. If not empty, FLAG=RESET means TX exists but has not entered the send shift register.
	Need to wait until FLAG=SET to ensure TX has entered the shift register for sending, preventing data from being overwritten before transmission.
	When TX is written, FLAG=RESET is automatically cleared by hardware.
	
	The logic here is: after writing to TX register, wait for it to shift to the shift register for sending (TXE=SET)
	Thus ensuring that the TX register is empty each time we write to it, and no data will be overwritten.
	*/
}

void Serial_SendArray(uint8_t *Array_Address,uint16_t Length)
{
	uint16_t i;
	
	for(i=0;i<Length;i++)
	{
		Serial_SendByte(*(Array_Address+i));
	}
}

void Serial_SendString(char* String_Address) //Pass a character type pointer, also string has its own terminator '\0'
{
	uint16_t i;
	
	for(i=0;String_Address[i]!='\0';i++) 
	{
		Serial_SendByte(*(String_Address+i));
	}
		
}

uint32_t Pow(uint32_t X,uint32_t Y)
{
	uint32_t Result=1;
	
	while(Y--)
	{
		Result=Result*X;
	}
	
	return Result;
		
}

void Serial_SendNum(uint32_t Number,uint8_t Length)
{
	uint8_t i;
	
	for(i=0;i<Length;i++)
	{
		Serial_SendByte(Number/Pow(10,Length-i-1)%10+'0');
	}
}

//int fputc(int ch,FILE* f)
//{
//	Serial_SendByte(ch); //Redirect printf output to serial port
//	return ch;
//}


//void Serial_Printf(char *format,...) //Variable parameter subfunction definition
//{
//char String[200];
//
//va_list arg;
//va_start(arg,format);
//vsprintf(String,format,arg);
//va_end(arg);
//Serial_SendString(String);
//
//}


void Serial_SendFloat(float* DataSet,uint8_t Length)
{
	  uint8_t Tem[4],i,j; //Send each byte of the floating point number's 4-byte memory sequentially

    for(i=0;i<Length;i++)
    {
        for(j=0;j<4;j++)
        {
            Tem[j]=*((uint8_t*)DataSet+4*i+j); //Convert Data float pointer to byte type and assign values to send sequentially
            Serial_SendByte(Tem[j]);
        }

    }
}
