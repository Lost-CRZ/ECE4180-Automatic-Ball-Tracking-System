#include <stm32f10x.h>             // Device header
#include "Servo.h"
#include "PWM.h"
#include "Camera.h"
#include "OLED.h"
#include "GPIOSection_Init.h"
#include "PID.h"
#include "stdlib.h"
#include "Timer.h"
#include "Filter.h"
#include "Delay.h"
#include "ADC.h"

#define Pan_Init 65
#define Tilt_Init 90
#define TRUE 1
#define FALSE 0



/* self-define functions area */
float Ave(const float *arr, int n);
void err_store(float *arr, int length, float err_new);

PID_TypeDef PID_Pan, PID_Tilt;

/*Smooth the data*/
long cnt_T = 0, cnt_F = 0;          // status counts                                                                                              
float ang_err_T_True[2][10] = {0.0};
/*0 for pan, 1 for tilt*/

/*Status flag*/
int blobs_detect =FALSE;
uint8_t blobs_detect_last=FALSE;
int PID_Switch = TRUE;
uint8_t Change_Flag=FALSE;
uint8_t mannual_cmd=TRUE;


/*Servo Control*/
float pan_ang=Pan_Init, tilt_ang=Tilt_Init;
float pan_ang_delta = 0, tilt_ang_delta = 0;
// float err_ang[2][3];

/*
tilt_ang_delta <- Phi_Error
pan_ang_delta <- Theta_Error
*/

/*Serial Communications*/
char CameraData[12];
uint8_t MCU_Cmd;
float Data2MCU[3];
Openmv_DataSet_Typedef Openmv_DataSet;

//Clock variable
uint64_t Real_Time,Real_Time_ms;

//Filter Variable
float Phi_Error_Store[2];
float Theta_Error_Store[2];
float Disatance_Store[2];

//Counting variable
uint16_t detect_count=0;
uint16_t lost_count=0;

//status flag
uint32_t Fail_Count=0;
uint8_t Validation_flag=0;

// Stick command data
float tilt_cmd;
float pan_cmd;
uint16_t ADC_RawData[2] = {0}; // first 4 is tilt, last 4 is pan
float cmd;


