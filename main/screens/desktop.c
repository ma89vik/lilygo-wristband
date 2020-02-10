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

/* Weather icons */
LV_IMG_DECLARE(d1_icon);
LV_IMG_DECLARE(d2_icon);
LV_IMG_DECLARE(d3_icon);

typedef struct {
    weather_type_t type;
    const lv_img_dsc_t * img;
} weather_img_map_entry;

static weather_img_map_entry icon_map[]= {
    {WEATHER_SUN, &d1_icon},
    {WEATHER_FEW_CLOUDS, &d2_icon},
    {WEATHER_SCATTERED_CLOUDS, &d3_icon},
    {WEATHER_BROKEN_CLOUDS, &d2_icon},
    {WEATHER_SHOWER_RAIN, &d2_icon},
    {WEATHER_RAIN, &d2_icon},
    {WEATHER_THUNDERSTORM, &d2_icon},
    {WEATHER_SNOW, &d2_icon},
    {WEATHER_MIST, &d2_icon},
    {WEATHER_UNKOWN, NULL},
};

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

typedef struct {
    lv_style_t weather_style;

    lv_obj_t *cont;
    lv_obj_t *pm25;

    lv_obj_t *weather_icon;
    lv_obj_t *temp;

} weather_widget_t;

LV_FONT_DECLARE(roboto_numbers_36)

static time_widget_t time_widget;
static title_widget_t title_widget;
static weather_widget_t weather_widget;

static void desktop_create_title_widget(title_widget_t *title, lv_obj_t *scr) {
    title->cont = lv_cont_create(scr, NULL);

    lv_obj_set_auto_realign(title->cont, true);                    /*Auto realign when the size changes*/
    lv_obj_align(title->cont, NULL, LV_ALIGN_IN_TOP_RIGHT, 0, 0);
    lv_cont_set_fit(title->cont, LV_FIT_TIGHT);
    lv_cont_set_layout(title->cont, LV_LAYOUT_ROW_M);

    lv_cont_set_style(title->cont, LV_LABEL_STYLE_MAIN, &lv_style_transp);

    
    title->battery_pct =  lv_label_create(title->cont, NULL);
    title->battery =  lv_label_create(title->cont, NULL);

    
    lv_style_copy(&title->title_style, &lv_style_plain);
    title->title_style.text.font = &lv_font_roboto_12;
    title->title_style.text.color = LV_COLOR_WHITE;
;


    lv_label_set_style(title->battery_pct, LV_LABEL_STYLE_MAIN, &title->title_style);
    lv_label_set_style(title->battery, LV_LABEL_STYLE_MAIN, &title->title_style);

    lv_label_set_text(title->battery_pct, "");
    lv_label_set_text(title->battery, "");
}

void desktop_update_battery(uint8_t battery_pct) {
    char bat_str[10];
    display_aquire();
    snprintf(bat_str, sizeof(bat_str), "%d%%", battery_pct);
    lv_label_set_text(title_widget.battery_pct, bat_str);

    if (battery_pct > 90) {
        lv_label_set_text(title_widget.battery, LV_SYMBOL_BATTERY_FULL);
    }
    else if (battery_pct > 65 ) {
        lv_label_set_text(title_widget.battery, LV_SYMBOL_BATTERY_3);
    }
    else if (battery_pct > 40 ) {
        lv_label_set_text(title_widget.battery, LV_SYMBOL_BATTERY_2);
    }
    else if (battery_pct > 10 ) {
        lv_label_set_text(title_widget.battery, LV_SYMBOL_BATTERY_1);
    } else {
        lv_label_set_text(title_widget.battery, LV_SYMBOL_BATTERY_EMPTY);
    }

    display_release();

}

void desktop_update_time() {
    char date_buf[10];
    time_t cur_time = time(NULL);
    ESP_LOGE(TAG, "time %lu", cur_time);
    struct tm cur_tm = { 0 };
    localtime_r(&cur_time, &cur_tm);

    display_aquire();
    snprintf(date_buf, sizeof(date_buf), "%02d", cur_tm.tm_min);
    lv_label_set_text(time_widget.min , date_buf);
    
    snprintf(date_buf, sizeof(date_buf), "%02d", cur_tm.tm_hour);
    lv_label_set_text(time_widget.hour , date_buf);
    
    /* Months are [0-11] */
    snprintf(date_buf, sizeof(date_buf), "%d/%d", cur_tm.tm_mday%99, (cur_tm.tm_mon+1)%99);
    lv_label_set_text(time_widget.date , date_buf);
    display_release();

}

