// Copyright (c) 2020 Marius Vikhammer
// 
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT
#pragma once

#include "esp_event.h"
#include "esp_types.h"

ESP_EVENT_DECLARE_BASE(INPUT_EVENT);

enum {
    INPUT_PRESS_EVENT,
    INPUT_LONG_PRESS_EVENT,
    INPUT_LONG_LONG_PRESS_EVENT,
} ;

void input_init();
