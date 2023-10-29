#pragma once
#include "../../../devices/gyml8511/include/gyml8511.hpp"

#include <freertos/FreeRTOS.h>
#include <freertos/portmacro.h>
#include <freertos/queue.h>
#include <freertos/semphr.h>
#include <freertos/task.h>

/* Task properties */
#define TASK_GYML_NAME "TaskGyml"
#define TASK_GYML_STACK_SIZE 2048
#define TASK_GYML_PRIORITY 2
#define TASK_GYML_PERIOD_MS 1600
#define TASK_GYML_CORE 0

extern TaskHandle_t xTaskGymlHandle;
extern SemaphoreHandle_t mutex;
extern SemaphoreHandle_t sensor_read_semphr;

void vTaskGyml(void *params);
