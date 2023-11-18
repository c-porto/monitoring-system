#pragma once
#include <freertos/FreeRTOS.h>
#include <freertos/portmacro.h>
#include <freertos/queue.h>
#include <freertos/semphr.h>
#include <freertos/task.h>

#include "../../../devices/dht11/include/dht11.hpp"
#include "../../task_api/include/task_api.hpp"

/* Task properties */
#define TASK_DHT_NAME "Dht"
#define TASK_DHT_STACK_SIZE 3072
#define TASK_DHT_PRIORITY 3
#define TASK_DHT_PERIOD_MS 5000
#define TASK_DHT_CORE 0
#define TASK_DHT_INIT_TIMEOUT_MS 2400

extern TaskHandle_t xTaskDhtHandle;
extern SemaphoreHandle_t mutex;
extern EventGroupHandle_t event_group;
extern sensor::Measure ms;
extern sensor::Display display;

void vTaskDht(void *params);
