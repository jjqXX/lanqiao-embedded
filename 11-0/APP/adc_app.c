#include "adc_app.h"

uint32_t dma_buff[2][30];
float adc_value[2];


void adc_proc(void)
{
    adc_value[0] = 0;
    adc_value[1] = 0;

    for(uint8_t i=0;i<30;i++)
    {
        adc_value[0] += (float)dma_buff[0][i];
        adc_value[1] += (float)dma_buff[1][i];
    }

    adc_value[0] = adc_value[0] / 30 * 3.3f / 4096;
    adc_value[1] = adc_value[1] / 30 * 3.3f / 4096;

    if(lcd_data_flag == 0)
    {
        uint8_t temp = (uint8_t)(100 * adc_value[0] / 3.3f);
        if(temp >= 90)
        {
            temp = 90;
        }
        if(temp <= 10)
        {
            temp = 10;
        }

        duty[0] = duty[1] = temp;
    }

    if(adc_value[0] == 0)
    {
        HAL_GPIO_WritePin(GPIOA,GPIO_PIN_6,GPIO_PIN_RESET);
        HAL_GPIO_WritePin(GPIOA,GPIO_PIN_7,GPIO_PIN_RESET);
    }
    else if(adc_value[1] == 3.3f)
	{
        HAL_GPIO_WritePin(GPIOA,GPIO_PIN_6,GPIO_PIN_SET);
        HAL_GPIO_WritePin(GPIOA,GPIO_PIN_7,GPIO_PIN_SET);
    }
}

