#ifndef BSP_SYSTEM_H
#define BSP_SYSTEM_H

#include "stdio.h"
#include "stdarg.h"
#include "string.h"

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

extern uint16_t uart_rx_index;//接收缓存索引
extern uint32_t uart_rx_ticks;//接收缓存时间戳
extern uint8_t uart_rx_buffer[128];//接收缓存
extern uint8_t ucLed[8];// LED 状态数组
extern uint8_t uart_rx_dma_buffer[128];//接收缓存
extern uint32_t dma_buff[2][30];//DMA接收缓存
extern float adc_value[2];//ADC采样值数组
extern RTC_TimeTypeDef time;//定义时间结构体
extern RTC_DateTypeDef date;//定义日期结构体
extern uint32_t tim_ic_buffer_A[64];
extern uint32_t tim_ic_buffer_B[64];
extern float tim_ic_val_A;// 最终计算得到的输入捕获值
extern float tim_ic_val_B;// 最终计算得到的输入捕获值
extern uint8_t lcd_disomode;
extern uint16_t data_ti[2];
extern uint8_t data_flag;
extern int para[3];
extern uint8_t recd[4];
extern float tim_ic_fre_A;
extern float tim_ic_fre_B;
extern int f_value_A;
extern int f_value_B;
extern uint8_t f_flag_A;
extern uint8_t f_flag_B;


#endif


