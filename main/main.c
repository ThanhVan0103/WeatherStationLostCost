#include <stdio.h>
#include "nvs_flash.h"
#include "wifi_mqtt.h"
#include "i2c_bme280.h"
#include "TASK_bme280.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "bme280.h"
#include "TASK_bh1750.h"
#include "TASK_ADC.h"
#include "TASK_SPEEDWIND.h"
#include "TASK_DIRECTWIND.h"

void app_main(void)
{
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    wifi_init();
    i2c_master_init();
    xTaskCreate(Publisher_Task, "Publisher_Task", 1024 * 5, NULL, 4, NULL);
    xTaskCreate(bh1750_task, "bh1750_task", 1024 * 5, NULL, 3, NULL);
    xTaskCreate(ADC_Task_ML8511, "ADC_Task_ML8511", 1024 * 5, NULL, 3, NULL);
    xTaskCreate(ADC_Task_MQ135, "ADC_Task_MQ135", 1024 * 5, NULL, 3, NULL);
    xTaskCreate(ADC_Task_BATTERY, "ADC_Task_BATTERY", 1024 * 5, NULL, 2, NULL);
    xTaskCreate(windSpeed_task, "windSpeed_task", 1024 * 5, NULL, 5, NULL);
    xTaskCreate(windDirect_task, "windSpeed_task", 1024 * 5, NULL, 5, NULL);
}
