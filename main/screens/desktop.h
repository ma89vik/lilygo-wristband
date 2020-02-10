#pragma once

#include "time.h"
#include "lvgl.h"
#include "weather.h"

void desktop_create(lv_obj_t *scr);

void desktop_update_time();
void desktop_update_battery(uint8_t battery_pct);
void desktop_update_pm25(uint16_t pm25);
void desktop_update_weather(float temp, weather_type_t weather_type);
