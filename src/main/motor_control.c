#include "motor_control.h"

/* Motor Control Variables */
int motor_pwm_value = 0;      // Default PWM value (75% duty cycle for 8-bit resolution)
int motor_direction = 1;        // 1 for forward, -1 for reverse 

/* Motor Control Task Configuration */
#define MOTOR_CONTROL_TASK_STACK_SIZE 4096
#define MOTOR_CONTROL_TASK_PRIORITY 2
static TaskHandle_t motor_control_task_handle = NULL;

/* Static function declarations */
static void motor_control_task(void *pvParameters);     // FreeRTOS task function for motor control loop
static void IRAM_ATTR encoder_isr_handler(void *arg);   // ISR handler for encoder events, marked IRAM_ATTR for faster execution
static void encoder_read(void);                         // Function to read encoder values and update state for PID calculations
static float low_pass_filter(float v_now);              // Function to apply low-pass filter to velocity measurements (if needed)
static void PID_control(void);                          // Function to compute PID control output based on target and current RPM (to be implemented)

/* PID Variables */
float target_rpm = RPM_DEFAULT;     // Target RPM for the motor, set by control loop or external commands
static float current_rpm = 0.0f;    // Current RPM calculated from encoder readings, used for PID feedback
static float kp = 5.0f;             // Proportional gain for PID controller
static float ki = 0.0f;             // Integral gain for PID controller
static float kd = 0.0f;             // Derivative gain for PID controller

/* Encoder Variables */
volatile int pos_i = 0;         // Current encoder position count
volatile int dir_i = 0;         // Current encoder direction (1 for forward, -1 for reverse)
volatile int32_t deltaT_i = 0;  // Time delta since last encoder event in microseconds, used for velocity calculation
volatile int64_t prevT_i = 0;   // Timestamp of last encoder event in microseconds, used for velocity calculation
static portMUX_TYPE encoderMux = portMUX_INITIALIZER_UNLOCKED;  // Mutex for protecting access to encoder state variables between ISR and task context

/* Low Pass Filter Variables */
static float v_filt = 0.0f;  // Filtered velocity value
static float v_prev = 0.0f;  // Previous velocity value, used for low-pass filter calculations

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
 * Function:    Motor Control Task Loop
 * Description: Main loop for the motor control task, 
 *              responsible for reading encoder values, 
 *              computing PID, and setting motor PWM
 * Parameters:  None
 * Returns:     None
 *-------------------------------------------------*/
static void motor_control_task(void *pvParameters)
{
    while (1) {
        encoder_read();
        PID_control();
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
    // Print for testing purposes
    printf(", Motor PWM: %d, Direction: %s\n", pwm, (dir == 1) ? "Forward" : "Reverse");
}

/*--------------------------------------------------
 * Function:    Encoder ISR Handler
 * Description: Interrupt Service Routine for handling encoder events,
 *              updates encoder state for processing in main control loop
 * Parameters:  None
 * Returns:     None
 *-------------------------------------------------*/
static void IRAM_ATTR encoder_isr_handler(void *arg)
{
    // General idea:
    // - Read the state of encoder A and B pins
    // - Update a count variable based on the direction of rotation
    // - Count is processed in encoder_read to determine position/speed

   int b = gpio_get_level(ENCODER_PIN_B);           // Read state of encoder B pin
   int increment = 0;
   (b > 0) ? (increment = 1) : (increment = -1);    // Determine direction based on B pin state
   
    int64_t currT = esp_timer_get_time();   // Get current time in microseconds
    int32_t deltaT = 0;                     // Variable to hold time delta since last encoder event

    portENTER_CRITICAL_ISR(&encoderMux);    // Enter critical section to safely update shared encoder state
    deltaT = (int32_t)(currT - prevT_i);    // Calculate time since last encoder event
    deltaT_i = deltaT;                      // Update global variable for time delta
    dir_i = increment;                      // Update global variable for direction
    pos_i += increment;                     // Update encoder count based on direction
    prevT_i = currT;                        // Update previous time for next calculation
    portEXIT_CRITICAL_ISR(&encoderMux);     // Exit critical section
}


/*--------------------------------------------------
 * Function:    Read Encoder Values
 * Description: Reads the values from the magnetic encoder
 * Parameters:  None
 * Returns:     None
 *-------------------------------------------------*/
 static void encoder_read(void)
{
    // General idea:
    // - Read encoder state accumulated from ISR events
    // - Convert counts to position / speed
    // - Update private module state for PID calculations

    /* Velocity Measurement */
    int pos;
    int dir;
    int32_t deltaT;

    portENTER_CRITICAL(&encoderMux);    // Enter critical section to safely read shared encoder state
    pos = pos_i;                        // Read current encoder position
    dir = dir_i;                        // Read current encoder direction
    deltaT = deltaT_i;                  // Read time delta since last encoder event
    portEXIT_CRITICAL(&encoderMux);     // Exit critical section

    float velocity = 0.0f;
    float rpm = 0.0f;
    if (deltaT > 0) {
        velocity = low_pass_filter((dir * 1.0f) / (deltaT / 1000000.0f));        // Calculate velocity in counts per second
        rpm = (velocity * VELOCITY_RPM_CONVERSION);                              // Convert velocity to RPM
    } else {
        velocity = 0.0f;    // If no time has passed, velocity is zero
        rpm = 0.0f;         // If no time has passed, rpm is zero
    }
    
    portENTER_CRITICAL(&encoderMux);    // Enter critical section to safely update current RPM state
    current_rpm = rpm;                  // Update current RPM for PID feedback
    portEXIT_CRITICAL(&encoderMux);     // Exit critical section

    // For testing purposes, print the encoder velocity in counts per second
    //printf("Velocity: %.2f counts/s\n", velocity);
    // For testing purposes, print the encoder rpm
    printf("Current RPM: %.2f", rpm);
}

/*--------------------------------------------------
 * Function:    Low Pass Filter for Velocity
 * Description: Applies a 15Hz LPF to reduce noise in velocity measurements
 * Parameters:  float v_now: The current raw velocity measurement to be filtered
 * Returns:     float: The filtered velocity value after applying the low-pass filter
 *-------------------------------------------------*/
static float low_pass_filter(float v_now)
{
    float v_filt_now = (0.6180*v_filt)
                        +(0.1910*v_prev)
                        +(0.1910*v_now);
    v_prev = v_now;
    v_filt = v_filt_now;
    return v_filt_now;
}

/*--------------------------------------------------
 * Function:    PID Control Computation
 * Description: Computes the PID control output based on the target RPM and current RPM,
 *              updates motor PWM and direction accordingly
 * Parameters:  None (uses global variables for target and current RPM)
 * Returns:     None (updates motor control outputs directly)
 *-------------------------------------------------*/
static void PID_control(void)
{
    float error = target_rpm - fabs(current_rpm);
    float control = kp * error;
    (control > 0) ? (motor_direction = 1) : (motor_direction = -1); // Set direction based on sign of control output
    int pwm = (int)fabs(control); // Use absolute value of control output for PWM duty cycle
    motor_pwm_value = pwm; // Update global variable for motor PWM value
}