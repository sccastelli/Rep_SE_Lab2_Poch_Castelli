#include "main_functions.h"

#include "detection_responder.h"
#include "image_provider.h"
#include "model_settings.h"
#include "digit_detect_model_data.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/micro/micro_log.h"
#include "tensorflow/lite/micro/micro_mutable_op_resolver.h"
#include "tensorflow/lite/schema/schema_generated.h"

#include <esp_heap_caps.h>
#include <esp_timer.h>
#include "esp_main.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

namespace {
const tflite::Model* model = nullptr;
tflite::MicroInterpreter* interpreter = nullptr;
TfLiteTensor* input = nullptr;

#if CONFIG_NN_OPTIMIZED
constexpr int scratchBufSize = 60 * 1024;
#else
constexpr int scratchBufSize = 0;
#endif

constexpr int kTensorArenaSize = 100 * 1024 + scratchBufSize;
static uint8_t* tensor_arena = nullptr;

constexpr float kDetectionThreshold = 0.6f;
constexpr int kNoDigitClassIndex = 10;
}  // namespace

void setup() {
  model = tflite::GetModel(g_digit_detect_model_data);
  if (model == nullptr) {
    MicroPrintf("❌ Error: el modelo no se cargó correctamente");
    return;
  }
  if (model->version() != TFLITE_SCHEMA_VERSION) {
    MicroPrintf("❌ Modelo incompatible (versión %d, esperado %d)",
                model->version(), TFLITE_SCHEMA_VERSION);
    return;
  }

  tensor_arena = (uint8_t*) heap_caps_malloc(kTensorArenaSize, MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
  if (!tensor_arena) {
    MicroPrintf("❌ No se pudo asignar %d bytes para tensor_arena", kTensorArenaSize);
    return;
  }

  static tflite::MicroMutableOpResolver<8> micro_op_resolver;
  micro_op_resolver.AddConv2D();
  micro_op_resolver.AddFullyConnected();
  micro_op_resolver.AddReshape();
  micro_op_resolver.AddSoftmax();
  micro_op_resolver.AddMaxPool2D();
  micro_op_resolver.AddQuantize();
  micro_op_resolver.AddCast();
  micro_op_resolver.AddMean();

  static tflite::MicroInterpreter static_interpreter(model, micro_op_resolver, tensor_arena, kTensorArenaSize);
  interpreter = &static_interpreter;

  if (interpreter->AllocateTensors() != kTfLiteOk) {
    MicroPrintf("❌ AllocateTensors() falló");
    return;
  }
  MicroPrintf("✅ AllocateTensors() exitoso");

  input = interpreter->input(0);
  MicroPrintf("✅ Input tensor type: %d", input->type);
  MicroPrintf("✅ Input scale: %f", input->params.scale);
  MicroPrintf("✅ Input zero_point: %d", input->params.zero_point);
  MicroPrintf("Input tensor dims: %d x %d x %d", input->dims->data[1], input->dims->data[2], input->dims->data[3]);
  MicroPrintf("Input tensor total bytes: %d", input->bytes);


#ifndef CLI_ONLY_INFERENCE
  if (InitCamera() != kTfLiteOk) {
    MicroPrintf("❌ InitCamera falló");
    return;
  }
#endif
}

#ifndef CLI_ONLY_INFERENCE
void loop() {
  MicroPrintf("📷 Capturando imagen...");
  if (input == nullptr) {
    MicroPrintf("❌ input == nullptr, saliendo del loop.");
    return;
  }

  if (input->bytes != 96 * 96) {
    MicroPrintf("❌ Tamaño del input incorrecto: %d bytes", input->bytes);
    return;
  }

  for (int i = 0; i < 10; ++i) {
    MicroPrintf("input[%d] = %d", i, input->data.int8[i]);
  }



  if (GetImage(kNumCols, kNumRows, kNumChannels, input->data.int8) != kTfLiteOk) {
    MicroPrintf("❌ Error al capturar imagen");
    return;
  }

  // Inferencia
  int64_t start_us = esp_timer_get_time();
  if (interpreter->Invoke() != kTfLiteOk) {
    MicroPrintf("❌ Error en Invoke()");
    return;
  }
  int64_t elapsed_us = esp_timer_get_time() - start_us;
  MicroPrintf("⏱ Tiempo de inferencia: %lld us", elapsed_us);

  TfLiteTensor* output = interpreter->output(0);
  MicroPrintf("✅ Output tensor type: %d", output->type);
  MicroPrintf("✅ Output scale: %f", output->params.scale);
  int8_t* raw_output = output->data.int8;
  float scale = output->params.scale;
  int zero_point = output->params.zero_point;

  MicroPrintf("📊 Scores por clase:");
  int predicted_digit = 0;
  float max_score = (raw_output[0] - zero_point) * scale;

  for (int i = 0; i < kCategoryCount; ++i) {
    float score = (raw_output[i] - zero_point) * scale;
    MicroPrintf(" - Clase %d (%s): %.2f%%", i, kCategoryLabels[i], score * 100.0f);

    if (score > max_score) {
      max_score = score;
      predicted_digit = i;
    }
  }

  // Mostrar resultado
  if (max_score < 0.5f) {
    MicroPrintf("🔢 Resultado: no digit (%.2f%%)", max_score * 100.0f);
  } else {
    MicroPrintf("🔢 Resultado: %s (%.2f%%)", kCategoryLabels[predicted_digit], max_score * 100.0f);
  }

  // Solo llama a RespondToDetection si la clase es válida (0-9)
  RespondToDetection(predicted_digit, max_score);

  vTaskDelay(1);
}

#endif  // CLI_ONLY_INFERENCE
