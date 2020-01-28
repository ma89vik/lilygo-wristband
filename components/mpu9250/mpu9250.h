#pragma once

#include "esp_system.h"

esp_err_t mpu9250_init();
void mpu9250_motion_wakeup_enable();
