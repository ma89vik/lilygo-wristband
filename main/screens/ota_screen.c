/**
 * Copyright (c) 2020 Marius Vikhammer
 * 
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */
#include "ota_screen.h"

#include "lvgl.h"
#include "esp_log.h"

static char *TAG = "ota screen";

static lv_obj_t *progress_arc, *cont, *msg;

void ota_screen_create(lv_obj_t *scr) {
  ESP_LOGI(TAG, "Creating OTA update screen");
  
  /*Create a window*/
  cont = lv_cont_create(scr, NULL);
                      

  /*Create style for the Arcs*/
  static lv_style_t style;
  lv_style_copy(&style, &lv_style_plain);
  style.line.color = LV_COLOR_BLUE;           /*Arc color*/
  style.line.width = 8;                       /*Arc width*/

  /*Create an Arc*/
  progress_arc = lv_arc_create(cont, NULL);
  lv_arc_set_style(progress_arc, LV_ARC_STYLE_MAIN, &style);          /*Use the new style*/
  lv_arc_set_angles(progress_arc, 180, 200);
  lv_obj_set_size(progress_arc, 50, 50);
  lv_obj_align(progress_arc, NULL, LV_ALIGN_CENTER, 0, 0);

  /* Create label for status update/error message */
  //msg = lv_label_create(win, NULL);
  //lv_label_set_long_mode(msg, LV_LABEL_LONG_BREAK);
  //lv_label_set_text(msg, "Initializing OTA update");

  lv_scr_load(scr);
  
}

void ota_screen_set_progress(int pct) {
  lv_arc_set_angles(progress_arc, 180, 180);
}

void ota_screen_set_error(char* error) {
    lv_label_set_text(msg, error);

}