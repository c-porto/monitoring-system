#pragma once
#include <freertos/FreeRTOS.h>
#include <freertos/event_groups.h>
#include <freertos/portmacro.h>
#include <freertos/queue.h>
#include <freertos/semphr.h>
#include <freertos/task.h>

#include "../../../devices/gyml8511/include/gyml8511.hpp"

/* Task properties */
#define TASK_GYML_NAME "Gyml"
#define TASK_GYML_STACK_SIZE 3072
#define TASK_GYML_PRIORITY 2
#define TASK_GYML_PERIOD_MS 5000
#define TASK_GYML_CORE 0

extern TaskHandle_t xTaskGymlHandle;
extern SemaphoreHandle_t mutex;
extern EventGroupHandle_t event_group;
extern sensor::Measure ms;
extern sensor::Display display;

void vTaskGyml(void *params);
