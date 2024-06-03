#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "esp_log.h"
#include "driver/adc.h"
#include "esp_adc_cal.h"
#include "TASK_ADC.h"
#include <math.h> 

#define MQTT_TOPIC_UV_INDEX "esp32/ml8511/uv_index"
#define MQTT_TOPIC_CO2_INDEX "esp32/mq135/co2_index"
#define MQTT_TOPIC_BATTERY_PERCENTAGE "esp32/16500/battery"

#define DEFAULT_VREF    1100        //Use adc2_vref_to_gpio() to obtain a better estimate
#define NO_OF_SAMPLES   64          //Multisampling

static esp_adc_cal_characteristics_t *adc_chars;
static const adc_channel_t channel = ADC_CHANNEL_4;     //GPIO32 if ADC1, GPIO13 if ADC2
static const adc_channel_t channel2 = ADC_CHANNEL_6;     //GPIO34 if ADC1, GPIO14 if ADC2
static const adc_channel_t channel3 = ADC_CHANNEL_0;     //GPIO36 if ADC1, GPIO4 if ADC2
static const adc_bits_width_t width = ADC_WIDTH_BIT_12;
static const adc_atten_t atten = ADC_ATTEN_DB_0;
static const adc_unit_t unit = ADC_UNIT_1;


float mapfloat(float x, float in_min, float in_max, float out_min, float out_max)
{
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void ADC_Task_ML8511(void *arg)
{
    adc1_config_width(width);
    adc1_config_channel_atten(channel, atten);
    
    adc_chars = calloc(1, sizeof(esp_adc_cal_characteristics_t));
    esp_adc_cal_characterize(unit, atten, width, DEFAULT_VREF, adc_chars);
    char uv_Index[20];
    while (1) {
        uint32_t adc_reading_ML8511 = 0;

        //Multisampling
        for (int i = 0; i < NO_OF_SAMPLES; i++) {
            adc_reading_ML8511 += adc1_get_raw((adc1_channel_t)channel);
        }
        adc_reading_ML8511 /= NO_OF_SAMPLES;

        uint32_t voltage_ML8511 = esp_adc_cal_raw_to_voltage(adc_reading_ML8511, adc_chars);
    
        float uvIndex = mapfloat(voltage_ML8511/1000.0, 0.8, 2.7, 0.0, 10.0);
        snprintf(uv_Index, sizeof(uv_Index), "%.2f ", uvIndex);   // Convert float to string
         if (MQTT_CONNECTED)
        {
            esp_mqtt_client_publish(client, MQTT_TOPIC_UV_INDEX, uv_Index, 0, 0, 0);

        }
        UBaseType_t uxHighWaterMark = uxTaskGetStackHighWaterMark(NULL);
        printf("HighWaterMark: %d\n", uxHighWaterMark);
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}

void ADC_Task_MQ135(void *arg)
{
    adc1_config_width(width);
    adc1_config_channel_atten(channel, atten);
    
    adc_chars = calloc(1, sizeof(esp_adc_cal_characteristics_t));
    esp_adc_cal_characterize(unit, atten, width, DEFAULT_VREF, adc_chars);

    int Rload = 20000;
    float rS,ppm = 0;
    float rO = 11000;                 //min[Rs/Ro]=(max[ppm]/a)^(1/b)
    float a = 110.7432567;
    float b = -2.856935538;  
    float rSrO;
    float minppm = 0.358;
    float maxppm = 2.428;
    char CO2_Index[20];

    while (1) {
        int32_t adc_reading_MQ135 = 0; // Đổi kiểu dữ liệu của biến adc_reading thành int32_t
            for (int i = 0; i < NO_OF_SAMPLES; i++) {
            adc_reading_MQ135 += adc1_get_raw((adc1_channel_t)channel2);
            }
        adc_reading_MQ135 /= NO_OF_SAMPLES;
        vTaskDelay(500 / portTICK_PERIOD_MS); // Thời gian lấy mẫ
        //uint32_t voltage = esp_adc_cal_raw_to_voltage(adc_reading_MQ135, adc_chars);
        //printf("voltage: %d\n", voltage);
        rS = ((4095.0 * Rload) / adc_reading_MQ135) - Rload;
        //printf("rO= %.2f\n", rO);
        //printf("rS= %.2f\n", rS);
        rSrO = rS/rO;
        if (rSrO < maxppm && rSrO > minppm)
        {
            ppm = a * pow(rSrO,b);
            //printf("ppm= %.2f\n", ppm);
        }
        snprintf(CO2_Index, sizeof(CO2_Index), "%.2f ", ppm);   // Convert float to string
         if (MQTT_CONNECTED)
        {
            esp_mqtt_client_publish(client, MQTT_TOPIC_CO2_INDEX, CO2_Index, 0, 0, 0);

        }
        vTaskDelay(pdMS_TO_TICKS(5000)); // Chờ 1 giây trước khi lấy mẫu tiếp theo
    }
}

void ADC_Task_BATTERY(void *arg)
{
    adc1_config_width(width);
    adc1_config_channel_atten(channel, atten);
    
    adc_chars = calloc(1, sizeof(esp_adc_cal_characteristics_t));
    esp_adc_cal_characterize(unit, atten, width, DEFAULT_VREF, adc_chars);
    char bat_Index[20];
    while (1) {
        uint32_t adc_reading_BATTERY = 0;

        //Multisampling
        for (int i = 0; i < NO_OF_SAMPLES; i++) {
            adc_reading_BATTERY += adc1_get_raw((adc1_channel_t)channel3);
        }
        adc_reading_BATTERY /= NO_OF_SAMPLES;

        uint32_t voltage_BATTERY = esp_adc_cal_raw_to_voltage(adc_reading_BATTERY, adc_chars);
    
        float battery_Index = mapfloat(voltage_BATTERY/1000.0, 2.8, 4.2, 0.0, 100.0);
        snprintf(bat_Index, sizeof(bat_Index), "%.2f ", battery_Index);   // Convert float to string
         if (MQTT_CONNECTED)
        {
            esp_mqtt_client_publish(client, MQTT_TOPIC_BATTERY_PERCENTAGE, bat_Index, 0, 0, 0);

        }
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}