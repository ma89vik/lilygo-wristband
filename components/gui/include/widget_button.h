#include "widget.h"

widget_t* widget_button_init(char* name, char* icon_path);
esp_err_t widget_button_set_icon(widget_t* widget, char* path);
esp_err_t widget_button_set_text(widget_t* widget, char* text);
void widget_button_set_click_cb(widget_t* widget, clicked_cb cb);
