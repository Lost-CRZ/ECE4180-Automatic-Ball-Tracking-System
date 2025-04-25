#include "PID.h"

//0. PID initialization function, assign values to each parameter
//Parameters (4): Kp, Ki, Kd, address of the PID structure to be processed
void PID_init(float Kp, float Ki, float Kd, PID_TypeDef* PID)
{
	PID->Kp = Kp;
	PID->Ki = Ki;
	PID->Kd = Kd;
	PID->p_out = 0.0;
	PID->i_out = 0.0; 
	PID->d_out = 0.0;
	PID->error = 0.0;
	PID->last_error = 0.0;
	PID->last_error2 = 0.0; 
	PID->last_error3 = 0.0;
	PID->last_error4 = 0.0;
	PID->last_error5 = 0.0;
	PID->output = 0.0;
}

//1. Position PID
//Parameters (3): current position, target position, address of the PID structure to be processed
float pid(float present, float target, PID_TypeDef* PID)
{
	static int32_t count=0;
	
	PID->error = target-present;	//Current error = target value - actual value
	
	PID->p_out = PID->Kp * PID->error;//Proportional
	PID->i_out += PID->Ki * PID->error;//Integral
	PID->d_out = PID->Kd * (PID->error - PID->last_error);//Derivative
	
	PID->output = PID->p_out + PID->i_out + PID->d_out;//Output
	
	PID->last_error = PID->error;//Last error = current error
	
	if(PID->output>10)
	{
		count++;
		if(count<40)
		{
			PID->output=10;
		}
		else
		{
			count=0;
		}

	}
	if(PID->output<-10) //Remove fleeting influences
	{
		count++;
		if(count<40)
		{
			PID->output=-10;
		}
		else
		{
			count=0;
		}
	}
	
	//Set upper and lower limits
	if(PID->output>12)
	{
		PID->output=0;
	}
		
	if(PID->output<-40)
	{
		PID->output=0;
	}
		
	return PID->output;
}


//2. Improved position PID (enhances the derivative term, considers historical information, reduces noise)
//Parameters (3): current position, target position, address of the PID structure to be processed
int better_PID(int present, u16 target, PID_TypeDef* PID)
{
	PID->error = target-present;	//Current error = target value - actual value
	
	PID->p_out = PID->Kp * PID->error;//Proportional
	PID->i_out += PID->Ki * PID->error;//Integral
	PID->d_out = PID->Kd * 1/16 * (PID->error + 3*PID->last_error + 2*PID->last_error2 -2*PID->last_error3 - 3*PID->last_error4 - PID->last_error5);//Derivative
	
	PID->output = PID->p_out + PID->i_out + PID->d_out;//Output
	
	PID->last_error5 = PID->last_error4;//Last error = current error
	PID->last_error4 = PID->last_error3;//Last error = current error
	PID->last_error3 = PID->last_error2;//Last error = current error
	PID->last_error2 = PID->last_error;//Last error = current error
	PID->last_error = PID->error;//Last error = current error
	
	return PID->output;
}


//3. Incremental PID (currently not working well)
//Parameters (3): current position, target position, address of the PID structure to be processed
int incre_PID(int present, u16 target, PID_TypeDef* PID)
{
	PID->error = target-present;	//Current error = target value - actual value
	
	PID->p_out = PID->Kp * (PID->error - PID->last_error);//Proportional
	PID->i_out += PID->Ki * PID->error;//Integral
	PID->d_out = PID->Kd * (PID->error - 2*PID->last_error + PID->last_error2);//Derivative
	
	PID->output += PID->p_out + PID->i_out + PID->d_out;//Output
	
	PID->last_error2 = PID->last_error;
	PID->last_error = PID->error;//Last error = current error
	
	return PID->output;
}
