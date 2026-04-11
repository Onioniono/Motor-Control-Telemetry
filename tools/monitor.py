import serial
import matplotlib.pyplot as plt
from collections import deque
import pandas as pd
import time
from matplotlib.animation import FuncAnimation

# Plot Settings
plt.style.use('dark_background')
plt.rcParams.update({
    'axes.facecolor': '#111111',
    'figure.facecolor': '#111111',
    'axes.edgecolor': 'white',
    'axes.labelcolor': 'white',
    'xtick.color': 'white',
    'ytick.color': 'white',
    'grid.color': '#333333',
    'grid.linestyle': '--',
    'grid.linewidth': 0.5
})

# Create a serial connection to the device
ser = serial.Serial(port = 'COM3', baudrate = 115200, timeout = 0.1)

# Store recent values for plotting
max_points = 50
target_rpm_data = deque(maxlen=max_points)
current_rpm_data = deque(maxlen=max_points)
pwm_data = deque(maxlen=max_points)
direction_data = deque(maxlen=max_points)
sample_index = deque(maxlen=max_points)
error_data = deque(maxlen=max_points)

index = 0
startup_ignore = 5  # Number of initial readings to ignore

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
        current_rpm = abs(float(parts[1]))
        pwm = int(parts[2])
        direction = int(parts[3])

        return target_rpm, current_rpm, pwm, direction
    
    except:
        return None

# Set up the plot
fig, (ax, ax2) = plt.subplots(2, 1, sharex=True)
ax.set_ylim(0,300)                  # Adjust as needed based on expected RPM and PWM ranges
ax.set_yticks(range(0, 301, 25))    # Set y-ticks for better readability
ax2.set_ylim(-10, 10)               # Adjust as needed based on expected error range
ax2.set_yticks(range(-10, 11, 2))   # Set y-ticks for better readability

line1, = ax.plot([], [], label = 'Target RPM', color='#00FFFF',linewidth=1)
line2, = ax.plot([], [], label = 'Current RPM',color='#FF4444',linewidth=1,marker='s',markersize=3)
line3, = ax.plot([], [], label = 'PWM',color='#FFAA00',linewidth=1,marker='^',markersize=3)
line4, = ax2.plot([], [], label = 'Error', color="#FF00B3", linewidth=1, marker='x', markersize=3)

ax.legend()
ax.set_xlabel('Sample')
ax.set_ylabel('RPM / PWM')
ax.set_title('Real-Time Motor Data')

ax2.legend()
ax2.set_xlabel('Sample')
ax2.set_ylabel('Error')
ax2.set_title('RPM Error')

# Animation function to update the plot
def update(frame):
    global index

    data = read_data()
    if data:
        target_rpm, current_rpm, pwm, direction = data

        # Ignore initial readings
        if index < startup_ignore:
            index += 1
            return line1, line2, line3

        # Append new data to the deques
        sample_index.append(index)
        target_rpm_data.append(target_rpm)
        current_rpm_data.append(current_rpm)
        pwm_data.append(pwm)
        direction_data.append(direction)
        error_data.append(target_rpm - current_rpm)

        # Increment the sample index
        index += 1

        # Update the plot data
        line1.set_data(sample_index, target_rpm_data)
        line2.set_data(sample_index, current_rpm_data)
        line3.set_data(sample_index, pwm_data)
        line4.set_data(sample_index, error_data)
        
        # Adjust the plot limits
        ax.relim()
        ax.autoscale_view()

    return line1, line2, line3, line4

# Run the animation
ani = FuncAnimation(fig, update, interval=50)  # Update every 50 ms
plt.show()