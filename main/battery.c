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

/* Every step in the table represent approx 7% */
#define PCT_PER_I 7
static float v_to_pct_table[] = {3.3, 3.4, 3.51, 3.56, 3.63, 3.68, 3.71, 3.75, 3.79, 3.85, 3.90, 3.97, 4.06, 4.2};

static uint8_t volt_to_battery_pct(float v);

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
uint8_t battery_lvl_read() {
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

    return volt_to_battery_pct(val);
}

static uint8_t volt_to_battery_pct(float v) {
    uint8_t pct = 0;
    for (int i = 1; i<sizeof(v_to_pct_table)/sizeof(v_to_pct_table[0]); i++){
        if (v < v_to_pct_table[i]) {
            ESP_LOGD(TAG, "v=%f, mod %d rem %f", v, PCT_PER_I*i, PCT_PER_I*(v_to_pct_table[i] - v)/(v_to_pct_table[i] - v_to_pct_table[i-1]));
            pct = PCT_PER_I*i -  PCT_PER_I*(v_to_pct_table[i] - v)/(v_to_pct_table[i] - v_to_pct_table[i-1]);
            return pct;
        }
        
    }
    return 100;
}