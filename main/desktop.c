/**
 * Copyright (c) 2020 Marius Vikhammer
 * 
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */

#include "desktop.h"

#include "esp_log.h"
#include <stdio.h>
#include <time.h>

#include "lvgl.h"
#include "display.h"
#include "battery.h"

static char* TAG = "desktop";

typedef struct {
    lv_style_t clock_style;
    lv_style_t date_style;

    lv_obj_t *cont;
    lv_obj_t *min;
    lv_obj_t *hour;
    lv_obj_t *date;

} time_widget_t;

typedef struct {
    lv_style_t title_style;

    lv_obj_t *cont;
    lv_obj_t *battery;
    lv_obj_t *battery_pct;

} title_widget_t;

LV_FONT_DECLARE(roboto_numbers_36)

static time_widget_t time_widget;
static title_widget_t title_widget;

void create_title_bar_cont(title_widget_t *title) {
    title->cont = lv_cont_create(lv_scr_act(), NULL);

    lv_obj_set_auto_realign(title->cont, true);                    /*Auto realign when the size changes*/
    lv_obj_align(title->cont, NULL, LV_ALIGN_IN_TOP_RIGHT, 0, 0);
    lv_cont_set_fit(title->cont, LV_FIT_TIGHT);
    lv_cont_set_layout(title->cont, LV_LAYOUT_ROW_M);


    lv_cont_set_style(title->cont, LV_LABEL_STYLE_MAIN, &lv_style_transp);

    
    title->battery_pct =  lv_label_create(title->cont, NULL);
    title->battery =  lv_label_create(title->cont, NULL);

    
    lv_style_copy(&title->title_style, &lv_style_plain);
    title->title_style.text.font = &lv_font_roboto_12;

    lv_label_set_style(title->battery_pct, LV_LABEL_STYLE_MAIN, &title->title_style);
    lv_label_set_style(title->battery, LV_LABEL_STYLE_MAIN, &title->title_style);

    lv_label_set_text(title->battery_pct, "");
    lv_label_set_text(title->battery, "");
}

void desktop_update_battery(float battery_pct) {
    char bat_str[10];
    display_aquire();

    snprintf(bat_str, sizeof(bat_str), "%.2f%%", battery_lvl_read());
    lv_label_set_text(title_widget.battery_pct, bat_str);
    lv_label_set_text(title_widget.battery, LV_SYMBOL_BATTERY_FULL);
    display_release();

}

void desktop_update_time(struct tm *time) {
    char date_buf[10];
    display_aquire();
    snprintf(date_buf, sizeof(date_buf), "%02d", time->tm_min);
    lv_label_set_text(time_widget.min , date_buf);
    
    snprintf(date_buf, sizeof(date_buf), "%02d", time->tm_hour);
    lv_label_set_text(time_widget.hour , date_buf);

    snprintf(date_buf, sizeof(date_buf), "%d/%d", time->tm_mday, time->tm_mon);
    lv_label_set_text(time_widget.date , date_buf);
    display_release();
}

void create_clock_cont(time_widget_t *time_widget)
{
    time_widget->cont = lv_cont_create(lv_scr_act(), NULL);
   
    lv_cont_set_style(time_widget->cont, LV_LABEL_STYLE_MAIN, &lv_style_transp);

    lv_obj_set_auto_realign(time_widget->cont, true);                    /*Auto realign when the size changes*/
    lv_obj_align_origo(time_widget->cont, NULL, LV_ALIGN_CENTER, 0, 0);  /*This parametrs will be sued when realigned*/
    lv_cont_set_fit(time_widget->cont, LV_FIT_TIGHT);
    lv_cont_set_layout(time_widget->cont, LV_LAYOUT_COL_M);

    time_widget->hour =  lv_label_create(time_widget->cont, NULL);
    time_widget->min =  lv_label_create(time_widget->cont, NULL);
    time_widget->date =  lv_label_create(time_widget->cont, NULL);
    
    lv_style_copy(&time_widget->clock_style, &lv_style_plain);
    time_widget->clock_style.text.font = &roboto_numbers_36;
    
    lv_style_copy(&time_widget->date_style, &lv_style_plain);
    time_widget->date_style.text.font = &lv_font_roboto_12;

    lv_label_set_style(time_widget->hour, LV_LABEL_STYLE_MAIN, &time_widget->clock_style);
    lv_label_set_style(time_widget->min, LV_LABEL_STYLE_MAIN, &time_widget->clock_style);
    lv_label_set_style(time_widget->date, LV_LABEL_STYLE_MAIN, &time_widget->date_style);

    lv_label_set_text(time_widget->min , "");
    lv_label_set_text(time_widget->hour , "");
    lv_label_set_text(time_widget->date , "");
}

void create_desktop()
{
    ESP_LOGE(TAG, "Creating desktop");

    create_title_bar_cont(&title_widget);
    create_clock_cont(&time_widget);
    /*Create the clock on the currently active screen*/
    

}

void desktop_init()
{
    display_aquire();
    create_desktop();
    display_release();
    
}