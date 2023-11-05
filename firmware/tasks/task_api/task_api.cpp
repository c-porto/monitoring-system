#include "include/task_api.hpp"

#include "../cjmcu811_task/include/cjmcu811_task.hpp"
#include "../gyml8511_task/include/gyml8511_task.hpp"
#include "../linux_task/include/linux_task.hpp"
#include "../warning_task/include/warning_task.hpp"
#include "dht11.hpp"
#include "dht11_task.hpp"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "freertos/task.h"

#define EMBEDDED_CPP 1
#define ELETRONIC_PROJECT 1
#define TASK_DEBUG
// #undef TASK_DEBUG

/* Equivalent to static globals*/
namespace {
/* TAG for logging*/
const char *TAG = "TASK";
/* Global measurement variable*/
sensor::Measure ms{};
}  // namespace

/* Global mutex to protect ms variable*/
SemaphoreHandle_t mutex;
/* Global event group handle to provide event syncronization */
EventGroupHandle_t event_group;

/* Creates all tasks*/
void create_tasks() {
  /* Creating Dht11 sensor task*/
  xTaskCreatePinnedToCore(vTaskDht, TASK_DHT_NAME, TASK_DHT_STACK_SIZE, &ms,
                          TASK_DHT_PRIORITY, &xTaskDhtHandle, TASK_DHT_CORE);
#ifdef TASK_DEBUG
  if (xTaskDhtHandle == NULL) {
    ESP_LOGI(::TAG, "Failed to Create DHT TASK");
  }
#endif
  /* Creating Gyml8511 sensor task*/
  xTaskCreatePinnedToCore(vTaskGyml, TASK_GYML_NAME, TASK_GYML_STACK_SIZE, &ms,
                          TASK_GYML_PRIORITY, &xTaskGymlHandle, TASK_GYML_CORE);
#ifdef TASK_DEBUG
  if (xTaskGymlHandle == NULL) {
    ESP_LOGI(::TAG, "Failed to Create GYML TASK");
  }
#endif
  /* Creating Cjmcu-811 sensor task*/
  xTaskCreatePinnedToCore(vTaskCjmcu, TASK_CJMCU811_NAME,
                          TASK_CJMCU811_STACK_SIZE, &ms, TASK_GYML_PRIORITY,
                          &xTaskCjmcu811Handle, TASK_CJMCU811_CORE);
#ifdef TASK_DEBUG
  if (xTaskCjmcu811Handle == NULL) {
    ESP_LOGI(::TAG, "Failed to Create CJMCU TASK");
  }
#endif
  /* Creating Linux communication task*/
#if defined(EMBEDDED_CPP) && (EMBEDDED_CPP == 1)
  xTaskCreatePinnedToCore(vTaskLinux, TASK_LINUX_NAME, TASK_LINUX_STACK_SIZE,
                          &ms, TASK_LINUX_PRIORITY, &xTaskLinuxHandle,
                          TASK_LINUX_CORE);
#endif  // For embedded c++ project
#ifdef TASK_DEBUG
  if (xTaskLinuxHandle == NULL) {
    ESP_LOGI(::TAG, "Failed to Create Linux TASK");
  }
#endif
  /* Creating Queue manipulation task*/
#if defined(EMBEDDED_CPP) && (EMBEDDED_CPP == 1)
  xTaskCreatePinnedToCore(vTaskQueue, TASK_QUEUE_NAME, TASK_QUEUE_STACK_SIZE,
                          &ms, TASK_QUEUE_PRIORITY, &xTaskQueueHandle,
                          TASK_QUEUE_CORE);
#endif  // For embedded c++ project
#ifdef TASK_DEBUG
  if (xTaskQueueHandle == NULL) {
    ESP_LOGI(::TAG, "Failed to Create Queue TASK");
  }
#endif
  /* Creating Buzzer and LED's Warning Task*/
#if defined(EMBEDDED_CPP) && (EMBEDDED_CPP == 1)
  xTaskCreatePinnedToCore(vTaskWarning, TASK_WARNING_NAME,
                          TASK_WARNING_STACK_SIZE, &ms, TASK_WARNING_PRIORITY,
                          &xTaskWarningHandle, TASK_WARNING_CORE);
#endif  // For embedded c++ project
#ifdef TASK_DEBUG
  if (xTaskWarningHandle == NULL) {
    ESP_LOGI(::TAG, "Failed to Create Warning TASK");
  }
#endif
  /* Creating Queue manipulation task*/
#if defined(EMBEDDED_CPP) && (EMBEDDED_CPP == 1)
  xTaskCreatePinnedToCore(vTaskUart, TASK_UART_NAME, TASK_UART_STACK_SIZE, NULL,
                          TASK_UART_PRIORITY, &xTaskUartHandle, TASK_UART_CORE);
#endif  // For embedded c++ project
#ifdef TASK_DEBUG
  if (xTaskUartHandle == NULL) {
    ESP_LOGI(::TAG, "Failed to Create Queue TASK");
  }
#endif
}

/* Creates all event_groups*/
void create_event_groups() { event_group = xEventGroupCreate(); }

/* Creates mutexes*/
void create_mutex() { mutex = xSemaphoreCreateMutex(); }
