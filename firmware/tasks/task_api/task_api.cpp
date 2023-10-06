#include "include/task_api.hpp"
#include "../dht11_task/include/dht11_task.hpp"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void create_tasks() {
  xTaskCreate(vTaskDht, TASK_DHT_NAME, TASK_DHT_STACK_SIZE, NULL,
              TASK_DHT_PRIORITY, &xTaskDhtHandle);
}

void create_event_groups() {}
