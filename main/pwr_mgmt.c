/**
 * Copyright (c) 2020 Marius Vikhammer
 * 
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */




#include "pwr_mgmt.h"

#include "esp_log.h"
#include "esp_sleep.h"
#include "driver/gpio.h"

#include "board.h"
#include "mpu9250.h"

static void pwr_mgmt_wake_on_mov_init() {

    /* Wake on motion interrupt not feasible, too easily triggered */
    //mpu9250_motion_wakeup_enable(); 
    //const int ext_wakeup_imu_pin = IMU_INT_PIN;
    

    const int ext_wakeup_tp_pin = TOUCH_PIN;
    const uint64_t ext_wakeup_pin_mask = 1ULL << ext_wakeup_tp_pin;

    esp_sleep_enable_ext1_wakeup(ext_wakeup_pin_mask, ESP_EXT1_WAKEUP_ANY_HIGH);
}



void pwr_mgmt_deep_sleep() {
    pwr_mgmt_wake_on_mov_init();
    
    esp_deep_sleep_disable_rom_logging();
    esp_deep_sleep_start();
}
