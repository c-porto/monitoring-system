#pragma once
#include "../../../devices/dht11/include/dht11.hpp"

#include <freertos/FreeRTOS.h>
#include <freertos/portmacro.h>
#include <freertos/task.h>
#include <freertos/queue.h>

/* Task properties */
#define TASK_DHT_NAME "TaskDht"
#define TASK_DHT_STACK_SIZE 2048
#define TASK_DHT_PRIORITY 1
#define TASK_DHT_PERIOD_MS 1200
#define TASK_DHT_CORE 0
#define TASK_DHT_INIT_TIMEOUT_MS 2400

extern TaskHandle_t xTaskDhtHandle;
extern QueueHandle_t xQueueTH_Handle;
extern StackType_t XTaskDhtStackStack[TASK_DHT_STACK_SIZE];

void vTaskDht(void *pvParameters);
