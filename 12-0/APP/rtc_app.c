#include "rtc_app.h"

RTC_TimeTypeDef rtc_time;//定义时间结构体
RTC_DateTypeDef rtc_date;//定义日期结构体

void rtc_proc(void)
{
    HAL_RTC_GetTime(&hrtc, &rtc_time, RTC_FORMAT_BIN);
    HAL_RTC_GetDate(&hrtc, &rtc_date, RTC_FORMAT_BIN);
}

