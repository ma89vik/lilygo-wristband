/**
 * Copyright (c) 2020 Marius Vikhammer
 * 
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */



#include "board.h"

#include "driver/i2c.h"
#include "driver/gpio.h"
#include "mpu9250.h"
#include "pcf8563.h"
#include "display.h"
#include "battery.h"
#include "input.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "sys/lock.h"
#include "esp_event.h"

#include "time.h"

const i2c_port_t i2c_port = I2C_NUM_1;
static _lock_t s_i2c_lock;


static void i2c_init();
static void board_lcd_enable();
static void board_lcd_backlight(bool enable);

void board_init() {
    //install gpio isr service
    gpio_install_isr_service(0);    
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    // Driver inits
    i2c_init();


    // Peripherals init
    mpu9250_init(i2c_port, &s_i2c_lock);
    // RTC
    pcf8563_init(i2c_port, &s_i2c_lock);

    board_lcd_enable();
    board_lcd_backlight(true);
    input_init();
    display_init();

    

    // Battery measurement
    battery_init();
    
}


static void board_lcd_enable(void)
{
    gpio_config_t pins_config = {
        .pin_bit_mask = BIT64(TFT_RST_PIN) | BIT64(TFT_BL_PIN) | BIT64(TFT_DC_PIN),
        .mode = GPIO_MODE_OUTPUT
    };
    ESP_ERROR_CHECK(gpio_config(&pins_config));
    gpio_set_level(TFT_RST_PIN, 1);
}

static void board_lcd_backlight(bool enable)
{
    gpio_set_level(TFT_BL_PIN, enable);
    
}



static void i2c_init() {

    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_SDA_PIN,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_io_num = I2C_SCL_PIN,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = 200000
    };

    ESP_ERROR_CHECK(i2c_param_config(i2c_port, &conf));
    ESP_ERROR_CHECK(i2c_driver_install(i2c_port, I2C_MODE_MASTER, 0, 0, 0));

}

