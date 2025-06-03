// path: main/main.c

#include <stdio.h>
#include <math.h>
#include <esp_timer.h>

#define NUM_INPUTS 19

void app_main() {
    float x_vals[NUM_INPUTS] = {
        0.000000, 0.314159, 0.628319, 0.942478, 1.256637, 1.570796,
        1.884956, 2.199115, 2.513274, 2.827433, 3.141593, 3.455752,
        3.769912, 4.084070, 4.398230, 4.712389, 5.026548, 5.340708,
        5.654867
    };

    float y_vals[NUM_INPUTS];

    int64_t start_time = esp_timer_get_time();

    for (int i = 0; i < NUM_INPUTS; i++) {
        y_vals[i] = sinf(x_vals[i]);
    }

    int64_t end_time = esp_timer_get_time();
    int64_t elapsed_us = end_time - start_time;

    for (int i = 0; i < NUM_INPUTS; i++) {
        printf("x_value: %.6f, y_value: %.6f\n", x_vals[i], y_vals[i]);
    }

    printf("Tiempo total de inferencia: %lld us\n", elapsed_us);
}
