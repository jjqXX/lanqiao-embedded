#include "lcd_app.h"

/**
 * @brief  格式化字符串并显示到指定的LCD行上。
 *
 * 该函数接受一个行号和一个格式化字符串，类似于printf函数。
 * 格式化字符串后，将其显示在LCD的指定行上。
 *
 * @param  Line    要显示字符串的LCD行号。
 * @param  format  格式化字符串，以及需要格式化的参数。
 *
 * 该函数内部使用 `vsprintf` 来格式化字符串，然后
 * 调用 `LCD_DisplayStringLine` 在LCD上显示格式化后的字符串。
 *
 * 示例用法:
 * @code
 * LcdSprintf(0, "Temperature: %d C", temperature);
 * @endcode
 */
void LcdSprintf(uint8_t Line, char *format, ...)
{
    char String[21];                     // 缓冲区用于存储格式化后的字符串
    va_list arg;                         // 参数列表，用于存储可变参数
    va_start(arg, format);               // 使用格式化字符串初始化参数列表
    vsprintf(String, format, arg);       // 格式化字符串并存储在缓冲区中
    va_end(arg);                         // 清理参数列表
    LCD_DisplayStringLine(Line, String); // 在LCD的指定行显示格式化后的字符串
}

uint8_t lcd_disp_mode = 0; // 0:监控界面,1:统计界面,2:参数界面

uint32_t cf_value_change[2] = {0,0};
uint32_t cd_value_change[2] = {0,0};
uint8_t st_flag = 0; // 0:解锁,1:锁定
RTC_TimeTypeDef time_value = {00, 00, 00};

uint32_t df_change = 0;
uint32_t xf_value = 0;

uint8_t time_change[3] = {0,0,0}; // 时分秒

uint32_t para_value[4] = {1,80,100,2000}; // ds,dr,fs,fr

void lcd_proc(void)
{
    switch (lcd_disp_mode)
    {
    case 0:
        LcdSprintf(Line1, "      PWM      ");
        LcdSprintf(Line3, "   CF=%dHz          ", cf_value_change[0]);
        LcdSprintf(Line4, "   CD=%d%%          ", cd_value_change[0]);
        LcdSprintf(Line5, "   DF=%dHz      ", tim_ic_val);
        LcdSprintf(Line6, "   ST=%s          ", st_flag ? "LOCK" : "UNLOCK");
        LcdSprintf(Line7, "   %02dH%02dM%02dS   ", time_value.Hours, time_value.Minutes, time_value.Seconds);
        break;

    case 1:
        LcdSprintf(Line1, "      RECD      ");
        LcdSprintf(Line3, "   CF=%dHz          ", cf_value_change[1]);
        LcdSprintf(Line4, "   CD=%d%%          ", cd_value_change[1]);
        LcdSprintf(Line5, "   DF=%dHz          ", df_change);
        LcdSprintf(Line6, "   XF=%dHz          ", xf_value);
        LcdSprintf(Line7, "   %02dH%02dM%02dS   ", time_change[0], time_change[1], time_change[2]);
        break;

    case 2:
        LcdSprintf(Line1, "      PARA      ");
        LcdSprintf(Line3, "   DS=%d%%          ", para_value[0]);
        LcdSprintf(Line4, "   DR=%d%%          ", para_value[1]);
        LcdSprintf(Line5, "   FS=%dHz          ", para_value[2]);
        LcdSprintf(Line6, "   FR=%dHz          ", para_value[3]);
        LcdSprintf(Line7, "                    ");
        break;
    }
}
