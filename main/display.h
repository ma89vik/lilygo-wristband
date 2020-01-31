#pragma once

#include "lvgl.h"

/* Locks for littlevGL and display */
void display_aquire();
void display_release();

lv_indev_t* get_disp_driver();

void display_init();