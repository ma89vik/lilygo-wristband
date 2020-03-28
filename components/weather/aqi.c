/**
 * Copyright (c) 2020 Marius Vikhammer
 * 
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */

#include "aqi.h"

#include "string.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_log.h"
#include "esp_http_client.h"
#include "cJSON.h"

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

#define AQI_API_KEY_NVS_KEY "aqi_api_key"
#define AQI_API_KEY_LEN 41
#define AQI_MAX_REQUEST_LEN 100
#define AQI_CN_URL "api.waqi.info"

static char *TAG = "aqi";

static void aqi_http_get(char *request);
static esp_err_t _http_event_handler(esp_http_client_event_t *evt);
static esp_err_t aqi_parse_response(char *json_str, int* pm25_val);

xSemaphoreHandle http_get_comp;
volatile static int s_pm25;

static esp_err_t aqi_get_api_key(char *api_key, size_t len) {
    
    nvs_handle_t nvs_handle;
    int err = nvs_open("storage", NVS_READWRITE, &nvs_handle);
    if (err != ESP_OK) {
       ESP_LOGE(TAG, "Error (%s) opening NVS handle!\n", esp_err_to_name(err));
    } 

    // Read
    err = nvs_get_str(nvs_handle, AQI_API_KEY_NVS_KEY, api_key, &len);

    if (err == ESP_ERR_NVS_NOT_FOUND) {
        ESP_LOGE(TAG, "The NVS value is not initialized yet!\n");
    }

    return err;
}

int aqi_read(char* city) {


    char api_key[AQI_API_KEY_LEN];
    char request[AQI_MAX_REQUEST_LEN];

    if (aqi_get_api_key(api_key, AQI_API_KEY_LEN) != ESP_OK) {
        ESP_LOGE(TAG, "AQI read failed");
        return -1;
    }

    snprintf(request, AQI_MAX_REQUEST_LEN, "/feed/%s/?token=%s", city, api_key );

    ESP_LOGE(TAG, "%s", request);
    http_get_comp = xSemaphoreCreateBinary();

    aqi_http_get(request);

    xSemaphoreTake(http_get_comp, 10000 / portTICK_PERIOD_MS);
    

    vSemaphoreDelete(http_get_comp);

    return s_pm25;

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
            aqi_parse_response(evt->data, &s_pm25);
            xSemaphoreGive(http_get_comp);


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

static void aqi_http_get(char *request){

    esp_http_client_config_t config = {
        .host = AQI_CN_URL,
        .event_handler = _http_event_handler,
        .path = request,
        .transport_type = HTTP_TRANSPORT_OVER_SSL,
        .buffer_size = 2048,
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

static esp_err_t aqi_parse_response(char *json_str, int* pm25_val) {
    
    int err;
    cJSON * root = cJSON_Parse(json_str);

    if(root == NULL) {
        ESP_LOGE(TAG, "Failed to parse JSON response");
        err  = ESP_ERR_INVALID_ARG;
        goto cleanup;
    }

    cJSON *status = cJSON_GetObjectItemCaseSensitive(root, "status");
    cJSON *data = cJSON_GetObjectItemCaseSensitive(root, "data");

    if (!cJSON_IsString(status) || (data == NULL)) {
        ESP_LOGE(TAG, "Failed to get status and data fields");
        err  = ESP_ERR_INVALID_ARG;
        goto cleanup;
    }

    if ( strcmp(status->valuestring, "ok") ) {
        ESP_LOGE(TAG, "Status not ok, %s", status->valuestring);
        err  = ESP_ERR_INVALID_ARG;
        goto cleanup;    
    }

    cJSON *iaqi = cJSON_GetObjectItemCaseSensitive(data, "iaqi");
    
    if (iaqi == NULL) {
        ESP_LOGE(TAG, "Failed to get iaqi field");
        err  = ESP_ERR_INVALID_ARG;
        goto cleanup;
    }
    cJSON *pm25 = cJSON_GetObjectItemCaseSensitive(iaqi, "pm25");
    
    if (pm25 == NULL) {
        ESP_LOGE(TAG, "Failed to get pwm25 field");
        err  = ESP_ERR_INVALID_ARG;
        goto cleanup;
    }

    cJSON *pm25_value = cJSON_GetObjectItemCaseSensitive(pm25, "v");
    
    if (pm25_value == NULL) {
        ESP_LOGE(TAG, "Failed to get pwm25 value field");
        err  = ESP_ERR_INVALID_ARG;
        goto cleanup;
    }

    *pm25_val = pm25_value->valueint;
    err =ESP_OK;
    ESP_LOGE(TAG, "AQI PM2.5 = %d", *pm25_val);


cleanup:

    cJSON_Delete(root);
    return err;
}
