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
    TIM2->CCR2 = (TIM2->ARR + 1) * (Duty / 100.0f);
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
    // 获取定时器的时钟频率（假设TIM2使用的时钟频率为TIM2_CLK）
    uint32_t TIM2_CLK = 72000000;  // 例如72MHz, 需要根据实际情况调整

    // 根据输入的频率计算自动重装载寄存器的值
    uint32_t ARR_Value = (TIM2_CLK / Frequency) - 1;

    // 设置自动重装载寄存器
    TIM2->ARR = ARR_Value;

    // 更新捕获/比较寄存器CCR2，保持当前占空比不变
    TIM2->CCR2 = (ARR_Value + 1) * (TIM2->CCR2 / (float)(TIM2->ARR + 1));
    
    // 触发更新事件，刷新寄存器
    TIM2->EGR = TIM_EGR_UG;
}



uint32_t tim_ic_buffer_A[64];// 定义存储输入捕获值的缓冲区
uint32_t tim_ic_buffer_B[64];// 定义存储输入捕获值的缓冲区
float tim_ic_val_A = 0;// 最终计算得到的输入捕获值
float tim_ic_temp_A = 0;// 临时存储输入捕获计算的中间值
float tim_ic_val_B = 0;// 最终计算得到的输入捕获值
float tim_ic_temp_B = 0;// 临时存储输入捕获计算的中间值
float tim_ic_fre_A = 0;
float tim_ic_fre_B = 0;
int f_value_A = 0;
int f_value_B = 0;
uint8_t f_flag_A = 0;
uint8_t f_flag_B = 0;

typedef struct
{
    uint32_t timestamp; // 时间戳
    uint32_t tim;
} tim_data_t;


int tim_buffer_start_A = 0;
int tim_buffer_end_A = 0;
int tim_buffer_start_B = 0;
int tim_buffer_end_B = 0;
uint8_t sudden_flag_A = 0;
uint8_t sudden_flag_B = 0;
uint8_t pd_flag_A = 0;
uint8_t pd_flag_B = 0;
uint8_t sudden_led_flag_A = 0;
uint8_t sudden_led_flag_B = 0;
uint8_t diff_A;
uint8_t diff_B;
uint8_t diff_A_flag;
uint8_t diff_B_flag;
uint8_t sudden_change_count_A;
uint8_t sudden_change_count_B;



tim_data_t tim_buffer_A[30];
tim_data_t tim_buffer_B[30];
int buffer_start = 0;
int buffer_end = 0;
uint8_t sudden_change_flag = 0;


void add_tim_data(uint32_t tim,uint32_t current_time,tim_data_t *tim_buffer)
{
    tim_buffer[buffer_start].timestamp = current_time;
    tim_buffer[buffer_start].tim = tim;
    buffer_end = (buffer_end + 1) % 30;

    if(buffer_start == buffer_end)
    {
        buffer_start = (buffer_start + 1) % 30;
    }

    if(current_time - tim_buffer[buffer_start].timestamp > 3000)
    {
        buffer_start = (buffer_start + 1) % 30;
    }
}

void check_tim_sudden_change(uint8_t *sudden_change_count,tim_data_t *tim_buffer)
{
    uint32_t f_max = tim_buffer[buffer_start].tim;
    uint32_t f_min = tim_buffer[buffer_start].tim;

    int index = buffer_start;
    while(index != buffer_end)
    {
        if(tim_buffer[index].tim > f_max)
        {
            f_max = tim_buffer[index].tim;
        }
        if(tim_buffer[index].tim < f_min)
        {
            f_min = tim_buffer[index].tim;
        }

        index = (index + 1) % 30;
    }

    uint32_t diff = f_max - f_min;
    if(diff < para[0])
    {
        sudden_change_flag = 1;
    }
    else if(sudden_change_flag == 1)
    {
        sudden_change_flag = 0;
        (*sudden_change_count)++;
    }
}





/**
 * @brief 处理输入捕获数据
 * 
 * 此函数用于处理输入捕获数据。它计算tim_ic_buffer中所有捕获值的平均值，
 * 然后将其转换为对应的频率值（单位为Hz），并存储在tim_ic_val中。
 * 最后，调用limit_value函数对tim_ic_val进行限制，确保其在1到20000之间。
 */
void ic_proc(void)
{
    uint32_t current_time = uwTick;
    
    tim_ic_temp_A = 0;
    tim_ic_temp_B = 0;
    
    // 对缓冲区中的捕获值求和
    for (int i = 0; i < 64; i++)
    {
        tim_ic_temp_A += tim_ic_buffer_A[i];
        tim_ic_temp_B += tim_ic_buffer_B[i];
    }

    // 计算平均值
    tim_ic_temp_A /= 64;
    tim_ic_temp_B /= 64;

    // 计算频率值，单位为Hz
    tim_ic_val_A = ((float)(1000.0f * 1000.0f) / (float)tim_ic_temp_A);
    tim_ic_val_B = ((float)(1000.0f * 1000.0f) / (float)tim_ic_temp_B);

    // 将计算得到的频率值限制在1到20000之间
    limit_value(&tim_ic_val_A, 1, 400, 20000);
    limit_value(&tim_ic_val_B, 1, 400, 20000);

    tim_ic_fre_A = 1000000.0f / tim_ic_val_A;
    tim_ic_fre_B = 1000000.0f / tim_ic_val_B;


    add_tim_data(tim_ic_fre_A,current_time,tim_buffer_A);
    check_tim_sudden_change(&recd[0],tim_buffer_A);
    add_tim_data(tim_ic_fre_B,current_time,tim_buffer_B);
    check_tim_sudden_change(&recd[1],tim_buffer_B);

    f_value_A = tim_ic_val_A + para[2];
	f_value_B = tim_ic_val_B + para[2];
    
    if(f_value_A > para[1])
    {
        f_flag_A = 1;
    }
    else if(f_flag_A == 1)
    {
        f_flag_A = 0;
        recd[2] += 1; 
    }

    if(f_value_B > para[1])
    {
        f_flag_B = 1;
    }
    else if(f_flag_B == 1)
    {
        f_flag_B = 0;
        recd[3] += 1;
    }

}

