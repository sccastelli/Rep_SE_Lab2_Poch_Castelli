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
}  // namespace

void setup() {
  model = tflite::GetModel(g_digit_detect_model_data);
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

static tflite::MicroMutableOpResolver<6> micro_op_resolver;

micro_op_resolver.AddConv2D();
micro_op_resolver.AddFullyConnected();
micro_op_resolver.AddReshape();
micro_op_resolver.AddSoftmax();
micro_op_resolver.AddMaxPool2D();  // 👈 ¡Esto es lo que te falta!
micro_op_resolver.AddQuantize();   // Por si acaso también la usas


  static tflite::MicroInterpreter static_interpreter(model, micro_op_resolver, tensor_arena, kTensorArenaSize);
  interpreter = &static_interpreter;

  if (interpreter->AllocateTensors() != kTfLiteOk) {
    MicroPrintf("❌ AllocateTensors() falló");
    return;
  }

  input = interpreter->input(0);

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
  if (GetImage(kNumCols, kNumRows, kNumChannels, input->data.int8) != kTfLiteOk) {
    MicroPrintf("❌ Error al capturar imagen");
    return;
  }

  if (interpreter->Invoke() != kTfLiteOk) {
    MicroPrintf("❌ Error en Invoke()");
    return;
  }

  TfLiteTensor* output = interpreter->output(0);
  float* probabilities = output->data.f;

  int predicted_digit = 0;
  float max_score = probabilities[0];

  for (int i = 1; i < kCategoryCount; ++i) {
    if (probabilities[i] > max_score) {
      max_score = probabilities[i];
      predicted_digit = i;
    }
  }

  // 🧠 Umbral de confianza mínima para considerar la predicción como válida
  constexpr float kDetectionThreshold = 0.6f;

  if (predicted_digit == 10 || max_score < kDetectionThreshold) {
    RespondToDetection(10, max_score);  // usamos 10 como el índice de "no digit"
  } else {
    RespondToDetection(predicted_digit, max_score);
  }

  vTaskDelay(1);  // evita que el watchdog se dispare
}

#endif  // CLI_ONLY_INFERENCE