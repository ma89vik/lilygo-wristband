#pragma once

#include "esp_system.h"
#include "sys/lock.h"
#include "driver/i2c.h"



esp_err_t mpu9250_init(i2c_port_t port, _lock_t *i2c_lock);
void mpu9250_motion_wakeup_enable();
void mpu9250_read_acc(int8_t *x, int8_t *y);
