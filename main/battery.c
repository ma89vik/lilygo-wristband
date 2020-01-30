/**
 * Copyright (c) 2020 Marius Vikhammer
 * 
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */

#include "battery.h"

#include "board.h"

#include <driver/adc.h>
#include "esp_adc_cal.h"
#include "esp_log.h"

#define N_OF_SAMPLES 5
#define DEFAULT_VREF    1100

static char *TAG = "battery measurement";

static esp_adc_cal_characteristics_t adc_chars;
static adc_channel_t channel;

void battery_init() {
    channel = BATTERY_ADC_CH;

    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(channel ,ADC_ATTEN_DB_11);

    //Characterize ADC at particular atten
    esp_adc_cal_value_t val_type = esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_12, DEFAULT_VREF, &adc_chars);
    //Check type of calibration value used to characterize ADC
    if (val_type == ESP_ADC_CAL_VAL_EFUSE_VREF) {
        ESP_LOGI(TAG, "Calibrate: eFuse Vref %d", adc_chars.vref);
    } else if (val_type == ESP_ADC_CAL_VAL_EFUSE_TP) {
        ESP_LOGI(TAG, "Calibrate: Two Point");
    } else {
        ESP_LOGI(TAG, "Calibrate: Default");
    }
}

/* Returns the current battery level as a percentage between 0 - 100% */
float battery_lvl_read() {
    float val = 0;

    uint32_t adc_reading = 0;
    //Multisampling
    for (int i = 0; i < N_OF_SAMPLES; i++) {
        adc_reading += adc1_get_raw(channel);
    }
    adc_reading /= N_OF_SAMPLES;

    //Convert adc_reading to voltage in mV
    uint32_t voltage = esp_adc_cal_raw_to_voltage(adc_reading, &adc_chars);
    ESP_LOGD(TAG, "Raw: %d\tVoltage: %dmV\n", adc_reading, voltage);

    /* Measured over a voltage divider, *2 to get real battery voltage */
    val = 2*voltage / 1000.0;

    return val;
}