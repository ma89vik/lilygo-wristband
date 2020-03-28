// Copyright (c) 2020 Marius Vikhammer
// 
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include "esp_event.h"

ESP_EVENT_DECLARE_BASE(STATE_CTRL_EVT);

typedef enum {
    STATE_CTRL_PRESS_EVT,
    STATE_CTRL_LONG_PRESS_EVT,
    STATE_CTRL_OTA_SCREEN_EVT,
    STATE_CTRL_SNTP_SCREEN_EVT,
} state_ctrl_evt_id;


/* Starts the state machine responsible for controlling 
    the main behaviour of the system, e.g. screen transitioning and sleep 
*/
void state_ctrl_init();

/* Post event to the state ctrl event loop */
void state_ctrl_isr_post_evt(state_ctrl_evt_id id, int* do_yield); 
void state_ctrl_post_evt(state_ctrl_evt_id id);
