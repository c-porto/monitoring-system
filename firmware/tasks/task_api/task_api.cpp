#include "include/task_api.hpp"
#include "dht11_task.hpp"
#include "dht11.hpp"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"

void create_tasks() {
  xTaskCreatePinnedToCore(vTaskDht, TASK_DHT_NAME, TASK_DHT_STACK_SIZE, NULL,
                          TASK_DHT_PRIORITY, &xTaskDhtHandle, 0);
}

void create_event_groups() {}
