#ifndef CONFIG_H
#define CONFIG_H

/* Pin Definitions *////////////////////////////////////////////////////////////////////////
//ENCODER
#define ENCODER_PIN_A 18
#define ENCODER_PIN_B 19

//MOTOR DRIVER
#define MOTOR_PWM_PIN 4         //Speed Control
#define MOTOR_IN1_PIN 26        //Direction Control
#define MOTOR_IN2_PIN 27        //Direction Control
                                //00, 01, 10, 11 -> Stop, Forward, Reverse, Brake
/* Unused Pins
//MOSFET
#define MOSFET_GATE_PIN GPIO_NUM_32      //Adversial Load PWM

//INA219 I2C ADDRESSES
#define INA219_MOTOR_ADDR 0x40  //Motor Side
#define INA219_LOAD_ADDR 0x41   //Load Side

//I2C BUS (SHARED BY INA219 AND DISPLAY)
#define I2C_SDA_PIN 21
#define I2C_SCL_PIN 22

//DISPLAY (SHARED I2C PINS)
#define DISPLAY_SDA_PIN I2C_SDA_PIN
#define DISPLAY_SCL_PIN I2C_SCL_PIN
*/

//UART - Declared but Unused
#define UART_TX_PIN 17         //Stream to PC
#define UART_RX_PIN 16         //Receive Commands from PC

/* SYSTEM PARAMETERS *////////////////////////////////////////////////////////////////////////
/* Motor Control */
#define RPM_DEFAULT 225         //75% of max RPM for testing
#define RPM_MAX 300             //Max RPM of the motor (from datasheet)
#define PID_SAMPLE_RATE_MS 10
#define MOTOR_PWM_FREQ_HZ 5000
#define PWM_RPM_CONVERSION 1.176470588f
/* Encoder Control */
#define ENCODER_PPR 240                 //Pulses Per Revolution for the encoder
#define VELOCITY_RPM_CONVERSION 0.25f   // RPM (60 seconds / PPR) = 0.25 for 240 PPR encoder)
/* Telemetry */
#define TELEMETRY_SAMPLE_RATE_MS 100
#define UART UART_NUM_0  

#endif /* CONFIG_H */