// Simple PCF8563 (Real Time Clock) driver
// Copyright (c) 2020 Marius Vikhammer
// 
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT
#include "driver/i2c.h"
#include <time.h>
#include "sys/lock.h"
#include "driver/i2c.h"


void pcf8563_init(i2c_port_t port, _lock_t *i2c_lock);

/* Read the time */
void pcf8563_read_time(struct tm *time);

/* Set the current time */
void pcf8563_set_time(struct tm *time);
