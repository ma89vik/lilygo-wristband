/* SPI Master example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "driver/gpio.h"
#include "lvgl/lvgl.h"
#include "lv_examples/lv_apps/demo/demo.h"
#include "esp_freertos_hooks.h"


#include "sys/lock.h"
#include "disp_spi.h"
#include "st7735.h"

static void IRAM_ATTR lv_tick_task(void);

static lv_color_t *buf1;

static _lock_t s_lock;

void display_aquire() {
    _lock_acquire(&s_lock);
}

void display_release() {
    _lock_release(&s_lock);

}

void *display_lvgl_task(void *vParameters) {
	while(1) {
		vTaskDelay(1);
		display_aquire();
		lv_task_handler();
		display_release();
	}
}

void display_init()
{
	lv_init();

	disp_spi_init();
	st7735_init();


    buf1 = malloc(sizeof(lv_color16_t) *  DISP_BUF_SIZE);

    static lv_disp_buf_t disp_buf;
    lv_disp_buf_init(&disp_buf, buf1, NULL,  DISP_BUF_SIZE);

	lv_disp_drv_t disp_drv;
	lv_disp_drv_init(&disp_drv);
	disp_drv.flush_cb = st7735_flush;
	disp_drv.buffer = &disp_buf;
	lv_disp_drv_register(&disp_drv);


	esp_register_freertos_tick_hook(lv_tick_task);

	xTaskCreate(display_lvgl_task, "littlevGL", 8000, NULL, 5, NULL);

}

static void IRAM_ATTR lv_tick_task(void)
{
	lv_tick_inc(portTICK_RATE_MS);
}