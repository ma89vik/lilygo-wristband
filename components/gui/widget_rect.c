#define LOG_LOCAL_LEVEL 7

#include "esp_log.h"
#include "widget_rect.h"
#include "display.h"
#include <string.h>
#include "widget.h"

static char* TAG = "widget rect";

static esp_err_t draw(widget_t* this, rect_t pos);
static void clicked(widget_t* this, int x, int y);

widget_t* widget_rect_init(char * name)
{
    widget_t* widget = widget_init(name, 0, 0);

    widget->draw = draw;
    widget->clicked = clicked;

    widget->widget_type = NULL;

    widget_needs_draw(widget);


    return ESP_OK;
}



static esp_err_t draw(widget_t* this, rect_t pos)
{
    if (!this->need_draw) {
        ESP_LOGD(TAG, "no draw");
        return ESP_OK;
    }

    widget_add_padding(this, &pos);

    ESP_LOGD(TAG, "Draw rect with name: %s, x = %d, x_len = %d, y = %d, y_len  %d", this->name, pos.x, pos.x_len, pos.y,pos.y_len);


    uint16_t** pixels;
    pixels = calloc(pos.y_len, sizeof(uint16_t *));
    if (pixels == NULL) {
        ESP_LOGE(TAG, "Error allocating memory for lines");
        goto clean_up;
    }
    for (int i = 0; i < pos.y_len; i++) {
        (pixels)[i] = malloc(pos.x_len * sizeof(uint16_t));
        if ((pixels)[i] == NULL) {
            ESP_LOGE(TAG, "Error allocating memory for line %d", i);
            goto clean_up;
        }

        memset(pixels[i], 0x33, 2*pos.x_len);
    }


    //draw_rect(pos.x, pos.x_len , pos.y, pos.y_len, pixels);
    this->need_draw = false;

clean_up:
    if (pixels != NULL) {
        for (int i = 0; i < pos.y_len; i++) {
            free((pixels)[i]);
        }
        free(pixels);
    }

    return ESP_OK;
}

static void clicked(widget_t* this, int x, int y)
{
    ESP_LOGE(TAG, "clicked %s", this->name);
}