import serial
import matplotlib.pyplot as plt
from collections import deque

ser = serial.Serial('/dev/cu.usbserial-0001', 115200)
ecg_data = deque(maxlen=200)
presion_data = deque(maxlen=200)

plt.ion()
fig, ax = plt.subplots()

while True:
    line = ser.readline().decode().strip()
    if "," in line:
        ecg, presion = map(int, line.split(","))
        ecg_data.append(ecg)
        presion_data.append(presion)

        ax.clear()
        ax.plot(ecg_data, label="ECG")
        ax.plot(presion_data, label="Presión")
        ax.legend()
        plt.pause(0.01)