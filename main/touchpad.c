/**
 * Copyright (c) 2020 Marius Vikhammer
 * 
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */

#include "touchpad.h"
#include "board.h"
#include "input.h"

#include "esp_log.h"
#include "driver/gpio.h"
#include "esp_timer.h"

#define LONG_PRESS_MS 2000
#define LONG_LONG_PRESS_MS 4000

static char *TAG = "touchpad";

static int touch_press_start;



static void IRAM_ATTR touchpad_isr() {
    int pressed = gpio_get_level(TOUCH_PIN);
    int time_pressed;
    int do_yield =0;

 ESP_EARLY_LOGE(TAG, "pressed %d", pressed);
    if (pressed) {
        touch_press_start = esp_timer_get_time();
    } else {
        time_pressed = esp_timer_get_time() - touch_press_start;

        if (time_pressed < LONG_PRESS_MS) {
            esp_event_isr_post(INPUT_EVENT, INPUT_PRESS_EVENT, NULL, 0, &do_yield);
        } 
        else if ( time_pressed < LONG_LONG_PRESS_MS) {
            esp_event_isr_post(INPUT_EVENT, INPUT_LONG_PRESS_EVENT, NULL, 0, &do_yield);
        } else {
            esp_event_isr_post(INPUT_EVENT, INPUT_LONG_LONG_PRESS_EVENT, NULL, 0, &do_yield);
        }
    }

    if(do_yield) {
        portYIELD_FROM_ISR();
    }

}

void touchpad_init(){
    gpio_config_t pwr_pin_config = {
        .pin_bit_mask = BIT64(TOUCH_POWER_PIN),
        .mode = GPIO_MODE_OUTPUT
    };
    ESP_ERROR_CHECK(gpio_set_level(TOUCH_POWER_PIN, 1));
    ESP_ERROR_CHECK(gpio_config(&pwr_pin_config));
    ESP_ERROR_CHECK(gpio_hold_en(TOUCH_POWER_PIN));
    gpio_deep_sleep_hold_en();

    gpio_config_t int_pin_config = {
        .pin_bit_mask = BIT64(TOUCH_PIN),
        .mode = GPIO_MODE_INPUT,
        .intr_type = GPIO_PIN_INTR_ANYEDGE
    };
    ESP_ERROR_CHECK(gpio_config(&int_pin_config));
    ESP_ERROR_CHECK(gpio_isr_handler_add(TOUCH_PIN, touchpad_isr, NULL));
}

