#include "include/dht11_task.hpp"
#include "../../devices/utils/include/utils.hpp"
#include "freertos/portmacro.h"
#include "freertos/projdefs.h"
#include "freertos/queue.h"
#include "gpio_cxx.hpp"

TaskHandle_t xTaskDhtHandle;
StackType_t XTaskDhtStackStack[TASK_DHT_STACK_SIZE];

namespace {
const char *TAG = "DHT Task";
Dht11 dht_sensor{4};
} // namespace

void vTaskDht(void *params) {
  ESP_LOGI(TAG, "Creating Dht Task");
  TickType_t lastWakeTime = xTaskGetTickCount();
  auto measurement = static_cast<sensor::MeasureP>(params);
  sensor::SensorAPI sensor{&dht_sensor, measurement};

  while (true) {
    ESP_LOGI(TAG, "Reading Temperature");
    try {
      /* Mutex lock */
      if (auto pass = xSemaphoreTake(mutex, pdMS_TO_TICKS(1000));
          pass == pdPASS) {
        sensor.update_data();
        /* Mutex unlock */
        xSemaphoreGive(mutex);
      } else {
        ESP_LOGI(TAG, "Failed to obtain mutex in time");
      }
    } catch (idf::GPIOException &err) {
      ESP_LOGI(TAG, "Exception Ocurred in Dht measurement");
      /* Mutex unlock */
      xSemaphoreGive(mutex);
    }
    /* Event Group */
    vTaskDelayUntil(&lastWakeTime, pdMS_TO_TICKS(TASK_DHT_PERIOD_MS));
  }
}
