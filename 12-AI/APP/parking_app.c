#include "parking_app.h"

// 停车场车位数组
car_info_t parking_slots[MAX_PARKING_SLOTS];

// 各类型车辆计数
uint8_t cnbr_count = 0;     // CNBR类型车辆数量
uint8_t vnbr_count = 0;     // VNBR类型车辆数量
uint8_t idle_count = MAX_PARKING_SLOTS;  // 空闲车位数量

// 费率(乘以100存储,如350表示3.50元/小时)
uint16_t cnbr_rate = 350;   // CNBR费率:3.50元/小时
uint16_t vnbr_rate = 200;   // VNBR费率:2.00元/小时

// LCD显示模式
lcd_mode_t lcd_mode = LCD_MODE_DATA;  // 默认车位显示界面

// PA7输出状态(默认低电平)
uint8_t pa7_pwm_enable = 0;

/**
 * @brief 停车场初始化
 */
void parking_init(void)
{
    // 清空所有车位
    memset(parking_slots, 0, sizeof(parking_slots));

    // 初始化计数
    cnbr_count = 0;
    vnbr_count = 0;
    idle_count = MAX_PARKING_SLOTS;

    // 初始化费率
    cnbr_rate = 350;  // 3.50元/小时
    vnbr_rate = 200;  // 2.00元/小时

    // 默认显示车位界面
    lcd_mode = LCD_MODE_DATA;

    // PA7默认低电平
    pa7_pwm_enable = 0;
}

/**
 * @brief 更新车辆计数
 */
void parking_update_count(void)
{
    cnbr_count = 0;
    vnbr_count = 0;

    for (int i = 0; i < MAX_PARKING_SLOTS; i++)
    {
        if (parking_slots[i].is_occupied)
        {
            if (parking_slots[i].type == CAR_TYPE_CNBR)
                cnbr_count++;
            else if (parking_slots[i].type == CAR_TYPE_VNBR)
                vnbr_count++;
        }
    }

    idle_count = MAX_PARKING_SLOTS - cnbr_count - vnbr_count;
}

/**
 * @brief 车辆入场
 * @param type 车辆类型
 * @param car_id 车辆编号(4字符)
 * @param year 年(如2020)
 * @param month 月
 * @param day 日
 * @param hour 时
 * @param minute 分
 * @param second 秒
 * @return 0:成功, -1:车位已满, -2:车辆已存在
 */
int parking_car_enter(car_type_t type, const char* car_id,
                      uint16_t year, uint8_t month, uint8_t day,
                      uint8_t hour, uint8_t minute, uint8_t second)
{
    // 检查车辆是否已存在
    for (int i = 0; i < MAX_PARKING_SLOTS; i++)
    {
        if (parking_slots[i].is_occupied &&
            strncmp(parking_slots[i].car_id, car_id, 4) == 0)
        {
            return -2;  // 车辆已存在
        }
    }

    // 查找空闲车位
    for (int i = 0; i < MAX_PARKING_SLOTS; i++)
    {
        if (!parking_slots[i].is_occupied)
        {
            // 填充车辆信息
            parking_slots[i].type = type;
            strncpy(parking_slots[i].car_id, car_id, 4);
            parking_slots[i].car_id[4] = '\0';
            parking_slots[i].enter_year = year;
            parking_slots[i].enter_month = month;
            parking_slots[i].enter_day = day;
            parking_slots[i].enter_hour = hour;
            parking_slots[i].enter_minute = minute;
            parking_slots[i].enter_second = second;
            parking_slots[i].is_occupied = 1;

            // 更新计数
            parking_update_count();
            return 0;
        }
    }

    return -1;  // 车位已满
}

/**
 * @brief 计算两个时间点之间的小时数(不足1小时按1小时计)
 */
static uint32_t calculate_hours(uint16_t year1, uint8_t month1, uint8_t day1,
                                uint8_t hour1, uint8_t minute1, uint8_t second1,
                                uint16_t year2, uint8_t month2, uint8_t day2,
                                uint8_t hour2, uint8_t minute2, uint8_t second2)
{
    // 简化计算:将时间转换为分钟数后比较
    // 每月按30天计算(简化处理)
    uint32_t total_min1 = (uint32_t)year1 * 365 * 24 * 60 +
                          (uint32_t)month1 * 30 * 24 * 60 +
                          (uint32_t)day1 * 24 * 60 +
                          (uint32_t)hour1 * 60 +
                          (uint32_t)minute1;

    uint32_t total_min2 = (uint32_t)year2 * 365 * 24 * 60 +
                          (uint32_t)month2 * 30 * 24 * 60 +
                          (uint32_t)day2 * 24 * 60 +
                          (uint32_t)hour2 * 60 +
                          (uint32_t)minute2;

    // 计算时间差(分钟)
    uint32_t diff_min = 0;
    if (total_min2 >= total_min1)
        diff_min = total_min2 - total_min1;

    // 考虑秒数
    if (second2 > second1)
        diff_min++;  // 有额外的秒数,分钟数+1

    // 转换为小时数(不足1小时按1小时计)
    uint32_t hours = diff_min / 60;
    if (diff_min % 60 > 0)
        hours++;

    // 至少1小时
    if (hours == 0)
        hours = 1;

    return hours;
}

