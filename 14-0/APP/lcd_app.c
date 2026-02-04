#include "lcd_app.h"

uint8_t lcd_displaymode = 0;
uint8_t p_value;
float v_value;
uint8_t lcd_data_flag = 0;
uint8_t para[2] = {1, 1};
uint8_t para_set[2] = {1, 1};
float recd[3] = {0, 0, 0};

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
void LcdSprintf(uint8_t Line, char *format, ...)
{
    char String[21];                     // 缓冲区用于存储格式化后的字符串
    va_list arg;                         // 参数列表用于存储可变参数
    va_start(arg, format);               // 使用格式化字符串初始化参数列表
    vsprintf(String, format, arg);       // 格式化字符串并存储在缓冲区中
    va_end(arg);                         // 清理参数列表
    LCD_DisplayStringLine(Line, String); // 在LCD的指定行显示格式化后的字符串
}

void lcd_proc(void)
{
    switch (lcd_displaymode)
    {
    case 0:

        LcdSprintf(Line1, "       DATA");
        LcdSprintf(Line3, "    M=%s      ", lcd_data_flag == 0 ? "L" : "H");
        LcdSprintf(Line4, "    P=%d%%    ", p_value);
        LcdSprintf(Line5, "    V=%.1f    ", v_value);
        break;

    case 1:
        LcdSprintf(Line1, "       PARA");
        LcdSprintf(Line3, "    R=%d      ", para[0]);
        LcdSprintf(Line4, "    K=%d      ", para[1]);
        LcdSprintf(Line5, "                     ");
        break;

    case 2:
        LcdSprintf(Line1, "       RECD");
        LcdSprintf(Line3, "    N=%d        ", (uint8_t)recd[0]);
        LcdSprintf(Line4, "    MH=%.1f      ", recd[1]);
        LcdSprintf(Line5, "    ML=%.1f      ", recd[2]);

        lcd_data_flag = 0;
        lcd_para_index = 0;
        lcd_data_flag = 0;
        break;
    }
}
