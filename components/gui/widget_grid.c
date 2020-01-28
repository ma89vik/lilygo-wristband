#include "widget_grid.h"
#define LOG_LOCAL_LEVEL 7

#include "esp_log.h"
#include "esp_system.h"

#include <string.h>

typedef struct widget_list_item {
    widget_t* widget;
    uint8_t weight;
    STAILQ_ENTRY(widget_list_item) next;
} widget_list_item_t;

typedef struct widget_row_list_item {
    uint16_t total_weight; // Sum of weights for a specific row
    STAILQ_HEAD(widget_list_t, widget_list_item) widget_list;
} widget_row_list_item_t;

typedef struct grid_t {
    widget_t* widget;
    int row_cnt;
    uint8_t* row_weights;  // Individual weight for rows
    uint16_t total_weight; // Sum of weights for rows
    widget_row_list_item_t * rows;
} widget_grid_t;

static char* TAG = "widget grid";


static esp_err_t draw(widget_t* this, rect_t pos);
static void clicked(widget_t* this, int x, int y);
static void destroy(widget_t* this);


widget_t* widget_grid_init(char * name, uint8_t row_cnt, uint8_t* row_weights)
{
    widget_t* widget = widget_init(name, 0, 0);
    widget_grid_t* grid = calloc(1, sizeof(widget_grid_t));

    if(!grid || !widget) {
        ESP_LOGE(TAG, "Failed to alloc mem for grid");
        return NULL;
    }



    widget->draw = draw;
    widget->clicked = clicked;
    widget->destroy = destroy;

    grid->row_cnt = row_cnt;

    grid->rows = calloc(row_cnt, sizeof(widget_row_list_item_t));
    if(!grid->rows) {
            ESP_LOGE(TAG, "Failed to alloc mem for grid");
            return NULL;
    }

    grid->row_weights = malloc(row_cnt*sizeof(row_weights));
    memcpy(grid->row_weights, row_weights, row_cnt);

    /* Init widget list for each row */
    for(int i = 0; i<row_cnt; i++) {
        grid->rows[i].total_weight = 0;
        STAILQ_INIT(&grid->rows[i].widget_list);

        grid->total_weight = grid->total_weight + row_weights[i];
    }

    widget->widget_type = (void*)grid;

    return widget;
}

void widget_grid_attach_widget(widget_t* grid_widget, widget_t* widget, uint8_t row, uint8_t weight)
{
    widget_grid_t* grid = (widget_grid_t*)grid_widget->widget_type;

    widget_list_item_t* item = calloc(1, sizeof(widget_list_item_t));
    item->widget = widget;
    item->weight = weight;

    xSemaphoreTake(grid_widget->lock, portMAX_DELAY);

    if(row >= grid->row_cnt) {
        ESP_LOGI(TAG, "Row outside row-space, insert at last row");
        row = grid->row_cnt - 1;
    }

    STAILQ_INSERT_TAIL(&grid->rows[row].widget_list, item, next);

    grid->rows[row].total_weight = grid->rows[row].total_weight + weight;

    widget->parent = grid_widget;

    ESP_LOGD(TAG, "Attach widget with name: %s", widget->name);

    xSemaphoreGive(grid_widget->lock);

    //If grid is update then we need to redraw all sub widgets
    widget_needs_draw(grid_widget);
}

void widget_grid_remove_widget(widget_t* grid_widget, char* target_name)
{
    xSemaphoreTake(grid_widget->lock, portMAX_DELAY);

    WIDGET_TYPE_CHECK(grid_widget);
    widget_grid_t* grid = (widget_grid_t*)grid_widget->widget_type;

    widget_list_item_t* item;
    for (int i = 0;i < grid->row_cnt; i++) {
        STAILQ_FOREACH(item, &grid->rows[i].widget_list, next) {
            if (strcmp(item->widget->name, target_name) == 0) {
                grid->rows[i].total_weight = grid->rows[i].total_weight - item->weight;
                STAILQ_REMOVE(&grid->rows[i].widget_list, item, widget_list_item, next);
                ESP_LOGD(TAG, "Remove attached widget with name: %s", item->widget->name);
            }
        }
    }
    xSemaphoreGive(grid_widget->lock);

    //If grid is update then we need to redraw all sub widgets
    widget_needs_draw(grid_widget);
}

