#pragma once
#include "freertos/FreeRTOS.h"
#include <freertos/semphr.h>
#include <freertos/task.h>
#include <freertos/event_groups.h>

#define TASK_LINUX_NAME "TaskLinuxCommunication"
#define TASK_LINUX_STACK_SIZE 5096
#define TASK_LINUX_PRIORITY 1
#define TASK_LINUX_CORE 1

extern SemaphoreHandle_t mutex;
extern EventGroupHandle_t event_group;
extern TaskHandle_t xTaskLinuxHandle;

void vTaskLinux(void *params);
