#include "app_env_monitor.h"
#include "bmp280.h"
#include "lcd_i2c.h"
#include "buzzer.h"
#include "main.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

extern I2C_HandleTypeDef hi2c1;
extern UART_HandleTypeDef huart2;

#define TEMP_LIMIT_X100        3500
#define SAMPLE_TIME_MS         2000

#define LCD_ADDR_1             0x27
#define LCD_ADDR_2             0x3F

static BMP280_HandleTypeDef bmp280;
static LCD_I2C_HandleTypeDef lcd;

static uint8_t sensor_ok = 0;
static uint8_t alarm_state = 0;

static void uart_send(const char *msg)
{
    HAL_UART_Transmit(&huart2,
                      (uint8_t *)msg,
                      strlen(msg),
                      HAL_MAX_DELAY);
}

static void format_x100(char *out, size_t out_size, int32_t value_x100)
{
    int32_t whole;
    int32_t frac;

    if (value_x100 < 0)
    {
        value_x100 = -value_x100;
        whole = value_x100 / 100;
        frac = value_x100 % 100;
        snprintf(out, out_size, "-%ld.%02ld", (long)whole, (long)frac);
    }
    else
    {
        whole = value_x100 / 100;
        frac = value_x100 % 100;
        snprintf(out, out_size, "%ld.%02ld", (long)whole, (long)frac);
    }
}

static void format_pressure_hpa(char *out, size_t out_size, uint32_t pressure_pa)
{
    uint32_t whole;
    uint32_t frac;

    /*
     * pressure_pa = pressure in Pascal.
     * hPa = Pa / 100.
     * So 101325 Pa = 1013.25 hPa.
     */
    whole = pressure_pa / 100;
    frac = pressure_pa % 100;

    snprintf(out, out_size, "%lu.%02lu", (unsigned long)whole, (unsigned long)frac);
}

void APP_EnvMonitor_Init(void)
{
    Buzzer_Init();

    /*
     * Most I2C LCD backpacks use address 0x27.
     * Some use 0x3F.
     */
    LCD_I2C_Init(&lcd, &hi2c1, LCD_ADDR_1, 20, 4);

    LCD_I2C_Clear(&lcd);
    LCD_I2C_SetCursor(&lcd, 0, 0);
    LCD_I2C_Print(&lcd, "Environmental");
    LCD_I2C_SetCursor(&lcd, 0, 1);
    LCD_I2C_Print(&lcd, "Monitoring Station");
    HAL_Delay(1000);

    if (BMP280_Init(&bmp280, &hi2c1) == HAL_OK)
    {
        sensor_ok = 1;

        LCD_I2C_Clear(&lcd);
        LCD_I2C_SetCursor(&lcd, 0, 0);
        LCD_I2C_Print(&lcd, "BMP280: OK");

        uart_send("Environmental Monitoring Station Started\r\n");
        uart_send("Temperature_C,Pressure_hPa,Alarm\r\n");
    }
    else
    {
        sensor_ok = 0;

        LCD_I2C_Clear(&lcd);
        LCD_I2C_SetCursor(&lcd, 0, 0);
        LCD_I2C_Print(&lcd, "BMP280 ERROR");
        LCD_I2C_SetCursor(&lcd, 0, 1);
        LCD_I2C_Print(&lcd, "Check wiring/I2C");

        uart_send("BMP280 ERROR: Check wiring, address, and I2C configuration\r\n");
    }
}

void APP_EnvMonitor_Run(void)
{
    static uint32_t last_tick = 0;

    int32_t temperature_x100;
    uint32_t pressure_pa;

    char temp_str[16];
    char press_str[16];
    char uart_buffer[96];

    if ((HAL_GetTick() - last_tick) < SAMPLE_TIME_MS)
    {
        return;
    }

    last_tick = HAL_GetTick();

    if (!sensor_ok)
    {
        Buzzer_Off();
        return;
    }

    if (BMP280_Read(&bmp280, &temperature_x100, &pressure_pa) == HAL_OK)
    {
        format_x100(temp_str, sizeof(temp_str), temperature_x100);
        format_pressure_hpa(press_str, sizeof(press_str), pressure_pa);

        if (temperature_x100 > TEMP_LIMIT_X100)
        {
            alarm_state = 1;
            Buzzer_On();
        }
        else
        {
            alarm_state = 0;
            Buzzer_Off();
        }

        LCD_I2C_ClearLine(&lcd, 0);
        LCD_I2C_Print(&lcd, "Env Monitor Station");

        LCD_I2C_ClearLine(&lcd, 1);
        LCD_I2C_Printf(&lcd, "Temp : %s C", temp_str);

        LCD_I2C_ClearLine(&lcd, 2);
        LCD_I2C_Printf(&lcd, "Press: %s hPa", press_str);

        LCD_I2C_ClearLine(&lcd, 3);
        if (alarm_state)
        {
            LCD_I2C_Print(&lcd, "ALERT: Temp > 35C");
        }
        else
        {
            LCD_I2C_Print(&lcd, "Status: Normal");
        }

        snprintf(uart_buffer,
                 sizeof(uart_buffer),
                 "%s,%s,%s\r\n",
                 temp_str,
                 press_str,
                 alarm_state ? "ON" : "OFF");

        uart_send(uart_buffer);
    }
    else
    {
        LCD_I2C_Clear(&lcd);
        LCD_I2C_SetCursor(&lcd, 0, 0);
        LCD_I2C_Print(&lcd, "Read Error");

        uart_send("BMP280 READ ERROR\r\n");

        Buzzer_Off();
    }
}
