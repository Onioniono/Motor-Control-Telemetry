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

void task_setup(void)
{
    // Create FreeRTOS Tasks
    // xTaskCreate(task_function, "Task Name", stack_size, parameters, priority, task_handle);
    xTaskCreate(motor_control_task, "Motor Control Task", 4096, NULL, 4, NULL);         // High priority for motor control
    xTaskCreate(electronic_load_task, "Electronic Load Task", 4096, NULL, 3, NULL);     // Medium priority for load management
    xTaskCreate(duel_monitor_task, "Duel Monitor Task", 4096, NULL, 2, NULL);           // Medium priority for duel monitoring
    xTaskCreate(telemetry_task, "Telemetry Task", 4096, NULL, 1, NULL);                 // Low priority for telemetry
    // Note: Stack size and priority are placeholders and may need adjustment based on actual task requirements
}

void app_main(void)
{   
    // Bootup Test Message
    printf("Hello world!\n");
    printf("ESP-IDF version: %s\n", esp_get_idf_version());

    // Task Setup
    task_setup();
}
