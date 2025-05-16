#include <esp_log.h>
#include <esp_system.h>
#include <nvs_flash.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>
#include "esp_timer.h"

#include "esp_private/esp_clk.h"
#include "esp_pm.h"

#include "esp_spiffs.h"
#include "esp_vfs.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_camera.h"

#define MAX_IMAGES 20
#define WIDTH 96
#define HEIGHT 96

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

static const char *TAG = "FINAL";

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
    .pixel_format = PIXFORMAT_GRAYSCALE,
    .frame_size = FRAMESIZE_96X96,
    .jpeg_quality = 12,
    .fb_count = 1,
    .fb_location = CAMERA_FB_IN_DRAM,
    .grab_mode = CAMERA_GRAB_WHEN_EMPTY,
};

uint8_t *image_buffer[MAX_IMAGES] = {0};
int image_index = 0;
int image_count = 0;

esp_err_t init_camera(void) {
    return esp_camera_init(&camera_config);
}

void histogram_equalization(uint8_t *buf, size_t len) {
    uint32_t hist[256] = {0};
    for (size_t i = 0; i < len; i++) hist[buf[i]]++;

    uint32_t cdf[256] = {0};
    cdf[0] = hist[0];
    for (int i = 1; i < 256; i++) cdf[i] = cdf[i - 1] + hist[i];

    uint32_t cdf_min = 0;
    for (int i = 0; i < 256; i++) {
        if (cdf[i] != 0) {
            cdf_min = cdf[i];
            break;
        }
    }

    uint8_t lut[256];
    for (int i = 0; i < 256; i++) {
        lut[i] = (uint8_t)(((float)(cdf[i] - cdf_min) / (len - cdf_min)) * 255);
    }

    for (size_t i = 0; i < len; i++) {
        buf[i] = lut[buf[i]];
    }
}

void sobel_filter(uint8_t *in, uint8_t *out, int width, int height) {
    for (int y = 1; y < height - 1; y++) {
        for (int x = 1; x < width - 1; x++) {
            int gx = -in[(y - 1) * width + (x - 1)] - 2 * in[y * width + (x - 1)] - in[(y + 1) * width + (x - 1)]
                     + in[(y - 1) * width + (x + 1)] + 2 * in[y * width + (x + 1)] + in[(y + 1) * width + (x + 1)];

            int gy = -in[(y - 1) * width + (x - 1)] - 2 * in[(y - 1) * width + x] - in[(y - 1) * width + (x + 1)]
                     + in[(y + 1) * width + (x - 1)] + 2 * in[(y + 1) * width + x] + in[(y + 1) * width + (x + 1)];

            int magnitude = abs(gx) + abs(gy);
            if (magnitude > 255) magnitude = 255;
            out[y * width + x] = magnitude;
        }
    }
}

void export_image_to_file(uint8_t* img, int width, int height, int id) {
    char filename[32];
    snprintf(filename, sizeof(filename), "/spiffs/image_%d.txt", id);
    FILE *f = fopen(filename, "w");
    if (f == NULL) {
        ESP_LOGE(TAG, "Failed to open file for writing: %s", filename);
        return;
    }

    fprintf(f, "START_IMAGE_%d\n", id);
    for (int i = 0; i < width * height; i++) {
        fprintf(f, "%d,", img[i]);
    }
    fprintf(f, "\nEND_IMAGE_%d\n", id);
    fclose(f);

    ESP_LOGI(TAG, "Imagen %d guardada en %s", id, filename);
}

void print_spiffs_file(const char *filename) {
    FILE *f = fopen(filename, "r");
    if (!f) {
        ESP_LOGE(TAG, "No se pudo abrir %s", filename);
        return;
    }

    printf("START_FILE:%s\n", filename);
    char line[128];
    while (fgets(line, sizeof(line), f)) {
        printf("%s", line);
    }
    printf("END_FILE:%s\n", filename);
    fclose(f);
}


void app_main(void) {
    ESP_ERROR_CHECK(nvs_flash_init());

    esp_pm_config_esp32_t pm_config = {
        .max_freq_mhz = 80, // Cambia a 160 o 240 para otras pruebas
        .min_freq_mhz = 80,
        .light_sleep_enable = false
    };
    ESP_ERROR_CHECK(esp_pm_configure(&pm_config));
    vTaskDelay(2000 / portTICK_PERIOD_MS);  // da tiempo a que la frecuencia se estabilice

    if (init_camera() != ESP_OK) {
        ESP_LOGE(TAG, "Camera init failed");
        return;
    }

    bool images_exported = false;  // Para evitar exportar más de una vez
    esp_vfs_spiffs_conf_t conf = {
        .base_path = "/spiffs",
        .partition_label = NULL,
        .max_files = 5,
        .format_if_mount_failed = true
    };
    
    esp_err_t ret = esp_vfs_spiffs_register(&conf);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to mount or format filesystem");
        return;
    }
    int64_t t_start, t_capture, t_equalize, t_sobel, t_save, t_end;

    while (1) {
        t_start = esp_timer_get_time();
        camera_fb_t *pic = esp_camera_fb_get();
        t_capture = esp_timer_get_time();
        if (!pic) {
            ESP_LOGE(TAG, "Failed to capture image");
            continue;
        }
    
        uint8_t *temp = malloc(pic->len);
        memcpy(temp, pic->buf, pic->len);
        histogram_equalization(temp, pic->len);
        t_equalize = esp_timer_get_time();
    
        uint8_t *sobel_img = malloc(pic->len);
        memset(sobel_img, 0, pic->len);
        sobel_filter(temp, sobel_img, WIDTH, HEIGHT);
        t_sobel = esp_timer_get_time();
    
        // Guardar imagen en buffer circular
        if (image_buffer[image_index]) {
            free(image_buffer[image_index]);
        }
        image_buffer[image_index] = sobel_img;
    
        // Borrar imagen antigua en SPIFFS si ya hay 20
        if (image_count >= MAX_IMAGES) {
            char old_filename[32];
            int oldest = image_index;
            snprintf(old_filename, sizeof(old_filename), "/spiffs/image_%d.txt", oldest);
            remove(old_filename);
        } else {
            image_count++;
        }
    
        export_image_to_file(sobel_img, WIDTH, HEIGHT, image_index);  
        t_save = esp_timer_get_time();

        if (image_count % 10 == 0) {
            for (int i = 0; i < 10; i++) {
                char filename[32];
                snprintf(filename, sizeof(filename), "/spiffs/image_%d.txt", i);
                print_spiffs_file(filename);
                vTaskDelay(500 / portTICK_PERIOD_MS);
            }
        }
      
    
        esp_camera_fb_return(pic);
        free(temp);
        image_index = (image_index + 1) % MAX_IMAGES;
    
        t_end = esp_timer_get_time();
    
        // Imprimir tiempos en microsegundos (us)
        printf("TIME_LOG: capture=%lld, equalize=%lld, sobel=%lld, save=%lld, total=%lld\n",
            t_capture - t_start,
            t_equalize - t_capture,
            t_sobel - t_equalize,
            t_save - t_sobel,
            t_end - t_start
        );
    
        ESP_LOGI(TAG, "Imagen procesada y almacenada (%d/%d)", image_count, MAX_IMAGES);
    
        vTaskDelay(1000 / portTICK_PERIOD_MS);  // 1 segundo entre capturas
    }
}