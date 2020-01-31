/**
 * Copyright (c) 2020 Marius Vikhammer
 * 
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */



#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "board.h"
#include "pwr_mgmt.h"
#include "desktop.h"

#include "pcf8563.h"
#include "mpu9250.h"
#include "acc_filter.h"
#include "battery.h"
#include "time.h"


static char* TAG = "main";

void app_main(void)
{
    ESP_LOGE(TAG, "App started");
    
    board_init();

    struct tm time;
    while(1) {

        pcf8563_read_time(&time);
        desktop_update_time(&time);
        desktop_update_battery(battery_lvl_read());
        vTaskDelay(5000 / portTICK_PERIOD_MS);
    }

    /*vTaskDelay(5000 / portTICK_PERIOD_MS);
    ESP_LOGE(TAG, "Preparing for sleep");
    pwr_mgmt_deep_sleep();*/

}
