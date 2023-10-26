#include "include/dht11_task.hpp"
#include "../../devices/utils/include/utils.hpp"
#include "freertos/portmacro.h"
#include "freertos/projdefs.h"
#include "freertos/queue.h"
#include "gpio_cxx.hpp"
#include <cstddef>

/* Task handle*/
TaskHandle_t xTaskDhtHandle;

/* Equivalent to static globals*/
namespace {
/* Tag used for logging*/
const char *TAG = "DHT Task";
/* Pin where dht sensor data pin is connected*/
constexpr std::size_t dht_pin = 25;
/* Dht11 sensor instance*/
sensor::Dht11 dht_sensor{dht_pin};
} // namespace

/* Dht11 task*/
void vTaskDht(void *params) {
  /* Used for logging*/
  ESP_LOGI(TAG, "Creating Dht Task");
  /* Used for managing time requirements*/
  TickType_t lastWakeTime = xTaskGetTickCount();
  /* Getting global measurement variable*/
  auto measurement = static_cast<sensor::MeasureP>(params);
  /* Creating Dht11 Sensor API */
  sensor::SensorAPI sensor{&dht_sensor, measurement};

  /* Main loop*/
  while (true) {
    /* Used for logging */
    ESP_LOGI(TAG, "Reading Temperature");
    try {
      /* Mutex lock */
      if (auto pass = xSemaphoreTake(mutex, pdMS_TO_TICKS(1000));
          pass == pdPASS) {
        /* Updating global measurement*/
        sensor.update_data();
        /* Mutex unlock */
        xSemaphoreGive(mutex);
      } else {
        /* Logs if task failed to obtain mutex*/
        ESP_LOGI(TAG, "Failed to obtain mutex in time");
      }
    }
    /* Treats gpio exceptions*/
    catch (idf::GPIOException &err) {
      /* Logs error */
      ESP_LOGI(TAG, "Exception Ocurred in Dht measurement");
      /* Mutex unlock to prevent deadlock*/
      xSemaphoreGive(mutex);
    }
    /* Event Group */
    /* Yields back to scheduler */
    vTaskDelayUntil(&lastWakeTime, pdMS_TO_TICKS(TASK_DHT_PERIOD_MS));
  }
}
