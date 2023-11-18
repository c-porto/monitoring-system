#include "include/task_api.hpp"

#include "../cjmcu811_task/include/cjmcu811_task.hpp"
#include "../gyml8511_task/include/gyml8511_task.hpp"
#include "../linux_task/include/linux_task.hpp"
#include "../warning_task/include/warning_task.hpp"
#include "dht11.hpp"
#include "dht11_task.hpp"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/projdefs.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "freertos/task.h"

#define EMBEDDED_CPP 1
#define ELETRONIC_PROJECT 1
#define TASK_DEBUG
// #undef TASK_DEBUG

/* Global mutex to protect measurament global variable and i2c bus */
SemaphoreHandle_t mutex;
/* Global event group handle to provide event syncronization */
EventGroupHandle_t event_group;

/* Creates all tasks*/
void create_tasks() {
  /* Creating Dht11 sensor task*/
  xTaskCreatePinnedToCore(vTaskDht, TASK_DHT_NAME, TASK_DHT_STACK_SIZE, NULL,
                          TASK_DHT_PRIORITY, &xTaskDhtHandle, TASK_DHT_CORE);
  vTaskDelay(pdMS_TO_TICKS(500));
  /* Creating Gyml8511 sensor task*/
  xTaskCreatePinnedToCore(vTaskGyml, TASK_GYML_NAME, TASK_GYML_STACK_SIZE, NULL,
                          TASK_GYML_PRIORITY, &xTaskGymlHandle, TASK_GYML_CORE);
  vTaskDelay(pdMS_TO_TICKS(500));
  /* Creating Cjmcu-811 sensor task*/
  xTaskCreatePinnedToCore(
      vTaskCjmcu, TASK_CJMCU811_NAME, TASK_CJMCU811_STACK_SIZE, NULL,
      TASK_CJMCU811_PRIORITY, &xTaskCjmcu811Handle, TASK_CJMCU811_CORE);
  /* Creating Linux communication task*/
  vTaskDelay(pdMS_TO_TICKS(500));
#if defined(EMBEDDED_CPP) && (EMBEDDED_CPP == 1)
  xTaskCreatePinnedToCore(vTaskLinux, TASK_LINUX_NAME, TASK_LINUX_STACK_SIZE,
                          NULL, TASK_LINUX_PRIORITY, &xTaskLinuxHandle,
                          TASK_LINUX_CORE);
#endif // For embedded c++ project
  /* Creating Buzzer and LED's Warning Task*/
  vTaskDelay(pdMS_TO_TICKS(500));
#if defined(EMBEDDED_CPP) && (EMBEDDED_CPP == 1)
  xTaskCreatePinnedToCore(vTaskWarning, TASK_WARNING_NAME,
                          TASK_WARNING_STACK_SIZE, NULL, TASK_WARNING_PRIORITY,
                          &xTaskWarningHandle, TASK_WARNING_CORE);
#endif // For embedded c++ project
  /* Creating Uart manipulation task*/
  vTaskDelay(pdMS_TO_TICKS(500));
#if defined(EMBEDDED_CPP) && (EMBEDDED_CPP == 1)
  xTaskCreatePinnedToCore(vTaskUart, TASK_UART_NAME, TASK_UART_STACK_SIZE, NULL,
                          TASK_UART_PRIORITY, &xTaskUartHandle, TASK_UART_CORE);
#endif // For embedded c++ project
  /* Creating Queue manipulation task*/
  vTaskDelay(pdMS_TO_TICKS(500));
#if defined(EMBEDDED_CPP) && (EMBEDDED_CPP == 1)
  xTaskCreatePinnedToCore(vTaskQueue, TASK_QUEUE_NAME, TASK_QUEUE_STACK_SIZE,
                          NULL, TASK_QUEUE_PRIORITY, &xTaskQueueHandle,
                          TASK_QUEUE_CORE);
#endif // For embedded c++ project
}

/* Creates event_group */
void create_event_groups() { event_group = xEventGroupCreate(); }

/* Creates mutex */
void create_mutex() {
  mutex = xSemaphoreCreateMutex();
  xSemaphoreGive(mutex);
}
