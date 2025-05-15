#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/adc.h"
#include "esp_adc_cal.h"

#define ECG_CHANNEL ADC1_CHANNEL_6  // GPIO32 --> G27
#define PRESION_CHANNEL ADC1_CHANNEL_4  // GPIO35
#define LED_GPIO GPIO_NUM_14

esp_adc_cal_characteristics_t *adc_chars;

void read_sensors_task(void *pvParameter) {
    int last_peak_time = 0;
    int peaks_detected = 0;

    while (1) {
        int ecg_raw = adc1_get_raw(ECG_CHANNEL);
        int presion_raw = adc1_get_raw(PRESION_CHANNEL);

        // Enviar por serial con etiquetas
        printf("ECG: %d, Presion: %d\n", ecg_raw, presion_raw);

        // Detección de compresiones RCP
        
            int now = xTaskGetTickCount() * portTICK_PERIOD_MS;
            int interval = now - last_peak_time;
            if (interval > 300) {
                float freq = 60000.0 / interval;

                printf("⚠️  Frecuencia de compresiones: %.1f cpm\n", freq);

                if (freq < 100 || freq > 120) {
                    gpio_set_level(LED_GPIO, 1);
                    printf("🚨 Frecuencia fuera de rango. LED ENCENDIDO\n");
                } else {
                    gpio_set_level(LED_GPIO, 0);
                    printf("✅ Frecuencia correcta. LED APAGADO\n");
                }

                last_peak_time = now;
                peaks_detected++;
            }
        

        vTaskDelay(pdMS_TO_TICKS(5000)); // 100 Hz
    }
}

void app_main() {
    // Configurar ADC
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(ECG_CHANNEL, ADC_ATTEN_DB_11);
    adc1_config_channel_atten(PRESION_CHANNEL, ADC_ATTEN_DB_11);

    // LED
    gpio_reset_pin(LED_GPIO);
    gpio_set_direction(LED_GPIO, GPIO_MODE_OUTPUT);

    xTaskCreate(read_sensors_task, "read_sensors_task", 4096, NULL, 1, NULL);
}