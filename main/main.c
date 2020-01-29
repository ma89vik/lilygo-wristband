/* Hello World Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "board.h"
#include "pwr_mgmt.h"
#include "desktop.h"


void board_lcd_enable(void)
{
    gpio_config_t pins_config = {
        .pin_bit_mask = BIT64(TFT_RST_PIN) | BIT64(TFT_BL_PIN) | BIT64(TFT_DC_PIN),
        .mode = GPIO_MODE_OUTPUT
    };
    ESP_ERROR_CHECK(gpio_config(&pins_config));
    gpio_set_level(TFT_RST_PIN, 1);
}

void board_lcd_backlight(bool enable)
{
    gpio_set_level(TFT_BL_PIN, enable);
}


static char* TAG = "main";

void app_main(void)
{
    ESP_LOGE(TAG, "App started");

    board_lcd_enable();
    board_lcd_backlight(true);

    
    board_init();
    desktop_init();
    //vTaskDelay(2000 / portTICK_PERIOD_MS);
    //ESP_LOGE(TAG, "Preparing for sleep");
    //pwr_mgmt_deep_sleep();

}
