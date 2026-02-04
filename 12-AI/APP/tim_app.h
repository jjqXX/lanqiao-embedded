#ifndef __TIM_APP_H__
#define __TIM_APP_H__

#include "bsp_system.h"

void pwm_set_duty(float Duty);
void pwm_set_frequency(int Frequency);
void ic_proc(void);
void pa7_pwm_init(void);
void pa7_pwm_control(void);

#endif
