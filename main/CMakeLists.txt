idf_component_register(SRCS "TASK_DIRECTWIND.c" "TASK_bme280.c" "main.c" "TASK_bh1750.c" "TASK_ADC.c" "TASK_SPEEDWIND.c"
                    INCLUDE_DIRS "include_header"
                    REQUIRES wifi_mqtt bme280 bh1750 func_i2c esp_adc_cal)