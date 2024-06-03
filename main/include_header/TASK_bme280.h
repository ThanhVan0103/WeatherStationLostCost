#ifndef PUBLISHER_H
#define PUBLISHER_H

#include <stdint.h>
#include "mqtt_client.h"

// Declare the external variables
extern esp_mqtt_client_handle_t client;
extern uint32_t MQTT_CONNECTED;

// Function declaration for the publisher task
void Publisher_Task(void *params);

#endif // PUBLISHER_H

