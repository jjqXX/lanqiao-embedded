#include "lcd_app.h"

/**
 * @brief  格式化字符串并显示在指定的LCD行上
 *
 * 该函数接收一个行号和一个格式化字符串,类似于printf函数
 * 格式化字符串后,将其显示在LCD的指定行上
 *
 * @param  Line    要显示字符串的LCD行号
 * @param  format  格式化字符串,可包含要格式化的参数
 */
void LcdSprintf(uint8_t Line, char *format, ...)
{
    char String[21];
    va_list arg;
    va_start(arg, format);
    vsprintf(String, format, arg);
    va_end(arg);
    LCD_DisplayStringLine(Line, String);
}

uint8_t lcd_disp_mode = 0;                          // 0:行程界面,1:参数界面,2:统计界面
const char *st_value[3] = {"Idle", "Busy", "Wait"}; // 空闲,忙碌,等待
uint8_t st_index = 0;                               // 0:空闲,1:忙碌,2:等待
uint16_t cp_value[2] = {0, 0};                       // 设备当前位置(x,y)
uint16_t tp_value[2] = {0, 0};                      // 下一个途经地坐标(x,y)
float se_value = 0.0f;                              // 运行状态下的速度值
uint8_t rn_value = 0;                               // 剩余途经地(含目的地)数量


float r_value = 1.0f;  // 1.0~2.0,单位为(cm/s)/Hz
uint8_t b_value = 10;  // 10~100,单位为cm/s
uint8_t r_b_index = 0; // 0:r,1:b

float ts_value = 0.0f; // 累计行程
uint32_t tt_value = 0; // 累计运行时间

void lcd_proc(void)
{
    switch (lcd_disp_mode)
    {
    case 0:
        LcdSprintf(Line1, "        DATA       ");
        LcdSprintf(Line3, "     ST=%s        ", st_value[st_index]);
        LcdSprintf(Line4, "     CP=%d,%d     ", cp_value[0], cp_value[1]);
        st_index == 0 ? LcdSprintf(Line5, "     TP=NF       ") : LcdSprintf(Line5, "     TP=%d,%d     ", tp_value[0], tp_value[1]);
        LcdSprintf(Line6, "     SE=%.1f      ", se_value);
        st_index == 0 ? LcdSprintf(Line7, "     RN=NF       ") : LcdSprintf(Line7, "     RN=%d        ", rn_value);
        break;

    case 1:
        LcdSprintf(Line1, "        PARA       ");
        LcdSprintf(Line3, "     R=%.1f        ", r_value);
        LcdSprintf(Line4, "     B=%d        ", b_value);
        LcdSprintf(Line5, "                      ");
        LcdSprintf(Line6, "                      ");
        LcdSprintf(Line7, "                      ");
        break;

    case 2:
        LcdSprintf(Line1, "        RECD       ");
        LcdSprintf(Line3, "     TS=%.1f        ", ts_value);
        LcdSprintf(Line4, "     TT=%d         ", tt_value);
        LcdSprintf(Line5, "                      ");
        LcdSprintf(Line6, "                      ");
        LcdSprintf(Line7, "                      ");
        break;
    }
}
