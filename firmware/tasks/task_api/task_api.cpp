#include "include/task_api.hpp"
#include "../../devices/utils/include/utils.hpp"
#include "../linux_task/include/linux_task.hpp"
#include "dht11.hpp"
#include "dht11_task.hpp"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include <freertos/semphr.h>

/* Equivalent to static globals*/
namespace {
/* Global measurement variable*/
sensor::MeasureP ms{};
} // namespace

/* Global mutex to protect ms variable*/
SemaphoreHandle_t mutex;

/* Creates all tasks*/
void create_tasks() {
  /* Creating Dht11 sensor task*/
  xTaskCreatePinnedToCore(vTaskDht, TASK_DHT_NAME, TASK_DHT_STACK_SIZE, &ms,
                          TASK_DHT_PRIORITY, &xTaskDhtHandle, TASK_DHT_CORE);
  /* Creating Linux communication task*/
  xTaskCreatePinnedToCore(vTaskLinux, TASK_LINUX_NAME, TASK_LINUX_STACK_SIZE,
                          &ms, TASK_LINUX_PRIORITY, &xTaskLinuxHandle,
                          TASK_LINUX_CORE);
}

/* Creates all event_groups*/
void create_event_groups() {}

/* Creates mutexes*/
void create_mutex() { mutex = xSemaphoreCreateMutex(); }
