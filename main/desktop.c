#include "desktop.h"

#define LOG_LOCAL_LEVEL 7
#include "esp_log.h"

#include <lvgl.h>
#include "display.h"

static char* TAG = "desktop";

static lv_style_t clock_style;
static lv_style_t title_style;

void create_title_bar_cont(void) {
    lv_obj_t * cont;
    cont = lv_cont_create(lv_scr_act(), NULL);

    lv_cont_set_style(cont, LV_LABEL_STYLE_MAIN, &lv_style_transp);

    //lv_obj_set_auto_realign(cont, true);                    /*Auto realign when the size changes*/
    //lv_obj_align_origo(cont, NULL, LV_ALIGN_IN_TOP_LEFT, 0, 0);  /*This parametrs will be sued when realigned*/
    
    lv_obj_set_auto_realign(cont, true);                    /*Auto realign when the size changes*/
    lv_obj_align(cont, NULL, LV_ALIGN_IN_TOP_RIGHT, 0, 0);
    lv_cont_set_fit(cont, LV_FIT_TIGHT);
    lv_cont_set_layout(cont, LV_LAYOUT_ROW_M);


    
    lv_obj_t * battery_pct =  lv_label_create(cont, NULL);
    lv_obj_t * battery =  lv_label_create(cont, NULL);

    
    lv_style_copy(&title_style, &lv_style_plain);
    title_style.text.font = &lv_font_roboto_12;

    lv_label_set_style(battery_pct, LV_LABEL_STYLE_MAIN, &title_style);
    lv_label_set_style(battery, LV_LABEL_STYLE_MAIN, &title_style);

    lv_label_set_text(battery_pct, "78%");
    lv_label_set_text(battery, LV_SYMBOL_BATTERY_FULL);

}

void create_clock_cont(void)
{
    lv_obj_t * cont;
    cont = lv_cont_create(lv_scr_act(), NULL);
   
    lv_cont_set_style(cont, LV_LABEL_STYLE_MAIN, &lv_style_transp);

    lv_obj_set_auto_realign(cont, true);                    /*Auto realign when the size changes*/
    lv_obj_align_origo(cont, NULL, LV_ALIGN_CENTER, 0, 0);  /*This parametrs will be sued when realigned*/
    lv_cont_set_fit(cont, LV_FIT_TIGHT);
    lv_cont_set_layout(cont, LV_LAYOUT_COL_M);

    lv_obj_t * hour =  lv_label_create(cont, NULL);
    lv_obj_t * min =  lv_label_create(cont, NULL);

    
    lv_style_copy(&clock_style, &lv_style_plain);
    clock_style.text.font = &lv_font_roboto_28;

    lv_label_set_style(hour, LV_LABEL_STYLE_MAIN, &clock_style);
    lv_label_set_style(min, LV_LABEL_STYLE_MAIN, &clock_style);

    /*Modify the clocks's text*/
    lv_label_set_text(hour, "11");
    lv_label_set_text(min, "32");
}

void create_desktop()
{
    ESP_LOGE(TAG, "Creating desktop");
    lv_obj_t * scr = lv_disp_get_scr_act(NULL);     /*Get the current screen*/

    create_title_bar_cont();
    create_clock_cont();
    /*Create the clock on the currently active screen*/
    

}

void desktop_init()
{
    display_aquire();
    create_desktop();
    display_release();
    
}
