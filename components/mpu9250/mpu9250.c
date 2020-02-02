/**
 * Copyright (c) 2020 Marius Vikhammer
 * 
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */



#include "mpu9250.h"
#include "driver/i2c.h"
#include "esp_log.h"


#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define AK8963_ADDRESS  0x0C
#define AK8963_RA_HXL   0x03
#define AK8963_RA_CNTL1 0x0A
#define AK8963_RA_ASAX  0x10

#define MPU9250_ADRESS            0x69 // AD0 pulled up
#define MPU9250_ADDR_ACCELCONFIG  0x1C
#define MPU9250_ADDR_ACCELCONFIG_2  0x1D
#define MPU9250_ADDR_INT_PIN_CFG  0x37
#define MPU9250_ADDR_INT_ENABLE  0x38

#define MPU9250_ADDR_ACCEL_XOUT_H 0x3B
#define MPU9250_ADDR_ACCEL_YOUT_H 0x3D

#define MPU9250_ADDR_GYRO_XOUT_H  0x43
#define MPU9250_ADDR_PWR_MGMT_1   0x6B
#define MPU9250_ADDR_PWR_MGMT_2   0x6C
#define MPU9250_ADDR_MOT_DETECT_CTRL 0x69
#define MPU9250_ADDR_WAKE_MOTION_THR        0x1F
#define MPU9250_ADDR_LP_ACCEL_OD    0x1E
#define MPU9250_ADDR_WHOAMI       0x75

/* MOT_DETECT_CTRL */
#define MPU9250_ACCEL_INTEL_EN  0x7
#define MPU9250_ACCEL_INTEL_MODE  0x6


/* INT_ENABLE */
#define MPU9250_WAKE_ON_MOTION_INT_EN_PIN 0x6

/* ACCELCONFIG_2 */
#define MPU9250_ACCEL_FCHOICE_B_BIT 0x3
#define MPU9250_A_DLPFCFG_BIT       0x0


/* PWR_MGMT_1 */
#define MPU9250_SLEEP_BIT         0x6
#define MPU9250_CYCLE_BIT         0x5
#define MPU9250_GYRO_STDBY_BIT    0x4

/* PWR_MGMT_2 */
#define MPU9250_DIS_XA_BIT        0x5
#define MPU9250_DIS_YA_BIT        0x4
#define MPU9250_DIS_ZA_BIT        0x3
#define MPU9250_DIS_XG_BIT        0x2
#define MPU9250_DIS_YG_BIT        0x1
#define MPU9250_DIS_ZG_BIT        0x0


#define MPU9250_WHOAMI_VAL        0x71
#define NACK                      0x01
#define ACK                       0x00
#define ACK_REQ                    0x01

static char *TAG = "mpu9250";
static i2c_port_t i2c_port;
static _lock_t *s_i2c_lock;

static esp_err_t mpu9250_write_reg(uint8_t address, uint8_t reg, uint8_t data) {

    _lock_acquire(s_i2c_lock);

    i2c_cmd_handle_t cmd = i2c_cmd_link_create();

    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (address << 1)  | I2C_MASTER_WRITE, ACK_REQ);
    i2c_master_write_byte(cmd, reg, ACK_REQ);
    i2c_master_write_byte(cmd, data, ACK_REQ);
    i2c_master_stop(cmd);

    esp_err_t ret = i2c_master_cmd_begin(i2c_port, cmd, 500 / portTICK_RATE_MS);

    i2c_cmd_link_delete(cmd);
    _lock_release(s_i2c_lock);
    return ret;
}

