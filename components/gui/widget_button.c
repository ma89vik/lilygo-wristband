#include "widget_button.h"
#include "widget_icon.h"
#include "widget_text.h"

#include "esp_log.h"

#include <stdint.h>
#include <string.h>

#define BUTTON_PADDING 0
#define DEFAULT_TEXT_WEIGHT 1
#define DEFAULT_ICON_WEIGHT 4

typedef struct {
    widget_t* icon;     // Clickable icon
    widget_t* text;     // Text displayed below icon
    uint8_t icon_weight;
    uint8_t text_weight;
    clicked_cb click_cb;

} widget_button_t;

static char* TAG = "widget_button";

static esp_err_t draw(widget_t* this, rect_t pos);
static void clicked(widget_t* this, int x, int y);
static void destroy(widget_t* this);

widget_t* widget_button_init(char* name, char* icon_path)
{
    char name_buf[100];
    strncat(name_buf, name, 99);

    widget_t* widget = widget_init(name, BUTTON_PADDING , BUTTON_PADDING);
    widget->destroy = destroy;
    widget->draw = draw;
    widget->clicked = clicked;

    widget_button_t* button = calloc(1, sizeof(widget_button_t));
    if(!button) {
        ESP_LOGE(TAG, "Failed to allocate memory for button");
        abort();
    }

    button->icon_weight = DEFAULT_ICON_WEIGHT;
    button->text_weight = DEFAULT_TEXT_WEIGHT;

    widget->widget_type = (void*)button;

    strncpy(name_buf, name, 99);
    strncat(name_buf, "_icon", 99-strlen(name_buf));

    widget_t* icon = widget_icon_init(name_buf);
    button->icon = icon;
    widget_button_set_icon(widget, icon_path);

    strncpy(name_buf, name, 99);
    strncat(name_buf, "_text", 99-strlen(name_buf));

    widget_t* text = widget_text_init(name_buf);
    button->text = text;

    widget_needs_draw(widget);

    return widget;
}

void widget_button_set_weights(widget_t* widget, uint8_t icon_weight, uint8_t text_weight)
{
    WIDGET_TYPE_CHECK(widget);
    widget_button_t* button = (widget_button_t*)widget->widget_type;

    button->icon_weight = icon_weight;
    button->text_weight = text_weight;

}

esp_err_t widget_button_set_icon(widget_t* widget, char* path)
{
    WIDGET_TYPE_CHECK(widget);
    widget_button_t* button = (widget_button_t*)widget->widget_type;

    return widget_icon_set_icon(button->icon, path);
}

esp_err_t widget_button_set_text(widget_t* widget, char* text)
{
    WIDGET_TYPE_CHECK(widget);
    widget_button_t* button = (widget_button_t*)widget->widget_type;

    return widget_text_set_text(button->text, text, ALIGN_CENTER);
}

static esp_err_t draw(widget_t* this, rect_t pos)
{
    if (this->need_draw == false) {
        ESP_LOGD(TAG, "%s doesnt need drawing", this->name);
        return ESP_OK;
    }

    WIDGET_TYPE_CHECK(this);

    widget_button_t* button = (widget_button_t*)this->widget_type;

    widget_add_padding(this, &pos);

    // Set up relative position for icon vs text
    rect_t icon_pos = pos;
    rect_t text_pos = pos;

    icon_pos.y_len = (100*button->icon_weight/(button->icon_weight + button->text_weight) * pos.y_len) / 100;

    text_pos.y = pos.y + icon_pos.y_len;
    text_pos.y_len = pos.y_len - icon_pos.y_len;

    if (this->need_draw) {
        button->icon->need_draw = true;
        button->text->need_draw = true;
    }

    button->icon->draw(button->icon, icon_pos);
    button->text->draw(button->text, text_pos);

    this->need_draw = false;

    return ESP_OK;
}


static void clicked(widget_t* this, int x, int y)
{
    WIDGET_TYPE_CHECK(this);
    widget_button_t* button = (widget_button_t*)this->widget_type;

    ESP_LOGE(TAG, "clicked %s", this->name);

    /* Only clicks on the icon will register as a click */
    if(y <= 100*button->icon_weight/(button->icon_weight + button->text_weight)) {
        if(button->click_cb) {
            button->click_cb();
        }
    }
}

static void destroy(widget_t* this)
{
    WIDGET_TYPE_CHECK(this);

    widget_button_t* button = (widget_button_t*)this->widget_type;

    ESP_LOGD(TAG, "Destroy widget %s", this->name);

    widget_delete(button->icon);
    widget_delete(button->text);

}

void widget_button_set_click_cb(widget_t* widget, clicked_cb cb)
{
    WIDGET_TYPE_CHECK(widget);
    widget_button_t* button = (widget_button_t*)widget->widget_type;

    button->click_cb = cb;
}