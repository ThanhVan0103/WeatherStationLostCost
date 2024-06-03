#ifndef I2CBME280_H
#define I2CBME280_H

#include "esp_err.h"
#include "bme280.h"

void BME280_delay_msek(u32 msek);
void i2c_master_init();
s8 BME280_I2C_bus_write(u8 dev_addr, u8 reg_addr, u8 *reg_data, u8 cnt);
s8 BME280_I2C_bus_read(u8 dev_addr, u8 reg_addr, u8 *reg_data, u8 cnt);

#endif // I2CBME280_H