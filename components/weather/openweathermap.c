/**
 * Copyright (c) 2020 Marius Vikhammer
 * 
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */

/**
 * Copyright (c) 2020 Marius Vikhammer
 * 
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */

#include "openweathermap.h"

#include "string.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_log.h"
#include "esp_http_client.h"
#include "cJSON.h"
#include "weather.h"

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

#define OWM_API_KEY_NVS_KEY "ow_api_key"
#define OWM_API_KEY_LEN 33
#define OWM_MAX_REQUEST_LEN 120
#define OWM_URL "api.openweathermap.org"

#define KELVIN_TO_C 273.15 
#define OPENWEATHER_READ_TIMEOUT_MS 10000

typedef struct {
    weather_type_t type;
    const char *icon_name;
} weather_img_map_entry;

static weather_img_map_entry s_icon_map[]= {
    {WEATHER_SUN, "01"},
    {WEATHER_FEW_CLOUDS, "02"},
    {WEATHER_SCATTERED_CLOUDS, "03"},
    {WEATHER_BROKEN_CLOUDS, "04"},
    {WEATHER_SHOWER_RAIN, "09"},
    {WEATHER_RAIN, "10"},
    {WEATHER_THUNDERSTORM, "11"},
    {WEATHER_SNOW, "13"},
    {WEATHER_MIST, "50"},
    {WEATHER_UNKOWN, ""},
};

static char *TAG = "openweathermap";

static void openweather_http_get(char *request);
static esp_err_t _http_event_handler(esp_http_client_event_t *evt);
static esp_err_t openweather_parse_response(char *json_str, float *temp, weather_type_t *weather_type);
static weather_type_t openweather_code_to_weather_type(char *code);

static volatile QueueHandle_t weather_data_queue;

static esp_err_t openweather_get_api_key(char *api_key, size_t len) {
    
    nvs_handle_t nvs_handle;
    int err = nvs_open("storage", NVS_READWRITE, &nvs_handle);
    if (err != ESP_OK) {
       ESP_LOGE(TAG, "Error (%s) opening NVS handle!\n", esp_err_to_name(err));
    } 

    // Read
    err = nvs_get_str(nvs_handle, OWM_API_KEY_NVS_KEY, api_key, &len);

    if (err == ESP_ERR_NVS_NOT_FOUND) {
        ESP_LOGE(TAG, "The NVS value is not initialized yet!\n");
    }

    ESP_LOGE(TAG, "err %d ", err);
    return err;
}

int openweather_read(char* city, openweather_data_t *weather_data) {


    char api_key[OWM_API_KEY_LEN];
    char request[OWM_MAX_REQUEST_LEN];

    if (city == NULL) {
        ESP_LOGE(TAG, "No city argument");
    }

    if (openweather_get_api_key(api_key, OWM_API_KEY_LEN) != ESP_OK) {
        ESP_LOGE(TAG, "Open weather API key read failed");
        return -1;
    }

    weather_data_queue = xQueueCreate( 1, sizeof( openweather_data_t) );
    if (weather_data_queue == NULL) {
        ESP_LOGE(TAG, "Failed to create queue");
        return -1;
    }

    snprintf(request, OWM_MAX_REQUEST_LEN, "/data/2.5/weather?q=%s&appid=%s", city, api_key );

    ESP_LOGI(TAG, "%s", request);

    openweather_http_get(request);

    xQueueReceive(weather_data_queue, weather_data, OPENWEATHER_READ_TIMEOUT_MS / portTICK_PERIOD_MS);

    vQueueDelete(weather_data_queue);

    // Check if fetch succeeded 
    if ( (weather_data->temp!= 0)) {
        ESP_LOGI(TAG, "Fetched weather from openweather");
        return 0;
    } else {
        ESP_LOGW(TAG, "Failed to fetch weather from openweather");
        return -1;
    }

}

