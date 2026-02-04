#include "lcd_app.h"


uint8_t lcd_disp_mode = 0;
uint8_t lcd_data_flag = 0;
uint8_t duty[2] = {10,10}; 


void LcdSprintf(uint8_t Line, char *format,...)
{
    char String[21];  
    va_list arg;      
    va_start(arg, format); 
    vsprintf(String, format, arg); 
    va_end(arg); 
    LCD_DisplayStringLine(Line,String);  
}

void lcd_proc(void)
{
    if(lcd_disp_mode == 0)
    {
        LcdSprintf(Line0,"      Data       ");
        LcdSprintf(Line2,"    V:%.2f       ",adc_value[0]);
        LcdSprintf(Line4,"    Mode:%s      ",lcd_data_flag == 0?"AUTO":"MANU");
    }
    else
    {
        LcdSprintf(Line0,"      Para       ");
        LcdSprintf(Line2,"    PA6:%d%%        ",duty[0]);
        LcdSprintf(Line4,"    PA7:%d%%       ",duty[1]);
    }
}
