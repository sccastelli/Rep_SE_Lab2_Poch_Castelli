/* Copyright 2019 The TensorFlow Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/

/*
 * SPDX-FileCopyrightText: 2019-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "detection_responder.h"
#include "tensorflow/lite/micro/micro_log.h"

#include "driver/gpio.h"
#include "esp_main.h"


#define LED_GPIO GPIO_NUM_4  // GPIO pin for the LED indicator


// Initialize the LED GPIO pin
void InitLed() {
  gpio_set_direction(LED_GPIO, GPIO_MODE_OUTPUT); 
}


void RespondToDetection(float person_score, float no_person_score) {
  int person_score_int = (person_score) * 100 + 0.5;
  (void) no_person_score; // unused

  static bool led_initialized = false;
  if (!led_initialized) {
      gpio_set_direction(LED_GPIO, GPIO_MODE_OUTPUT);
      led_initialized = true;
  }

  if (person_score_int < 60) {
      gpio_set_level(LED_GPIO, 0); // Apagar LED
  } else {
      gpio_set_level(LED_GPIO, 1); // Encender LED
  }

  MicroPrintf("person score:%d%%, no person score %d%%",
              person_score_int, 100 - person_score_int);
}
