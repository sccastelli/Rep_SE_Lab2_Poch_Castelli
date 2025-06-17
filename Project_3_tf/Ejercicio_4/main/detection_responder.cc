#include "detection_responder.h"
#include "tensorflow/lite/micro/micro_log.h"
#include "model_settings.h"  
#include "esp_main.h"

void RespondToDetection(int digit, float score) {
  if (digit >= 0 && digit < kCategoryCount) {
    MicroPrintf("🔢 Resultado: %s (%.2f%%)", kCategoryLabels[digit], score * 100.0f);
  } else {
    MicroPrintf("❌ Clase inválida detectada (%.2f%%)", score * 100.0f);
  }
}
