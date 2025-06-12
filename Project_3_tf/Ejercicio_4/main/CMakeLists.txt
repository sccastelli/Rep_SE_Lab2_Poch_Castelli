#
# Main component of TF Micro project 'digit_detection'.
#

idf_component_register(
    SRCS
        "detection_responder.cc"
        "image_provider.cc"
        "main.cc"
        "main_functions.cc"
        "model_settings.cc"
        "digit_detect_model_data.cc"  # ✅ Cambiado aquí
        "app_camera_esp.c"
        "esp_cli.c"

    PRIV_REQUIRES console static_images spi_flash
    INCLUDE_DIRS "")
