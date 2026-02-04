#include "adc_app.h"

uint32_t dma_buff[2][30]; // DMA接收缓冲
float adc_value[2];       // ADC采样值数组
uint8_t step = 10;

void adc_proc(void)
{
    adc_value[0] = 0;
    adc_value[1] = 0;

    for (uint8_t i = 0; i < 30; i++)
    {
        adc_value[0] += (float)dma_buff[0][i];
        adc_value[1] += (float)dma_buff[1][i];
    }

    adc_value[0] = adc_value[0] / 30 * 3.3f / 4096;
    adc_value[1] = adc_value[1] / 30 * 3.3f / 4096;

    if (st_flag == 0)
    {
        for (uint8_t i = 0; i < step; i++)
        {
            if (adc_value[1] > i * (3.3 / step))
            {
                cd_value_change[0] = 10 + (i * para_value[0]);
                if (cd_value_change[0] >= 80)
                {
                    cd_value_change[0] = 80;
                }
            }
            if (adc_value[1] < 0.01f)
            {
                para_value[1] = 10;
            }
        }

        for (uint8_t i = 0; i < step; i++)
        {
            if (adc_value[0] > i * (3.3 / step))
            {
                cf_value_change[0] = 1000 + (i * para_value[2]);
                if (cf_value_change[0] >= 2000)
                {
                    cf_value_change[0] = 2000;
                }
            }
        }
    }
}
