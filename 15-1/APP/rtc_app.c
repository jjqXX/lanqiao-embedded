#include "rtc_app.h"

RTC_TimeTypeDef time;//定义时间结构体
RTC_DateTypeDef date;//定义日期结构体

void rtc_proc(void)
{
    if(st_index == 1)
    {
        tt_value++;
    }
}

