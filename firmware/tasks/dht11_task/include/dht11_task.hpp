#pragma once
#include "../../../devices/dht11/include/dht11.hpp"

#include <freertos/FreeRTOS.h>
#include <freertos/portmacro.h>
#include <freertos/queue.h>
#include <freertos/semphr.h>
#include <freertos/task.h>

/* Task properties */
#define TASK_DHT_NAME "TaskDht"
#define TASK_DHT_STACK_SIZE 2048
#define TASK_DHT_PRIORITY 3
#define TASK_DHT_PERIOD_MS 1200
#define TASK_DHT_CORE 0
#define TASK_DHT_INIT_TIMEOUT_MS 2400

extern TaskHandle_t xTaskDhtHandle;
extern SemaphoreHandle_t mutex;
extern SemaphoreHandle_t sensor_read_semphr;

void vTaskDht(void *params);
