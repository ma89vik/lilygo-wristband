idf_component_register(SRCS "weather.c" "aqi.c" "openweathermap.c"                  
                       INCLUDE_DIRS "."
                       REQUIRES log driver nvs_flash esp_http_client json wifi_manager)
