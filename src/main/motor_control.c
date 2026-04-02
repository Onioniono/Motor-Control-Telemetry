#include "motor_control.h"

int motor_pwm_value = 255; // Test variable to hold current PWM value for motor control
int motor_direction = 1; // 1 for forward, -1 for reverse (for testing purposes)

void motor_control_task(void *pvParameters)
{
    // Placeholder for motor control logic
    while (1) {
        // Read encoder values, compute PID, set motor PWM
        motor_control_init(); // Initialize motor control peripherals
        encoder_read(); // Read encoder values
        set_motor(motor_pwm_value, motor_direction); // Set motor with current PWM value and direction (for testing)
        vTaskDelay(pdMS_TO_TICKS(PID_SAMPLE_RATE_MS)); // Sample rate delay
    }
}

void motor_control_init(void)
{
    //Motor Driver Pins
    gpio_reset_pin(MOTOR_PWM_PIN);
    gpio_set_direction(MOTOR_IN1_PIN, GPIO_MODE_OUTPUT);
    gpio_set_direction(MOTOR_IN2_PIN, GPIO_MODE_OUTPUT);
    //Magnetic Encoder Pins
    gpio_set_direction(ENCODER_PIN_A, GPIO_MODE_INPUT);
    gpio_set_pull_mode(ENCODER_PIN_A, GPIO_PULLUP_ONLY); // Enable pull-up for encoder input
    gpio_set_direction(ENCODER_PIN_B, GPIO_MODE_INPUT);
    gpio_set_pull_mode(ENCODER_PIN_B, GPIO_PULLUP_ONLY); // Enable pull-up for encoder input
    //PWM Setup
    ledc_timer_config_t ledc_timer = {
        .speed_mode = LEDC_LOW_SPEED_MODE,      //Use low speed mode for motor control
        .timer_num = LEDC_TIMER_0,              //Use timer 0 for motor PWM
        .duty_resolution = LEDC_TIMER_8_BIT,    //8-bit resolution duty cycle (0-255)
        .freq_hz = MOTOR_PWM_FREQ_HZ,           // 5 kHz PWM frequency
        .clk_cfg = LEDC_AUTO_CLK                // Auto select source clock
    };
    ledc_timer_config(&ledc_timer);             // Configure timer for motor PWM

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
}

void set_motor(int pwm, int dir)
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

void encoder_read(void)
{
    // Placeholder for reading encoder values
}