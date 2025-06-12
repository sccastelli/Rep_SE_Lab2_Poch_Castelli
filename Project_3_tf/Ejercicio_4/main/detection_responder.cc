#include "detection_responder.h"
#include "tensorflow/lite/micro/micro_log.h"

#include "esp_main.h"

#if DISPLAY_SUPPORT
#include "image_provider.h"
#include "bsp/esp-bsp.h"

#define IMG_WD (96 * 2)
#define IMG_HT (96 * 2)

static lv_obj_t *camera_canvas = NULL;
static lv_obj_t *digit_label = NULL;

static void create_gui(void) {
  bsp_display_start();
  bsp_display_backlight_on();
  bsp_display_lock(0);

  camera_canvas = lv_canvas_create(lv_scr_act());
  assert(camera_canvas);
  lv_obj_align(camera_canvas, LV_ALIGN_TOP_MID, 0, 0);

  digit_label = lv_label_create(lv_scr_act());
  assert(digit_label);
  lv_label_set_text(digit_label, "Digit: ?");
  lv_obj_align(digit_label, LV_ALIGN_BOTTOM_MID, 0, 0);

  bsp_display_unlock();
}
#endif

void RespondToDetection(int digit, float score) {
#if DISPLAY_SUPPORT
  if (!camera_canvas) {
    create_gui();
  }

  uint16_t *buf = (uint16_t *) image_provider_get_display_buf();
  bsp_display_lock(0);
  lv_canvas_set_buffer(camera_canvas, buf, IMG_WD, IMG_HT, LV_IMG_CF_TRUE_COLOR);

  char text[32];
  snprintf(text, sizeof(text), "Digit: %d (%.2f%%)", digit, score * 100.0f);
  lv_label_set_text(digit_label, text);
  bsp_display_unlock();
#endif

  MicroPrintf("🔢 Dígito detectado: %d (%.2f%%)", digit, score * 100.0f);
}
