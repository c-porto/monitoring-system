#include "include/dht11_task.hpp"

#include <cstddef>

#include "../../devices/utils/include/utils.hpp"
#include "../task_api/include/task_api.hpp"
#include "esp_log.h"
#include "freertos/portmacro.h"
#include "freertos/projdefs.h"
#include "freertos/queue.h"
#include "gpio_cxx.hpp"

/* Task handle*/
TaskHandle_t xTaskDhtHandle;

/* Equivalent to static globals*/
namespace {
/* Tag used for logging*/
const char *TAG = "DHT Task";
/* Pin where dht sensor data pin is connected*/
constexpr std::size_t kDhtPin{32};
/* Dht11 sensor instance*/
sensor::Dht11 dht_sensor{kDhtPin};
}  // namespace

/* Dht11 task*/
void vTaskDht(void *params) {
  /* Used for logging*/
  ESP_LOGI(TAG, "Creating Dht Task");
  /* Used for managing time requirements*/
  TickType_t lastWakeTime = xTaskGetTickCount();
  /* Getting global measurement variable*/
  sensor::MeasureP measurement = &ms;

  /* Creating Dht11 Sensor API */
  sensor::SensorAPI sensor{&dht_sensor, measurement,&display};

  /* Main loop*/
  while (true) {
    /* Used for logging */
    ESP_LOGI(TAG, "Reading Temperature");
    try {
      ESP_LOGI(::TAG, "Trying to take Mutex");
      /* Mutex lock */
      if (auto pass = xSemaphoreTake(mutex, pdMS_TO_TICKS(10000));
          pass == pdPASS) {
        ESP_LOGI(::TAG, "Mutex Taken");
        /* Updating global measurement*/
        sensor.update_data();
        /* Mutex unlock */
        xSemaphoreGive(mutex);
        ESP_LOGI(::TAG, "Mutex Given");
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
    ESP_LOGI(::TAG,"Setting Event Group Bits");
    xEventGroupSetBits(
        event_group,
        DHT_READ_EVENT | DHT_READ_EVENT_HTTP); /* Setting event bits related
                                                  to sensor reading*/
    /* Yields back to scheduler */
    ESP_LOGI(::TAG,"Yielding for %i seconds", TASK_DHT_PERIOD_MS/1000);
    vTaskDelayUntil(&lastWakeTime, pdMS_TO_TICKS(TASK_DHT_PERIOD_MS));
  }
}
