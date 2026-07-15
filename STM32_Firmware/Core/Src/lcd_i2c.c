#include "lcd_i2c.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#define LCD_RS      0x01
#define LCD_RW      0x02
#define LCD_EN      0x04
#define LCD_BL      0x08

static HAL_StatusTypeDef lcd_write_expander(LCD_I2C_HandleTypeDef *lcd,
                                            uint8_t data)
{
    return HAL_I2C_Master_Transmit(lcd->hi2c,
                                   lcd->addr,
                                   &data,
                                   1,
                                   HAL_MAX_DELAY);
}

static void lcd_pulse_enable(LCD_I2C_HandleTypeDef *lcd, uint8_t data)
{
    lcd_write_expander(lcd, data | LCD_EN);
    HAL_Delay(1);

    lcd_write_expander(lcd, data & ~LCD_EN);
    HAL_Delay(1);
}

static void lcd_send4bits(LCD_I2C_HandleTypeDef *lcd, uint8_t data)
{
    lcd_write_expander(lcd, data | lcd->backlight);
    lcd_pulse_enable(lcd, data | lcd->backlight);
}

static void lcd_send(LCD_I2C_HandleTypeDef *lcd, uint8_t value, uint8_t mode)
{
    uint8_t high_nibble;
    uint8_t low_nibble;

    high_nibble = value & 0xF0;
    low_nibble  = (value << 4) & 0xF0;

    lcd_send4bits(lcd, high_nibble | mode);
    lcd_send4bits(lcd, low_nibble  | mode);
}

static void lcd_cmd(LCD_I2C_HandleTypeDef *lcd, uint8_t cmd)
{
    lcd_send(lcd, cmd, 0);
}

static void lcd_data(LCD_I2C_HandleTypeDef *lcd, uint8_t data)
{
    lcd_send(lcd, data, LCD_RS);
}

HAL_StatusTypeDef LCD_I2C_Init(LCD_I2C_HandleTypeDef *lcd,
                               I2C_HandleTypeDef *hi2c,
                               uint8_t address_7bit,
                               uint8_t cols,
                               uint8_t rows)
{
    lcd->hi2c = hi2c;
    lcd->addr = address_7bit << 1;
    lcd->cols = cols;
    lcd->rows = rows;
    lcd->backlight = LCD_BL;

    HAL_Delay(50);

    lcd_cmd(lcd, 0x33);
    HAL_Delay(5);

    lcd_cmd(lcd, 0x32);
    HAL_Delay(5);

    lcd_cmd(lcd, 0x28);   // 4-bit mode, 2-line/4-line LCD mode
    HAL_Delay(1);

    lcd_cmd(lcd, 0x0C);   // display on, cursor off
    HAL_Delay(1);

    lcd_cmd(lcd, 0x06);   // entry mode
    HAL_Delay(1);

    LCD_I2C_Clear(lcd);

    return HAL_OK;
}

void LCD_I2C_Clear(LCD_I2C_HandleTypeDef *lcd)
{
    lcd_cmd(lcd, 0x01);
    HAL_Delay(2);
}

void LCD_I2C_SetCursor(LCD_I2C_HandleTypeDef *lcd, uint8_t col, uint8_t row)
{
    static const uint8_t row_offsets[] = {0x00, 0x40, 0x14, 0x54};

    if (row >= lcd->rows)
    {
        row = lcd->rows - 1;
    }

    if (col >= lcd->cols)
    {
        col = lcd->cols - 1;
    }

    lcd_cmd(lcd, 0x80 | (col + row_offsets[row]));
}

void LCD_I2C_Print(LCD_I2C_HandleTypeDef *lcd, const char *str)
{
    while (*str)
    {
        lcd_data(lcd, (uint8_t)(*str));
        str++;
    }
}

void LCD_I2C_Printf(LCD_I2C_HandleTypeDef *lcd, const char *fmt, ...)
{
    char buffer[32];

    va_list args;
    va_start(args, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);

    LCD_I2C_Print(lcd, buffer);
}

void LCD_I2C_ClearLine(LCD_I2C_HandleTypeDef *lcd, uint8_t row)
{
    LCD_I2C_SetCursor(lcd, 0, row);

    for (uint8_t i = 0; i < lcd->cols; i++)
    {
        LCD_I2C_Print(lcd, " ");
    }

    LCD_I2C_SetCursor(lcd, 0, row);
}
