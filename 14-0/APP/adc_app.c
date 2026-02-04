#include "adc_app.h"

uint32_t dma_buff[2][30]; // DMA接收缓存
float adc_value[2];       // ADC采样值数组
uint32_t pwm_ticks;
uint32_t freq_val = 4000;

void adc_proc(void)
{
    for (uint8_t i = 0; i < 30; i++)
    {
        adc_value[0] += (float)dma_buff[0][i];
        adc_value[1] += (float)dma_buff[1][i];
    }

    adc_value[0] = adc_value[0] / 30 * 3.3f / 4096;
    adc_value[1] = adc_value[1] / 30 * 3.3f / 4096;

    if (key4_flag == 0)
    {
        if (adc_value[1] <= 1.0f)
        {
            p_value = 10;
        }
        else if (adc_value[1] >= 3.0f)
        {
            p_value = 85;
        }
        else
        {
            p_value = (35 * adc_value[1]) - 25;
        }
    }

    if(uwTick - pwm_ticks >= 100)
    {
        if(tim_ic_val != lcd_data_flag == 0?8000:4000)
        {
            tim_ic_val += (4000 / 50) * (lcd_data_flag == 0?1:-1);
            pwm_set_frequency(freq_val);
        }
        pwm_ticks = uwTick;
    }

    pwm_set_duty(p_value);
}
