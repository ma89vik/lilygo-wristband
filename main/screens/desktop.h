#pragma once

#include "time.h"

void desktop_create();

void desktop_update_time(struct tm *time);
void desktop_update_battery(float battery_pct);