int main(void)
{

    uint8_t i,j;

    SystemCoreClockUpdate();

    Clock_Init();
    PWM_Init();                                                     
    /* PB8 -> SCL, PB9 -> SDA */
    OLED_Init();
    /*Initilize the servo angle*/
    Servo_SetAngle_Pan(Pan_Init);
    Servo_SetAngle_Tilt(Tilt_Init);
    Camera_Serial_Init();
    MCU_Serial_Init();
//  GPIOA_Init(GPIO_Pin_0,GPIO_Mode_Out_PP);
		//PID parameters
    PID_init(0.1,0.001,0.1,&PID_Pan);
    PID_init(0.2,0.003,0.1, &PID_Tilt);
	// ADC Init for receiving command
		DMA1_ADC1_Init((uint32_t)ADC_RawData);
	
		//GPIO Input read for external button
		ButtonDetectInit();
    OLED_Clear();
    OLED_ShowString(1, 1, "Pan_Angle:");
    OLED_ShowString(3, 1, "Tilt_Angle:");
        while(TRUE)
        {
					
					// Check mannual control or hand control
					if ( CheckButton() == 0 ){
							if(mannual_cmd == TRUE)
							{
								mannual_cmd = FALSE;
							}
							else{
								mannual_cmd = TRUE; 
							}
					}
					
					
      
			
			if(mannual_cmd){
				
//				AD_GetValue(ADC_RawData);
				
				//ADC_RawData[0] = Read_ADC_SingleChannel(0);
				//ADC_RawData[1] = Read_ADC_SingleChannel(1);
				
				// Tilt
				cmd = ( (float) ADC_RawData[0] / 4095 *3.3 - 0.5*3.3) / 3.3 * 5;
				if( (cmd<1) & (cmd>-1) ){}
				else{
					tilt_ang += cmd;
				}
				
				// Pan
				cmd = ( (float) ADC_RawData[1] / 4095 *3.3 - 0.5*3.3) / 3.3 * 5;
				if( (cmd<1) & (cmd>-1) ){}
				else{
					pan_ang += cmd;
				}
				
			
				OLED_ShowSignedNum(2,1,(int)pan_ang,4);
				OLED_ShowSignedNum(4,1,(int)tilt_ang,4);
				Servo_SetAngle_Pan(pan_ang);
				Servo_SetAngle_Tilt(tilt_ang);
				Delay_ms(100);//ensure servo has enough time to respond
				
			}
			else{
				

			// Check the parameters sent from camera
					CameraData_Process(&Openmv_DataSet);

					/* blobs_detect  judge*/

					//


					//Two-level judgment - first level checks if yellow appears, second level checks if pass

					//First check, determine whether yellow appears
					if((Openmv_DataSet.Distance==-1)||(Openmv_DataSet.Phi_Error==-181)||(Openmv_DataSet.Theta_Error == -181))
					{
						blobs_detect=FALSE;
						// GPIO_WriteBit(GPIOA,GPIO_Pin_0,Bit_RESET);
					}
					else
					{
							detect_count++;
							if(detect_count>800)
							{
									blobs_detect = TRUE;
									detect_count=0;
							}
							// GPIO_WriteBit(GPIOA,GPIO_Pin_0,Bit_RESET);
					}


					//Eliminate the effect of sudden change
					if(blobs_detect_last!=blobs_detect)//Detect whethere there is a ball
					{
							Change_Flag=1;
					}
					blobs_detect_last=blobs_detect; //Store current judge

					if(Change_Flag)
					{
							for(i=0;i<2;i++)
							{
									for(j=0;j<5;j++)
									{
											ang_err_T_True[i][j]=0;//When there is an another sudden change, erease counts
									}
							}
							Change_Flag=0;
					}


        //Check whether the data is valid or not
				if(blobs_detect){
						if((Openmv_DataSet.Distance==-2)||(Openmv_DataSet.Phi_Error==-200)||(Openmv_DataSet.Theta_Error == -200))
						{
								Validation_flag=0;
						}
						else
						{
								Validation_flag=1;
						}
				}
			
				if(blobs_detect&&Validation_flag)
				{
					cnt_F=0; // erase counting

					//Using low-pass filter to eliminate high-frequency noisy
					Phi_Error_Store[0]=Phi_Error_Store[1];
					Phi_Error_Store[1]=Openmv_DataSet.Phi_Error;

					Theta_Error_Store[0]=Theta_Error_Store[1];
					Theta_Error_Store[1]=Openmv_DataSet.Theta_Error;

					Openmv_DataSet.Phi_Error+=Low_Pass_Filter(Real_Time_ms, Real_Time,Phi_Error_Store[1]-Phi_Error_Store[0],0.05);
					Openmv_DataSet.Theta_Error+=Low_Pass_Filter(Real_Time_ms, Real_Time,Theta_Error_Store[1]-Theta_Error_Store[0],0.05);

					tilt_ang_delta = Openmv_DataSet.Phi_Error;
					pan_ang_delta = Openmv_DataSet.Theta_Error;

					err_store(ang_err_T_True[0],10, pan_ang_delta);
					err_store(ang_err_T_True[1],10, tilt_ang_delta);


					/*PID control or not*/
					if(PID_Switch)
					{
					
						pan_ang += pid(-Ave(ang_err_T_True[0],10),0,&PID_Pan);
						tilt_ang -= pid(-Ave(ang_err_T_True[1],10),0,&PID_Tilt);
					}
					else
					{
						pan_ang+=Ave(ang_err_T_True[0], 5);
						tilt_ang-=Ave(ang_err_T_True[1], 5);
					}

					{/* set limits */
						if(pan_ang>150)
						{
							pan_ang=150;

						}
						if(pan_ang<30)
						{
							pan_ang=30;

						}
						if(tilt_ang<30)
						{
							tilt_ang=30;

						}
						if(tilt_ang>180)
						{
							tilt_ang=180;

						}
					}

					// Change pan & tilt angle
					OLED_ShowSignedNum(2,1,(int)pan_ang,4);
					OLED_ShowSignedNum(4,1,(int)tilt_ang,4);
					Servo_SetAngle_Pan(pan_ang);
					Servo_SetAngle_Tilt(tilt_ang);
					Delay_ms(50);//Ensure enough time for response of servo

				}
				else
				{

					/*Using counting to determine whether to reset*/			
						cnt_F++;
						if(cnt_F==12000){
							cnt_F = 0;
							pan_ang_delta = 0;
							tilt_ang_delta = 0;
							pan_ang = Pan_Init;
							tilt_ang = Tilt_Init;
							OLED_ShowNum(2,1,pan_ang,4);
							OLED_ShowNum(4,1,tilt_ang,4);
							// Send command to servos
							Servo_SetAngle_Pan(pan_ang);
							Servo_SetAngle_Tilt(tilt_ang);
						}
				}
			}
           
     }

}


float Ave(const float *arr, int length)
{
    int i;
    float ave_list = 0;

    for(i = 0; i<length; i++)
    {
            ave_list += *(arr+i);
    }
    ave_list /= length;
    return ave_list;
}
void err_store(float *arr, int length, float err_new)
{
    int i;
    for(i=0; i<length;i++)
    {
        arr[i] = arr[i+1];
    }
    arr[i-1] = err_new;
}



//Real-time counting

void TIM1_UP_IRQHandler(void)
{
    if(TIM_GetFlagStatus(TIM1, TIM_FLAG_Update)==1)
    {
        Real_Time_ms+=1;
        if(Real_Time_ms==1000)
        {
            Real_Time+=1;


            Real_Time_ms=0;
        }
        TIM_ClearFlag(TIM1, TIM_FLAG_Update);
    }
}





