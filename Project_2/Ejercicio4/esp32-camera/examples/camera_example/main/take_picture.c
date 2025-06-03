#include <esp_log.h>
#include <esp_system.h>
#include <nvs_flash.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#ifndef portTICK_RATE_MS
#define portTICK_RATE_MS portTICK_PERIOD_MS
#endif

#include "esp_camera.h"

#define BOARD_ESP32CAM_AITHINKER

// PIN Map para ESP32CAM AiThinker
#ifdef BOARD_ESP32CAM_AITHINKER
#define CAM_PIN_PWDN 32
#define CAM_PIN_RESET -1
#define CAM_PIN_XCLK 0
#define CAM_PIN_SIOD 26
#define CAM_PIN_SIOC 27

#define CAM_PIN_D7 35
#define CAM_PIN_D6 34
#define CAM_PIN_D5 39
#define CAM_PIN_D4 36
#define CAM_PIN_D3 21
#define CAM_PIN_D2 19
#define CAM_PIN_D1 18
#define CAM_PIN_D0 5
#define CAM_PIN_VSYNC 25
#define CAM_PIN_HREF 23
#define CAM_PIN_PCLK 22
#endif

static const char *TAG = "example:take_picture";

static camera_config_t camera_config = {
    .pin_pwdn = CAM_PIN_PWDN,
    .pin_reset = CAM_PIN_RESET,
    .pin_xclk = CAM_PIN_XCLK,
    .pin_sccb_sda = CAM_PIN_SIOD,
    .pin_sccb_scl = CAM_PIN_SIOC,
    .pin_d7 = CAM_PIN_D7,
    .pin_d6 = CAM_PIN_D6,
    .pin_d5 = CAM_PIN_D5,
    .pin_d4 = CAM_PIN_D4,
    .pin_d3 = CAM_PIN_D3,
    .pin_d2 = CAM_PIN_D2,
    .pin_d1 = CAM_PIN_D1,
    .pin_d0 = CAM_PIN_D0,
    .pin_vsync = CAM_PIN_VSYNC,
    .pin_href = CAM_PIN_HREF,
    .pin_pclk = CAM_PIN_PCLK,
    .xclk_freq_hz = 20000000,
    .ledc_timer = LEDC_TIMER_0,
    .ledc_channel = LEDC_CHANNEL_0,
    .pixel_format = PIXFORMAT_GRAYSCALE,  // Escala de grises
    .frame_size = FRAMESIZE_96X96,        // Tamaño pequeño para ejemplo
    .fb_location = CAMERA_FB_IN_DRAM,
    .jpeg_quality = 12,
    .fb_count = 1,
    .grab_mode = CAMERA_GRAB_WHEN_EMPTY,
};

static esp_err_t init_camera(void)
{
    esp_err_t err = esp_camera_init(&camera_config);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Camera Init Failed");
        return err;
    }
    return ESP_OK;
}

// Función para ecualización de histograma en escala de grises
void histogram_equalization(uint8_t *buf, size_t len)
{
    uint32_t hist[256] = {0};
    for (size_t i = 0; i < len; i++)
    {
        hist[buf[i]]++;
    }

    uint32_t cdf[256] = {0};
    cdf[0] = hist[0];
    for (int i = 1; i < 256; i++)
    {
        cdf[i] = cdf[i - 1] + hist[i];
    }

    uint32_t cdf_min = 0;
    for (int i = 0; i < 256; i++)
    {
        if (cdf[i] != 0)
        {
            cdf_min = cdf[i];
            break;
        }
    }

    uint32_t total_pixels = len;

    uint8_t lut[256];
    for (int i = 0; i < 256; i++)
    {
        lut[i] = (uint8_t)(((float)(cdf[i] - cdf_min) / (total_pixels - cdf_min)) * 255);
    }

    for (size_t i = 0; i < len; i++)
    {
        buf[i] = lut[buf[i]];
    }
}

void app_main(void)
{
    if (init_camera() != ESP_OK)
    {
        return;
    }

    while (1)
    {
        ESP_LOGI(TAG, "Taking picture...");
        camera_fb_t *pic = esp_camera_fb_get();

        if (!pic)
        {
            ESP_LOGE(TAG, "Failed to capture image");
            vTaskDelay(5000 / portTICK_RATE_MS);
            continue;
        }

        ESP_LOGI(TAG, "Picture taken! Size: %zu bytes", pic->len);

        // Imprimir imagen original en hexadecimal
        printf("Imagen original (sin ecualizar):\n");
        for (int i = 0; i < pic->len; i++)
        {
            printf("0x%02X, ", pic->buf[i]);
            if ((i + 1) % 16 == 0)
                printf("\n");
        }
        printf("\n");

        // Crear copia para ecualizar
        uint8_t *img_eq = malloc(pic->len);
        if (!img_eq)
        {
            ESP_LOGE(TAG, "Error en malloc");
            esp_camera_fb_return(pic);
            vTaskDelay(5000 / portTICK_RATE_MS);
            continue;
        }
        memcpy(img_eq, pic->buf, pic->len);

        // Ecualizar la imagen copia
        histogram_equalization(img_eq, pic->len);

        // Imprimir imagen ecualizada en hexadecimal
        printf("Imagen ecualizada:\n");
        for (int i = 0; i < pic->len; i++)
        {
            printf("0x%02X, ", img_eq[i]);
            if ((i + 1) % 16 == 0)
                printf("\n");
        }
        printf("\n");

        free(img_eq);
        esp_camera_fb_return(pic);

        vTaskDelay(5000 / portTICK_RATE_MS);
    }
}
