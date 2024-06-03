#ifndef TASKBH1750_H
#define TASKBH1750_H

#include <stdint.h>
#include "mqtt_client.h"

// Declare the external variables
extern esp_mqtt_client_handle_t client;
extern uint32_t MQTT_CONNECTED;

// Function declaration for the publisher task
void bh1750_task(void *pvParameters);

#endif // TASKBH1750_H

