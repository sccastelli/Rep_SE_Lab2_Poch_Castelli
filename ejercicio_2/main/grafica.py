import matplotlib
matplotlib.use('MacOSX')
import serial
import matplotlib.pyplot as plt
from collections import deque

ser = serial.Serial('/dev/cu.usbserial-0001', 115200)
ecg_data = deque(maxlen=200)
presion_data = deque(maxlen=200)

plt.ion()
fig, ax = plt.subplots()

while True:
    try:
        line = ser.readline().decode(errors='ignore').strip()
        if ',' in line:
            parts = line.split(',')
            if len(parts) == 2 and parts[0].isdigit() and parts[1].isdigit():
                ecg, presion = map(int, parts)
                ecg_data.append(ecg)
                presion_data.append(presion)

                ax.clear()
                ax.plot(ecg_data, label="ECG")
                ax.plot(presion_data, label="Presión")
                ax.legend()
                plt.pause(0.01)
    except Exception as e:
        print(f"Error al leer datos: {e}")