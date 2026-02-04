#include "tim_app.h"

/**
 * @brief 设置PWM占空比
 *
 * 此函数用于设置TIM2的CH1通道的PWM占空比。占空比的范围为0%到100%。
 *
 * @param Duty 占空比，范围为0.0到100.0。
 */
void pwm_set_duty(float Duty)
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
void pwm_set_frequency(int Frequency)
{
    // 获取定时器的时钟频率，假设TIM2使用的时钟频率为TIM2_CLK。
    uint32_t TIM3_CLK = 80000000; // 假设72MHz, 需要根据实际情况调整

    // 根据给定频率计算自动重装载寄存器的值
    uint32_t ARR_Value = (TIM3_CLK / Frequency) - 1;

    uint32_t old_ARR_Value = TIM3->ARR;

    // 设置自动重装载寄存器
    TIM3->ARR = ARR_Value;

    // 更新捕获/比较寄存器CCR2，保持当前占空比不变
    TIM3->CCR2 = (ARR_Value + 1) * (TIM3->CCR2 / (float)(old_ARR_Value + 1));

    // 产生更新事件以刷新寄存器
    TIM3->EGR = TIM_EGR_UG;
}

uint32_t tim_ic_buffer[64] = {0}; // 用于存储输入捕获值的缓冲区
uint32_t tim_ic_val = 0;          // 最终计算得到的输入捕获值
uint32_t tim_ic_temp = 0;         // 临时存储输入捕获计算的中间值

uint8_t error_flag = 0; // 频率异常
static uint32_t error_ticks = 0;
uint8_t recd_flag = 1;

/**
 * @brief 处理输入捕获数据
 *
 * 此函数用于处理输入捕获数据。它首先对tim_ic_buffer数组中采样值求平均值，
 * 然后将其转换为对应的频率值（单位为Hz），并存储在tim_ic_val中。
 * 最后，调用limit_value函数对tim_ic_val进行限制，确保其在1到20000之间。
 */
void ic_proc(void)
{
    tim_ic_temp = 0;

    // 对缓冲区中的采样值求和
    for (int i = 0; i < 64; i++)
    {
        tim_ic_temp += tim_ic_buffer[i];
    }

    // 计算平均值
    tim_ic_temp /= 64;

    // 计算频率值，单位为Hz
    tim_ic_val = (int)((float)(1000.0f * 1000.0f) / (float)tim_ic_temp);

    // 限制计算得到的频率值在1到20000之间
    limit_value(&tim_ic_val, 1, 500, 20000);

    pwm_set_duty(cd_value_change[0]);
    pwm_set_frequency(cf_value_change[0]);

    if (abs(tim_ic_val - cf_value_change[0]) > 1000)
    {
        if (error_ticks == 0)
        {
            error_ticks = HAL_GetTick();
        }
        else if (uwTick - error_ticks > 200)
        {
            error_flag = 1;
            error_ticks = 0;
        }
    }
    else
    {
        error_ticks = 0;
    }

    if (error_flag == 1)
    {
        if (recd_flag == 1)
        {
            cf_value_change[1] = cf_value_change[0];
            cd_value_change[1] = cd_value_change[0];
            df_change = tim_ic_val;
            xf_value = abs(tim_ic_val - cf_value_change[0]);

            time_change[0] = time_value.Hours;
            time_change[1] = time_value.Minutes;
            time_change[2] = time_value.Seconds;

            recd_flag = 0;
        }

        error_flag = 0;
    }
}
