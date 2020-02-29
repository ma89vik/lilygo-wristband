/**
 * Copyright (c) 2020 Marius Vikhammer
 * 
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */
#include "ota_screen.h"

#include "lvgl.h"
#include "esp_log.h"
#include "display.h"

static char *TAG = "ota screen";

static lv_obj_t *progress_arc, *cont, *msg;

void ota_screen_create(lv_obj_t *scr) {
    ESP_LOGI(TAG, "Creating OTA update screen");
                        
    display_aquire();

    /* Create label for title */
    lv_obj_t *title = lv_label_create(scr, NULL);


    static lv_style_t title_style;
    lv_style_copy(&title_style, &lv_style_plain);
    title_style.text.font = &lv_font_roboto_28;
    title_style.text.color = LV_COLOR_WHITE;
    lv_label_set_style(title, LV_LABEL_STYLE_MAIN, &title_style);

    lv_label_set_text(title, "OTA");
    lv_obj_align(title, NULL, LV_ALIGN_IN_TOP_MID, 0, 0);
    lv_label_set_align(title, LV_LABEL_ALIGN_CENTER);


    /*Create style for the Arcs*/
    static lv_style_t arc_style;
    lv_style_copy(&arc_style, &lv_style_plain);
    arc_style.line.color = LV_COLOR_YELLOW;           
    arc_style.line.width = 8;                      

    /*Create an Arc*/
    progress_arc = lv_arc_create(scr, NULL);

    lv_obj_set_hidden(progress_arc, 1);

    lv_arc_set_style(progress_arc, LV_ARC_STYLE_MAIN, &arc_style); 

    lv_obj_set_size(progress_arc, 50, 50);
    lv_obj_align(progress_arc, NULL, LV_ALIGN_CENTER, 0, 0);

    /* Create status msg label */
    msg = lv_label_create(scr, NULL);
    lv_obj_align(msg, NULL, LV_ALIGN_IN_BOTTOM_LEFT, 0, -10);
    lv_label_set_align(msg, LV_LABEL_ALIGN_LEFT);

    static lv_style_t msg_style;
    lv_style_copy(&msg_style, &lv_style_plain);
    msg_style.text.font = &lv_font_roboto_12;
    msg_style.text.color = LV_COLOR_WHITE;
    lv_label_set_style(msg, LV_LABEL_STYLE_MAIN, &msg_style);
    lv_label_set_text(msg, "Connecting");

    lv_scr_load(scr);

    display_release();


}

void ota_screen_set_progress(int pct) {
    
    display_aquire();
    
    uint16_t angle = pct*3.6;
    if(angle >= 359) {
        angle = 359;
    } 
    if(angle < 180) {
        lv_arc_set_angles(progress_arc, 180-angle ,180);
    } else {
        lv_arc_set_angles(progress_arc, 540-angle ,180);
    }

    if (lv_obj_get_hidden(progress_arc)) {
        lv_obj_set_hidden(progress_arc, 0);
    }
    

    display_release();
}

void ota_screen_set_msg(char* msg_str) {
    display_aquire();
    lv_label_set_text(msg, msg_str);
    display_release();
}