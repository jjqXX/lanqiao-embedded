#include "tim_app.h"

/**
 * @brief 设置PWM占空比
 *
 * 此函数用于设置TIM2的CH1通道的PWM占空比。占空比的范围为0%到100%。
 *
 * @param Duty 占空比，范围为0.0到100.0。
 */
void pwm_set_duty_A(float Duty)
{
    // 根据占空比计算捕获/比较寄存器的值
    TIM3->CCR1 = (TIM3->ARR + 1) * (Duty / 100.0f);
}
void pwm_set_duty_B(float Duty)
{
    // 根据占空比计算捕获/比较寄存器的值
    TIM3->CCR2 = (TIM3->ARR + 1) * (Duty / 100.0f);
}

/**
 * @brief 设置PWM频率
 *
 * 此函数用于设置TIM2的PWM频率。频率的单位为Hz。
 *
 * @param Frequency 频率，单位为Hz。
 */
void pwm_set_frequency_A(int Frequency)
{
    // 获取定时器的时钟频率（假设TIM2使用的时钟频率为TIM2_CLK）
    uint32_t TIM3_CLK = 72000000; // 例如72MHz, 需要根据实际情况调整

    // 根据输入的频率计算自动重装载寄存器的值
    uint32_t ARR_Value = (TIM3_CLK / Frequency) - 1;

    uint32_t old_ARR_Value = TIM3->ARR;

    // 设置自动重装载寄存器
    TIM3->ARR = ARR_Value;

    // 更新捕获/比较寄存器CCR2，保持当前占空比不变
    TIM3->CCR1 = (ARR_Value + 1) * (TIM3->CCR1 / (float)(old_ARR_Value + 1));

    // 触发更新事件，刷新寄存器
    TIM3 -> EGR = TIM_EGR_UG;
}
void pwm_set_frequency_B(int Frequency)
{
    // 获取定时器的时钟频率（假设TIM2使用的时钟频率为TIM2_CLK）
    uint32_t TIM3_CLK = 72000000; // 例如72MHz, 需要根据实际情况调整

    // 根据输入的频率计算自动重装载寄存器的值
    uint32_t ARR_Value = (TIM3_CLK / Frequency) - 1;

    uint32_t old_ARR_Value = TIM3->ARR;

    // 设置自动重装载寄存器
    TIM3->ARR = ARR_Value;

    // 更新捕获/比较寄存器CCR2，保持当前占空比不变
    TIM3->CCR2 = (ARR_Value + 1) * (TIM3->CCR2 / (float)(old_ARR_Value + 1));

    // 触发更新事件，刷新寄存器
    TIM3->EGR = TIM_EGR_UG;
}

uint32_t tim_ic_buffer[64]; // 定义存储输入捕获值的缓冲区
uint32_t tim_ic_val = 0;    // 最终计算得到的输入捕获值
uint32_t tim_ic_temp = 0;   // 临时存储输入捕获计算的中间值

float duty_A = 0; // PWM A的占空比
float duty_B = 0; // PWM B的占空比
int freq_A = 0;   // PWM A的频率
int freq_B = 0;   // PWM B的频率
float vate = 0;

/**
 * @brief 处理输入捕获数据
 *
 * 此函数用于处理输入捕获数据。它计算tim_ic_buffer中所有捕获值的平均值，
 * 然后将其转换为对应的频率值（单位为Hz），并存储在tim_ic_val中。
 * 最后，调用limit_value函数对tim_ic_val进行限制，确保其在1到20000之间。
 */
void ic_proc(void)
{
    tim_ic_temp = 0;

    // 对缓冲区中的捕获值求和
    for (int i = 0; i < 64; i++)
    {
        tim_ic_temp += tim_ic_buffer[i];
    }

    // 计算平均值
    tim_ic_temp /= 64;

    // 计算频率值，单位为Hz
    tim_ic_val = (int)((float)(1000.0f * 1000.0f) / (float)tim_ic_temp);

    // 将计算得到的频率值限制在1到20000之间
    limit_value(&tim_ic_val, 1, 400, 20000);

    if (st_index == 1)
    {
        pwm_set_duty_A(duty_A);
        pwm_set_duty_B(duty_B);
        pwm_set_frequency_A(freq_A);
        pwm_set_frequency_B(freq_B);
    }
    else 
    {
        duty_A = 0;
        duty_B = 0;
    }

    if(st_index == 1)
    {
        se_value = ((float)PI * r_value * (float)tim_ic_val) / 100.0f + (float)b_value;
        vate = se_value;
    }
    else
    {
        se_value = 0;
    }

    ts_value = vate * tt_value;
}
