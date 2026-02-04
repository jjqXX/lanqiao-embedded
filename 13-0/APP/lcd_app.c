#include "lcd_app.h"

/**
 * @brief  格式化字符串并显示在指定的LCD行上。
 * 
 * 该函数接受一个行号和一个格式化字符串（类似于printf），
 * 格式化字符串后，将其显示在LCD的指定行上。
 *
 * @param  Line    要显示字符串的LCD行号。
 * @param  format  格式化字符串，后跟要格式化的参数。
 *
 * 该函数内部使用 `vsprintf` 来格式化字符串，然后
 * 调用 `LCD_DisplayStringLine` 在LCD上显示格式化后的字符串。
 *
 * 示例用法:
 * @code
 * LcdSprintf(0, "Temperature: %d C", temperature);
 * @endcode
 */
void LcdSprintf(uint8_t Line, char *format,...)
{
    char String[21];  // 缓冲区用于存储格式化后的字符串
    va_list arg;      // 参数列表用于存储可变参数
    va_start(arg, format);  // 使用格式化字符串初始化参数列表
    vsprintf(String, format, arg);  // 格式化字符串并存储在缓冲区中
    va_end(arg);  // 清理参数列表
    LCD_DisplayStringLine(Line,String);  // 在LCD的指定行显示格式化后的字符串
}

uint8_t lcd_disp_mode = 0;
uint8_t password_data[3] = {'1','2','3'};
uint8_t password_show[3] = {'@','@','@'};

void lcd_proc(void)
{
    freqency = 1000 + 1000 * lcd_disp_mode;
    duty = 50 - 40 * lcd_disp_mode;

    if(lcd_disp_mode == 0)
    {
        LcdSprintf(Line1,"       PSD    ");
        LcdSprintf(Line3,"   B1:%c      ",password_show[0]);
        LcdSprintf(Line4,"   B2:%c      ",password_show[1]);
        LcdSprintf(Line5,"   B3:%c      ",password_show[2]);
    }
    else if(lcd_disp_mode == 1)
    {
        LcdSprintf(Line1,"       STA    ");
        LcdSprintf(Line3,"   F:%dHz      ",freqency);
        LcdSprintf(Line4,"   D:%d%%      ",(int)duty);
        LcdSprintf(Line5,"                  ");
    }
}
