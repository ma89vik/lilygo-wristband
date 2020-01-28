#ifndef _WIDGET_H_
#define _WIDGET_H_

#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_system.h"

typedef struct rect {
    int x;
    int x_len;
    int y;
    int y_len;
}rect_t;


#define WIDGET_TYPE_CHECK(widget)                                                 \
    do                                                                            \
    {                                                                             \
        if (widget->widget_type == NULL)                                                 \
        {                                                                         \
            ESP_LOGE(TAG, "Widget with no type %s(%d) ", __FUNCTION__, __LINE__);         \
            abort();                                                       \
        }                                                                         \
    } while (0)

typedef struct widget_t widget_t;

// Widget destroy fp for child class
typedef void(*widget_child_destroy)(widget_t *widget);

// Screen clicked fp for child class
typedef void(*widget_child_clicked)(widget_t *widget, int x,int y);

// Draw widget fp for child class
typedef esp_err_t(*widget_child_draw)(widget_t *widget, rect_t pos);

// Callback for clicked events
typedef void(*clicked_cb)();

struct widget_t {
    widget_child_destroy destroy;
    widget_child_clicked clicked;
    widget_child_draw draw;

    void* widget_type;
    char* name;
    uint8_t x_padding;
    uint8_t y_padding;
    uint8_t border_width;
    uint16_t border_color;

    bool need_draw;
    SemaphoreHandle_t lock;

    clicked_cb click_cb;

    widget_t* parent; // Pointer to the widget this widget is attached to
};

widget_t* widget_init(char* name, uint8_t x_pad, uint8_t y_pad);

void widget_delete(widget_t* widget);

void widget_clicked(widget_t* widget, int x, int y);

void widget_add_padding(widget_t* widget, rect_t *pos);

void widget_set_border_width(widget_t* widget, uint8_t border_width, uint16_t color);

void widget_border_draw(widget_t* widget, rect_t *pos);

void widget_set_click_cb(widget_t* widget, clicked_cb cb);

/* Mark widget as needs drawing and request draw from main window manager */
void widget_needs_draw(widget_t* widget);


#endif