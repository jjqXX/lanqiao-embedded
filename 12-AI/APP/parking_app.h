#ifndef PARKING_APP_H
#define PARKING_APP_H

#include "stdio.h"
#include "stdint.h"
#include "string.h"

// 停车场总车位数
#define MAX_PARKING_SLOTS  8

// 车辆类型枚举
typedef enum {
    CAR_TYPE_NONE = 0,
    CAR_TYPE_CNBR,      // CNBR类型
    CAR_TYPE_VNBR       // VNBR类型
} car_type_t;

// 车辆信息结构体
typedef struct {
    car_type_t type;           // 车辆类型
    char car_id[5];            // 车辆编号(4个ASCII字符+'\0')
    uint16_t enter_year;       // 进入年份
    uint8_t enter_month;       // 进入月份
    uint8_t enter_day;         // 进入日期
    uint8_t enter_hour;        // 进入小时
    uint8_t enter_minute;      // 进入分钟
    uint8_t enter_second;      // 进入秒
    uint8_t is_occupied;       // 是否占用
} car_info_t;

// 停车场状态
extern car_info_t parking_slots[MAX_PARKING_SLOTS];  // 车位数组
extern uint8_t cnbr_count;      // CNBR类型车辆数量
extern uint8_t vnbr_count;      // VNBR类型车辆数量
extern uint8_t idle_count;      // 空闲车位数量

// 费率(单位:元/小时,乘以100存储以避免浮点运算,如350表示3.50元)
extern uint16_t cnbr_rate;      // CNBR费率
extern uint16_t vnbr_rate;      // VNBR费率

// LCD显示模式
typedef enum {
    LCD_MODE_DATA = 0,   // 车位显示界面
    LCD_MODE_PARA        // 费率设置界面
} lcd_mode_t;

extern lcd_mode_t lcd_mode;     // 当前LCD显示模式

// PA7输出状态
extern uint8_t pa7_pwm_enable;  // PA7 PWM使能标志(1:PWM输出, 0:低电平)

// 函数声明
void parking_init(void);
void parking_update_count(void);
int parking_car_enter(car_type_t type, const char* car_id,
                      uint16_t year, uint8_t month, uint8_t day,
                      uint8_t hour, uint8_t minute, uint8_t second);
int parking_car_exit(car_type_t type, const char* car_id,
                     uint16_t year, uint8_t month, uint8_t day,
                     uint8_t hour, uint8_t minute, uint8_t second,
                     uint32_t* duration, float* fee);
int parking_parse_message(const char* msg, uint8_t len);

#endif
