#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "driver/gpio.h"
#include "TASK_DIRECTWIND.h"
#include "hal/gpio_types.h"

// Định nghĩa các chân GPIO cho 4 cảm biến
#define KY003_PINS { GPIO_NUM_12, GPIO_NUM_14, GPIO_NUM_27, GPIO_NUM_26 }
#define DIRECTIONS {"Bắc", "Đông", "Nam", "Tây"}
#define COMB_DIRECTIONS {"Đông Bắc", "Tây Bắc", "Đông Nam", "Tây Nam"}
#define MQTT_TOPIC_WIND_DIRECTION "esp32/ky003/wind_direct"

volatile int interrupt_flags[4] = {0, 0, 0, 0}; // ngắt direct

// Hàm ngắt cho direct_wind
void IRAM_ATTR gpio_isr_handler2(void* arg) {
    int sensor_index = (int)arg;
    interrupt_flags[sensor_index] = 1;
}

void windDirect_task(void *pvParameters) {
    
    gpio_num_t ky003_pins[] = KY003_PINS;
    const char* directions[] = DIRECTIONS;
    const char* comb_directions[] = COMB_DIRECTIONS;
    int num_sensors = sizeof(ky003_pins) / sizeof(ky003_pins[0]);
    char char_wind_direct[20];

    // Cấu hình chân GPIO cho các cảm biến
    for (int i = 0; i < num_sensors; i++) {
        gpio_config_t io_conf = {
            .pin_bit_mask = (1ULL << ky003_pins[i]),
            .mode = GPIO_MODE_INPUT,
            .pull_up_en = GPIO_PULLUP_ENABLE,
            .intr_type = GPIO_INTR_NEGEDGE
        };
        gpio_config(&io_conf);
    }

        // Cài đặt dịch vụ ISR cho windDirect 
    for (int i = 0; i < num_sensors; i++) {
        gpio_isr_handler_add(ky003_pins[i], gpio_isr_handler2, (void*)i);
    }

    while (1) {
        // Kiểm tra tất cả các cảm biến
        int sensor_values[4];
        for (int i = 0; i < num_sensors; i++) {
            if (interrupt_flags[i]) {
                sensor_values[i] = gpio_get_level(ky003_pins[i]);
                interrupt_flags[i] = 0;
            } else {
                sensor_values[i] = 1; // Giá trị mặc định nếu không có ngắt
            }
        }

        // Kiểm tra các tổ hợp hướng
        if (sensor_values[0] == 0 && sensor_values[1] == 0) {
            snprintf(char_wind_direct, sizeof(char_wind_direct), "%s", comb_directions[0]);
        } else if (sensor_values[0] == 0 && sensor_values[3] == 0) {
            snprintf(char_wind_direct, sizeof(char_wind_direct), "%s", comb_directions[1]);
        } else if (sensor_values[2] == 0 && sensor_values[1] == 0) {
            snprintf(char_wind_direct, sizeof(char_wind_direct), "%s", comb_directions[2]);
        } else if (sensor_values[2] == 0 && sensor_values[3] == 0) {
            snprintf(char_wind_direct, sizeof(char_wind_direct), "%s", comb_directions[3]);
        } else {
            // Kiểm tra các hướng đơn lẻ
            if (sensor_values[0] == 0) {
                snprintf(char_wind_direct, sizeof(char_wind_direct), "%s", directions[0]);
            } else if (sensor_values[1] == 0) {
                snprintf(char_wind_direct, sizeof(char_wind_direct), "%s", directions[1]);
            } else if (sensor_values[2] == 0) {
                snprintf(char_wind_direct, sizeof(char_wind_direct), "%s", directions[2]);
            } else if (sensor_values[3] == 0) {
                snprintf(char_wind_direct, sizeof(char_wind_direct), "%s", directions[3]);
            }
        }

        if (MQTT_CONNECTED) {
            esp_mqtt_client_publish(client, MQTT_TOPIC_WIND_DIRECTION, char_wind_direct, 0, 0, 0);
        }

        vTaskDelay(pdMS_TO_TICKS(1000)); // Đợi 1 giây
    }
}