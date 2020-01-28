
#ifndef _WIDGET_GRID_H_
#define _WIDGET_GRID_H_

#include <stdint.h>
#include "esp_system.h"

#include "widget.h"
#include "sys/queue.h"


widget_t* widget_grid_init(char * name, uint8_t row_cnt, uint8_t* row_weights);

/* Attach the widget to the grid */
void widget_grid_attach_widget(widget_t* grid_widget, widget_t* widget, uint8_t row, uint8_t weight);

/* Remove an attached widget from the grid */
void widget_grid_remove_widget(widget_t* grid_widget, char* target_name);

#endif