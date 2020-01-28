#include "main_menu.h"
#include "widget.h"
#include "widget_button.h"
#include "widget_icon.h"
#include "widget_text.h"
#include "widget_grid.h"
#include "display.h"

#define LOG_LOCAL_LEVEL 7
#include "esp_log.h"

#include <string.h>

#define DESKTOP_GRID_ROWS 2

static char* TAG = "menu";


static widget_t* main_window;
static widget_t* desktop;
static widget_t* content;


widget_t* create_desktop()
{
/* Create the main window grid that will hold the top and bottom status/menu bars, as well as the main content */

    uint8_t row_weights[] = {1, 6};
    main_window = widget_grid_init("main_menu_grid", DESKTOP_GRID_ROWS, row_weights);

    widget_set_border_width(main_window, 2, GREEN);


    /* Create top menu bar */
    ESP_LOGD(TAG, "Initializing menu bar");
    widget_t* title_txt = widget_text_init( "title_widget");
    widget_set_border_width(title_txt, 2, WHITE);
    widget_text_set_text(title_txt, "ESP Watch", ALIGN_LEFT);

    widget_grid_attach_widget(main_window, title_txt, 0, 6);

    /* Create main content window as a seperate grid so we can easily replace it it when transitioning to another screen*/
    ESP_LOGD(TAG, "Initializing desktop window");
    uint8_t desktop_row_weights[] = {1};
    desktop = widget_grid_init("desktop_grid", 1, desktop_row_weights);

    widget_t* time = widget_text_init("time_widget");
    widget_text_set_text(time, "Time", ALIGN_LEFT);

    widget_grid_attach_widget(desktop, time, 0, 1);
    widget_grid_attach_widget(main_window, desktop, 1, 1);

    return main_window;
}

void main_menu_init()
{
    window_init();
    widget_t* main_menu = create_desktop();
    window_set(main_menu);
}
