#pragma once
#include <freertos/FreeRTOS.h>
#include <freertos/event_groups.h>
#include <freertos/semphr.h>
#include <freertos/task.h>

#define TASK_QUEUE_NAME "TaskQueueManipulation"
#define TASK_QUEUE_STACK_SIZE 1536
#define TASK_QUEUE_PRIORITY 3
#define TASK_QUEUE_CORE 1
#define TASK_LINUX_NAME "TaskLinuxCommunication"
#define TASK_LINUX_STACK_SIZE 1536
#define TASK_LINUX_PRIORITY 4
#define TASK_LINUX_CORE 1
#define TASK_UART_NAME "TaskUart"
#define TASK_UART_STACK_SIZE 2048
#define TASK_UART_PRIORITY 5
#define TASK_UART_CORE 1
namespace logs {

enum class HostCmd {
  TOTAL_TIME,
  EVENTS,
  WAIT,
};

}

extern SemaphoreHandle_t mutex;
extern EventGroupHandle_t event_group;
extern TaskHandle_t xTaskLinuxHandle;
extern TaskHandle_t xTaskQueueHandle;
extern TaskHandle_t xTaskUartHandle;

void vTaskLinux(void *params);
void vTaskQueue(void *params);
void vTaskUart(void *params);
