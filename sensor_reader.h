/**
 * @file sensor_reader.h
 * @brief Header file for reading temperature and humidity sensor data via I2C.
 *
 * This file provides functions to start the sensor reading loop, retrieve the latest
 * sensor data, and access historical temperature and humidity data.
 */

#ifndef SENSOR_READER_H
#define SENSOR_READER_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>  // For threading
#include <fcntl.h>
#include <linux/i2c-dev.h>  // For I2C communication
#include <sys/ioctl.h>  // For I2C device control

#define I2C_DEV "/dev/i2c-1"  // I2C device path
#define SENSOR_ADDR 0x40  // Sensor I2C address
#define MAX_HISTORY 300  // Maximum history size for sensor data

// Macros to calculate temperature and humidity from raw sensor data
#define CALC_TEMP(raw) (-46.85 + (175.72 * ((float)raw / 65536.0)))
#define CALC_HUM(raw)  (-6.0 + (125.0 * ((float)raw / 65536.0)))

// Starts the sensor reading loop in a separate thread
void start_sensor_loop(void);

// Retrieves the latest sensor data as a string
const char* get_latest_sensor_data(void);

// Retrieves historical temperature and humidity data
void get_history(float* temp_history, float* hum_history);

#endif
