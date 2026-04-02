#include <stdio.h>
#include <inttypes.h>
#include "sdkconfig.h"
#include "esp_chip_info.h"
#include "esp_flash.h"
#include "esp_system.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "config.h"
#include "telemetry.h"
#include "motor_control.h"

static void system_init(void)
{
    motor_control_init(); // Initialize motor control peripherals
    telemetry_init(); // Initialize telemetry peripherals
}

static void system_start_tasks(void)
{
    motor_control_start(); // Start the motor control task in FreeRTOS
    telemetry_start(); // Start the telemetry task in FreeRTOS
}


void app_main(void)
{   
    // Bootup Test Messages
    printf("Hello world!\n");
    printf("ESP-IDF version: %s\n", esp_get_idf_version());

    // Start FreeRTOS Tasks
    system_init();
    system_start_tasks();
}
