#pragma once
#include <freertos/FreeRTOS.h>
#include <freertos/event_groups.h>
#include <freertos/semphr.h>
#include <freertos/task.h>


#define TASK_QUEUE_NAME "TaskQueueManipulation"
#define TASK_QUEUE_STACK_SIZE 1536
#define TASK_QUEUE_PRIORITY 4
#define TASK_QUEUE_CORE 1
#define TASK_LINUX_NAME "TaskLinuxCommunication"
#define TASK_LINUX_STACK_SIZE 1536
#define TASK_LINUX_PRIORITY 4
#define TASK_LINUX_CORE 1

enum class HostCmd {
  TOTAL_TIME,
  EVENTS,
  WAIT,
};

extern SemaphoreHandle_t mutex;
extern EventGroupHandle_t event_group;
extern TaskHandle_t xTaskLinuxHandle;
extern TaskHandle_t xTaskQueueHandle;

void vTaskLinux(void *params);
void vTaskQueue(void *params);
static void some_uart_interrupt();
