#ifndef BSP_SYSTEM_H
#define BSP_SYSTEM_H

#define PI 3.14f
#define MAX_COORDINATES 100

#include "stdio.h"
#include "stdarg.h"
#include "string.h"
#include "math.h"

#include "rtc.h"
#include "lcd.h"
#include "main.h"
#include "usart.h"
#include "system.h"
#include "filter.h"
#include "scheduler.h"
#include "led_app.h"
#include "key_app.h"
#include "lcd_app.h"
#include "uart_app.h"
#include "adc_app.h"
#include "rtc_app.h"
#include "tim_app.h"
#include "ringbuffer.h"

extern uint16_t uart_rx_index;  // 接收缓冲区索引
extern uint32_t uart_rx_ticks;  // 接收缓冲时间戳
extern uint8_t uart_rx_buffer[128];  // 接收缓冲
extern uint8_t ucLed[8];  // LED状态数组
extern uint8_t uart_rx_dma_buffer[128];  // DMA接收缓冲
extern uint32_t dma_buff[2][30];  // DMA接收缓冲
extern float adc_value[2];  // ADC采样值数组
extern RTC_TimeTypeDef time;  // 当前时间结构体
extern RTC_DateTypeDef date;  // 当前日期结构体
extern uint32_t tim_ic_buffer[64];  // 存储输入捕获值的缓冲区
extern uint32_t tim_ic_val;  // 最终计算得到的输入捕获值

extern uint8_t lcd_disp_mode;  // 0:行程界面,1:参数界面,2:统计界面
extern const char *st_value[3];  // 空闲,忙碌,等待
extern uint8_t st_index;
extern uint16_t cp_value[2];  // 设备当前位置(x,y)
extern uint16_t tp_value[2];  // 下一个途经地坐标(x,y)
extern float se_value;  // 运行状态下的速度值
extern uint8_t rn_value;  // 剩余途经地(含目的地)数量

extern float r_value;  // 1.0~2.0,单位为(cm/s)/Hz
extern uint8_t b_value;  // 10~100,单位为cm/s
extern uint8_t r_b_index;

extern float ts_value;  // 累计行程
extern uint32_t tt_value;  // 累计运行时间
extern float duty_A;  // PWM A的占空比
extern float duty_B;  // PWM B的占空比
extern int freq_A;  // PWM A的频率
extern int freq_B;  // PWM B的频率

extern uint8_t freq_number;  // 频率参数
extern uint16_t coordinate_count;
extern uint16_t coordinate_arry[MAX_COORDINATES][2];

#endif
