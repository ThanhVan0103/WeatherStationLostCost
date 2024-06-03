#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "mqtt_client.h"
#include "driver/i2c.h"
#include "bme280.h"
#include "i2c_bme280.h"
#include "TASK_bme280.h"

static const char *TAG_BME280 = "BME280";

#define MQTT_TOPIC_TEMPERATURE "esp32/bme280/temperature"
#define MQTT_TOPIC_HUMIDITY "esp32/bme280/humidity"
#define MQTT_TOPIC_PRESSURE "esp32/bme280/pressure"

void Publisher_Task(void *params)
{
    struct bme280_t bme280 = {
        .bus_write = BME280_I2C_bus_write,
        .bus_read = BME280_I2C_bus_read,
        .dev_addr = BME280_I2C_ADDRESS1,
        .delay_msec = BME280_delay_msek};

    s32 com_rslt;
    s32 v_uncomp_pressure_s32;
    s32 v_uncomp_temperature_s32;
    s32 v_uncomp_humidity_s32;

    com_rslt = bme280_init(&bme280);

    com_rslt += bme280_set_oversamp_pressure(BME280_OVERSAMP_16X);
    com_rslt += bme280_set_oversamp_temperature(BME280_OVERSAMP_2X);
    com_rslt += bme280_set_oversamp_humidity(BME280_OVERSAMP_1X);

    com_rslt += bme280_set_standby_durn(BME280_STANDBY_TIME_1_MS);
    com_rslt += bme280_set_filter(BME280_FILTER_COEFF_16);

    com_rslt += bme280_set_power_mode(BME280_NORMAL_MODE);
    if (com_rslt == SUCCESS)
    {
        while (true)
        {
            vTaskDelay(40 / portTICK_PERIOD_MS);

            com_rslt = bme280_read_uncomp_pressure_temperature_humidity(
                &v_uncomp_pressure_s32, &v_uncomp_temperature_s32, &v_uncomp_humidity_s32);

            double temp = bme280_compensate_temperature_double(v_uncomp_temperature_s32);
            char temperature[12];
            sprintf(temperature, "%.2f degC", temp);

            double press = bme280_compensate_pressure_double(v_uncomp_pressure_s32) / 100; // Pa -> hPa
            char pressure[10];
            sprintf(pressure, "%.2f hPa", press);

            double hum = bme280_compensate_humidity_double(v_uncomp_humidity_s32);
            char humidity[10];
            sprintf(humidity, "%.2f %%", hum);

            if (com_rslt == SUCCESS)
            {
                if (MQTT_CONNECTED)
                {
                    esp_mqtt_client_publish(client, MQTT_TOPIC_TEMPERATURE, temperature, 0, 0, 0);
                    esp_mqtt_client_publish(client, MQTT_TOPIC_PRESSURE, pressure, 0, 0, 0);
                    esp_mqtt_client_publish(client, MQTT_TOPIC_HUMIDITY, humidity, 0, 0, 0);
                    vTaskDelay(5000 / portTICK_PERIOD_MS);
                }
            }
            else
            {
                ESP_LOGE(TAG_BME280, "measure error. code: %d", com_rslt);
            }
        }
    }
    else
    {
        ESP_LOGE(TAG_BME280, "init or setting error. code: %d", com_rslt);
    }
}
