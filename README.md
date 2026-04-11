# PID DC Motor Speed Controller with Real-Time Telemetry

## Overview

This project implements a real-time closed-loop DC motor speed control system using an ESP32 running FreeRTOS. 
The system uses encoder feedback and PWM actuation to regulate motor speed with high accuracy, while streaming live telemetry data to a Python-based visualization tool.

## Features

* Real-time PID speed control on ESP32
* ±1.5% steady-state accuracy using encoder feedback
* UART-based telemetry streaming to PC
* Live visualization of:

  * Target RPM
  * Current RPM
  * PWM output
  * Control error
* Python dashboard with real-time plotting using matplotlib

## Demo

![Telemetry Plot](docs/images/real_time_telemetry_plot.png)

## Description

The control system runs on the ESP32 using FreeRTOS tasks for deterministic timing. A PID controller regulates motor speed by adjusting PWM output based on encoder feedback.

Telemetry data is transmitted over UART to a Python application, where it is parsed and plotted in real time. This enables live monitoring of system performance, including tracking error and response behavior.

## Technologies Used

* ESP32 (C / ESP-IDF / FreeRTOS)
* Python (pyserial, matplotlib)
* UART communication
* PID control systems
