#include "rtc_app.h"

RTC_TimeTypeDef time = {11,59,50};//定义时间结构体
RTC_DateTypeDef date;//定义日期结构体
RTC_TimeTypeDef time_set;//设置时间结果体
RTC_TimeTypeDef alarm_time = {12,0,0};//报警时间结构体

uint8_t alarm_flag = 0;
uint8_t alarm_cnt = 0;

void rtc_set(RTC_TimeTypeDef *set_Time,RTC_DateTypeDef *set_Data)
{
    HAL_RTC_SetTime(&hrtc,set_Time,RTC_FORMAT_BIN);
    HAL_RTC_SetDate(&hrtc,set_Data,RTC_FORMAT_BIN);
}

uint8_t rtc_equal(RTC_TimeTypeDef *time1,RTC_TimeTypeDef *time2)
{
    if(time1->Hours == time2->Hours && time1->Minutes == time2->Minutes && time1->Seconds == time2->Seconds)
    {
        return 1;
    }
    return 0;
}

void rtc_proc(void)
{
    HAL_RTC_GetTime(&hrtc, &time, RTC_FORMAT_BIN);
    HAL_RTC_GetDate(&hrtc, &date, RTC_FORMAT_BIN);

    if(rtc_equal(&time,&alarm_time))
    {
       alarm_flag = 1; 
    }

    if(alarm_flag == 1)
    {
        alarm_cnt++;
        eeprom_write((uint8_t *)&alarm_cnt,0,1);
        alarm_flag = 0;
    }
}


