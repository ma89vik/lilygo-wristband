/**
 * Copyright (c) 2020 Marius Vikhammer
 * 
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */
#include "input.h"
#include "touchpad.h"

ESP_EVENT_DEFINE_BASE(INPUT_EVENT);


void input_init() {
    touchpad_init();
}