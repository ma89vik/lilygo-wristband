/**
 * Copyright (c) 2020 Marius Vikhammer
 * 
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */
#include "imu.h"

#include "mpu9250.h"
#include "acc_filter.h"
#include "input.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_timer.h"

#define SAMPLE_FREQ_HZ 25

#define INPUT_TRESH_POS_Y 10
#define INPUT_TRESH_NEG_Y -10

#define INPUT_LIMIT_MS 1000

static char *TAG = "imu";

static int imu_input_limit(int *time_since_last_input);

static int timestamp_last_y_input;

static void imu_get_input_task(void* vParameters) {
    
    TickType_t xLastWakeTime;
    const TickType_t xFrequency = (1000/SAMPLE_FREQ_HZ) / portTICK_PERIOD_MS;

     // Initialise the xLastWakeTime variable with the current time.
    xLastWakeTime = xTaskGetTickCount();
    
    accFilter filt_y;
    accFilter_init(&filt_y);

    int8_t acc_x, acc_y;
    float x,y;
    while(1) {
        mpu9250_read_acc(&acc_x, &acc_y);

        /* High pass filter input */ 
        accFilter_put(&filt_y, (float)acc_y);   

        y = accFilter_get(&filt_y);  
        
        if (y > INPUT_TRESH_POS_Y) {
            if(imu_input_limit(&timestamp_last_y_input)){
                input_post(INPUT_DOWN_EVENT);
            }
                
        }
        else if (y < INPUT_TRESH_NEG_Y) {
            if(imu_input_limit(&timestamp_last_y_input)) {
                input_post(INPUT_UP_EVENT);
            }
                
        }

        vTaskDelayUntil( &xLastWakeTime, xFrequency );
    }
}

void imu_input_init() {
	    xTaskCreate(imu_get_input_task, "imu inpu", 2000, NULL, 4, NULL);    
}

/* Limit the trigger of input per axis to once every X sec, this is mainly to filter out
    triggers due to "breaking" */
static int imu_input_limit(int *time_since_last_input) {
    int cur_time = esp_timer_get_time()/1000;
    if (cur_time - *time_since_last_input > INPUT_LIMIT_MS) {
        *time_since_last_input = cur_time;
        return 1;
    } else {
        return 0;
    }

}