#include "motor_control.h"

int motor_pwm_value = 255; // Test variable to hold current PWM value for motor control
int motor_direction = 1; // 1 for forward, -1 for reverse (for testing purposes)

/* Motor Control Task Configuration */
#define MOTOR_CONTROL_TASK_STACK_SIZE 4096
#define MOTOR_CONTROL_TASK_PRIORITY 4
static TaskHandle_t motor_control_task_handle = NULL;

/* Static function declarations */
static void motor_control_task(void *pvParameters);
static void IRAM_ATTR encoder_isr_handler(void *arg);
static void encoder_read(void);

/*--------------------------------------------------
 * Function:    Initalize Motor Control
 * Description: Initializes the motor control peripherals
 * Parameters:  None
 * Returns:     None
 *-------------------------------------------------*/
void motor_control_init(void)
{
    //Motor Driver Pins
    gpio_reset_pin(MOTOR_PWM_PIN);
    gpio_reset_pin(MOTOR_IN1_PIN);
    gpio_reset_pin(MOTOR_IN2_PIN);

    gpio_set_direction(MOTOR_IN1_PIN, GPIO_MODE_OUTPUT);
    gpio_set_direction(MOTOR_IN2_PIN, GPIO_MODE_OUTPUT);

    //Magnetic Encoder Pins
    gpio_set_direction(ENCODER_PIN_A, GPIO_MODE_INPUT);
    gpio_set_pull_mode(ENCODER_PIN_A, GPIO_PULLUP_ONLY); // Enable pull-up for encoder input
    gpio_set_direction(ENCODER_PIN_B, GPIO_MODE_INPUT);
    gpio_set_pull_mode(ENCODER_PIN_B, GPIO_PULLUP_ONLY); // Enable pull-up for encoder input

    //PWM Timer Setup
    ledc_timer_config_t ledc_timer = {
        .speed_mode = LEDC_LOW_SPEED_MODE,      //Use low speed mode for motor control
        .timer_num = LEDC_TIMER_0,              //Use timer 0 for motor PWM
        .duty_resolution = LEDC_TIMER_8_BIT,    //8-bit resolution duty cycle (0-255)
        .freq_hz = MOTOR_PWM_FREQ_HZ,           // 5 kHz PWM frequency
        .clk_cfg = LEDC_AUTO_CLK                // Auto select source clock
    };
    ledc_timer_config(&ledc_timer);             // Configure timer for motor PWM
    //PWM Channel Setup
    ledc_channel_config_t ledc_channel = {
        .speed_mode = LEDC_LOW_SPEED_MODE,      // Use low speed mode for motor control
        .channel = LEDC_CHANNEL_0,              // Use channel 0 for motor PWM
        .timer_sel = LEDC_TIMER_0,              // Select timer 0 for motor PWM
        .intr_type = LEDC_INTR_DISABLE,         // Disable interrupts
        .gpio_num = MOTOR_PWM_PIN,              // GPIO pin for motor PWM
        .duty = 0,                              // Start with 0% duty cycle (motor off)
        .hpoint = 0                             // Not used in this configuration
    };
    ledc_channel_config(&ledc_channel);         // Configure channel for motor PWM
    
    //Interrupt for Encoder Reading
    gpio_set_intr_type(ENCODER_PIN_A, GPIO_INTR_POSEDGE);           // Interrupt on rising edge of encoder A signal
    gpio_install_isr_service(0);                                    // Install GPIO ISR service with default configuration
    gpio_isr_handler_add(ENCODER_PIN_A, encoder_isr_handler, NULL); // Add ISR handler for encoder A pin
    gpio_intr_enable(ENCODER_PIN_A);                                // Enable interrupt for encoder A pin
}

/*--------------------------------------------------
 * Function:    Initialize Motor Control Task
 * Description: FreeRTOS task that runs the motor control loop, 
 *              including reading encoder values, computing PID, 
 *              and setting motor PWM
 * Parameters:  None
 * Returns:     None
 *-------------------------------------------------*/
void motor_control_start(void)
{
    xTaskCreate(motor_control_task, "Motor Control Task", 
                MOTOR_CONTROL_TASK_STACK_SIZE, NULL, 
                MOTOR_CONTROL_TASK_PRIORITY, &motor_control_task_handle);
}

/*--------------------------------------------------
 * Function:    Set Motor PWM and Direction
 * Description: Sets the motor PWM duty cycle and direction based on input parameters
 * Parameters:  int pwm: PWM duty cycle (0-255)
 *              int dir: Direction (1 for forward, -1 for reverse)
 * Returns:     None
 *-------------------------------------------------*/
static void motor_control_task(void *pvParameters)
{
    while (1) {
        // Placeholder for PID control logic
        // Run Control Loop Here

        encoder_read();
        motor_set(motor_pwm_value, motor_direction);
        vTaskDelay(pdMS_TO_TICKS(PID_SAMPLE_RATE_MS)); // Delay for PID sample rate
    }
}

/*--------------------------------------------------
 * Function:    Set Motor PWM and Direction
 * Description: Sets the motor PWM duty cycle and direction based on input parameters
 * Parameters:  int pwm: PWM duty cycle (0-255)
 *              int dir: Direction (1 for forward, -1 for reverse)
 * Returns:     None
 *-------------------------------------------------*/
void motor_set(int pwm, int dir)
{
    //Constrain PWM to 0-255 range
    (pwm < 0) ? (pwm = 0) : (pwm > 255) ? (pwm = 255) : pwm;
    // Set motor direction
    gpio_set_level(MOTOR_IN1_PIN, (dir == 1) ? 1 : 0);
    gpio_set_level(MOTOR_IN2_PIN, (dir == -1) ? 1 : 0);
    // Apply PWM Duty Cycle
    ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, pwm);
    ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
}

/*--------------------------------------------------
 * Function:    Read Encoder Values
 * Description: Reads the values from the magnetic encoder
 * Parameters:  None
 * Returns:     None
 *-------------------------------------------------*/
static void IRAM_ATTR encoder_isr_handler(void *arg)
{
    // Placeholder for encoder ISR logic
    // This will be called on the rising edge of encoder A signal
}


/*--------------------------------------------------
 * Function:    Read Encoder Values
 * Description: Reads the values from the magnetic encoder
 * Parameters:  None
 * Returns:     None
 *-------------------------------------------------*/
 static void encoder_read(void)
{
   //Placeholder for encoder reading logic
}