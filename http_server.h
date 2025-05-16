/**
 * @file http_server.h
 * @brief Header file for a simple HTTP server that serves sensor data and visualizations.
 *
 * This file includes HTML content for displaying temperature and humidity data 
 * using Chart.js. It is designed for use with the HTU21D sensor on a Raspberry Pi.
 */
/*
 * http_server.h
 * 
 * Header file for a simple HTTP server that serves sensor data and visualizations.
 * Includes HTML content for displaying temperature and humidity data using Chart.js.
 * Designed for use with the HTU21D sensor on a Raspberry Pi.
 */

#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

#include <microhttpd.h> // MicroHTTPD library for HTTP server
#include <string.h>
#include <stdio.h>
#include <time.h>

#include "sensor_reader.h" // Custom header for reading sensor data

#define PORT 80 // Port number for the HTTP server

// HTML content served by the HTTP server
static const char *html_page =
"<!DOCTYPE html>"
"<html lang='en'>"
"<head>"
"  <meta charset='UTF-8'>"
"  <meta name='viewport' content='width=device-width, initial-scale=1.0'>"
"  <title>Sensor Data</title>"
"  <script src='https://cdn.jsdelivr.net/npm/chart.js'></script>" // Include Chart.js for data visualization
"  <style>"
"    body { font-family: sans-serif; text-align: center; background-color: #f0f0f0; padding: 20px; }"
"    h1 { margin-bottom: 5px; }"
"    #time { font-size: 1.2em; margin-bottom: 20px; }"
"    canvas { width: 100%; max-width: 800px; height: 250px; margin: 0 auto 20px auto; background: #fff; border: 1px solid #ccc; border-radius: 10px; }"
"    pre { background-color: #fff; padding: 10px; display: inline-block; border-radius: 8px; }"
"    .canvas-container {"
"       display: flex;"
"       justify-content: space-between;"
"       gap: 10px"
"    }"
"  </style>"
"</head>"
"<body>"
"  <h1>HTU21D @ Raspberry Pi Zero 2 W</h1>"
"  <div id='time'>--:--:--</div>" // Placeholder for current time
"  <pre id='data'>Loading...</pre><br><br>" // Placeholder for sensor data
"  <div class='canvas-container'>"
"    <canvas id='tempChart' width='800' height='300'></canvas>" // Temperature chart
"    <canvas id='humidityChart' width='800' height='300'></canvas>" // Humidity chart
"  </div>"
"  <script>"
"    let tempChart, humidityChart;"
"    function fetchData() {" // Fetch current sensor data
"      fetch('/data').then(r => r.json()).then(d => {"
"        document.getElementById('data').innerText = "
"          `Temperature: ${d.temperature} °C\\nHumidity: ${d.humidity} %`;"
"      });"
"    }"
"    function fetchHistory() {" // Fetch historical data for charts
"      fetch('/history').then(r => r.json()).then(d => {"
"        updateCharts(d.temperature, d.humidity);"
"      });"
"    }"
"    function updateCharts(temperature, humidity) {" // Update chart data
"      tempChart.data.datasets[0].data = temperature;"
"      humidityChart.data.datasets[0].data = humidity;"
"      tempChart.update();"
"      humidityChart.update();"
"    }"
"    function updateTime() {" // Update the displayed time
"      const now = new Date();"
"      document.getElementById('time').innerText = now.toLocaleTimeString();"
"    }"
"    function initCharts() {" // Initialize the charts
"      const labels = Array.from({length: 300}, (_, i) => i + 1);" // X-axis labels
"      const ctx1 = document.getElementById('tempChart').getContext('2d');"
"      const ctx2 = document.getElementById('humidityChart').getContext('2d');"
"      tempChart = new Chart(ctx1, {"
"        type: 'line',"
"        data: {"
"          labels: labels,"
"          datasets: [{"
"            label: 'Temperature (°C)',"
"            borderColor: 'red',"
"            borderWidth: 1,"
"            data: [],"
"            fill: false,"
"            tension: 0.1,"
"            pointRadius: 0"
"          }]"
"        },"
"        options: {"
"          scales: { x: { type: 'linear', min: 0, max: 300, ticks: { stepSize: 5 } }, y: { beginAtZero: true, min: 0, max: 40  } },"
"          animation: false"
"        }"
"      });"
"      humidityChart = new Chart(ctx2, {"
"        type: 'line',"
"        data: {"
"          labels: labels,"
"          datasets: [{"
"            label: 'Humidity (%)',"
"            borderColor: 'blue',"
"            borderWidth: 1,"
"            data: [],"
"            fill: false,"
"            tension: 0.1,"
"            pointRadius: 0"
"          }]"
"        },"
"        options: {"
"          scales: { x: { type: 'linear', min: 0, max: 300, ticks: { stepSize: 5 } }, y: { beginAtZero: true, min: 0, max: 100 } },"
"          animation: false"
"        }"
"      });"
"    }"
"    initCharts();" // Initialize charts on page load
"    setInterval(fetchData, 1000);" // Fetch data every second
"    setInterval(fetchHistory, 1000);" // Fetch history every second
"    setInterval(updateTime, 1000);" // Update time every second
"  </script>"
"</body>"
"</html>";

#endif