#include "board.h"

#include "driver/i2c.h"
#include "driver/gpio.h"
#include "mpu9250.h"
#include "display.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#define ESP_INTR_FLAG_DEFAULT 0

const i2c_port_t i2c_port = I2C_NUM_1;
static void i2c_init();


void board_init() {
    // Driver inits
    i2c_init();

    // Peripherals init
    mpu9250_init(i2c_port);
    // RTC
    display_init();
    
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


