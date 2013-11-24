#ifndef _I2C_H
#define _I2C_H 1

#include <stdint.h>

void I2Cconfigure(void);
int  I2CwriteDeviceRegister(uint8_t, uint8_t, uint8_t);
int  I2CreadDeviceRegister(uint8_t, uint8_t, uint8_t *);

#endif
