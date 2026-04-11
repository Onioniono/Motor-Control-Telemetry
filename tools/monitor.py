import serial
import matplotlib.pyplot as plt
from collections import deque
import pandas as pd
import time
from matplotlib.animation import FuncAnimation

# Plot Settings
plt.style.use('fivethirtyeight')

# Create a serial connection to the device
ser = serial.Serial(port = 'COM3', baudrate = 115200, timeout = 1)

# Store recent values for plotting
max_points = 100
target_rpm_data = deque(maxlen=max_points)
current_rpm_data = deque(maxlen=max_points)
pwm_data = deque(maxlen=max_points)
direction_data = deque(maxlen=max_points)
sample_index = deque(maxlen=max_points)

index = 0

# Function to read and parse data from the serial port
def read_data():
    try:
        line = ser.readline().decode('utf-8').strip()
        
        if not line:
            return None
        if line.startswith("DATA:"):
            line = line[len("DATA:"):]

        parts = [p.strip() for p in line.split(',')]
        if len(parts) != 4:
            return None
        
        target_rpm = float(parts[0])
        current_rpm = float(parts[1])
        pwm = int(parts[2])
        direction = int(parts[3])

        return target_rpm, current_rpm, pwm, direction
    
    except:
        return None

# Set up the plot
fig, ax = plt.subplots()
line1, = ax.plot([], [], label = 'Target RPM')
line2, = ax.plot([], [], label = 'Current RPM')
line3, = ax.plot([], [], label = 'PWM')

ax.legend()
ax.set_xlabel('Sample')
ax.set_ylabel('Value')
ax.set_title('Real-Time Motor Data')

# Animation function to update the plot
def update(frame):
    global index

    data = read_data()
    if data:
        target_rpm, current_rpm, pwm, direction = data

        sample_index.append(index)
        target_rpm_data.append(target_rpm)
        current_rpm_data.append(current_rpm)
        pwm_data.append(pwm)
        direction_data.append(direction)

        index += 1

        line1.set_data(sample_index, target_rpm_data)
        line2.set_data(sample_index, current_rpm_data)
        line3.set_data(sample_index, pwm_data)

        ax.relim()
        ax.autoscale_view()

    return line1, line2, line3

# Run the animation
ani = FuncAnimation(fig, update, interval=100)
plt.show()