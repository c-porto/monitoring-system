#pragma once

#include <freertos/FreeRTOS.h>
#include <freertos/event_groups.h>
#include <freertos/portmacro.h>
#include <freertos/queue.h>
#include <freertos/semphr.h>
#include <freertos/task.h>

#include "../../devices/cjmcu-811/include/cjmcu-811.hpp"

/* Task properties */
#define TASK_CJMCU811_NAME "TaskCjmcu"
#define TASK_CJMCU811_STACK_SIZE 1024
#define TASK_CJMCU811_PRIORITY 2
#define TASK_CJMCU811_PERIOD_MS 2000
#define TASK_CJMCU811_CORE 0

extern TaskHandle_t xTaskCjmcu811Handle;
extern SemaphoreHandle_t mutex;
extern EventGroupHandle_t event_group;

void vTaskCjmcu(void *params);