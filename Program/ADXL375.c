#ifndef  _ADXL375_C
#define _ADXL375_C

#include "ADXL375.h"
#include "stm32f4xx_hal.h"
#include "spi.h"

uint8_t data_rec[6];


void adxl_write (uint8_t address, uint8_t value)
{
    uint8_t data[2];
    data[0] = address|0x40;  // multibyte write enabled
    data[1] = value;
    HAL_GPIO_WritePin (SPI2_CS_PORT, SPI2_CS_PIN, GPIO_PIN_RESET); // pull the cs pin low to enable the slave
    HAL_SPI_Transmit (&hspi2, data, 2, 100);  // transmit the address and data
    HAL_GPIO_WritePin (SPI2_CS_PORT, SPI2_CS_PIN, GPIO_PIN_SET); // pull the cs pin high to disable the slave
}

void adxl_read (uint8_t address)
{
    address |= 0x80;  // read operation
    address |= 0x40;  // multibyte read
    HAL_GPIO_WritePin (SPI2_CS_PORT, SPI2_CS_PIN, GPIO_PIN_RESET);  // pull the cs pin low to enable the slave
    HAL_SPI_Transmit (&hspi2, &address, 1, 100);  // send the address from where you want to read data
    HAL_SPI_Receive (&hspi2, data_rec, 6, 100);  // read 6 BYTES of data
    HAL_GPIO_WritePin (SPI2_CS_PORT, SPI2_CS_PIN, GPIO_PIN_SET);  // pull the cs pin high to disable the slave
}

void adxl_init (void)
{
    adxl_write (DATA_FORMAT, 0x0f);  // data_format
    adxl_write (POWER_CTL, 0x00);  // reset all bits
    adxl_write (POWER_CTL, 0x08);  // power_cntl measure and wake up 8hz
    //adxl_write (0x2c, 0x06); // output speed
}

int16_t x,y,z;
float xg,yg,zg;
void adxl_Handle(void)
{
    // READ DATA
    adxl_read (DATAX0);
    x = (int16_t)((data_rec[1]<<8)|data_rec[0]);
    y = (int16_t)((data_rec[3]<<8)|data_rec[2]);
    z = (int16_t)((data_rec[5]<<8)|data_rec[4]);

    // Convert into 'g'
    xg = x*0.049;
    yg = y*0.049;
    zg = z*0.049;
}
#endif
/* #ifndef   */

