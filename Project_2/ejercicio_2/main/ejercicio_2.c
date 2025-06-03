#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/adc.h"
#include "esp_adc_cal.h"

#define ECG_CHANNEL ADC1_CHANNEL_6      // GPIO32
#define PRESION_CHANNEL ADC1_CHANNEL_4  // GPIO35
#define LED_GPIO GPIO_NUM_14            // LED indicador

esp_adc_cal_characteristics_t *adc_chars;

void read_sensors_task(void *pvParameter) {
    int last_peak_time = 0;

    while (1) {
        int ecg_raw = adc1_get_raw(ECG_CHANNEL);
        int presion_raw = adc1_get_raw(PRESION_CHANNEL);

        // Imprimir datos en formato limpio
        printf("%d,%d\n", ecg_raw, presion_raw);

        // Detección de compresiones basada en ECG
        if (ecg_raw > 2000) {  // Ajusta este umbral según tu señal
            int now = xTaskGetTickCount() * portTICK_PERIOD_MS;
            int interval = now - last_peak_time;

            if (last_peak_time != 0 && interval > 300) {
                float freq = 60000.0f / interval;

                if (freq < 100 || freq > 120) {
                    gpio_set_level(LED_GPIO, 1);
                } else {
                    gpio_set_level(LED_GPIO, 0);
                }
            }
            last_peak_time = now;
        }

        vTaskDelay(pdMS_TO_TICKS(100));  // Frecuencia de muestreo ~10Hz
    }
}

void app_main() {
    // Configuración del ADC
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(ECG_CHANNEL, ADC_ATTEN_DB_11);
    adc1_config_channel_atten(PRESION_CHANNEL, ADC_ATTEN_DB_11);

    // Configuración del LED
    gpio_reset_pin(LED_GPIO);
    gpio_set_direction(LED_GPIO, GPIO_MODE_OUTPUT);

    // Crear tarea
    xTaskCreate(read_sensors_task, "read_sensors_task", 4096, NULL, 1, NULL);
}