#include "adc_app.h"

uint32_t dma_buff[2][30];//DMA接收缓存
float adc_value[2];//ADC采样值数组


void adc_proc(void)
{
    for(uint8_t i=0;i<30;i++)
    {
        adc_value[0] += (float)dma_buff[0][i];
        adc_value[1] += (float)dma_buff[1][i];
    }

    adc_value[0] = adc_value[0] / 30 * 3.3f / 4096;
    adc_value[1] = adc_value[1] / 30 * 3.3f / 4096;

    if(freq_number == 1)
    {
        freq_A = 1000;
        freq_B = 1000;
    }
    else if(freq_number == 2)
    {
        freq_A = 4000;
        freq_B = 1000; 
    }
    else if(freq_number == 3)
    {
        freq_A = 1000;
        freq_B = 4000;
    }
    else if(freq_number == 4)
    {
        freq_A = 4000;
        freq_B = 4000;
    }

    if(adc_value[1] < 0.5f)
    {
        duty_A = 10;
        duty_B = 5;
    }
    else if(0.5f <= adc_value[1] && adc_value[1] < 1.0f)
    {
        duty_A = 30;
        duty_B = 25;
    }
    else if(1.0f <= adc_value[1] && adc_value[1] < 1.5f)
    {
        duty_A = 50;
        duty_B = 45;
    }
    else if(1.5f <= adc_value[1] && adc_value[1] < 2.0f)
    {
        duty_A = 70;
        duty_B = 65;
    }
    else if(2.0f <= adc_value[1] && adc_value[1] < 2.5f)
    {
        duty_A = 90;
        duty_B = 85;
    }
    else if(adc_value[1] >= 2.5f)
    {
        duty_A = 95;
        duty_B = 90;
    }
}

