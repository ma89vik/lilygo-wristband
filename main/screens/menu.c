/**
 * Copyright (c) 2020 Marius Vikhammer
 * 
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */

#include "menu.h"

#include "esp_log.h"
#include "lvgl.h"
#include "esp_event.h"

#include "state_ctrl.h"
#include "ota.h"
#include "display.h"

static char *TAG = "menu";

void focus_cb(struct _lv_group_t *g){
    lv_list_focus(*(g->obj_focus), LV_ANIM_OFF);
}


lv_obj_t *menu_list;

static void menu_ota_button_clicked_evt(lv_obj_t * obj, lv_event_t event) {
    if(event == LV_EVENT_CLICKED) {
        state_ctrl_post_evt(STATE_CTRL_OTA_SCREEN_EVT);
    }
}

static void menu_sntp_button_clicked_evt(lv_obj_t * obj, lv_event_t event) {
    if(event == LV_EVENT_CLICKED) {
        state_ctrl_post_evt(STATE_CTRL_SNTP_SCREEN_EVT);
    }
}

void menu_create(lv_obj_t *scr)
{
    display_aquire();
    
    lv_group_t * g = lv_group_create();

    /*Create a list*/
    menu_list = lv_list_create(scr, NULL);
    lv_obj_set_size(menu_list, LV_HOR_RES_MAX, LV_VER_RES_MAX);
    lv_obj_align(menu_list, NULL, LV_ALIGN_CENTER, 0, 0);

    /*Add buttons to the list*/

    lv_obj_t * list_btn;

    list_btn = lv_list_add_btn(menu_list, LV_SYMBOL_DOWNLOAD, "OTA");
    lv_obj_set_event_cb(list_btn, menu_ota_button_clicked_evt);
    lv_group_add_obj(g, list_btn);

    list_btn = lv_list_add_btn(menu_list, LV_SYMBOL_BELL, "SNTP");
    lv_obj_set_event_cb(list_btn, menu_sntp_button_clicked_evt);
    lv_group_add_obj(g, list_btn);

    list_btn = lv_list_add_btn(menu_list, LV_SYMBOL_CHARGE, "Laser");
    lv_group_add_obj(g, list_btn);

    list_btn = lv_list_add_btn(menu_list, LV_SYMBOL_CUT, "Sharks");
    lv_group_add_obj(g, list_btn);

  

    lv_indev_set_group(get_disp_driver(), g);
    lv_group_set_focus_cb(g, focus_cb );

    lv_scr_load(scr);

    display_release();

}