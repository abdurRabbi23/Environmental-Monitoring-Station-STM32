#include "bmp280.h"

#define BMP280_REG_CALIB        0x88
#define BMP280_REG_ID           0xD0
#define BMP280_REG_RESET        0xE0
#define BMP280_REG_STATUS       0xF3
#define BMP280_REG_CTRL_MEAS    0xF4
#define BMP280_REG_CONFIG       0xF5
#define BMP280_REG_PRESS_MSB    0xF7

#define BMP280_CHIP_ID          0x58
#define BMP280_RESET_VALUE      0xB6

static HAL_StatusTypeDef bmp280_read_reg(BMP280_HandleTypeDef *bmp,
                                          uint8_t reg,
                                          uint8_t *data,
                                          uint16_t len)
{
    return HAL_I2C_Mem_Read(bmp->hi2c,
                            bmp->dev_addr,
                            reg,
                            I2C_MEMADD_SIZE_8BIT,
                            data,
                            len,
                            HAL_MAX_DELAY);
}

static HAL_StatusTypeDef bmp280_write_reg(BMP280_HandleTypeDef *bmp,
                                           uint8_t reg,
                                           uint8_t value)
{
    return HAL_I2C_Mem_Write(bmp->hi2c,
                             bmp->dev_addr,
                             reg,
                             I2C_MEMADD_SIZE_8BIT,
                             &value,
                             1,
                             HAL_MAX_DELAY);
}

static uint16_t u16_le(uint8_t lsb, uint8_t msb)
{
    return (uint16_t)((msb << 8) | lsb);
}

static int16_t s16_le(uint8_t lsb, uint8_t msb)
{
    return (int16_t)((msb << 8) | lsb);
}

static HAL_StatusTypeDef bmp280_read_calibration(BMP280_HandleTypeDef *bmp)
{
    uint8_t calib[24];

    if (bmp280_read_reg(bmp, BMP280_REG_CALIB, calib, 24) != HAL_OK)
    {
        return HAL_ERROR;
    }

    bmp->dig_T1 = u16_le(calib[0],  calib[1]);
    bmp->dig_T2 = s16_le(calib[2],  calib[3]);
    bmp->dig_T3 = s16_le(calib[4],  calib[5]);

    bmp->dig_P1 = u16_le(calib[6],  calib[7]);
    bmp->dig_P2 = s16_le(calib[8],  calib[9]);
    bmp->dig_P3 = s16_le(calib[10], calib[11]);
    bmp->dig_P4 = s16_le(calib[12], calib[13]);
    bmp->dig_P5 = s16_le(calib[14], calib[15]);
    bmp->dig_P6 = s16_le(calib[16], calib[17]);
    bmp->dig_P7 = s16_le(calib[18], calib[19]);
    bmp->dig_P8 = s16_le(calib[20], calib[21]);
    bmp->dig_P9 = s16_le(calib[22], calib[23]);

    return HAL_OK;
}

static int32_t bmp280_compensate_temperature(BMP280_HandleTypeDef *bmp,
                                             int32_t adc_T)
{
    int32_t var1, var2, T;

    var1 = ((((adc_T >> 3) - ((int32_t)bmp->dig_T1 << 1))) *
            ((int32_t)bmp->dig_T2)) >> 11;

    var2 = (((((adc_T >> 4) - ((int32_t)bmp->dig_T1)) *
              ((adc_T >> 4) - ((int32_t)bmp->dig_T1))) >> 12) *
            ((int32_t)bmp->dig_T3)) >> 14;

    bmp->t_fine = var1 + var2;

    T = (bmp->t_fine * 5 + 128) >> 8;

    return T;   // temperature in °C x100
}

static uint32_t bmp280_compensate_pressure(BMP280_HandleTypeDef *bmp,
                                           int32_t adc_P)
{
    int64_t var1, var2, p;

    var1 = ((int64_t)bmp->t_fine) - 128000;
    var2 = var1 * var1 * (int64_t)bmp->dig_P6;
    var2 = var2 + ((var1 * (int64_t)bmp->dig_P5) << 17);
    var2 = var2 + (((int64_t)bmp->dig_P4) << 35);

    var1 = ((var1 * var1 * (int64_t)bmp->dig_P3) >> 8) +
           ((var1 * (int64_t)bmp->dig_P2) << 12);

    var1 = (((((int64_t)1) << 47) + var1)) *
           ((int64_t)bmp->dig_P1) >> 33;

    if (var1 == 0)
    {
        return 0;
    }

    p = 1048576 - adc_P;
    p = (((p << 31) - var2) * 3125) / var1;

    var1 = (((int64_t)bmp->dig_P9) * (p >> 13) * (p >> 13)) >> 25;
    var2 = (((int64_t)bmp->dig_P8) * p) >> 19;

    p = ((p + var1 + var2) >> 8) + (((int64_t)bmp->dig_P7) << 4);

    return (uint32_t)(p / 256);   // pressure in Pa
}

HAL_StatusTypeDef BMP280_Init(BMP280_HandleTypeDef *bmp, I2C_HandleTypeDef *hi2c)
{
    uint8_t id;

    bmp->hi2c = hi2c;

    bmp->dev_addr = BMP280_I2C_ADDR_76 << 1;
    if (bmp280_read_reg(bmp, BMP280_REG_ID, &id, 1) != HAL_OK || id != BMP280_CHIP_ID)
    {
        bmp->dev_addr = BMP280_I2C_ADDR_77 << 1;
        if (bmp280_read_reg(bmp, BMP280_REG_ID, &id, 1) != HAL_OK || id != BMP280_CHIP_ID)
        {
            return HAL_ERROR;
        }
    }

    if (bmp280_write_reg(bmp, BMP280_REG_RESET, BMP280_RESET_VALUE) != HAL_OK)
    {
        return HAL_ERROR;
    }

    HAL_Delay(10);

    if (bmp280_read_calibration(bmp) != HAL_OK)
    {
        return HAL_ERROR;
    }

    /*
     * CONFIG register:
     * standby time = 1000 ms
     * IIR filter = x4
     */
    if (bmp280_write_reg(bmp, BMP280_REG_CONFIG, 0xA8) != HAL_OK)
    {
        return HAL_ERROR;
    }

    /*
     * CTRL_MEAS register:
     * temperature oversampling x1
     * pressure oversampling x4
     * normal mode
     *
     * osrs_t = 001
     * osrs_p = 011
     * mode   = 11
     */
    if (bmp280_write_reg(bmp, BMP280_REG_CTRL_MEAS, 0x2F) != HAL_OK)
    {
        return HAL_ERROR;
    }

    return HAL_OK;
}

HAL_StatusTypeDef BMP280_Read(BMP280_HandleTypeDef *bmp,
                              int32_t *temperature_x100,
                              uint32_t *pressure_pa)
{
    uint8_t data[6];

    int32_t adc_P;
    int32_t adc_T;

    if (bmp280_read_reg(bmp, BMP280_REG_PRESS_MSB, data, 6) != HAL_OK)
    {
        return HAL_ERROR;
    }

    adc_P = ((int32_t)data[0] << 12) |
            ((int32_t)data[1] << 4)  |
            ((int32_t)data[2] >> 4);

    adc_T = ((int32_t)data[3] << 12) |
            ((int32_t)data[4] << 4)  |
            ((int32_t)data[5] >> 4);

    *temperature_x100 = bmp280_compensate_temperature(bmp, adc_T);
    *pressure_pa = bmp280_compensate_pressure(bmp, adc_P);

    return HAL_OK;
}
