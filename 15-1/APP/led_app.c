#include "led_app.h"

uint8_t ucLed[8];  // LED 状态数组

/**
 * @brief 显示或关闭Led
 *
 *
 * @param ucLed Led数据储存数组
 */
void led_disp(uint8_t *ucLed)
{
    // 用于记录当前 LED 状态的临时变量
    uint8_t temp = 0x00;
    // 记录之前 LED 状态的变量，用于判断是否需要更新显示
    static uint8_t temp_old = 0xff;

    for (int i = 0; i < 8; i++)
        // 将当前元素左移相应的位数，并累加到temp中
        temp |= (ucLed[i] << (i));

    // 仅当当前状态与之前状态不同的时候，才更新显示
    if (temp != temp_old)
    {
        // 将 GPIOC 低字节清零，高字节更新为新的 temp 值
        GPIOC->ODR &= 0x00ff;       // 清除 GPIOC 高字节
        GPIOC->ODR |= ~(temp << 8); // 设置 GPIOC 高字节为 temp 的相反值
        GPIOD->BSRR |= 0x01 << 2;   // 设置 GPIOD 第2位
        GPIOD->BRR |= 0x01 << 2;    // 重置 GPIOD 第2位
        temp_old = temp;            // 更新记录的旧状态
    }
}

uint32_t led_ticks = 0;

/**
 * @brief LED 显示处理函数
 *
 * 每次调用该函数时，LED 灯根据 ucLed 数组中的值来决定是开启还是关闭
 */
void led_proc(void)
{
    // 显示当前 Led_Pos 位置的 LED 灯状态
    led_disp(ucLed);

    if(st_index == 1)
    {
        ucLed[0] = 1;
    }
    else if(st_index == 2 && uwTick - led_ticks >= 200)
    {
        ucLed[0] ^= 1;
    }
    else 
    {
        ucLed[0] = 0;
    }

    ucLed[4] = freq_number == 1;
    ucLed[5] = freq_number == 2;
    ucLed[6] = freq_number == 3;
    ucLed[7] = freq_number == 4;
}
