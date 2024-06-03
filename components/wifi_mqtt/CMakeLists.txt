idf_component_register(SRCS "wifi_mqtt.c"
                    INCLUDE_DIRS "."
                    REQUIRES nvs_flash mqtt bme280)
