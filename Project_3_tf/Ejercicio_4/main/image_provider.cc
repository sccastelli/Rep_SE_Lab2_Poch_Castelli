
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
#include <cstdlib>
#include <cstring>
#include <iostream>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_spi_flash.h"
#include "esp_system.h"
#include "esp_timer.h"

#include "app_camera_esp.h"
#include "esp_camera.h"
#include "model_settings.h"
#include "image_provider.h"
#include "esp_main.h"

static const char* TAG = "app_camera";

// Get the camera module ready
TfLiteStatus InitCamera() {
#if CLI_ONLY_INFERENCE
  ESP_LOGI(TAG, "CLI_ONLY_INFERENCE enabled, skipping camera init");
  MicroPrintf("Camera init skipped for CLI_ONLY_INFERENCE mode");
  return kTfLiteOk;
#endif

#if ESP_CAMERA_SUPPORTED
  int ret = app_camera_init();
  if (ret != 0) {
    MicroPrintf("Camera init failed\n");
    return kTfLiteError;
  }
  MicroPrintf("Camera Initialized\n");
#else
  ESP_LOGE(TAG, "Camera not supported for this device");
#endif
  return kTfLiteOk;
}


// Get an image from the camera module
TfLiteStatus GetImage(int image_width, int image_height, int channels, int8_t* image_data) {
#if ESP_CAMERA_SUPPORTED
  // 1. Capturar la imagen desde la cámara
  camera_fb_t* fb = esp_camera_fb_get();
  if (!fb) {
    ESP_LOGE(TAG, "Camera capture failed");
    return kTfLiteError;
  }

  MicroPrintf("Image Captured\n");

  // 2. Procesar la imagen capturada:
  //    - Inversión vertical (para alinear con orientación esperada)
  //    - Inversión de colores (negativo)
  //    - Ajuste de offset (-128) para cuantización simétrica int8
  for (int y = 0; y < image_height; y++) {
    for (int x = 0; x < image_width; x++) {
      int dst_idx = y * image_width + x;
      int src_idx = (image_height - 1 - y) * image_width + x;

      // Tomar pixel original (uint8 de 0 a 255)
      uint8_t pixel = ((uint8_t*) fb->buf)[src_idx];

      // Invertir colores (negativo)
      pixel = 255 - pixel;

      // Aplicar offset para quedar en rango [-128, 127]
      int16_t shifted = static_cast<int16_t>(pixel) - 128;

      // Asegurar que esté dentro del rango válido
      shifted = std::max((int16_t)-127, std::min((int16_t)127, shifted));

      // Guardar en el arreglo de entrada del modelo
      image_data[dst_idx] = static_cast<int8_t>(shifted);
    }
  }

  // 3. Liberar el framebuffer
  esp_camera_fb_return(fb);

  return kTfLiteOk;
#else
  return kTfLiteError;
#endif
}


