#ifndef BSP_SYSTEM_H
#define BSP_SYSTEM_H

#include "stdio.h"
#include "stdarg.h"
#include "string.h"

#include "rtc.h"
#include "lcd.h"
#include "tim.h"
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
#include "parking_app.h"

// 串口相关外部变量
extern uint16_t uart_rx_index;
extern uint32_t uart_rx_ticks;
extern uint8_t uart_rx_buffer[128];
extern uint8_t uart_rx_dma_buffer[128];
extern DMA_HandleTypeDef hdma_usart1_rx;

// LED状态数组
extern uint8_t ucLed[8];

// DMA缓冲
extern uint32_t dma_buff[2][30];

// ADC采样值
extern float adc_value[2];

// RTC时间日期
extern RTC_TimeTypeDef time;
extern RTC_DateTypeDef date;

// 定时器输入捕获
extern uint32_t tim_ic_buffer[64];
extern uint32_t tim_ic_val;

#endif
