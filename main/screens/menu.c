/**
 * Copyright (c) 2020 Marius Vikhammer
 * 
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */

#include "menu.h"

#include "esp_log.h"
#include "lvgl.h"
#include "display.h"

static char *TAG = "menu";



void focus_cb(struct _lv_group_t *g){
    lv_list_focus(*(g->obj_focus), LV_ANIM_OFF);
}



static void event_handler(lv_obj_t * obj, lv_event_t event)
{
    if(event == LV_EVENT_CLICKED) {
        ESP_LOGE(TAG, "Clicked: %s\n", lv_list_get_btn_text(obj));
    }
}

void menu_create(void)
{
    lv_group_t * g = lv_group_create();

    /*Create a list*/
    lv_obj_t * list1 = lv_list_create(lv_scr_act(), NULL);
    lv_obj_set_size(list1, 160, 200);
    lv_obj_align(list1, NULL, LV_ALIGN_CENTER, 0, 0);

    /*Add buttons to the list*/

    lv_obj_t * list_btn;

    list_btn = lv_list_add_btn(list1, LV_SYMBOL_DOWNLOAD, "OTA");
    lv_obj_set_event_cb(list_btn, event_handler);
    lv_group_add_obj(g, list_btn);

    list_btn = lv_list_add_btn(list1, LV_SYMBOL_DIRECTORY, "Open");
    lv_obj_set_event_cb(list_btn, event_handler);
    lv_group_add_obj(g, list_btn);

    list_btn = lv_list_add_btn(list1, LV_SYMBOL_CLOSE, "Delete");
    lv_obj_set_event_cb(list_btn, event_handler);
    lv_group_add_obj(g, list_btn);

    list_btn = lv_list_add_btn(list1, LV_SYMBOL_EDIT, "Edit");
    lv_obj_set_event_cb(list_btn, event_handler);
    lv_group_add_obj(g,list_btn);

    list_btn = lv_list_add_btn(list1, LV_SYMBOL_SAVE, "Save");
    lv_obj_set_event_cb(list_btn, event_handler);
    lv_group_add_obj(g, list_btn);

    lv_indev_set_group(get_disp_driver(), g);
    lv_group_set_focus_cb(g, focus_cb );
}