static esp_err_t mpu9250_read_reg(uint8_t address, uint8_t reg, uint8_t len, uint8_t *data) {
    if (len == 0) {
        return ESP_OK;
    }
    _lock_acquire(s_i2c_lock);
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();

    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (address << 1)  | I2C_MASTER_WRITE, ACK_REQ);
    i2c_master_write_byte(cmd, reg, ACK_REQ);
    i2c_master_start(cmd);

    i2c_master_write_byte(cmd, (address << 1)  | I2C_MASTER_READ, ACK_REQ);
    if (len > 1) {
        i2c_master_read(cmd, data, len - 1, ACK);
    }
    i2c_master_read_byte(cmd , data + len - 1, NACK);
    i2c_master_stop(cmd);

    esp_err_t ret = i2c_master_cmd_begin(i2c_port, cmd, 500 / portTICK_RATE_MS);

    i2c_cmd_link_delete(cmd);
    _lock_release(s_i2c_lock);
    return ret;
}

esp_err_t mpu9250_init(i2c_port_t port, _lock_t *i2c_lock) { 
    i2c_port = port;
    s_i2c_lock = i2c_lock;
    uint8_t whoami_val = 0;
    int ret = mpu9250_read_reg(MPU9250_ADRESS, MPU9250_ADDR_WHOAMI, 1, &whoami_val);
 
    if ( whoami_val != MPU9250_WHOAMI_VAL) {
        ESP_LOGE(TAG, "Failed to init MPU9250, incorrect WHO_AM_I value: %d", whoami_val);
    }
    ESP_LOGI(TAG, "MPU9250 init successful");

    return ret;
}

void mpu9250_read_acc(int8_t *x, int8_t *y) {
    uint8_t x_acc,y_acc;
    mpu9250_read_reg(MPU9250_ADRESS, MPU9250_ADDR_ACCEL_XOUT_H, 1 , &x_acc);
    mpu9250_read_reg(MPU9250_ADRESS, MPU9250_ADDR_ACCEL_YOUT_H, 1 , &y_acc);

    *x = (int8_t) x_acc;
    *y = (int8_t) y_acc;
}


/* Prepare for sleep by powering down sensor and enabling motion interrupt */
void mpu9250_motion_wakeup_enable() { 

    
    /* Make sure accel is running and disable gyro */
    mpu9250_write_reg(MPU9250_ADRESS, MPU9250_ADDR_PWR_MGMT_1, 
                    0 << MPU9250_SLEEP_BIT | 0 << MPU9250_CYCLE_BIT | 0 << MPU9250_GYRO_STDBY_BIT);
    mpu9250_write_reg(MPU9250_ADRESS, MPU9250_ADDR_PWR_MGMT_2,
                    1 << MPU9250_DIS_YG_BIT | 1 << MPU9250_DIS_XG_BIT | 1 << MPU9250_DIS_ZG_BIT |
                    0 << MPU9250_DIS_YA_BIT | 0 << MPU9250_DIS_XA_BIT | 0 << MPU9250_DIS_ZA_BIT);


    /* Set Accel LPF setting to 184 Hz Bandwidth */
    mpu9250_write_reg(MPU9250_ADRESS, MPU9250_ADDR_ACCELCONFIG_2, 1 << MPU9250_ACCEL_FCHOICE_B_BIT | 1 << MPU9250_A_DLPFCFG_BIT);


    /* Enable only motion interrupt */
    mpu9250_write_reg(MPU9250_ADRESS, MPU9250_ADDR_INT_ENABLE, 1 << MPU9250_WAKE_ON_MOTION_INT_EN_PIN);

    /* Enable HW intelligence, set accel treshhold */
    mpu9250_write_reg(MPU9250_ADRESS, MPU9250_ADDR_WAKE_MOTION_THR, 0xFF);
    mpu9250_write_reg(MPU9250_ADRESS, MPU9250_ADDR_MOT_DETECT_CTRL, 0xC0);


    /* Set wake up frequency, 0x4 = 4 Hz */
    mpu9250_write_reg(MPU9250_ADRESS, MPU9250_ADDR_LP_ACCEL_OD, 0x3);

    /* Enter low power cycle mode */
    mpu9250_write_reg(MPU9250_ADRESS, MPU9250_ADDR_PWR_MGMT_1, 1 << MPU9250_CYCLE_BIT);

}