/**
 * @brief 车辆出场
 * @param type 车辆类型
 * @param car_id 车辆编号(4字符)
 * @param year 年
 * @param month 月
 * @param day 日
 * @param hour 时
 * @param minute 分
 * @param second 秒
 * @param duration 输出:停车时长(小时)
 * @param fee 输出:停车费用(元)
 * @return 0:成功, -1:未找到车辆, -2:类型不匹配
 */
int parking_car_exit(car_type_t type, const char* car_id,
                     uint16_t year, uint8_t month, uint8_t day,
                     uint8_t hour, uint8_t minute, uint8_t second,
                     uint32_t* duration, float* fee)
{
    // 查找车辆
    for (int i = 0; i < MAX_PARKING_SLOTS; i++)
    {
        if (parking_slots[i].is_occupied &&
            strncmp(parking_slots[i].car_id, car_id, 4) == 0)
        {
            // 检查类型是否匹配
            if (parking_slots[i].type != type)
            {
                return -2;  // 类型不匹配
            }

            // 计算停车时长
            *duration = calculate_hours(
                parking_slots[i].enter_year,
                parking_slots[i].enter_month,
                parking_slots[i].enter_day,
                parking_slots[i].enter_hour,
                parking_slots[i].enter_minute,
                parking_slots[i].enter_second,
                year, month, day, hour, minute, second
            );

            // 计算费用
            uint16_t rate = (type == CAR_TYPE_CNBR) ? cnbr_rate : vnbr_rate;
            *fee = (float)(*duration) * (float)rate / 100.0f;

            // 清除车位
            memset(&parking_slots[i], 0, sizeof(car_info_t));

            // 更新计数
            parking_update_count();
            return 0;
        }
    }

    return -1;  // 未找到车辆
}

/**
 * @brief 解析串口消息
 * @param msg 消息字符串
 * @param len 消息长度
 * @return 0:入场消息处理成功, 1:出场消息需要回复, -1:格式错误
 */
int parking_parse_message(const char* msg, uint8_t len)
{
    // 检查最小长度 (CNBR:XXXX:YYMMDDHHmmSS 至少21字符)
    if (len < 21)
        return -1;

    // 解析车辆类型
    car_type_t type = CAR_TYPE_NONE;
    if (strncmp(msg, "CNBR:", 5) == 0)
        type = CAR_TYPE_CNBR;
    else if (strncmp(msg, "VNBR:", 5) == 0)
        type = CAR_TYPE_VNBR;
    else
        return -1;  // 未知类型

    // 解析车辆编号(4个ASCII字符)
    char car_id[5] = {0};
    strncpy(car_id, msg + 5, 4);

    // 检查分隔符
    if (msg[9] != ':')
        return -1;

    // 解析时间 (YYMMDDHHmmSS - 12位数字)
    const char* time_str = msg + 10;
    if (len < 22)  // 需要至少22字符
        return -1;

    // 检查时间字符串是否都是数字
    for (int i = 0; i < 12; i++)
    {
        if (time_str[i] < '0' || time_str[i] > '9')
            return -1;
    }

    // 解析时间各字段
    uint16_t year = 2000 + (time_str[0] - '0') * 10 + (time_str[1] - '0');
    uint8_t month = (time_str[2] - '0') * 10 + (time_str[3] - '0');
    uint8_t day = (time_str[4] - '0') * 10 + (time_str[5] - '0');
    uint8_t hour = (time_str[6] - '0') * 10 + (time_str[7] - '0');
    uint8_t minute = (time_str[8] - '0') * 10 + (time_str[9] - '0');
    uint8_t second = (time_str[10] - '0') * 10 + (time_str[11] - '0');

    // 验证时间合法性
    if (month < 1 || month > 12 || day < 1 || day > 31 ||
        hour > 23 || minute > 59 || second > 59)
        return -1;

    // 尝试入场
    int result = parking_car_enter(type, car_id, year, month, day, hour, minute, second);

    if (result == 0)
    {
        // 入场成功,不需要回复
        return 0;
    }
    else if (result == -2)
    {
        // 车辆已存在,说明这是出场消息
        uint32_t duration = 0;
        float fee = 0.0f;

        result = parking_car_exit(type, car_id, year, month, day, hour, minute, second,
                                  &duration, &fee);

        if (result == 0)
        {
            // 出场成功,输出计费信息
            // 格式: 停车类型:车辆编号:停车时长:费用
            if (type == CAR_TYPE_CNBR)
                printf("CNBR:%s:%d:%.2f\r\n", car_id, duration, fee);
            else
                printf("VNBR:%s:%d:%.2f\r\n", car_id, duration, fee);
            return 1;
        }
        else
        {
            return -1;  // 出场失败
        }
    }
    else
    {
        // 车位已满或其他错误
        return -1;
    }
}
