#ifndef CONFIG_H
#define CONFIG_H

//ENCODER
#define ENCODER_PIN_A 18
#define ENCODER_PIN_B 19

//MOTOR DRIVER
#define MOTOR_PWM_PIN 4         //Speed Control
#define MOTOR_IN1_PIN 26        //Direction Control
#define MOTOR_IN2_PIN 27        //Direction Control
                                //00, 01, 10, 11 -> Stop, Forward, Reverse, Brake

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

//UART
#define UART_TX_PIN 17         //Stream to PC
#define UART_RX_PIN 16         //Receive Commands from PC

//SYSTEM PARAMETERS
#define RPM_DEFAULT 300
#define PID_SAMPLE_RATE_MS 10
#define MOSFET_PWM_FREQ_HZ 1000
#define MOTOR_PWM_FREQ_HZ 5000

#endif /* CONFIG_H */