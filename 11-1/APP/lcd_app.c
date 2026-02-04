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

void set_color_text(char *str, uint8_t pos, uint8_t length, uint8_t highlightLength, uint16_t highlightColor, uint16_t defaultColor, uint8_t line)
{
    int charWidth = 16;  // 假设每个字符的宽度为16像素
    int lcdWidth = 320;  // 假设LCD宽度为320像素
    int i;
    for (i = 0; i < length; i++)
    {
        int xPos = lcdWidth - (charWidth * i);
        if (i >= pos && i < pos + highlightLength)
        {
            LCD_SetTextColor(highlightColor); 
        }
        else
        {
            LCD_SetTextColor(defaultColor); 
        }
        LCD_DisplayChar(line, xPos, str[i]); 
    }
}


uint8_t lcd_disp_mode = 0;//0:显示界面,//1:设置界面,//2:报警设置界面
uint8_t set_index;

void lcd_proc(void)
{
    char timeString[21];
    switch (lcd_disp_mode)
    {
    case 0:
        LcdSprintf(Line1,"       MAIN          ");
        LcdSprintf(Line4,"    RTC:%2d:%2d:%2d       ",time.Hours,time.Minutes,time.Seconds);
        break;
    
    case 1:
        LcdSprintf(Line1,"     RTC_SETTING     ");
        sprintf(timeString, "    RTC:%02d:%02d:%02d    ",time_set.Hours,time_set.Minutes,time_set.Seconds);
        set_color_text(timeString, 8 + set_index * 3, strlen(timeString), 2, Red, White, Line4);
        break;

    case 2:
        LcdSprintf(Line1,"     ALARM_SETTING   ");
        sprintf(timeString,"    RTC:%2d:%2d:%2d       ",alarm_time.Hours,alarm_time.Minutes,alarm_time.Seconds);
        set_color_text(timeString, 8 + set_index * 3, strlen(timeString), 2, Red, White, Line4);
        break;
    }
}
