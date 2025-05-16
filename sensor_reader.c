/**
 * @file sensor_reader.c
 * @brief Sensor data acquisition module.
 *
 * This file implements functionality for reading temperature and humidity
 * data from a sensor using I2C communication. It provides real-time data
 * updates and maintains a history of sensor readings.
 *
 * Features:
 * - Reads temperature and humidity data from the sensor.
 * - Maintains a circular buffer for historical data.
 * - Provides functions to retrieve the latest sensor data and history.
 * - Runs a dedicated thread to continuously update sensor readings.
 *
 * Functions:
 * - `read_temperature`: Reads and calculates the temperature from the sensor.
 * - `read_humidity`: Reads and calculates the humidity from the sensor.
 * - `sensor_loop`: Continuously reads sensor data and updates shared variables.
 * - `start_sensor_loop`: Starts a thread to run the sensor loop.
 * - `get_latest_sensor_data`: Retrieves the latest sensor data in JSON format.
 * - `get_history`: Retrieves historical temperature and humidity data.
 *
 * Dependencies:
 * - I2C communication for sensor interaction (I2C-DEV).
 * - POSIX threads for running the sensor loop in a separate thread.
 */

#include "sensor_reader.h"

/* Global variables */
static char latest_data[128] = "No data"; // Buffer to store the latest sensor data in JSON format, including temperature, humidity, or error messages.

static float temperature_history[MAX_HISTORY] = {0}; // Circular buffer to store historical temperature readings.
static float humidity_history[MAX_HISTORY] = {0};    // Circular buffer to store historical humidity readings.
static int history_index = 0;                        // Index to track the current position in the circular buffers.

const uint8_t read_tmp = 0xF3; // Temperature read command
const uint8_t read_hum = 0xF5; // Humidity read command

/* Function definitions */
/**
 * @brief Retrieves the latest sensor data.
 * 
 * @return A pointer to the latest sensor data as a string.
 */
const char* get_latest_sensor_data(void) {
    return latest_data;
}

/**
 * @brief Reads temperature from the sensor.
 *
 * @param fd Sensor file descriptor.
 * @return Calculated temperature as a float.
 */
float read_temperature(int fd) {
    uint8_t data[3]; // Buffer to store raw data read from the sensor.
    int raw;         // Variable to store the raw temperature data.

    write(fd, &read_tmp, 1); // Send the temperature read command to the sensor.
    usleep(50000);           // 50ms delay to allow the sensor to process the command.
    
    read(fd, data, 3); // Read 3 bytes of data from the sensor.

    raw = (data[0] << 8) | data[1]; // Combine the first two bytes into a 16-bit raw value.
    raw &= 0xFFFC;                  // Mask the status bits from the raw data.

    return CALC_TEMP(raw);    // Convert the raw data to a temperature value using macro.
}

/**
 * @brief Reads humidity from the sensor.
 *
 * @param fd Sensor file descriptor.
 * @return Calculated humidity as a float.
 */
float read_humidity(int fd) {
    uint8_t data[3]; // Buffer to store raw data read from the sensor.
    int raw;         // Variable to store the raw temperature data.

    write(fd, &read_hum, 1); // Send the humidity read command to the sensor.
    usleep(50000);           // 50ms delay to allow the sensor to process the command.

    read(fd, data, 3);       // Read 3 bytes of data from the sensor.

    raw = (data[0] << 8) | data[1]; // Combine the first two bytes into a 16-bit raw value.
    raw &= 0xFFFC;                  // Mask the status bits from the raw data.

    return CALC_HUM(raw);    // Convert the raw data to a humidity value using macro.
}

/**
 * @brief Thread function to continuously read sensor data and update shared variables.
 *
 * This function initializes the I2C communication with the sensor, reads temperature
 * and humidity data in a loop, and updates the latest data and history buffers.
 *
 * @param arg Mandatory void argument.
 * @return NULL on error or when the thread exits.
 */
void* sensor_loop(void* arg) {
    int fd = open(I2C_DEV, O_RDWR); // Open I2C device
    /* Init */
    if (fd < 0 || ioctl(fd, I2C_SLAVE, SENSOR_ADDR) < 0) { // Check I2C initialization
        perror("I2C open error");
        strcpy(latest_data, "{\"error\": \"I2C error\"}"); // Log error
        return NULL;
    }

    /* Loop */
    while (1) {
        float temp = read_temperature(fd); // Read temperature
        float hum = read_humidity(fd);    // Read humidity

        if(hum > 100) hum = 100; // Cap humidity at 100%

        // Update latest data in JSON format
        snprintf(latest_data, sizeof(latest_data),
                 "{\"temperature\": %.2f, \"humidity\": %.2f}", temp, hum);

        // Store data in history buffers
        temperature_history[history_index] = temp;
        humidity_history[history_index] = hum;
        history_index = (history_index + 1) % MAX_HISTORY; // Update circular index

        sleep(1); // Wait 1 second before next reading
    }

    close(fd); // Close I2C device
    return NULL;
}

/**
 * @brief Starts a new thread to run the sensor loop.
 * 
 * This function creates a detached thread that executes the 
 * sensor_loop function. The thread runs independently and 
 * does not require manual joining.
 */
void start_sensor_loop(void) {
    pthread_t tid; // Thread identifier
    pthread_create(&tid, NULL, sensor_loop, NULL); // Create a new thread to run sensor_loop
    pthread_detach(tid); // Detach the thread to allow it to run independently
}

/**
 * @brief Retrieves the historical temperature and humidity data.
 * 
 * @param temp_history Pointer to an array where temperature history will be stored.
 * @param hum_history Pointer to an array where humidity history will be stored.
 */
void get_history(float* temp_history, float* hum_history) {
    int i = 0;
    for (i = 0; i < MAX_HISTORY; i++) {
        // Copy temperature history in circular order
        temp_history[i] = temperature_history[(history_index + i) % MAX_HISTORY];
        // Copy humidity history in circular order
        hum_history[i] = humidity_history[(history_index + i) % MAX_HISTORY];
    }
}