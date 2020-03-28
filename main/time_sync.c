/* LwIP SNTP example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include "time_sync.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_sntp.h"
#include "wifi_manager.h"
#include "pcf8563.h"

static const char *TAG = "time";

static void obtain_time(void);
static void initialize_sntp(void);

static void time_sync_notification_cb(struct timeval *tv)
{
    ESP_LOGI(TAG, "Notification of a time synchronization event");
}

void time_sync_init()
{
    /* Set time zone to China */
    setenv("TZ", "CST-8", 1);
    tzset();        

    struct tm cur_time;
    pcf8563_read_time(&cur_time);

    printf("prev time %ld\n", time(NULL));
    time_t t = mktime(&cur_time);
    printf("Setting time: %s \n", asctime(&cur_time));
    struct timeval now = { .tv_sec = t };
    settimeofday(&now, NULL);

    printf("new time %ld\n", time(NULL));
}

void time_update_rtc()
{
    time_t rawtime;
    struct tm info;
    time( &rawtime );
    localtime_r( &rawtime, &info );
    pcf8563_set_time( &info);
}

esp_err_t time_sync()
{
 
    obtain_time();
    time_update_rtc();
    return ESP_OK;
}

static void obtain_time(void)
{
    wifi_manager_request_access();

    initialize_sntp();

    int retry = 0;
    const int retry_count = 20;
    while (sntp_get_sync_status() == SNTP_SYNC_STATUS_RESET && ++retry < retry_count) {
        ESP_LOGI(TAG, "Waiting for system time to be set... (%d/%d)", retry, retry_count);
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }

    if (retry < retry_count){
        time_update_rtc();
    }

}

static void initialize_sntp(void)
{
    ESP_LOGI(TAG, "Initializing SNTP");
    sntp_setoperatingmode(SNTP_OPMODE_POLL);
    sntp_setservername(0, "pool.ntp.org");
    sntp_set_time_sync_notification_cb(time_sync_notification_cb);

    sntp_init();
}
