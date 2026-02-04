#ifndef __TIM_APP_H__
#define __TIM_APP_H__

#include "bsp_system.h"
void pwm_set_duty(float Duty);
void ic_proc(void);
void pwm_set_frequency(int Frequency);
uint32_t Pwm_GetFreq(void);
uint32_t PWM_GetDuty(void);

#endif
