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

/* Variables for Motor Control */
extern int motor_pwm_value; // Test variable to hold current PWM value for motor control
extern int motor_direction; // 1 for forward, -1 for reverse (for testing purposes)
extern float target_rpm;    // Target RPM for the motor, set by control loop or external commands
extern float current_rpm;   // Current RPM calculated from encoder readings, used for PID feedback
extern float kp;            // Proportional gain for PID controller
extern float ki;            // Integral gain for PID controller
extern float kd;            // Derivative gain for PID controller

extern SemaphoreHandle_t motor_data_mutex; // Mutex for protecting access to motor control variables between tasks

/* Motor Control Functions */
void motor_control_init(void);                  // Initialize motor control pins and peripherals
void motor_control_start(void);                 // Start the motor control task in FreeRTOS
void motor_set(int pwm_value, int dir);         // Set motor PWM and direction

#endif /* MOTOR_CONTROL_H */