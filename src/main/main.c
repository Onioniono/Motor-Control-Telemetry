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
#include "electronic_load.h"
#include "duel_monitor.h"

static void system_init(void)
{
    motor_control_init(); // Initialize motor control peripherals
}

static void system_start_tasks(void)
{
    motor_control_start(); // Start the motor control task in FreeRTOS
    xTaskCreate(electronic_load_task, "Electronic Load Task", 4096, NULL, 3, NULL);     // Medium priority for load management
    xTaskCreate(duel_monitor_task, "Duel Monitor Task", 4096, NULL, 2, NULL);           // Medium priority for duel monitoring
    xTaskCreate(telemetry_task, "Telemetry Task", 4096, NULL, 1, NULL);                 // Low priority for telemetry
}


void app_main(void)
{   
    // Bootup Test Message
    printf("Hello world!\n");
    printf("ESP-IDF version: %s\n", esp_get_idf_version());

    // Start FreeRTOS Tasks
    system_init();
    system_start_tasks();
}
