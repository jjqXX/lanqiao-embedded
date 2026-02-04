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
uint8_t cnbr_cnt;
uint8_t vnbr_cnt;
uint8_t idle_cnt = 8;
float cnbr_fee = 3.50f;
float vnbr_fee = 2.00f;

void lcd_proc(void)
{
	if(lcd_disp_mode == 0)
    {
        LcdSprintf(Line1,"       Data     ");
        LcdSprintf(Line3,"   CNBR:%d       ",cnbr_cnt);
        LcdSprintf(Line5,"   VNBR:%d       ",vnbr_cnt);
        LcdSprintf(Line7,"   IDLE:%d       ",idle_cnt);
    }
    else
    {
        LcdSprintf(Line1,"       Para     ");
        LcdSprintf(Line3,"   CNBR:%.2f      ",cnbr_fee);
        LcdSprintf(Line5,"   VNBR:%.2f      ",vnbr_fee);
        LcdSprintf(Line7,"                    ");
    }
}
