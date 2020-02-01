// Copyright (c) 2020 Marius Vikhammer
// 
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#pragma once
#include "lvgl.h"

void ota_screen_create(lv_obj_t *scr);

void ota_screen_set_progress(int pct);

void ota_screen_set_error(char* error);