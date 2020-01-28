#define LOG_LOCAL_LEVEL 7
#include <stdio.h>
#include <string.h>
#include "esp_log.h"
#include "esp_system.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

#include "window.h"
#include "widget_grid.h"
#include "widget_icon.h"
#include "widget_rect.h"

#include "display.h"

#ifdef HAS_INPUT
#include "input.h"
#endif

#define  WINDOW_TASK_PRI 5
#define  WINDOW_TASK_STACK 8192

#define DRAW_EVENT_BIT (1 << 0)

static void window_clear();

static char* TAG = "Window";

static volatile widget_t* main_window;
static xSemaphoreHandle window_lock;

static EventGroupHandle_t window_evt_group;

static rect_t fullscreen = {0, SCREEN_WIDTH, 0, SCREEN_HEIGHT};

void window_task(void *pvParameters)
{
    EventBits_t window_evt_bits;
    while(1) {
        window_evt_bits = xEventGroupWaitBits(window_evt_group, DRAW_EVENT_BIT, pdTRUE, pdFALSE, 100 / portTICK_PERIOD_MS );

        xSemaphoreTake(window_lock, portMAX_DELAY);
        if (window_evt_bits && DRAW_EVENT_BIT) {
            ESP_LOGD(TAG, "draw window");
            if (main_window) {
                main_window->draw(main_window, fullscreen);
            }
        }
        xSemaphoreGive(window_lock);
        uint8_t x,y;

#ifdef HAS_INPUT
        if(xpt2046_read_xy(&x, &y) == ESP_OK) {

            ESP_LOGI(TAG, "pressed x: %d y: %d", x,y);

            widget_clicked(main_window, x, y);

        }
#endif
    }
}

esp_err_t window_init()
{
    ESP_LOGD(TAG, "Initializing window manager");

    window_clear();


    window_evt_group =  xEventGroupCreate();
    window_lock = xSemaphoreCreateMutex();

    if (!window_evt_group) {
        ESP_LOGE(TAG, "Failed to init event group");
        abort();
    }

    if (!window_lock) {
        ESP_LOGE(TAG, "Failed to init lock");
        abort();
    }

    xTaskCreate(window_task, "Window manager task", WINDOW_TASK_STACK, NULL, WINDOW_TASK_PRI, NULL);

    return ESP_OK;
}

void window_delete()
{
    widget_delete(main_window);
}

static void window_clear()
{
    display_fill_screen(WHITE);

}

void window_request_draw(widget_t *widget)
{
    ESP_LOGD(TAG, "Draw requested by %s", widget->name);

    /* This may be called from high priority functions, do not block
        loosing GUI updates is acceptable
    */
    xEventGroupSetBits(window_evt_group, DRAW_EVENT_BIT);
}

void window_set(widget_t* widget)
{
    ESP_LOGD(TAG, "Setting new main window to %s", widget->name);
    xSemaphoreTake(window_lock, portMAX_DELAY);
    main_window = widget;
    xSemaphoreGive(window_lock);

    window_request_draw(widget);
}




