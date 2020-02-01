#pragma once

#include "time.h"
#include "lvgl.h"

void desktop_create(lv_obj_t *scr);

void desktop_update_time(struct tm *time);
void desktop_update_battery(uint8_t battery_pct);
