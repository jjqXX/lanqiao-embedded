#include "key_app.h"

uint8_t key_val = 0;  // 当前按键状态
uint8_t key_old = 0;  // 前一次按键状态
uint8_t key_down = 0; // 按下的按键
uint8_t key_up = 0;   // 释放的按键

/**
 * @brief 读取按键状态
 *
 * 该函数读取多个 GPIO 引脚上的按键状态，并返回相应的按键编号。
 *
 * @return 返回按键编号。0 表示没有按键按下，1-4 表示对应的按键被按下。
 */
uint8_t key_read(void)
{
    // 用于存储按键状态的临时变量
    uint8_t temp = 0;

    // 检测 GPIOB 引脚 0 的状态
    if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_0) == GPIO_PIN_RESET)
        temp = 1; // 如果引脚状态为 RESET，则按键 1 被按下

    // 检测 GPIOB 引脚 1 的状态
    if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_1) == GPIO_PIN_RESET)
        temp = 2; // 如果引脚状态为 RESET，则按键 2 被按下

    // 检测 GPIOB 引脚 2 的状态
    if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_2) == GPIO_PIN_RESET)
        temp = 3; // 如果引脚状态为 RESET，则按键 3 被按下

    // 检测 GPIOA 引脚 0 的状态
    if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0) == GPIO_PIN_RESET)
        temp = 4; // 如果引脚状态为 RESET，则按键 4 被按下

    // 返回检测到的按键编号
    return temp;
}

uint8_t set_index = 0;

uint32_t key2_ticks = 0;

/**
 * @brief 按键处理函数
 *
 * 该函数用于扫描按键的状态，并设置按下和释放的标志
 */
void key_proc(void)
{
    // 读取当前按键状态
    key_val = key_read();
    // 计算按下的按键（当前按键状态和前一状态异或，并与当前状态与运算）
    key_down = key_val & (key_old ^ key_val);
    // 计算释放的按键（当前未按下状态和前一状态异或，并与前一状态与运算）
    key_up = ~key_val & (key_old ^ key_val);
    // 更新前一次按键状态
    key_old = key_val;

    switch (key_down)
    {
    case 1:
        if (++lcd_disp_mode == 3)
        {
            lcd_disp_mode = 0;
        }
        break;

    case 2:
        if (lcd_disp_mode == 2)
        {
            if (++set_index == 4)
            {
                set_index = 0;
            }
        }
        else if (lcd_disp_mode == 1)
        {
            key2_ticks = HAL_GetTick();
        }
        break;
    }

    if (key_up == 2 && uwTick - key2_ticks >= 2000 && lcd_disp_mode == 1)
    {
        time_value.Hours = 0;
        time_value.Minutes = 0;
        time_value.Seconds = 0;
    }
    else if (key_up == 2 && lcd_disp_mode == 0)
    {
        if (++st_flag == 2)
        {
            st_flag = 0;
        }
    }

    if (key_down >= 3 && lcd_disp_mode == 2)
    {
        switch (set_index)
        {
        case 0:
            para_value[0] += 1 * (key_down == 3?1:-1);
            if(para_value[0] >= 100)
            {
                para_value[0] = 100;
            }
            if(para_value[0] > 200 || para_value[0] <= 0)
            {
                para_value[0] = 0;
            }
            break;

        case 1:
            para_value[1] += 10 * (key_down == 3?1:-1);
            if(para_value[1] >= 80)
            {
                para_value[1] = 80;
            }
            else if(para_value[1] <= 10)
            {
                para_value[1] = 10;
            }
            break;

        case 2:
            para_value[2] += 100 * (key_down == 3?1:-1);
            break;

        case 3:
            para_value[3] += 1000 * (key_down == 3?1:-1);
            if(para_value[3] >= 2000)
            {
                para_value[3] = 2000;
            }
            else if(para_value[3] < 1000)
            {
                para_value[3] = 1000;
            }
            break;
        }
    }
}

