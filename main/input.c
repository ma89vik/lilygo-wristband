/**
 * Copyright (c) 2020 Marius Vikhammer
 * 
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */
#include "input.h"
#include "touchpad.h"
#include "imu.h"

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "esp_log.h"

static QueueHandle_t input_event_queue;

static int evt_to_lvgl_key(input_event_t evt);

void input_init() {
    input_event_queue = xQueueCreate(10, sizeof(input_event_t));

    touchpad_init();
    imu_input_init();
}

void input_post(input_event_t evt){
    xQueueSend(input_event_queue, &evt, 0);
}
void input_post_from_isr(input_event_t evt, int *do_yield){
    xQueueSendFromISR(input_event_queue, &evt, do_yield);

}

bool input_read(lv_indev_drv_t * drv, lv_indev_data_t*data) {
    input_event_t evt = 0;
    static input_event_t last_evt = 0;

    if (last_evt != 0) {
        /* Released the previously pressed key before handling any new events */
        data->key = last_evt;
        data->state = LV_INDEV_STATE_REL;
        last_evt = 0;

        return uxQueueMessagesWaiting(input_event_queue);
    }
    
    if(xQueueReceive(input_event_queue, &evt, 0) == pdTRUE) {
        data->key = evt_to_lvgl_key(evt);
        data->state = LV_INDEV_STATE_PR;
        last_evt = evt;

        // Signal to lvgl that there is more events, to trigger release of key */
        return 1;
    }

    return 0;
}

static int evt_to_lvgl_key(input_event_t evt) {
       switch (evt) {
            case INPUT_UP_EVENT:
                return LV_KEY_NEXT;
            case INPUT_DOWN_EVENT:
                return LV_KEY_PREV;
            case INPUT_PRESS_EVENT:
                return LV_KEY_ENTER;
            case INPUT_LONG_PRESS_EVENT:
                return LV_KEY_ESC;
            case INPUT_LONG_LONG_PRESS_EVENT:
            default:
                return 0;
       }
}
