// Copyright (c) 2020 Marius Vikhammer
// 
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT
#pragma once

#include "esp_event.h"
#include "esp_types.h"

#include "lvgl.h"

typedef enum {
    INPUT_NO_EVENT = 0,
    INPUT_PRESS_EVENT,
    INPUT_LONG_PRESS_EVENT,
    INPUT_LONG_LONG_PRESS_EVENT,
    INPUT_LEFT_EVENT,
    INPUT_RIGHT_EVENT,
    INPUT_UP_EVENT,
    INPUT_DOWN_EVENT,
} input_event_t ;

void input_init();

/* Post input event to event queue */
void input_post(input_event_t evt);
void input_post_from_isr(input_event_t evt, int *do_yield);

/* Read input callback for LittlevGL */
bool input_read(lv_indev_drv_t * drv, lv_indev_data_t*data);

