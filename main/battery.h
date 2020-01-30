// Copyright (c) 2020 Marius Vikhammer
// 
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#pragma once

#include <stdint.h>
/* Initial the battery measurement and the underlying ADC */
void battery_init();

/* Returns the current battery level as a percentage between 0 - 100% */
float battery_lvl_read();