# Ejercicio 2 - Sensores y Actuadores con ESP32

Este proyecto implementa un sistema de monitoreo en tiempo real de RCP utilizando dos sensores conectados a un ESP32:
- Sensor de **ECG**
- Sensor de **presión torácica**

Los datos son enviados vía serial a un computador, donde se grafican en tiempo real mediante Python. Además, el sistema detecta la frecuencia de compresiones y enciende un LED si están fuera del rango correcto (100–120 cpm).

---

## Configuración de Hardware

| Dispositivo     | Pin ESP32         | Canal ADC         |
|-----------------|-------------------|-------------------|
| Sensor ECG      | GPIO32            | `ADC1_CHANNEL_6`  |
| Sensor Presión  | GPIO35            | `ADC1_CHANNEL_4`  |
| LED indicador   | GPIO14            | GPIO output       |

---

## Instrucciones de Uso

### 1. Compilar y cargar el firmware en el ESP32

```bash
. $HOME/esp/esp-idf/export.sh
cd ejercicio_2
idf.py build
idf.py flash
idf.py monitor
```

### 2. Visualizar datos en tiempo real (Python)

```bash
cd ejercicio_2
python3 -m venv grafica_env
source grafica_env/bin/activate
pip install matplotlib pyserial
python main/grafica.py
```