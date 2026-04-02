#include "telemetry.h"

/* Telemetry Task Configuration */
#define TELEMETRY_TASK_STACK_SIZE 4096
#define TELEMETRY_TASK_PRIORITY 1
static TaskHandle_t telemetry_task_handle = NULL;

/* Static function declarations */
static void telemetry_task(void *pvParameters);

/*--------------------------------------------------
 * Function:    Initialize Telemetry
 * Description: Initializes the telemetry peripherals
 * Parameters:  None
 * Returns:     None
 *-------------------------------------------------*/
void telemetry_init(void)
{
    // Initialize UART for telemetry
    // General idea:
    // - Configure UART parameters (baud rate, data bits, stop bits, parity)
    // - Install UART driver
    // - Set up any necessary buffers or queues for telemetry data
}

/*--------------------------------------------------
 * Function:    Initialize Telemetry Task
 * Description: FreeRTOS task that runs the telemetry loop, 
 *              including sending data over UART
 * Parameters:  None
 * Returns:     None
 *-------------------------------------------------*/
void telemetry_start(void)
{
    xTaskCreate(telemetry_task, "Telemetry Task", 
                TELEMETRY_TASK_STACK_SIZE, NULL, 
                TELEMETRY_TASK_PRIORITY, &telemetry_task_handle);
}

/*--------------------------------------------------
 * Function:    Telemetry Task Loop
 * Description: Main loop for the telemetry task, 
 *              responsible for gathering data and 
 *              sending it over UART at a fixed sample rate
 * Parameters:  None
 * Returns:     None
 *-------------------------------------------------*/
static void telemetry_task(void *pvParameters)
{
    while (1) {
        // Placeholder for telemetry logic
        // General idea:
        // - Run the telemetry loop here at a fixed sample rate.
        // - Gather any relevant data (e.g. motor state, encoder readings, etc.)
        // - Format the data into a string or binary format for transmission
        // - Send the data over UART using telemetry_send()
        vTaskDelay(pdMS_TO_TICKS(TELEMETRY_SAMPLE_RATE_MS)); // Delay for telemetry sample rate
    }
}

/*--------------------------------------------------
 * Function:    Send Telemetry Data
 * Description: Sends telemetry data over UART
 * Parameters:  const char *data: Data to send
 * Returns:     None
 *-------------------------------------------------*/
void telemetry_send(const char *data)
{
    // Placeholder for UART transmission logic
    // General idea:
    // - Use the UART driver to send the provided data string over the UART interface
    // - Handle any necessary buffering or formatting for the UART transmission
}