static esp_err_t _http_event_handler(esp_http_client_event_t *evt)
{
    switch(evt->event_id) {
        case HTTP_EVENT_ERROR:
            ESP_LOGI(TAG, "HTTP_EVENT_ERROR");
            break;
        case HTTP_EVENT_ON_CONNECTED:
            ESP_LOGI(TAG, "HTTP_EVENT_ON_CONNECTED");
            break;
        case HTTP_EVENT_HEADER_SENT:
            ESP_LOGI(TAG, "HTTP_EVENT_HEADER_SENT");
            break;
        case HTTP_EVENT_ON_HEADER:
            ESP_LOGI(TAG, "HTTP_EVENT_ON_HEADER, key=%s, value=%s", evt->header_key, evt->header_value);
            break;
        case HTTP_EVENT_ON_DATA:
            ESP_LOGI(TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
            
            openweather_data_t data;
            int ret = openweather_parse_response(evt->data, &data.temp, &data.type);
            
            if (ret == ESP_OK) {
                xQueueSend(weather_data_queue, &data, portMAX_DELAY);       
            }

            break;
        case HTTP_EVENT_ON_FINISH:
            ESP_LOGI(TAG, "HTTP_EVENT_ON_FINISH");
            break;
        case HTTP_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "HTTP_EVENT_DISCONNECTED");
           
            break;
    }
    return ESP_OK;
}

static void openweather_http_get(char *request){

    esp_http_client_config_t config = {
        .host = OWM_URL,
        .event_handler = _http_event_handler,
        .path = request,
        .transport_type = HTTP_TRANSPORT_OVER_SSL,
        .buffer_size = 1024,
        .timeout_ms = 10000,
    };
    esp_http_client_handle_t client = esp_http_client_init(&config);

    // GET
    esp_err_t err = esp_http_client_perform(client);
    if (err == ESP_OK) {
        ESP_LOGI(TAG, "HTTP GET Status = %d, content_length = %d",
                esp_http_client_get_status_code(client),
                esp_http_client_get_content_length(client));
    } else {
        ESP_LOGE(TAG, "HTTP GET request failed: %s", esp_err_to_name(err));
    }

    esp_http_client_cleanup(client);
}

static esp_err_t openweather_parse_response(char *json_str,  float *temp, weather_type_t *weather_type) {
    
    int err;
    cJSON * root = cJSON_Parse(json_str);

    if(root == NULL) {
        ESP_LOGE(TAG, "Failed to parse JSON response");
        err  = ESP_ERR_INVALID_ARG;
        goto cleanup;
    }

    cJSON *weather = cJSON_GetObjectItemCaseSensitive(root, "weather");
    cJSON *main_data = cJSON_GetObjectItemCaseSensitive(root, "main");

    if (weather==NULL || (main_data == NULL)) {
        ESP_LOGE(TAG, "Failed to get weather and main fields");
        err  = ESP_ERR_INVALID_ARG;
        goto cleanup;
    }

    cJSON *temp_item = cJSON_GetObjectItemCaseSensitive(main_data, "temp");
    
    if (!cJSON_IsNumber(temp_item)) {
        ESP_LOGE(TAG, "Failed to get temp field");
        err  = ESP_ERR_INVALID_ARG;
        goto cleanup;
    }

    *temp = temp_item->valuedouble - KELVIN_TO_C;
    ESP_LOGE(TAG, "temp %f", *temp);

    cJSON *subitem = cJSON_GetArrayItem(weather, 0);

    if (subitem==NULL) {
        ESP_LOGE(TAG, "Failed to get array index 0 from weather field");
        err  = ESP_ERR_INVALID_ARG;
        goto cleanup;
    }

    cJSON *icon = cJSON_GetObjectItemCaseSensitive(subitem, "icon");
    
    if (!cJSON_IsString(icon)) {
        ESP_LOGE(TAG, "Failed to get icon field");
        err  = ESP_ERR_INVALID_ARG;
        goto cleanup;
    }

    ESP_LOGE(TAG, "icon %s", icon->valuestring);
    *weather_type = openweather_code_to_weather_type(icon->valuestring);
    err =ESP_OK;

cleanup:

    cJSON_Delete(root);
    return err;
}


static weather_type_t openweather_code_to_weather_type(char *code) {
    for (int i = 0; i<sizeof(s_icon_map)/sizeof(weather_img_map_entry); i++) {
        if (!strncmp(code, s_icon_map[i].icon_name, 2)) {
            return s_icon_map[i].type;
        }
    }

    return WEATHER_UNKOWN;
}
