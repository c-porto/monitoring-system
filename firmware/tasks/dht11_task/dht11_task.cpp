#include "include/dht11_task.hpp"
#include "freertos/portmacro.h"
#include "freertos/projdefs.h"
#include "freertos/queue.h"
#include <dht11.hpp>

extern QueueSetHandle_t xQueueTH_Handle;
TaskHandle_t xTaskDhtHandle;
StackType_t XTaskDhtStackStack[TASK_DHT_STACK_SIZE];

static const char *TAG = "DHT  Task";

void vTaskDht(void *pvParameters) {
  ESP_LOGI(TAG, "Creating Dht Task");
  Dht11 dht_sensor{4};
  TickType_t lastWakeTime = xTaskGetTickCount();

  for (;;) {
    ESP_LOGI(TAG, "Reading Temperature");

    auto sample = dht_sensor.read();

    xQueueSend(xQueueTH_Handle, sample.get(), portMAX_DELAY);

    vTaskDelayUntil(&lastWakeTime, pdMS_TO_TICKS(TASK_DHT_PERIOD_MS));
  }
}
