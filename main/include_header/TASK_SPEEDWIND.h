#ifndef TASKWIND_H
#define TASKWIND_H

#include <stdint.h>
#include "mqtt_client.h"

// Declare the external variables
extern esp_mqtt_client_handle_t client;
extern uint32_t MQTT_CONNECTED;

// Function declaration for the publisher task
void windSpeed_task(void *pvParameters);
void IRAM_ATTR gpio_isr_handler(void* arg);

#endif // TASKWIND_H

