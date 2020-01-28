#ifndef _WIDGET_ICON_H_
#define _WIDGET_ICON_H_

#include "widget.h"
#include "window.h"
#include "esp_system.h"
#include <stdint.h>

widget_t* widget_icon_init(char* name);
esp_err_t widget_icon_set_icon(widget_t* widget, char* path);



#endif
