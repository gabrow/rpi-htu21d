/**
 * @file http_server.c
 * @brief HTTP server for serving sensor data.
 *
 * This file implements a simple HTTP server using the MHD (MicroHTTPD) library.
 * The server provides endpoints to retrieve the latest sensor data and historical
 * temperature and humidity readings in JSON format. It also serves a default HTML
 * page for unsupported routes.
 *
 * Key Features:
 * - `/data`: Returns the latest temperature and humidity readings as a JSON object.
 * - `/history`: Returns historical temperature and humidity data as a JSON object.
 * - Default route: Serves an HTML page for unsupported endpoints.
 *
 * Functions:
 * - `handler`: Handles incoming HTTP requests and generates appropriate responses.
 * - `main`: Initializes the sensor loop and starts the HTTP server daemon.
 *
 * Dependencies:
 * - MHD (MicroHTTPD) library for HTTP server functionality.
 * - Sensor data acquisition module (`sensor_reader.c`).
 */

#include "http_server.h"

/**
 * @brief HTTP request handler for the server.
 *
 * Handles different endpoints ("/data", "/history", others) and generates appropriate HTTP responses.
 *
 * @param cls Unused user-defined pointer.
 * @param connection The MHD connection object.
 * @param url The requested URL.
 * @param method The HTTP method (e.g., "GET").
 * @param version The HTTP version.
 * @param upload_data Data uploaded by the client (unused).
 * @param upload_data_size Size of the uploaded data (unused).
 * @param con_cls Connection-specific pointer (unused).
 * @return MHD result code.
 */
int handler(void *cls, struct MHD_Connection *connection,
            const char *url, const char *method,
            const char *version, const char *upload_data,
            size_t *upload_data_size, void **con_cls)
{
    const char *response_data;
    struct MHD_Response *response;
    int ret;

    if (strcmp(url, "/data") == 0) // Handle /data endpoint
    {
        response_data = get_latest_sensor_data(); // Get latest sensor data as a JSON string
        response = MHD_create_response_from_buffer(strlen(response_data),
                                                   (void *)response_data, MHD_RESPMEM_PERSISTENT); // Create HTTP response
        MHD_add_response_header(response, "Content-Type", "application/json");                     // Set response content type to JSON
    }
    else if (strcmp(url, "/history") == 0) // Handle /history endpoint
    {
        float temp_history[MAX_HISTORY], hum_history[MAX_HISTORY];
        get_history(temp_history, hum_history); // Retrieve historical temperature and humidity data

        char json_response[8192];                     // Buffer to hold the JSON response
        strcpy(json_response, "{\"temperature\": ["); // Start JSON response with temperature array

        for (int i = 0; i < MAX_HISTORY; i++) // Build temperature JSON array
        {
            char num[32];
            snprintf(num, sizeof(num), "%.2f", temp_history[i]); // Format temperature value
            strcat(json_response, num);                          // Append value to JSON response
            if (i < MAX_HISTORY - 1)
                strcat(json_response, ","); // Add comma between values
        }
        strcat(json_response, "],\"humidity\": ["); // Start humidity array in JSON response

        for (int i = 0; i < MAX_HISTORY; i++) // Build humidity JSON array
        {
            char num[32];
            snprintf(num, sizeof(num), "%.2f", hum_history[i]); // Format humidity value
            strcat(json_response, num);                         // Append value to JSON response
            if (i < MAX_HISTORY - 1)
                strcat(json_response, ","); // Add comma between values
        }
        strcat(json_response, "]}"); // Close JSON response

        response = MHD_create_response_from_buffer(strlen(json_response),
                                                   (void *)json_response, MHD_RESPMEM_PERSISTENT); // Create HTTP response
        MHD_add_response_header(response, "Content-Type", "application/json");                     // Set response content type to JSON
    }
    else // Handle unsupported routes
    {
        response_data = html_page; // Serve default HTML page for unsupported routes
        response = MHD_create_response_from_buffer(strlen(response_data),
                                                   (void *)response_data, MHD_RESPMEM_PERSISTENT); // Create HTTP response
        MHD_add_response_header(response, "Content-Type", "text/html");                            // Set response content type to HTML
    }

    ret = MHD_queue_response(connection, MHD_HTTP_OK, response); // Send the HTTP response to the client
    MHD_destroy_response(response);                              // Clean up the response object
    return ret;                                                  // Return the status of the response queuing
}

/**
 * @brief Entry point of the HTTP server application.
 *
 * Initializes the sensor loop and starts the HTTP server daemon
 * using the MHD library. The server runs indefinitely, handling
 * incoming requests on the specified port.
 *
 * @return Returns 0 on successful execution, or 1 if the server
 *         daemon fails to start.
 */
int main(void)
{
    start_sensor_loop(); // Start the sensor data acquisition loop in a separate thread or process

    // Start the HTTP server daemon on the specified port
    struct MHD_Daemon *daemon = MHD_start_daemon(MHD_USE_INTERNAL_POLLING_THREAD,
                                                 PORT, NULL, NULL,
                                                 &handler, NULL, MHD_OPTION_END);
    if (NULL == daemon) // Check if the server failed to start
        return 1;

    printf("HTTP server started on port %d\n", PORT); // Log that the server has started successfully

    while (1)
    {
        sleep(60); // Keep the server running by sleeping in an infinite loop
    }

    MHD_stop_daemon(daemon); // Stop the HTTP server daemon when the program exits
    return 0; // Return 0 to indicate successful execution
}