static esp_err_t draw(widget_t* this, rect_t pos)
{
    xSemaphoreTake(this->lock, portMAX_DELAY);

    WIDGET_TYPE_CHECK(this);

    widget_grid_t* grid = (widget_grid_t*)this->widget_type;

    ESP_LOGD(TAG, "Draw grid with name: %s, x = %d, x_len = %d, y = %d, y_len 0 %d", this->name, pos.x, pos.x_len, pos.y,pos.y_len);

    if (this->border_width) {
        widget_border_draw(this, &pos);
    }

    widget_list_item_t* item;

    int x_pnt = pos.x;
    int y_pnt = pos.y;
    for(int i = 0; i<grid->row_cnt; i++) {
        int y_len = (pos.y_len*grid->row_weights[i]) / grid->total_weight;
        x_pnt = pos.x;
        STAILQ_FOREACH(item, &grid->rows[i].widget_list, next) {
            int x_len = (pos.x_len*item->weight) / grid->rows[i].total_weight;

            rect_t pos = {x_pnt, x_len, y_pnt, y_len };

            // Force redraw of all attached widgets
            if (this->need_draw) {
                item->widget->need_draw = true;
            }

            item->widget->draw(item->widget, pos);

            x_pnt = x_pnt + x_len;
        }

        y_pnt = y_pnt + y_len;
    }
    this->need_draw = false;


    xSemaphoreGive(this->lock);

    return ESP_OK;
}

static inline bool click_inside(int x, int x_min, int x_len)
{
    if (( x >= x_min ) && (x <= (x_min + x_len)) ){
        return true;
    }
    else {
        return false;
    }

}

static inline int rescale_click(int x, int x_min, int x_len)
{
    return 100*(x - x_min) / x_len;
}

// x,y will always be 0-100%
static void clicked(widget_t* this, int x_click, int y_click)
{
    ESP_LOGE(TAG, "Grid clicked %s", this->name);

    WIDGET_TYPE_CHECK(this);

    xSemaphoreTake(this->lock, portMAX_DELAY);

    widget_grid_t* grid = (widget_grid_t*)this->widget_type;

    widget_list_item_t* item;

    int x_pnt = 0;
    int y_pnt = 0;

    // Loop through row by row and find the widget that was clicked

    for(int i = 0; i<grid->row_cnt; i++) {
        int y_len = (100*grid->row_weights[i]) / grid->total_weight;
        x_pnt = 0;

        if ( click_inside(y_click, y_pnt, y_len) ) {
            STAILQ_FOREACH(item, &grid->rows[i].widget_list, next) {
                int x_len = (100*item->weight) / grid->rows[i].total_weight;

                ESP_LOGD(TAG, "Click x=%d, y=%d", x_click, y_click);
                ESP_LOGD(TAG, "Pos rect: x_pnt=%d x_len=%d, y_pnt=%d, y_len=%d", x_pnt, x_len, y_pnt, y_len);

                if( click_inside(x_click, x_pnt, x_len) ) {
                    // Rescale click coords so each widget gets a "relative" click coord
                    x_click = rescale_click(x_click, x_pnt, x_len);
                    y_click = rescale_click(y_click, y_pnt, y_len);

                    xSemaphoreGive(this->lock);
                    widget_clicked(item->widget, x_click, y_click);

                    return;
                }
                x_pnt = x_pnt + x_len;
            }
        }

        y_pnt = y_pnt + y_len;
    }

    xSemaphoreGive(this->lock);

}

static void destroy(widget_t* this)
{

    if (!this->widget_type) {
        ESP_LOGE(TAG, "No grid subclass");
    }
    widget_grid_t* grid = (widget_grid_t*)this->widget_type;

    ESP_LOGD(TAG, "Destroy widget %s", this->name);

    widget_list_item_t* item;

    for(int i = 0; i<grid->row_cnt; i++) {
        STAILQ_FOREACH(item, &grid->rows[i].widget_list, next) {
            widget_delete(item->widget);
            free(item);
            //STAILQ_REMOVE(&grid->rows[i].widget_list, )
        }
    }
    free(grid->rows);

}

