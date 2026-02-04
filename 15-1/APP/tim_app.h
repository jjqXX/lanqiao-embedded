#ifndef __TIM_APP_H__
#define __TIM_APP_H__

#include "bsp_system.h"
void pwm_set_duty_A(float Duty);
void pwm_set_duty_B(float Duty);
void ic_proc(void);
void pwm_set_frequency_A(int Frequency);
void pwm_set_frequency_B(int Frequency);

#endif
