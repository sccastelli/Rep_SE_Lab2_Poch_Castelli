#ifndef TENSORFLOW_LITE_MICRO_EXAMPLES_DIGIT_DETECTION_MODEL_SETTINGS_H_
#define TENSORFLOW_LITE_MICRO_EXAMPLES_DIGIT_DETECTION_MODEL_SETTINGS_H_

// Configuración para modelo MNIST
constexpr int kNumCols = 28;
constexpr int kNumRows = 28;
constexpr int kNumChannels = 1;

constexpr int kMaxImageSize = kNumCols * kNumRows * kNumChannels;

// MNIST tiene 10 clases: dígitos del 0 al 9
constexpr int kCategoryCount = 11;

extern const char* kCategoryLabels[kCategoryCount];

#endif  // TENSORFLOW_LITE_MICRO_EXAMPLES_DIGIT_DETECTION_MODEL_SETTINGS_H_
