#pragma once

#include <stdint.h>
#include "esp_system.h"

#include "widget.h"
#include "sys/queue.h"

typedef enum {ALIGN_LEFT, ALIGN_RIGHT, ALIGN_CENTER} widget_alignment;

widget_t* widget_text_init(char * name);

esp_err_t widget_text_set_text(widget_t *widget, char* text, widget_alignment alignment);
