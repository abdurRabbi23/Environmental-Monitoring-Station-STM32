#ifndef BMP280_H
#define BMP280_H

#include "stm32f4xx_hal.h"
#include <stdint.h>

#define BMP280_I2C_ADDR_76      (0x76)
#define BMP280_I2C_ADDR_77      (0x77)

typedef struct
{
    I2C_HandleTypeDef *hi2c;
    uint8_t dev_addr;

    uint16_t dig_T1;
    int16_t  dig_T2;
    int16_t  dig_T3;

    uint16_t dig_P1;
    int16_t  dig_P2;
    int16_t  dig_P3;
    int16_t  dig_P4;
    int16_t  dig_P5;
    int16_t  dig_P6;
    int16_t  dig_P7;
    int16_t  dig_P8;
    int16_t  dig_P9;

    int32_t t_fine;
} BMP280_HandleTypeDef;

HAL_StatusTypeDef BMP280_Init(BMP280_HandleTypeDef *bmp, I2C_HandleTypeDef *hi2c);
HAL_StatusTypeDef BMP280_Read(BMP280_HandleTypeDef *bmp,
                              int32_t *temperature_x100,
                              uint32_t *pressure_pa);

#endif
