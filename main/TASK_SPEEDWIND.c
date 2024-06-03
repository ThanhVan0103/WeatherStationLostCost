#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "driver/gpio.h"
#include "TASK_SPEEDWIND.h"
#include "hal/gpio_types.h"

#define MQTT_TOPIC_WIND_SPEED "esp32/ky003/wind_speed"
#define KY003_PIN GPIO_NUM_25 //Wind
#define TICK_PERIOD_MS 10

// Biến để lưu trạng thái ngắt
volatile int interrupt_flag = 0; // ngắt speed

// Biến đếm số lần value = 0
int count = 0;
float wind_speed = 0.0;
TickType_t reset_interval_ticks = 60000 / TICK_PERIOD_MS;


void IRAM_ATTR gpio_isr_handler(void* arg) {
    interrupt_flag = 1;
}

void windSpeed_task(void *pvParameters) {
        // Cấu hình chân GPIO cho cảm biến KY-003
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << KY003_PIN),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .intr_type = GPIO_INTR_NEGEDGE   // Kích hoạt ngắt khi cạnh xuống
    };
    gpio_config(&io_conf);

        // // Cài đặt dịch vụ ISR cho windSpeed 
    gpio_install_isr_service(0);
    gpio_isr_handler_add(KY003_PIN, gpio_isr_handler, (void*) KY003_PIN);

    char char_wind_speed[20];
    float wind_speed = 0.0;
    int previous_sensor_value = 1; // Giả sử bắt đầu ở mức cao (1
    TickType_t last_reset_time = xTaskGetTickCount();
    while (1) {
        if (interrupt_flag) {
            // Đọc giá trị từ cảm biến
            int sensor_value = gpio_get_level(KY003_PIN);

            // Chỉ in ra giá trị khi bằng 0
            if (sensor_value == 0 && previous_sensor_value == 1) {
                printf("Sensor Value: %d\n", sensor_value);
                // Tăng biến đếm lên một
                count++;
                printf("Count: %d\n", count);
            }
            previous_sensor_value = sensor_value;

            if (xTaskGetTickCount() - last_reset_time >= reset_interval_ticks) {
                wind_speed = count * 0.18; // Tính toán tốc độ gió
                printf("Wind Speed: %.2f m/s\n", wind_speed);
                count = 0;
                printf("Count reset to 0\n");
                last_reset_time = xTaskGetTickCount();
            }

            snprintf(char_wind_speed, sizeof(char_wind_speed), "%.2f", wind_speed);   // Convert float to string
            if (MQTT_CONNECTED)
            {
                esp_mqtt_client_publish(client, MQTT_TOPIC_WIND_SPEED, char_wind_speed, 0, 0, 0);
            }
            // Đặt lại cờ ngắt
            interrupt_flag = 0;
        }
        // Đợi một thời gian ngắn trong vòng lặp chính
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
}
