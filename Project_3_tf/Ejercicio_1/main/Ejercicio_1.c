#include <stdio.h>
#include <math.h>
#include <esp_timer.h>

#define NUM_INPUTS 20 

void app_main() {
    float x_vals[NUM_INPUTS] = {
        0.000000, 0.314159, 0.628319, 0.942478, 1.256637, 1.570796,
        1.884956, 2.199115, 2.513274, 2.827433, 3.141593, 3.455752,
        3.769912, 4.084070, 4.398230, 4.712389, 5.026548, 5.340708,
        5.654867, 5.969026
    };

    float y_vals[NUM_INPUTS];
    float y_tf_micro[NUM_INPUTS] = {
        0.000000, 0.372770, 0.559154, 0.838731, 0.965812, 1.042060,
        0.957340, 0.821787, 0.533738, 0.237217, 0.008472, -0.304993,
        -0.533738, -0.779427, -0.965812, -1.109837, -0.982756, -0.745539,
        -0.533738, -0.355825
    };

    printf("\nHello World - Versión manual en C (sin TensorFlow)\n");

    int64_t start_time = esp_timer_get_time();

    for (int i = 0; i < NUM_INPUTS; i++) {
        y_vals[i] = sinf(x_vals[i]);  
    }

    int64_t end_time = esp_timer_get_time();
    float elapsed_ms = (end_time - start_time) / 1000.0;

    float max_diff = 0.0;
    printf("\nResultados:\n");
    for (int i = 0; i < NUM_INPUTS; i++) {
        float diff = fabs(y_vals[i] - y_tf_micro[i]);
        if (diff > max_diff) max_diff = diff;

        printf("x: %.6f | y_manual: %.6f | y_TF: %.6f | Δ: %.6f\n",
               x_vals[i], y_vals[i], y_tf_micro[i], diff);
    }

    printf("\nTiempo de inferencia (manual): %.2f ms\n", elapsed_ms);
    float tiempo_tf_micro_ms = 64.5;
    float diferencia = tiempo_tf_micro_ms - elapsed_ms;
    printf("Tiempo de TF Micro (referencia): %.2f ms\n", tiempo_tf_micro_ms);
    printf("Diferencia: %.2f ms (%s)\n",
           fabs(diferencia),
           (diferencia > 0) ? "más rápido" : "más lento");

    printf("Máxima diferencia entre y_manual y y_TF_micro: %.6f\n", max_diff);
}
