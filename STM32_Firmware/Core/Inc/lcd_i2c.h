#ifndef LCD_I2C_H
#define LCD_I2C_H

#include "stm32f4xx_hal.h"
#include <stdint.h>

typedef struct
{
    I2C_HandleTypeDef *hi2c;
    uint8_t addr;
    uint8_t cols;
    uint8_t rows;
    uint8_t backlight;
} LCD_I2C_HandleTypeDef;

HAL_StatusTypeDef LCD_I2C_Init(LCD_I2C_HandleTypeDef *lcd,
                               I2C_HandleTypeDef *hi2c,
                               uint8_t address_7bit,
                               uint8_t cols,
                               uint8_t rows);

void LCD_I2C_Clear(LCD_I2C_HandleTypeDef *lcd);
void LCD_I2C_SetCursor(LCD_I2C_HandleTypeDef *lcd, uint8_t col, uint8_t row);
void LCD_I2C_Print(LCD_I2C_HandleTypeDef *lcd, const char *str);
void LCD_I2C_Printf(LCD_I2C_HandleTypeDef *lcd, const char *fmt, ...);
void LCD_I2C_ClearLine(LCD_I2C_HandleTypeDef *lcd, uint8_t row);

#endif
