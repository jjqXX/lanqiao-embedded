#include "scheduler.h"

// 全局变量，用于存储任务数量
uint8_t task_num;

typedef struct {
    void (*task_func)(void);
    uint32_t rate_ms;
    uint32_t last_run;
} task_t;

// 静态任务数组
static task_t scheduler_task[] =
{
    {led_proc, 1, 0},         // LED处理,1ms周期
    {key_proc, 10, 0},        // 按键处理,10ms周期
    {lcd_proc, 100, 0},       // LCD显示,100ms周期
    {uart_proc, 10, 0},       // 串口处理,10ms周期
    {pa7_pwm_control, 10, 0}  // PA7 PWM控制,10ms周期
};

/**
 * @brief 调度器初始化函数
 */
void scheduler_init(void)
{
    task_num = sizeof(scheduler_task) / sizeof(task_t);
}

/**
 * @brief 调度器运行函数
 */
void scheduler_run(void)
{
    for (uint8_t i = 0; i < task_num; i++)
    {
        uint32_t now_time = HAL_GetTick();

        if (now_time >= scheduler_task[i].rate_ms + scheduler_task[i].last_run)
        {
            scheduler_task[i].last_run = now_time;
            scheduler_task[i].task_func();
        }
    }
}
