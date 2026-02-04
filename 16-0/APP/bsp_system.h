#ifndef BSP_SYSTEM_H
#define BSP_SYSTEM_H

#include "stdio.h"
#include "stdarg.h"
#include "string.h"
#include "tim.h"
#include "math.h"
#include <stdlib.h>

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

extern uint16_t uart_rx_index;      // 接收缓冲区索引
extern uint32_t uart_rx_ticks;      // 接收缓冲时间戳
extern uint8_t uart_rx_buffer[128]; // 接收缓冲
extern uint8_t ucLed[8];            // LED 状态数组
extern uint8_t uart_rx_dma_buffer[128]; // 接收缓冲
extern uint32_t dma_buff[2][30];    // DMA接收缓冲
extern float adc_value[2];          // ADC采样值数组
extern RTC_TimeTypeDef time;        // 定义时间结构体
extern RTC_DateTypeDef date;        // 定义日期结构体
extern uint32_t tim_ic_buffer[64];  // 用于存储输入捕获值的缓冲区
extern uint32_t tim_ic_val;         // 最终计算得到的输入捕获值

extern uint8_t set_index;
extern uint32_t key2_ticks;
extern uint8_t lcd_disp_mode; // 0:监控界面,1:统计界面,2:参数界面

extern uint32_t cf_value_change[2]; // [0]:当前值, [1]:记录值
extern uint32_t cd_value_change[2]; // [0]:当前值, [1]:记录值
extern uint8_t st_flag;             // 0:解锁,1:锁定
extern RTC_TimeTypeDef time_value;

extern uint32_t df_change;
extern uint32_t xf_value;
extern uint8_t time_change[3]; // [0]:时, [1]:分, [2]:秒

extern uint32_t para_value[4]; // [0]:ds, [1]:dr, [2]:fs, [3]:fr

extern uint8_t error_flag;
extern uint8_t recd_flag;

#endif
