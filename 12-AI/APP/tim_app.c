#include "tim_app.h"
#include "parking_app.h"

/**
 * @brief 设置TIM2 PWM占空比
 *
 * @param Duty 占空比，范围为0.0到100.0
 */
void pwm_set_duty(float Duty)
{
    TIM2->CCR2 = (TIM2->ARR + 1) * (Duty / 100.0f);
}

/**
 * @brief 设置TIM2 PWM频率
 *
 * @param Frequency 频率，单位为Hz
 */
void pwm_set_frequency(int Frequency)
{
    uint32_t TIM2_CLK = 80000000;  // 80MHz
    uint32_t ARR_Value = (TIM2_CLK / Frequency) - 1;
    uint32_t old_ARR_Value = TIM2->ARR;

    TIM2->ARR = ARR_Value;
    TIM2->CCR2 = (ARR_Value + 1) * (TIM2->CCR2 / (float)(old_ARR_Value + 1));
    TIM2->EGR = TIM_EGR_UG;
}

uint32_t tim_ic_buffer[64];  // 输入捕获缓冲区
uint32_t tim_ic_val = 0;     // 计算得到的输入捕获值
uint32_t tim_ic_temp = 0;    // 临时存储输入捕获计算的中间值

/**
 * @brief 处理输入捕获数据
 */
void ic_proc(void)
{
    tim_ic_temp = 0;

    for (int i = 0; i < 64; i++)
    {
        tim_ic_temp += tim_ic_buffer[i];
    }

    tim_ic_temp /= 64;

    tim_ic_val = (int)((float)(1000.0f * 1000.0f) / (float)tim_ic_temp);

    limit_value(&tim_ic_val, 1, 500, 20000);
}

/**
 * @brief PA7 PWM输出初始化
 *
 * 配置PA7输出2KHz，20%占空比的PWM信号
 * 使用TIM17 CH1
 */
void pa7_pwm_init(void)
{
    // PA7默认输出低电平
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_RESET);
}

/**
 * @brief PA7 PWM输出控制
 *
 * 根据pa7_pwm_enable标志控制PA7输出状态:
 * - pa7_pwm_enable = 1: 输出2KHz,20%占空比的PWM信号
 * - pa7_pwm_enable = 0: 输出持续低电平
 */
void pa7_pwm_control(void)
{
    static uint8_t last_state = 0;

    if (pa7_pwm_enable != last_state)
    {
        last_state = pa7_pwm_enable;

        if (pa7_pwm_enable)
        {
            // 启动PWM输出
            HAL_TIM_PWM_Start(&htim17, TIM_CHANNEL_1);
        }
        else
        {
            // 停止PWM输出，设置低电平
            HAL_TIM_PWM_Stop(&htim17, TIM_CHANNEL_1);
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_RESET);
        }
    }
}
