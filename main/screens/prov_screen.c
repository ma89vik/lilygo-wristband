/**
 * Copyright (c) 2020 Marius Vikhammer
 * 
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */
#include "prov_screen.h"

#include "lvgl.h"
#include "esp_log.h"
#include "display.h"

static char *TAG = "prov screen";

static lv_obj_t *cont, *msg;

void prov_screen_create(lv_obj_t *scr) {
    ESP_LOGI(TAG, "Creating provisioning screen");
                        
    display_aquire();

    /* Create label for title */
    lv_obj_t *title = lv_label_create(scr, NULL);


    static lv_style_t title_style;
    lv_style_copy(&title_style, &lv_style_plain);
    title_style.text.font = &lv_font_roboto_28;
    title_style.text.color = LV_COLOR_WHITE;
    lv_label_set_style(title, LV_LABEL_STYLE_MAIN, &title_style);

    lv_label_set_text(title, "Provisioning");
    lv_obj_align(title, NULL, LV_ALIGN_IN_TOP_MID, 0, 0);
    lv_label_set_align(title, LV_LABEL_ALIGN_CENTER);


    /* Create status msg label */
    msg = lv_label_create(scr, NULL);
    lv_obj_align(msg, NULL, LV_ALIGN_IN_BOTTOM_LEFT, 0, -10);
    lv_label_set_align(msg, LV_LABEL_ALIGN_LEFT);

    static lv_style_t msg_style;
    lv_style_copy(&msg_style, &lv_style_plain);
    msg_style.text.font = &lv_font_roboto_12;
    msg_style.text.color = LV_COLOR_WHITE;
    lv_label_set_style(msg, LV_LABEL_STYLE_MAIN, &msg_style);
    lv_label_set_text(msg, "Waiting");

    lv_scr_load(scr);

    display_release();


}


void ota_screen_set_msg(char* msg_str) {
    display_aquire();
    lv_label_set_text(msg, msg_str);
    display_release();
}