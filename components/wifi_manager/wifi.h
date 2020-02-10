// Copyright (c) 2020 Marius Vikhammer
// 
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

/* Init wifi and connects to predfined station
   Does not return before connection is established
*/
#pragma once

#include "esp_system.h"

esp_err_t wifi_init_and_connect(void);
void wifi_manager_deinit(void);
