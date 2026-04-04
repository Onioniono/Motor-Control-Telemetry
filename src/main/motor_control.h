// Encoder + PID

#ifndef MOTOR_CONTROL_H
#define MOTOR_CONTROL_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/ledc.h"
#include "esp_timer.h"
#include <math.h>
#include "config.h"

/* Test Variables for Motor Control */
extern int motor_pwm_value; // Test variable to hold current PWM value for motor control
extern int motor_direction; // 1 for forward, -1 for reverse (for testing purposes)

/* Motor Control Functions */
void motor_control_init(void);                  // Initialize motor control pins and peripherals
void motor_control_start(void);                 // Start the motor control task in FreeRTOS
void motor_set(int pwm_value, int dir);         // Set motor PWM and direction

#endif /* MOTOR_CONTROL_H */