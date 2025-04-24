#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "driver/gpio.h"

#define LED_GPIO GPIO_NUM_4    
#define BUTTON_GPIO GPIO_NUM_0    

SemaphoreHandle_t xMutex;
volatile int counter = 1;
volatile int hello_count = 0;

void hello_task(void *pvParameter) {
    while (1) {
        printf("Hello World\n");
        printf("hello_count = %d\n", hello_count);

        xSemaphoreTake(xMutex, portMAX_DELAY);
        hello_count++;
        if (hello_count == 10) {
            counter = 1;
            hello_count = 0;
        }
        xSemaphoreGive(xMutex);

        vTaskDelay(pdMS_TO_TICKS(2000)); // 2 segundos
    }
}

void blink_led_task(void *pvParameter) {
    while (1) {
        xSemaphoreTake(xMutex, portMAX_DELAY);
        int local_counter = counter;
        xSemaphoreGive(xMutex);

        gpio_set_level(LED_GPIO, 1);
        vTaskDelay(pdMS_TO_TICKS(500 / local_counter));
        gpio_set_level(LED_GPIO, 0);
        vTaskDelay(pdMS_TO_TICKS(500 / local_counter));
    }
}

void IRAM_ATTR button_isr_handler(void* arg) {
    xSemaphoreTakeFromISR(xMutex, NULL);
    counter++;
    xSemaphoreGiveFromISR(xMutex, NULL);
}

void button_task(void *pvParameter) {
    gpio_set_direction(BUTTON_GPIO, GPIO_MODE_INPUT);
    gpio_set_intr_type(BUTTON_GPIO, GPIO_INTR_POSEDGE);
    gpio_install_isr_service(0);
    gpio_isr_handler_add(BUTTON_GPIO, button_isr_handler, NULL);

    while (1) {
        vTaskDelay(portMAX_DELAY); // Nunca termina
    }
}

void app_main() {
    xMutex = xSemaphoreCreateMutex();

    gpio_reset_pin(LED_GPIO);
    gpio_set_direction(LED_GPIO, GPIO_MODE_OUTPUT);

    xTaskCreate(hello_task, "hello_task", 2048, NULL, 1, NULL);
    xTaskCreate(blink_led_task, "blink_led_task", 2048, NULL, 1, NULL);
    xTaskCreate(button_task, "button_task", 2048, NULL, 1, NULL);
}