/**
 * Copyright (c) 2020 Marius Vikhammer
 * 
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */

#include "display.h"


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "driver/gpio.h"
#include "lvgl/lvgl.h"
#include "esp_freertos_hooks.h"

#include "sys/lock.h"
#include "disp_spi.h"
#include "st7735.h"
#include "input.h"
#include "esp_log.h"

static void IRAM_ATTR lv_tick_task(void);
static lv_color_t *buf1;

static _lock_t s_lock;

void display_aquire() {
    _lock_acquire(&s_lock);
}

void display_release() {
    _lock_release(&s_lock);

}

void display_lvgl_task(void *vParameters) {
	while(1) {
		vTaskDelay(1);
		display_aquire();
		lv_task_handler();
		display_release();
	}
}

lv_indev_t* input_dev;

lv_indev_t* get_disp_driver( ) {
	return input_dev;
}


void display_init()
{
	lv_init();

	disp_spi_init();
	st7735_init();

    buf1 = heap_caps_malloc(sizeof(lv_color16_t) *  DISP_BUF_SIZE , MALLOC_CAP_DMA);

    static lv_disp_buf_t disp_buf;
    lv_disp_buf_init(&disp_buf, buf1, NULL,  DISP_BUF_SIZE);

	/* Set up display callbacks */
	lv_disp_drv_t disp_drv;
	lv_disp_drv_init(&disp_drv);
	disp_drv.flush_cb = st7735_flush;
	disp_drv.buffer = &disp_buf;
	lv_disp_drv_register(&disp_drv);

	/* Set up input callbacks */	
    lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.read_cb = input_read;
    indev_drv.type = LV_INDEV_TYPE_KEYPAD;
    input_dev = lv_indev_drv_register(&indev_drv);

	/*Initialize the night theme */
	lv_theme_t *th = lv_theme_night_init(100, NULL);
	lv_theme_set_current(th);

	esp_register_freertos_tick_hook_for_cpu(lv_tick_task, 0);
	xTaskCreatePinnedToCore(display_lvgl_task, "littlevGL", 16000, NULL, 10, NULL, 0);
}





static void IRAM_ATTR lv_tick_task(void)
{
	lv_tick_inc(portTICK_RATE_MS);
	
}