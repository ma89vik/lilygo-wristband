// Simple PCF8563 (Real Time Clock) driver
// Copyright (c) 2020 Marius Vikhammer
// 
// This software is released under the MIT License.
// https://opensource.org/licenses/MIT

#include "pcf8563.h"
#include "driver/i2c.h"
#include "esp_log.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sys/lock.h"

#include "string.h"

#define PCF8563_ADDRESS 0xA2
#define NACK            0x01
#define ACK             0x00
#define ACK_REQ         0x01

#define PCF8563_SEC_REG         0x02
#define PCF8563_SECS_READ_MASK  0x7f
#define PCF8563_MINS_READ_MASK  0x7f
#define PCF8563_HOURS_READ_MASK 0x3f
#define PCF8563_DAYS_READ_MASK  0x3f
#define PCF8563_WKDAY_READ_MASK  0x07
#define PCF8563_MONTH_READ_MASK 0x1f
#define PCF8563_YEARS_READ_MASK 0xff

typedef struct {
    union {
        struct {
            uint8_t low: 4;
            uint8_t high: 4;
        };
        uint8_t val;
    };
} bct8_t;

typedef struct {
    bct8_t seconds;
    bct8_t minutes;
    bct8_t hours;
    bct8_t days;
    bct8_t weekdays;
    bct8_t months;
    bct8_t years;

} pcf8563_time_t;

static i2c_port_t s_i2c_port;
static _lock_t *s_i2c_lock;
static char *TAG = "pcf8563";

void pcf8563_init(i2c_port_t port, _lock_t *i2c_lock) {
    s_i2c_port = port;
    s_i2c_lock = i2c_lock;
}

static uint8_t bct8_to_uint8(bct8_t val) {
    return (val.low + val.high*10);
}

static bct8_t uint8_to_bct8(uint8_t val) {
    bct8_t res;
    res.high = val/10;
    res.low = val%10;
    return res;
}

static esp_err_t pcf8563_write_reg(uint8_t reg, uint8_t *data, size_t len) {
    _lock_acquire(s_i2c_lock);
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();

    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (PCF8563_ADDRESS)  | I2C_MASTER_WRITE, ACK_REQ);
    i2c_master_write_byte(cmd, reg, ACK_REQ);

    for (int i = 0; i < len; i++) {
        i2c_master_write_byte(cmd, data[i], ACK_REQ);
    }
    i2c_master_stop(cmd);


    esp_err_t ret = i2c_master_cmd_begin(s_i2c_port, cmd, 500 / portTICK_RATE_MS);

    i2c_cmd_link_delete(cmd);
    _lock_release(s_i2c_lock);
    return ret;
}

static esp_err_t pcf8563_read_reg(uint8_t reg, uint8_t len, uint8_t *data) {
    if (len == 0) {
        return ESP_OK;
    }
    _lock_acquire(s_i2c_lock);
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();

    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (PCF8563_ADDRESS)  | I2C_MASTER_WRITE, ACK_REQ);
    i2c_master_write_byte(cmd, reg, ACK_REQ);
    i2c_master_start(cmd);

    i2c_master_write_byte(cmd, (PCF8563_ADDRESS)  | I2C_MASTER_READ, ACK_REQ);
    if (len > 1) {
        i2c_master_read(cmd, data, len - 1, ACK);
    }
    i2c_master_read_byte(cmd , data + len - 1, NACK);
    i2c_master_stop(cmd);

    esp_err_t ret = i2c_master_cmd_begin(s_i2c_port, cmd, 500 / portTICK_RATE_MS);

    i2c_cmd_link_delete(cmd);
    _lock_release(s_i2c_lock);
    return ret;
}

static void pcf8563_rm_unused_bits(pcf8563_time_t* time) {
    time->seconds.val &= PCF8563_SECS_READ_MASK;
    time->minutes.val &= PCF8563_MINS_READ_MASK;
    time->hours.val &= PCF8563_HOURS_READ_MASK;
    time->days.val &= PCF8563_DAYS_READ_MASK;
    time->weekdays.val &= PCF8563_WKDAY_READ_MASK;
    time->months.val &= PCF8563_MONTH_READ_MASK;
    time->years.val &= PCF8563_YEARS_READ_MASK;
}

void pcf8563_set_time(struct tm *time) {

    pcf8563_time_t cur_time;
    
    cur_time.seconds = uint8_to_bct8(time->tm_sec);
    cur_time.minutes = uint8_to_bct8(time->tm_min);
    cur_time.hours = uint8_to_bct8(time->tm_hour);
    cur_time.days = uint8_to_bct8(time->tm_mday);
    cur_time.weekdays = uint8_to_bct8(time->tm_wday);
    cur_time.months = uint8_to_bct8((time->tm_mon + 1));
    cur_time.years = uint8_to_bct8( (time->tm_year - 100 ));

    pcf8563_write_reg(PCF8563_SEC_REG, (uint8_t*)&cur_time, sizeof(pcf8563_time_t));
}

void pcf8563_read_time(struct tm *time) {
    pcf8563_time_t cur_time;
    memset(&cur_time, 0, sizeof(cur_time));

    pcf8563_read_reg(PCF8563_SEC_REG, sizeof(cur_time), (uint8_t*)&cur_time);    
    pcf8563_rm_unused_bits(&cur_time);

    time->tm_sec = bct8_to_uint8(cur_time.seconds);
    time->tm_min = bct8_to_uint8(cur_time.minutes);
    time->tm_hour = bct8_to_uint8(cur_time.hours);
    time->tm_mday = bct8_to_uint8(cur_time.days);
    time->tm_wday = bct8_to_uint8(cur_time.weekdays);
    time->tm_mon = bct8_to_uint8(cur_time.months) - 1;
    /* Year is given as 0 - 99, need to be number of years from 1900 */
    time->tm_year = bct8_to_uint8(cur_time.years) + 100;

    ESP_LOGE(TAG, "%d:%d:%d, day %d, weekday %d, month %d year %d", time->tm_hour, time->tm_min, time->tm_sec, time->tm_mday, time->tm_wday, time->tm_mon,time->tm_year);
}