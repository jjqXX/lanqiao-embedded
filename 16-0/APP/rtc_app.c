#include "rtc_app.h"

RTC_TimeTypeDef time;//定义时间结构体
RTC_DateTypeDef date;//定义日期结构体

void timetamp_to_time(RTC_TimeTypeDef *time)
{
    uint32_t temp = uwTick / 1000;//转化为s
    time->Hours = (temp / 3600) % 24;
    time->Minutes = (temp % 3600) / 60;
    time->Seconds = temp % 60;
}

void rtc_proc(void)
{
    timetamp_to_time(&time_value);

    HAL_RTC_GetTime(&hrtc, &time, RTC_FORMAT_BIN);
    HAL_RTC_GetDate(&hrtc, &date, RTC_FORMAT_BIN);
}

