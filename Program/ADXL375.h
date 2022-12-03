#ifndef  _ADXL375_H
#define  _ADXL375_H

#include "struct_typedef.h"

/**Register Map Begin**/
#define ID                      0x00
#define THRESH_SHOCK            0x1D
#define OFSX                    0x1E
#define OFSY                    0x1F
#define OFSZ                    0x20
#define DUR                     0x21
#define Latent                  0x22
#define Window                  0x23
#define THRESH_ACT              0x24
#define THRESH_INACT            0x25
#define TIME_INACT              0x26
#define ACT_INACT_CTL           0x27
#define SHOCK_AXES              0x2A
#define ACT_SHOCK_STATUS        0x2B
#define BW_RATE                 0x2C
#define POWER_CTL               0x2D
#define INT_ENABLE              0x2E
#define INT_MAP                 0x2F
#define INT_SOURCE              0x30
#define DATA_FORMAT             0x31
#define DATAX0                  0x32
#define DATAX1                  0x33
#define DATAY0                  0x34
#define DATAY1                  0x35
#define DATAZ0                  0x36
#define DATAZ1                  0x37
#define FIFO_CTL                0x38
#define FIFO_STATU              0x39
#define ADXL375ID               0xe5
/**Register Map End**/

#define SPI2_CS_PIN  GPIO_PIN_12
#define SPI2_CS_PORT  GPIOB

#ifdef __cplusplus //C与CPP交叉编译所添加的宏定义
extern "C" {
#endif
extern void adxl_write (uint8_t address, uint8_t value);
extern void adxl_read (uint8_t address);
extern void adxl_init (void);
extern void adxl_Handle(void);
extern int16_t x,y,z;
extern float xg,yg,zg;
#ifdef __cplusplus
}
#endif

#endif