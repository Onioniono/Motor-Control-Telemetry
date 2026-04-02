// Logging + UART command interface

#ifndef TELEMETRY_H
#define TELEMETRY_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/uart.h"
#include "config.h"

void telemetry_init(void); // Initialize telemetry peripherals (UART, etc.)
void telemetry_start(void); // Start the telemetry task in FreeRTOS
void telemetry_send(const char *data); // Send telemetry data over UART

#endif /* TELEMETRY_H */