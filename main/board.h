#pragma once

#include "esp_system.h"
#include "driver/i2c.h"
#include "soc/adc_channel.h"

#define TFT_MOSI_PIN    19
#define TFT_SCKL_PIN    18
#define TFT CS_PIN      5
#define TFT_DC_PIN      23
#define TFT_RST_PIN     26
#define TFT_BL_PIN      27
#define TOUCH_PIN       33
#define TOUCH_POWER_PIN 25
#define IMU_INT_PIN     39
#define RTC_INT_PIN     34
#define BATTERY_ADC_CH  ADC1_GPIO35_CHANNEL
#define VBUS_ADC_PIN    ADC1_GPIO36_CHANNEL
#define I2C_SDA_PIN     21
#define I2C_SCL_PIN     22
#define LED_PIN         4


void board_init();
