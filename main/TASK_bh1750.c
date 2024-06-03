#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "mqtt_client.h"
#include "TASK_bh1750.h"
#include "bh1750.h"

#define MQTT_TOPIC_LIGHT_INTENSITY "esp32/bh1750/light_intensity"

void bh1750_task(void *pvParameters)
{
    bh1750_init();
    char light_intensity[20]; // Buffer to hold the light intensity as a string
    while (1)
    {
        float lux = bh1750_read_light_intensity(); //ESP_LOGI(TAG, "Light intensity: %.2f lux", lux);
        snprintf(light_intensity, sizeof(light_intensity), "%.2f", lux);   // Convert float to string
        if (MQTT_CONNECTED)
        {
            esp_mqtt_client_publish(client, MQTT_TOPIC_LIGHT_INTENSITY, light_intensity, 0, 0, 0);
        }
        vTaskDelay(5000 / portTICK_PERIOD_MS);
    }
}