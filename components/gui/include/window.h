

#ifndef _WINDOW_H_
#define _WINDOW_H_

#include <stdint.h>
#include "esp_system.h"

#include "widget.h"
#include "sys/queue.h"


esp_err_t window_init();
void window_request_draw(widget_t *widget);

/* Sets the current main window and request a redraw */
void window_set(widget_t* widget);


#endif