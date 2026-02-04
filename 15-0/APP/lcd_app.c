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
void LcdSprintf(uint8_t Line, char *format, ...)
{
    char String[21];                     // 缓冲区用于存储格式化后的字符串
    va_list arg;                         // 参数列表用于存储可变参数
    va_start(arg, format);               // 使用格式化字符串初始化参数列表
    vsprintf(String, format, arg);       // 格式化字符串并存储在缓冲区中
    va_end(arg);                         // 清理参数列表
    LCD_DisplayStringLine(Line, String); // 在LCD的指定行显示格式化后的字符串
}

uint8_t lcd_disomode = 0;
int para[3] = {1000, 5000, 0};
uint8_t recd[4] = {0,0,0,0};

void lcd_proc(void)
{
    switch (lcd_disomode)
    {
    case 0:
        if (data_flag == 0)
        {
            LcdSprintf(Line1, "       DATA");
            if (tim_ic_val_A > 1000)
            {
                LcdSprintf(Line2, "    A=%2.2fKHz        ", (float)tim_ic_val_A / 1000.0f);
            }
            else
            {
                if (f_value_A < 0)
                {
                    LcdSprintf(Line2, "    A=NULL            ");
                }
                else
                {
                    LcdSprintf(Line2, "    A=%dHz            ", (int)tim_ic_val_A);
                }
            }

            if (tim_ic_val_B > 1000)
            {
                LcdSprintf(Line3, "    B=%2.2fKHz         ", (float)tim_ic_val_B / 1000.0f);
            }
            else
            {
                if (f_value_B < 0)
                {
                    LcdSprintf(Line3, "    B=NULL            ");
                }
                else
                {
                    LcdSprintf(Line3, "    B=%dHz            ", (int)tim_ic_val_B);
                }
            }

            LcdSprintf(Line4, "              ");
            LcdSprintf(Line5, "              ");
        }
        else if (data_flag == 1)
        {
            LcdSprintf(Line1, "       DATA");
            if (tim_ic_fre_A > 1000)
            {
                LcdSprintf(Line2, "    A=%.2fms      ", f_value_A / 1000.0f);
            }
            else
            {
                LcdSprintf(Line2, "    A=%dus      ", (int)f_value_A);
            }
            if (tim_ic_fre_B > 1000)
            {
                LcdSprintf(Line3, "    B=%.2fms      ", f_value_B / 1000.0f);
            }
            else
            {
                LcdSprintf(Line3, "    B=%dus      ", (int)f_value_B);
            }
            LcdSprintf(Line4, "                ");
            LcdSprintf(Line5, "                ");
        }
        break;

    case 1:
        LcdSprintf(Line1, "       PARA");
        LcdSprintf(Line2, "    PD=%dHZ    ", para[0]);
        LcdSprintf(Line3, "    PH=%dHZ    ", para[1]);
        LcdSprintf(Line4, "    PX=%dHZ    ", para[2]);
        LcdSprintf(Line5, "              ");
        break;

    case 2:
        LcdSprintf(Line1, "       RECD");
        LcdSprintf(Line2, "    NDA=%d      ", recd[0]);
        LcdSprintf(Line3, "    NDB=%d      ", recd[1]);
        LcdSprintf(Line4, "    NHA=%d      ", recd[2]);
        LcdSprintf(Line5, "    NHB=%d      ", recd[3]);
        break;
    }
}
