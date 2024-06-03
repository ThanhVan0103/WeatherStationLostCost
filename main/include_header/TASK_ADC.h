#ifndef TASKML8511_H
#define TASKML8511_H

#include <stdint.h>
#include "mqtt_client.h"

// Declare the external variables
extern esp_mqtt_client_handle_t client;
extern uint32_t MQTT_CONNECTED;

// Function declaration for the publisher task
float mapfloat(float x, float in_min, float in_max, float out_min, float out_max);
void ADC_Task_ML8511(void *arg);
void ADC_Task_MQ135(void *arg);
void ADC_Task_BATTERY(void *arg);

#endif // TASKML8511_H

