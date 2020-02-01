// Copyright (c) 2020 Marius Vikhammer
// 
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include "esp_event.h"

ESP_EVENT_DECLARE_BASE(STATE_CTRL_EVT);

enum {
    STATE_CTRL_PRESS_EVT,
    STATE_CTRL_LONG_PRESS_EVT,
    STATE_CTRL_OTA_SCREEN_EVT,
};


/* Starts the state machine responsible for controlling 
    the main behaviour of the system, e.g. screen transitioning and sleep 
*/
void state_ctrl_init();