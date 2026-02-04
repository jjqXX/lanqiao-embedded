#include "led_app.h"
#include "parking_app.h"

uint8_t ucLed[8];  // LED状态数组

/**
 * @brief LED显示函数
 *
 * @param ucLed LED数据存储数组
 */
void led_disp(uint8_t *ucLed)
{
    uint8_t temp = 0x00;
    static uint8_t temp_old = 0xff;

    for (int i = 0; i < 8; i++)
        temp |= (ucLed[i] << (7 - i));

    if (temp != temp_old)
    {
        GPIOC->ODR &= 0x00ff;
        GPIOC->ODR |= ~(temp << 8);
        GPIOD->BSRR |= 0x01 << 2;
        GPIOD->BRR |= 0x01 << 2;
        temp_old = temp;
    }
}

/**
 * @brief LED处理函数
 *
 * LED指示功能:
 * LD1: 若停车场内存在空闲车位，指示灯点亮，否则熄灭
 * LD2: PA7输出2KHz,20%占空比脉冲信号期间，指示灯点亮，否则熄灭
 */
void led_proc(void)
{
    // LD1: 空闲车位指示
    // 有空闲车位时点亮(ucLed[0]=1)，否则熄灭(ucLed[0]=0)
    if (idle_count > 0)
        ucLed[0] = 1;
    else
        ucLed[0] = 0;

    // LD2: PA7 PWM输出状态指示
    // PWM输出时点亮(ucLed[1]=1)，低电平时熄灭(ucLed[1]=0)
    if (pa7_pwm_enable)
        ucLed[1] = 1;
    else
        ucLed[1] = 0;

    // 显示LED状态
    led_disp(ucLed);
}
