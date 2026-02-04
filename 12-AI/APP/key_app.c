#include "key_app.h"
#include "parking_app.h"

uint8_t key_val = 0;  // 当前按键状态
uint8_t key_old = 0;  // 前一次按键状态
uint8_t key_down = 0; // 按下的按键
uint8_t key_up = 0;   // 释放的按键

/**
 * @brief 读取按键状态
 *
 * @return 返回按键编号。0表示没有按键按下，1-4表示对应的按键被按下。
 */
uint8_t key_read(void)
{
    uint8_t temp = 0;

    // B1: PB0
    if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_0) == GPIO_PIN_RESET)
        temp = 1;

    // B2: PB1
    if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_1) == GPIO_PIN_RESET)
        temp = 2;

    // B3: PB2
    if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_2) == GPIO_PIN_RESET)
        temp = 3;

    // B4: PA0
    if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0) == GPIO_PIN_RESET)
        temp = 4;

    return temp;
}

/**
 * @brief 按键处理函数
 *
 * B1: 界面切换按键，切换LCD显示"车位显示界面"和"费率设置界面"
 * B2: "加"按键，每次按下CNBR、VNBR费率增加0.5元(仅费率设置界面有效)
 * B3: "减"按键，每次按下CNBR、VNBR费率减少0.5元(仅费率设置界面有效)
 * B4: "控制"按键，切换PA7端口输出状态(2KHz,20%占空比脉冲/持续低电平)
 */
void key_proc(void)
{
    // 读取当前按键状态
    key_val = key_read();
    // 计算按下的按键
    key_down = key_val & (key_old ^ key_val);
    // 计算释放的按键
    key_up = ~key_val & (key_old ^ key_val);
    // 更新前一次按键状态
    key_old = key_val;

    // 处理按键按下事件
    switch (key_down)
    {
    case 1:  // B1: 界面切换
        if (lcd_mode == LCD_MODE_DATA)
            lcd_mode = LCD_MODE_PARA;
        else
            lcd_mode = LCD_MODE_DATA;
        break;

    case 2:  // B2: 加(仅在费率设置界面有效)
        if (lcd_mode == LCD_MODE_PARA)
        {
            // CNBR和VNBR费率各增加0.5元(即50,因为存储时乘以100)
            cnbr_rate += 50;
            vnbr_rate += 50;
        }
        break;

    case 3:  // B3: 减(仅在费率设置界面有效)
        if (lcd_mode == LCD_MODE_PARA)
        {
            // CNBR和VNBR费率各减少0.5元
            // 防止费率变为负数(最低0.5元)
            if (cnbr_rate > 50)
                cnbr_rate -= 50;
            else
                cnbr_rate = 50;

            if (vnbr_rate > 50)
                vnbr_rate -= 50;
            else
                vnbr_rate = 50;
        }
        break;

    case 4:  // B4: 控制PA7输出状态
        pa7_pwm_enable = !pa7_pwm_enable;  // 切换PWM使能状态
        break;

    default:
        break;
    }
}

typedef struct
{
    GPIO_TypeDef *gpiox;
    uint16_t pin;
    uint16_t ticks;
    uint8_t level;
    uint8_t id;
    uint8_t state;
    uint8_t debouce_cnt;
    uint8_t repeat;
} button;

button btns[4];

/**
 * @brief 按键初始化函数
 */
void key_init(void)
{
    // B1: PB0
    btns[0].gpiox = GPIOB;
    btns[0].pin = GPIO_PIN_0;
    btns[0].level = 1;
    btns[0].id = 0;

    // B2: PB1
    btns[1].gpiox = GPIOB;
    btns[1].pin = GPIO_PIN_1;
    btns[1].level = 1;
    btns[1].id = 1;

    // B3: PB2
    btns[2].gpiox = GPIOB;
    btns[2].pin = GPIO_PIN_2;
    btns[2].level = 1;
    btns[2].id = 2;

    // B4: PA0
    btns[3].gpiox = GPIOA;
    btns[3].pin = GPIO_PIN_0;
    btns[3].level = 1;
    btns[3].id = 3;
}

/**
 * @brief 按键任务处理函数(带状态机的高级按键处理,本项目暂不使用)
 */
void key_task(button *btn)
{
    uint8_t gpio_level = HAL_GPIO_ReadPin(btn->gpiox, btn->pin);

    if (btn->state > 0)
        btn->ticks++;

    if (btn->level != gpio_level)
    {
        if (++(btn->debouce_cnt) >= 3)
        {
            btn->level = gpio_level;
            btn->debouce_cnt = 0;
        }
    }
    else
    {
        btn->debouce_cnt = 0;
    }

    switch (btn->state)
    {
    case 0:
        if (btn->level == 0)
        {
            btn->ticks = 0;
            btn->repeat = 1;
            btn->state = 1;
        }
        break;
    case 1:
        if (btn->level != 0)
        {
            if (btn->ticks >= 30)
            {
                ucLed[btn->id] ^= 1;
                btn->state = 0;
            }
            else
            {
                btn->ticks = 0;
                btn->state = 2;
            }
        }
        else if (btn->ticks >= 30)
        {
            btn->repeat = 0;
        }
        break;
    case 2:
        if (btn->ticks >= 15)
        {
            btn->state = 0;
            if (btn->repeat == 1)
            {
                ucLed[btn->id + 4] ^= 1;
            }
            else if (btn->repeat == 2)
            {
                ucLed[btn->id] ^= 1;
            }
        }
        else
        {
            if (btn->level == 0)
            {
                btn->repeat++;
                btn->ticks = 0;
                btn->state = 1;
            }
        }
        break;
    }
}

/**
 * @brief 按键状态处理函数
 */
void key_state(void)
{
    for (uint8_t i = 0; i < 4; i++)
    {
        key_task(&btns[i]);
    }
}
