// Encoder + PID

#ifndef MOTOR_CONTROL_H
#define MOTOR_CONTROL_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/ledc.h"
#include "config.h"

void motor_control_task(void *pvParameters);
void motor_control_init(void); // Initialize motor control pins and peripherals
void set_motor(int pwm_value, int dir); // Set motor PWM and direction
void encoder_read(void); // Read encoder values


#endif /* MOTOR_CONTROL_H */