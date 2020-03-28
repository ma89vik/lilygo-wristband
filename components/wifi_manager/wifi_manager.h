// Copyright (c) 2020 Marius Vikhammer
// 
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

/* Controls init/close and connect/disconnect of the TCP/IP stack and Wifi
   will only shutdown the wifi when the last task releases the wifi access
*/
#pragma once

#include "esp_system.h"

esp_err_t wifi_manager_request_access(void);
void wifi_manager_finished(void);