typedef struct
{
    GPIO_TypeDef *gpiox; // 指向 GPIO 端口的指针，例如 GPIOA、GPIOB 等。
    uint16_t pin;        // 指定 GPIO 引脚，例如 GPIO_PIN_0、GPIO_PIN_1 等。
    uint16_t ticks;      // 用于计时的变量，通常用于去抖动计数。
    uint8_t level;       // 当前按键的电平状态，高电平或低电平。
    uint8_t id;          // 按键的唯一标识符，可以用来区分不同的按键。
    uint8_t state;       // 按键的当前状态，可以用于表示按键是否被按下或释放。
    uint8_t debouce_cnt; // 用于去抖动的计数器，用于防止机械抖动导致误触发。
    uint8_t repeat;      // 按键重复按下的次数。
} button;

button btns[4]; // 按键数组

// 按键初始化函数
void key_init(void)
{
    // 初始化第一个按键
    btns[0].gpiox = GPIOB;    // 指定GPIO端口
    btns[0].pin = GPIO_PIN_0; // 指定引脚
    btns[0].level = 1;        // 设置初始电平
    btns[0].id = 0;           // 设置按键ID

    // 初始化第二个按键
    btns[1].gpiox = GPIOB;
    btns[1].pin = GPIO_PIN_1;
    btns[1].level = 1;
    btns[1].id = 1;

    // 初始化第三个按键
    btns[2].gpiox = GPIOB;
    btns[2].pin = GPIO_PIN_2;
    btns[2].level = 1;
    btns[2].id = 2;

    // 初始化第四个按键
    btns[3].gpiox = GPIOA;
    btns[3].pin = GPIO_PIN_0;
    btns[3].level = 1;
    btns[3].id = 3;
}

// 按键任务处理函数
void key_task(button *btn)
{
    // 读取按键当前电平
    uint8_t gpio_level = HAL_GPIO_ReadPin(btn->gpiox, btn->pin);

    // 如果按键状态不为 0，增加计时器
    if (btn->state > 0)
        btn->ticks++;

    // 如果当前电平与按键记录的电平不同，增加去抖动计数
    if (btn->level != gpio_level)
    {
        // 如果达到 3 次，确认电平变化
        if (++(btn->debouce_cnt) >= 3)
        {
            btn->level = gpio_level; // 更新电平
            btn->debouce_cnt = 0;    // 重置去抖动计数器
        }
    }
    else
    {
        btn->debouce_cnt = 0; // 电平没有变化，重置去抖动计数器
    }

    // 按键状态机
    switch (btn->state)
    {
    case 0:                  // 初始状态
        if (btn->level == 0) // 等待按键按下
        {
            btn->ticks = 0;  // 重置计时器
            btn->repeat = 1; // 初始化按键重复次数
            btn->state = 1;  // 进入按键按下状态
        }
        break;
    case 1:                  // 按键按下状态
        if (btn->level != 0) // 等待按键松开
        {
            if (btn->ticks >= 30) // 长按检测
            {
                ucLed[btn->id] ^= 1; // 执行长按下的操作
                btn->state = 0;      // 返回初始状态
            }
            else
            {
                btn->ticks = 0; // 重置计时器
                btn->state = 2; // 进入按键释放状态
            }
        }
        else if (btn->ticks >= 30) // 长按检测
        {

            btn->repeat = 0; // 禁止释放的时候再次触发短按事件
        }
        break;
    case 2:                   // 按键释放状态
        if (btn->ticks >= 15) // 超时阈值达到阈值
        {
            btn->state = 0; // 返回初始状态
            if (btn->repeat == 1)
            {
                ucLed[btn->id + 4] ^= 1; // 点亮对应的LED
            }
            else if (btn->repeat == 2)
            {
                ucLed[btn->id] ^= 1; // 点亮对应的LED
            }
        }
        else
        {
            if (btn->level == 0) // 按键再次按下
            {
                btn->repeat++;  // 增加重复次数
                btn->ticks = 0; // 重置计时器
                btn->state = 1; // 返回按键按下状态
            }
        }
        break;
    }
}

// 按键状态处理函数
void key_state(void)
{
    for (uint8_t i = 0; i < 4; i++) // 遍历所有按键
    {
        key_task(&btns[i]); // 处理每个按键的状态
    }
}
