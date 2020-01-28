#include <string.h>
#include <math.h>

#include "widget_text.h"
#include "display.h"

#define LOG_LOCAL_LEVEL 7

#include "esp_log.h"
#include "esp_system.h"

#define TXT_MAX_LEN 256

static char* TAG = "widget text";

typedef struct widget_text_t {
    widget_t* widget;
    char* text;
    widget_alignment text_alignment;
} widget_text_t;

static esp_err_t draw(widget_t* this, rect_t pos);
static void clicked(widget_t* this, int x, int y);
static void destroy(widget_t* this);


widget_t* widget_text_init(char * name)
{
    widget_t* widget = widget_init(name, 0, 0);
    widget_text_t* text = calloc(1, sizeof(widget_text_t));

    widget->draw = draw;
    widget->clicked = clicked;
    widget->destroy = destroy;

    widget->widget_type = (void*)text;

    return widget;
}

esp_err_t widget_text_set_text(widget_t *widget, char* text, widget_alignment alignment)
{
    WIDGET_TYPE_CHECK(widget);

    widget_text_t* text_widget = (widget_text_t*)widget->widget_type;

    if(strlen(text) > TXT_MAX_LEN)
    {
        ESP_LOGW(TAG, "Text longer than max len %d", TXT_MAX_LEN);
    }

    xSemaphoreTake(widget->lock, portMAX_DELAY);

    if(text_widget->text) {
        free(text_widget->text);
    }

    text_widget->text = strndup(text, TXT_MAX_LEN);
    xSemaphoreGive(widget->lock);

    text_widget->text_alignment = alignment;

    widget->need_draw = true;

    return ESP_OK;
}

static inline int rescale_to_range(int x, int x_min, int x_len)
{
    return 100*(x - x_min) / x_len;
}




static void get_draw_start_pos(widget_t* widget, int* x, int *y, rect_t pos)
{
    WIDGET_TYPE_CHECK(widget);

    widget_text_t* text = (widget_text_t*)widget->widget_type;
    int len = strlen(text->text);
    ESP_LOGE(TAG, "len %d, pos x %d pos x len %d", len, pos.x, pos.x_len);
    switch (text->text_alignment) {
        case ALIGN_CENTER:
            *x = pos.x + fmax((pos.x_len / 2) - (len / 2), 0 );
            *y = pos.y + (pos.y_len / 2);
            break;

        case ALIGN_LEFT:
        default:
            *x = pos.x;
            *y = pos.y + (pos.y_len / 2);
            break;
    }

}


static esp_err_t draw(widget_t* this, rect_t pos)
{
    if (this->need_draw == false) {
        ESP_LOGD(TAG, "%s doesnt need drawing", this->name);
        return ESP_OK;
    }

    WIDGET_TYPE_CHECK(this);

    widget_text_t* text = (widget_text_t*)this->widget_type;

    xSemaphoreTake(this->lock, portMAX_DELAY);
    widget_border_draw(this, &pos);
    int x,y;
    get_draw_start_pos(this, &x, &y, pos);
    ESP_LOGD(TAG, "Draw text with name: %s, x = %d, x_len = %d, y = %d, y_len = %d", this->name, x, pos.x_len, y,pos.y_len);

    uint16_t color = 0xF800;
    display_draw_string(x, y, text->text, color, 1, 1);

    xSemaphoreGive(this->lock);

    this->need_draw = false;

    return ESP_OK;
}

static void clicked(widget_t* this, int x, int y)
{

    ESP_LOGE(TAG, "clicked %s", this->name);
}

static void destroy(widget_t* this)
{
    WIDGET_TYPE_CHECK(this);

    widget_text_t* text = (widget_text_t*)this->widget_type;

    ESP_LOGD(TAG, "Destroy widget %s", this->name);

    free(text->text);

}

