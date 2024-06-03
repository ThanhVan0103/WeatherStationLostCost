#ifndef TASKDIRECT_H
#define TASKDIRECT_H

#include <stdint.h>
#include "mqtt_client.h"

// Declare the external variables
extern esp_mqtt_client_handle_t client;
extern uint32_t MQTT_CONNECTED;

// Function declaration for the publisher task
void windDirect_task(void *pvParameters);
void IRAM_ATTR gpio_isr_handler2(void* arg);

#endif // TASKDIRECT_H