// Logging + UART command interface

#ifndef TELEMETRY_H
#define TELEMETRY_H

#include "config.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/uart.h"
#include "string.h"

void telemetry_init(void); // Initialize telemetry peripherals (UART, etc.)
void telemetry_start(void); // Start the telemetry task in FreeRTOS
static void telemetry_send(const char *data); // Send telemetry data over UART
static void telemetry_read(char *buf, size_t len); // Read incoming data from UART (for command interface, if needed)

#endif /* TELEMETRY_H */