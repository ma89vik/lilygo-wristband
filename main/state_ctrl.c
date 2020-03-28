/**
 * Copyright (c) 2020 Marius Vikhammer
 * 
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */

#include "state_ctrl.h"

#include "esp_log.h"
#include "lvgl.h"
#include "esp_event.h"


#include "pwr_mgmt.h"
#include "display.h"
#include "desktop.h"
#include "menu.h"
#include "ota.h"
#include "ota_screen.h"
#include "time_sync.h"

static char *TAG = "state_ctrl";

ESP_EVENT_DEFINE_BASE(STATE_CTRL_EVT);

/* Task responsible for handling transitioning between the different states
	listens for events that will trigger a transition and:
	1. Cleans up the old screen
	2. Starts any functionality required by the screen, e.g. desktop->start clock reading
	3. Create and load the new screen
*/

typedef enum {
    WATCH_DESKTOP,
    WATCH_MENU,
    WATCH_OTA,
    WATCH_PROV,

} watch_state_t;

static lv_obj_t *desktop_scr, *cur_scr;
static watch_state_t watch_state;
static esp_event_handler_t state_ctrl_evt_loop;

static void state_ctrl_trans_handler(void* event_handler_arg, 
                                        esp_event_base_t event_base, 
                                        int32_t event_id, 
                                        void* event_data) {
	ESP_LOGE(TAG, "screen evt %d", event_id);

    /* State transitions are relatively few and simple so far, 
        no need for a more complex structure */

    switch (event_id) {

        case STATE_CTRL_PRESS_EVT:
            switch (watch_state) {
                case WATCH_DESKTOP:
                    pwr_mgmt_deep_sleep();
                    return;
                case WATCH_MENU:
                case WATCH_OTA:
                case WATCH_PROV:
                    return;
            }

        case STATE_CTRL_LONG_PRESS_EVT:
            switch (watch_state) {
                case WATCH_DESKTOP:
                    menu_create(cur_scr);
                    watch_state = WATCH_MENU;
                    return;
                case WATCH_MENU:
                case WATCH_OTA:
                case WATCH_PROV:
                    display_aquire();                    
                    lv_scr_load(desktop_scr);
                    lv_obj_clean(cur_scr);
                    display_release();
                    watch_state = WATCH_DESKTOP;

                    return;
            }

        case STATE_CTRL_OTA_SCREEN_EVT:
            switch (watch_state) {
                case WATCH_MENU:
                    display_aquire();     
                    lv_obj_clean(cur_scr);
                    display_release();
                    ota_screen_create(cur_scr);                    
 #ifdef CONFIG_OTA_ENABLE
                    ota_init();
 #endif      
                     watch_state = WATCH_OTA;
                    return;
                case WATCH_DESKTOP:
                case WATCH_OTA:
                    return;
            } 

        case STATE_CTRL_SNTP_SCREEN_EVT:
            switch (watch_state) {
                case WATCH_MENU:
                    time_sync();
                    return;
                case WATCH_DESKTOP:
                case WATCH_OTA:
                    return;
            }
    }
}

void state_ctrl_init() {

    desktop_scr = lv_obj_create(NULL, NULL);
    cur_scr = lv_obj_create(NULL, NULL);
    desktop_create(desktop_scr);
    lv_scr_load(desktop_scr);

    watch_state = WATCH_DESKTOP;          
	/* Event handler for watch state machine, e.g. screen transitions */
        esp_event_loop_args_t loop_args = {
        .queue_size = 10,
        .task_name = "state ctrl event loop",
        .task_priority = 5,
        .task_stack_size = 8000,
    };


    esp_event_loop_create(&loop_args, &state_ctrl_evt_loop);
	esp_event_handler_register_with(state_ctrl_evt_loop, STATE_CTRL_EVT, ESP_EVENT_ANY_ID, state_ctrl_trans_handler, NULL);


}

void state_ctrl_post_evt(state_ctrl_evt_id id){

    esp_event_post_to(state_ctrl_evt_loop, STATE_CTRL_EVT, id, NULL, 0, 0);
}

void state_ctrl_isr_post_evt(state_ctrl_evt_id id, int* do_yield){
    esp_event_isr_post_to(state_ctrl_evt_loop, STATE_CTRL_EVT, id, NULL, 0, do_yield);

}

