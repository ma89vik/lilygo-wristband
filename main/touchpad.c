/**
 * Copyright (c) 2020 Marius Vikhammer
 * 
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */

#include "touchpad.h"
#include "board.h"
#include "input.h"
#include "state_ctrl.h"

#include "esp_log.h"
#include "driver/gpio.h"
#include "esp_timer.h"

#define LONG_PRESS_MS 1000

static char *TAG = "touchpad";

static int touch_press_start;



static void IRAM_ATTR touchpad_isr() {
    int pressed = gpio_get_level(TOUCH_PIN);
    int time_pressed;
    int do_yield =0;
    int do_yield2 =0;


    if (pressed) {
        touch_press_start = esp_timer_get_time()/1000;
    } else {
        time_pressed = esp_timer_get_time()/1000 - touch_press_start;

        if (time_pressed < LONG_PRESS_MS) {
            input_post_from_isr(INPUT_PRESS_EVENT, &do_yield);            
            /* Also post to event base used for state control */
            state_ctrl_isr_post_evt(STATE_CTRL_PRESS_EVT, &do_yield2);
        } 
        else {
            input_post_from_isr(INPUT_LONG_PRESS_EVENT, &do_yield);
            /* Also post to event base used for state control */
            state_ctrl_isr_post_evt(STATE_CTRL_LONG_PRESS_EVT, &do_yield2);

        }
    }

    if(do_yield || do_yield2) {
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

