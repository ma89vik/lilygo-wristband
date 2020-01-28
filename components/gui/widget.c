#include "widget.h"
#include "window.h"
#define LOG_LOCAL_LEVEL 7

#include "esp_log.h"

#include "string.h"
#include "display.h"

#define NAME_MAX_LEN 128

static char* TAG = "widget";

widget_t* widget_init(char* name, uint8_t x_pad, uint8_t y_pad)
{
    widget_t* widget = calloc(1, sizeof(widget_t));

    widget->x_padding = x_pad;
    widget->y_padding = y_pad;
    widget->name = strndup(name, NAME_MAX_LEN);

    if(!widget->name) {
        ESP_LOGE(TAG, "Failed to allocate memory for widget name");
        abort();
    }

    ESP_LOGV(TAG, "Init widget with name %s", widget->name);

    widget->lock = xSemaphoreCreateMutex();

    if(widget->lock == NULL) {
        ESP_LOGE(TAG, "failed to create mutex");
    }

    return widget;
}


/* Sub class widget can implement other more specific click ++ cbs */
void widget_set_click_cb(widget_t* widget, clicked_cb cb)
{
    widget->click_cb = cb;
}

void widget_delete(widget_t* widget)
{
    // Free sub type
    widget->destroy(widget);

    free(widget->name);
    free(widget->widget_type);
    vSemaphoreDelete(widget->lock);

    free(widget);
}


void widget_clicked(widget_t* widget, int x, int y)
{
    ESP_LOGD(TAG, "Widget clicked %s at x=%d y=%d", widget->name, x ,y);


    if(widget->click_cb) {
        widget->click_cb();
    }

    // Implementing clicked for sub classes is optional
    if(widget->clicked) {
        widget->clicked(widget, x, y);
    }

}

/* Adds padding to the draw rectangle */
void widget_add_padding(widget_t* widget, rect_t *pos)
{
    if( (pos->x_len < 2*widget->x_padding) || (pos->y_len < 2*widget->y_padding) ) {
        ESP_LOGE(TAG, "Unable to add padding to %s, size too small",widget->name);
        return;
    }
    pos->x = pos->x + widget->x_padding;
    pos->y = pos->y + widget->y_padding;
    pos->x_len = pos->x_len - 2*widget->x_padding;
    pos->y_len = pos->y_len - 2*widget->y_padding;
}

/* Sets border width in number of pixels */
void widget_set_border_width(widget_t* widget, uint8_t border_width, uint16_t color)
{
    widget->border_width = border_width;
    widget->border_color = color;
}

/* Draws the border and shrinks the position rectangle */
void widget_border_draw(widget_t* widget, rect_t *pos)
{
    if(!widget->border_width || !widget->border_color) {
        return;
    }
    ESP_LOGD(TAG, "Draw border with name: %s, x = %d, x_len = %d, y = %d, y_len = %d", widget->name, pos->x, pos->x_len, pos->y,pos->y_len);

    display_draw_border(pos->x, pos->x_len, pos->y,pos->y_len, widget->border_width, widget->border_color);
    pos->x = pos->x + widget->border_width;
    pos->y = pos->y + widget->border_width;
    pos->x_len = pos->x_len - 2*widget->border_width;
    pos->y_len = pos->y_len - 2*widget->border_width;

}


void widget_needs_draw(widget_t* widget)
{
    widget->need_draw = true;
    window_request_draw(widget);
}
