// Copyright (c) 2020 Marius Vikhammer
// 
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#pragma once

#include "weather.h"

typedef struct {
    weather_type_t type;
    float temp;
} openweather_data_t;

int openweather_read(char* city, openweather_data_t *weather_data);
