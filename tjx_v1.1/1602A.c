#include "1602A.h"

void lcd_init()
{
    delay(15);
    lcd_write(0, LCD_CMD_MODE);
    delay(5);
    lcd_write(0, LCD_CMD_MODE);
    delay(5);
    lcd_write(0, LCD_CMD_MODE);
    delay(5);
    lcd_write_cmd(LCD_CMD_MODE); // 8位总线, 双行, 5x7
    delay(5);
    lcd_write_cmd(LCD_CMD_DISPLAY | 0x4 | 0x2); // 开启显示, 关闭光标, 不闪烁
    delay(5);
    lcd_write_cmd(LCD_CMD_CLR);
    delay(5);
    lcd_write_cmd(LCD_CMD_ENTRY | 0x2); // 光标右移, 字符不移位
    delay(5);
    lcd_write_cmd(LCD_CMD_RST);
    delay(5);
}

byte lcd_read(bit type)
{
    byte content;
    LCD_RS = type;
    LCD_RW = 1;
    LCD_E = 1;
#ifdef LCD_8BIT_DB
    content = LCD_DB;
#else
    // TODO 未完成
#endif
    return content;
}

byte lcd_read_state()
{
    return lcd_read(0);
}

byte lcd_read_dat()
{
    while (lcd_busy());
    return lcd_read(1);
}

static bit lcd_busy()
{
    return (bit) (lcd_read_state() & 0x80);
}

void lcd_write(bit type, byte content)
{
    LCD_RS = type;
    LCD_RW = 0;
#ifdef LCD_8BIT_DB
    LCD_DB = content;
#else
    LCD_DB &= 0x0F; //清高四位
    LCD_DB |= (content & 0xF0); //送高四位
    LCD_E = 1;
    _nop_();
    LCD_E = 0;
    LCD_DB &= 0x0F; //清高四位
    LCD_DB |= (content << 4); //送低四位
#endif
    LCD_E = 1;
    _nop_();
    LCD_E = 0;
}

void lcd_write_cmd(byte cmd)
{
    while (lcd_busy());
    lcd_write(0, cmd);
}

void lcd_write_dat(byte dat)
{
    while (lcd_busy());
    lcd_write(1, dat);
}

void lcd_write_cgram(byte pos, byte *arr)
{
    byte i;
    pos <<= 3; // pos *= 8, 获得地址
    pos |= 0x40; // 设定CGRAM地址命令
    for (i = 0; i < 8; i++)
    {
        lcd_write_cmd(pos);
        lcd_write_dat(*arr);
        pos++;
        arr++;
    }
}

void lcd_set_pos(bit row, byte col)
{
        lcd_write_cmd(0x80 | (0x40 * row) | col);
}

void lcd_disp(bit row, byte col, byte ch)
{
    lcd_set_pos(row, col);
    lcd_write_dat(ch);
}

void lcd_print(byte *str)
{
    while (*str) lcd_write_dat(*str++);
}
