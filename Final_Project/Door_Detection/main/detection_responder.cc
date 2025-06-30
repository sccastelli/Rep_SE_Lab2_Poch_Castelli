#include "detection_responder.h"
#include "tensorflow/lite/micro/micro_log.h"

#include "driver/gpio.h"
#include "esp_main.h"

#define LED_GPIO GPIO_NUM_4  // Puedes cambiarlo según el pin que uses

void RespondToDetection(float door_open_score, float door_closed_score) {
  int open_score_percent = door_open_score * 100 + 0.5;
  (void) door_closed_score; // No se usa directamente

  static bool led_initialized = false;
  if (!led_initialized) {
      gpio_set_direction(LED_GPIO, GPIO_MODE_OUTPUT);
      led_initialized = true;
  }

  if (open_score_percent < 60) {
      gpio_set_level(LED_GPIO, 0); // Puerta cerrada → LED apagado
  } else {
      gpio_set_level(LED_GPIO, 1); // Puerta abierta → LED encendido
  }

  MicroPrintf("door open score: %d%%, door closed score: %d%%",
              open_score_percent, 100 - open_score_percent);
}
