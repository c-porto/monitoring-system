#pragma once
#include "freertos/FreeRTOS.h"
#include <freertos/semphr.h>
#include <freertos/task.h>

#define TASK_LINUX_NAME "TaskLinuxCommunication"
#define TASK_LINUX_STACK_SIZE 2048
#define TASK_LINUX_PRIORITY 1
#define TASK_LINUX_CORE 1

extern SemaphoreHandle_t mutex;
extern TaskHandle_t xTaskLinuxHandle;

void vTaskLinux(void *params);
