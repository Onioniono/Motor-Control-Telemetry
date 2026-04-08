#include "telemetry.h"
#include "motor_control.h"

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
    const uart_config_t uart_config = {
        .baud_rate = 115200,                     // Set baud rate for UART communication
        .data_bits = UART_DATA_8_BITS,           // Use 8 data bits
        .parity = UART_PARITY_DISABLE,           // Disable parity bit
        .stop_bits = UART_STOP_BITS_1,           // Use 1 stop bit
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,    // Disable hardware flow control
        .source_clk = UART_SCLK_APB,             // Use APB clock for UART source
    };
    // Install UART driver
    uart_driver_install(UART, 1024, 0, 0, NULL, 0); // Install UART driver with buffer size
    uart_param_config(UART, &uart_config);              // Configure UART parameters
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
    char data[128]; // Buffer to hold telemetry data to send
    while (1) {
        telemetry_read(data, sizeof(data));                     // Read current telemetry data into buffer
        telemetry_send(data);                                   // Send telemetry data over UART
        vTaskDelay(pdMS_TO_TICKS(TELEMETRY_SAMPLE_RATE_MS));    // Delay for telemetry sample rate
    }
}

/*--------------------------------------------------
 * Function:    Send Telemetry Data
 * Description: Sends telemetry data over UART0
 * Parameters:  const char *data: Data to send
 * Returns:     None
 *-------------------------------------------------*/
static void telemetry_send(const char *data)
{
    char buf[128];                                      // Buffer to hold formatted telemetry data
    snprintf(buf, sizeof(buf), "DATA:%s", data);        // Format data with a prefix for easier parsing on the receiving end
    uart_write_bytes(UART_NUM_0, buf, strlen(buf));     // Send formatted telemetry data over UART
}

/*--------------------------------------------------
 * Function:    Read Telemetry Data
 * Description: Reads current telemetry data from global variables and formats it into a string buffer
 *              for sending over UART. This function is called from the telemetry task loop.
 * Parameters:  char *buf: Buffer to hold formatted telemetry data
 *              size_t len: Length of the buffer
 * Returns:     None
 *-------------------------------------------------*/
static void telemetry_read(char *buf, size_t len)
{
    xSemaphoreTake(motor_data_mutex, portMAX_DELAY);    // Take mutex to safely read shared motor control variables for telemetry output
    float local_target_rpm = target_rpm;                // Read target RPM for telemetry output
    float local_current_rpm = current_rpm;              // Read current RPM for telemetry output
    int local_pwm_value = motor_pwm_value;              // Read current motor PWM value for telemetry output
    int local_motor_direction = motor_direction;        // Read current motor direction for telemetry output
    xSemaphoreGive(motor_data_mutex);                   // Give mutex after reading shared variables
    snprintf(buf, len, "Target RPM: %.2f, Current RPM: %.2f, PWM: %d, Direction: %s\n", 
             local_target_rpm, local_current_rpm, local_pwm_value, 
             (local_motor_direction == 1) ? "Forward" : "Reverse"); // Format telemetry data into buffer
}