// Copyright (c) 2020 Marius Vikhammer
// 
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#pragma once

#include "weather.h"

int openweather_read(char* city, float *temp, weather_type_t *weather_type);
