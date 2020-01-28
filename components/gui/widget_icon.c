#define LOG_LOCAL_LEVEL 7

#include "esp_log.h"
#include "widget_icon.h"
#include "decode_image.h"
#include "display.h"
#include "gui_jpegs.h"
#include <string.h>
#include "esp_event.h"

typedef struct widget_icon {
    uint8_t* jpeg_icon;
} widget_icon_t;


static char* TAG = "widget icon";

static esp_err_t draw(widget_t* this, rect_t pos);
static void clicked(widget_t* this, int x, int y);
static void widget_icon_destroy(widget_t* this);


widget_t* widget_icon_init(char * name)
{
    widget_t* widget = widget_init(name, 0, 0);
    widget_icon_t* icon = calloc(1, sizeof(widget_icon_t));

    if(!icon) {
        ESP_LOGE(TAG, "Failed to alloc mem for icon");
        return NULL;
    }

    widget->draw = draw;
    widget->clicked = clicked;
    widget->destroy = widget_icon_destroy;

    widget->widget_type = (void*)icon;


    return widget;
}
static esp_err_t widget_icon_get_start_draw_pos(widget_t* widget, rect_t *pos, int image_height, int image_width ) 
{
    if (image_width > pos->x_len || image_height > pos->y_len) {
        ESP_LOGE(TAG, "%s image is bigger than the draw space", widget->name);
        return ESP_FAIL;        
    }
    pos->x = pos->x + (pos->x_len - image_width)/2;
    pos->y = pos->y + (pos->y_len - image_height)/2;

    return ESP_OK;
}

esp_err_t widget_icon_set_icon(widget_t* widget, char* path)
{
    WIDGET_TYPE_CHECK(widget);

    widget_icon_t* icon = (widget_icon_t*)widget->widget_type;

     // Open image file for reading
    FILE* f = fopen(path, "r");
    if (f == NULL) {
        ESP_LOGE(TAG, "Failed to open %s", path);
        return ESP_FAIL;
    }

    fseek(f, 0, SEEK_END);
    size_t size = ftell(f);
    fseek(f, 0, SEEK_SET);

    xSemaphoreTake(widget->lock, portMAX_DELAY);

    if(icon->jpeg_icon) {
        free(icon->jpeg_icon);
    }

    icon->jpeg_icon = malloc(size);
    if(!icon->jpeg_icon) {
        ESP_LOGE(TAG, "Failed to allocate %d bytes for image %s", size, path);
        return ESP_ERR_NO_MEM;
    }
    ESP_LOGE(TAG, "Allocated %d", size);
    memset(icon->jpeg_icon, 0, size);
    fread(icon->jpeg_icon, 1, size, f);

    xSemaphoreGive(widget->lock);
    fclose(f);

    widget_needs_draw(widget);



    return ESP_OK;
}

static void widget_icon_destroy(widget_t* this)
{
    WIDGET_TYPE_CHECK(this);

    ESP_LOGD(TAG, "Destroy widget %s", this->name);

    widget_icon_t* icon = (widget_icon_t*)this->widget_type;

    free(icon->jpeg_icon);
}


static esp_err_t draw(widget_t* this, rect_t pos)
{
    if (this->need_draw == false) {
        ESP_LOGD(TAG, "%s doesnt need drawing", this->name);
        return ESP_OK;
    }

    ESP_LOGD(TAG, "Draw icon with name: %s, x = %d, x_len = %d, y = %d, y_len = %d", this->name, pos.x, pos.x_len, pos.y,pos.y_len);


    widget_icon_t* widget_icon = (widget_icon_t*)this->widget_type;
    uint16_t** image;

    if(!widget_icon->jpeg_icon) {
        ESP_LOGE(TAG, "Draw failed, no image to draw");
        return ESP_FAIL;
    }

    int height, width;
    display_draw_fill_rect(pos.x, pos.x_len, pos.y, pos.y_len, 0xFF);

    xSemaphoreTake(this->lock, portMAX_DELAY);
    decode_image(&image, widget_icon->jpeg_icon, &height, &width);
    xSemaphoreGive(this->lock);

    if (widget_icon_get_start_draw_pos(this, &pos, height, width) != ESP_OK) {
        return ESP_FAIL;
    }
    display_draw_rect(pos.x, width , pos.y, height, image);
    this->need_draw = false;

    for (int i = 0; i < height; i++) {
            free(image[i]);
    }
    free(image);

    return ESP_OK;

}

static void clicked(widget_t* this, int x, int y)
{
    widget_icon_t* widget_icon = (widget_icon_t*)this->widget_type;

    ESP_LOGE(TAG, "clicked %s", this->name);


}