static void desktop_create_clock_widget(time_widget_t *time_widget, lv_obj_t *scr)
{
    time_widget->cont = lv_cont_create(scr, NULL);
   
    lv_cont_set_style(time_widget->cont, LV_LABEL_STYLE_MAIN, &lv_style_transp);

    lv_obj_set_auto_realign(time_widget->cont, true);                    /*Auto realign when the size changes*/
    lv_obj_align_origo(time_widget->cont, NULL, LV_ALIGN_CENTER, 0, -10);  /*This parametrs will be sued when realigned*/
    lv_cont_set_fit(time_widget->cont, LV_FIT_TIGHT);
    lv_cont_set_layout(time_widget->cont, LV_LAYOUT_COL_M);

    time_widget->hour =  lv_label_create(time_widget->cont, NULL);
    time_widget->min =  lv_label_create(time_widget->cont, NULL);
    time_widget->date =  lv_label_create(time_widget->cont, NULL);
    
    lv_style_copy(&time_widget->clock_style, &lv_style_plain);
    time_widget->clock_style.text.font = &roboto_numbers_36;
    time_widget->clock_style.text.color = LV_COLOR_WHITE;
    
    lv_style_copy(&time_widget->date_style, &lv_style_plain);
    time_widget->date_style.text.font = &lv_font_roboto_12;
    time_widget->date_style.text.color = LV_COLOR_WHITE;


    lv_label_set_style(time_widget->hour, LV_LABEL_STYLE_MAIN, &time_widget->clock_style);
    lv_label_set_style(time_widget->min, LV_LABEL_STYLE_MAIN, &time_widget->clock_style);
    lv_label_set_style(time_widget->date, LV_LABEL_STYLE_MAIN, &time_widget->date_style);

    lv_label_set_text(time_widget->min , "");
    lv_label_set_text(time_widget->hour , "");
    lv_label_set_text(time_widget->date , "");
}

static void desktop_create_weather_widget(weather_widget_t *weather_widget, lv_obj_t *scr)
{
    weather_widget->cont = lv_cont_create(scr, NULL);
   
    lv_cont_set_style(weather_widget->cont, LV_LABEL_STYLE_MAIN, &lv_style_transp);

    lv_obj_set_auto_realign(weather_widget->cont, true);                    /*Auto realign when the size changes*/
    lv_obj_align(weather_widget->cont, NULL, LV_ALIGN_IN_BOTTOM_LEFT, 0, 0);
    lv_cont_set_fit(weather_widget->cont, LV_FIT_TIGHT);
    lv_cont_set_layout(weather_widget->cont, LV_LAYOUT_PRETTY);

    /* Weather icon + temp */
    lv_style_copy(&weather_widget->weather_style, &lv_style_plain);
    weather_widget->weather_style.text.font = &lv_font_roboto_12;
    weather_widget->weather_style.text.color = LV_COLOR_WHITE;

    weather_widget->weather_icon = lv_img_create(weather_widget->cont, NULL);
    weather_widget->temp = lv_label_create(weather_widget->cont, NULL);
    
    lv_img_set_src(weather_widget->weather_icon, &d2_icon);

    lv_label_set_style(weather_widget->temp, LV_LABEL_STYLE_MAIN, &weather_widget->weather_style);
    lv_label_set_text(weather_widget->temp , "10");

    /* PM 2.5 */
    weather_widget->pm25 =  lv_label_create(weather_widget->cont, NULL);

    lv_label_set_style(weather_widget->pm25, LV_LABEL_STYLE_MAIN, &weather_widget->weather_style);
    lv_label_set_text(weather_widget->pm25 , "PM 2.5: 0");

}

void desktop_update_weather(float temp, weather_type_t weather_type){
    char weather_buf[15];
    display_aquire();

    snprintf(weather_buf, sizeof(weather_buf), "%.1f", temp);

    bool img_found = false;
    for (int i = 0; i<sizeof(icon_map) / sizeof(weather_img_map_entry); i++) {
        if (weather_type == icon_map[i].type){
            lv_img_set_src(weather_widget.weather_icon, icon_map[i].img);
            img_found = true;
        }
    }

    if (!img_found) {
        ESP_LOGE(TAG, "Failed to find weather icon %d", weather_type);
    }
    lv_label_set_text(weather_widget.temp, weather_buf);

    display_release();
}

void desktop_update_pm25(uint16_t pm25) {
    char weather_buf[15];
    display_aquire();

    snprintf(weather_buf, sizeof(weather_buf), "PM 2.5: %u", pm25 % 999);
    lv_label_set_text(weather_widget.pm25, weather_buf);
    display_release();
}



void desktop_create(lv_obj_t *scr)
{
    ESP_LOGE(TAG, "Creating desktop");
    display_aquire();
    desktop_create_title_widget(&title_widget, scr);
    desktop_create_clock_widget(&time_widget,scr );
    desktop_create_weather_widget(&weather_widget,scr );

    display_release();   

}
