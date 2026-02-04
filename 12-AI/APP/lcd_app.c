#include "lcd_app.h"
#include "parking_app.h"

/**
 * @brief  格式化字符串并显示在指定的LCD行上
 *
 * @param  Line    要显示字符串的LCD行号
 * @param  format  格式化字符串
 */
void LcdSprintf(uint8_t Line, char *format,...)
{
    char String[21];
    va_list arg;
    va_start(arg, format);
    vsprintf(String, format, arg);
    va_end(arg);
    LCD_DisplayStringLine(Line, (uint8_t*)String);
}

/**
 * @brief 显示车位界面(Data)
 *
 * 界面布局:
 * 第2行: "       Data"
 * 第4行: "     CNBR:X"
 * 第6行: "     VNBR:X"
 * 第8行: "     IDLE:X"
 */
static void lcd_show_data_page(void)
{
    LcdSprintf(Line2, "       Data         ");
    LcdSprintf(Line3, "                    ");
    LcdSprintf(Line4, "     CNBR:%d         ", cnbr_count);
    LcdSprintf(Line5, "                    ");
    LcdSprintf(Line6, "     VNBR:%d         ", vnbr_count);
    LcdSprintf(Line7, "                    ");
    LcdSprintf(Line8, "     IDLE:%d         ", idle_count);
    LcdSprintf(Line9, "                    ");
}

/**
 * @brief 显示费率设置界面(Para)
 *
 * 界面布局:
 * 第2行: "       Para"
 * 第4行: "     CNBR:X.XX"
 * 第6行: "     VNBR:X.XX"
 */
static void lcd_show_para_page(void)
{
    // 将费率转换为浮点数显示
    float cnbr_rate_f = (float)cnbr_rate / 100.0f;
    float vnbr_rate_f = (float)vnbr_rate / 100.0f;

    LcdSprintf(Line2, "       Para         ");
    LcdSprintf(Line3, "                    ");
    LcdSprintf(Line4, "     CNBR:%.2f      ", cnbr_rate_f);
    LcdSprintf(Line5, "                    ");
    LcdSprintf(Line6, "     VNBR:%.2f      ", vnbr_rate_f);
    LcdSprintf(Line7, "                    ");
    LcdSprintf(Line8, "                    ");
    LcdSprintf(Line9, "                    ");
}

/**
 * @brief LCD处理函数
 *
 * 根据当前显示模式切换显示内容
 */
void lcd_proc(void)
{
    if (lcd_mode == LCD_MODE_DATA)
    {
        lcd_show_data_page();
    }
    else if (lcd_mode == LCD_MODE_PARA)
    {
        lcd_show_para_page();
    }
}
