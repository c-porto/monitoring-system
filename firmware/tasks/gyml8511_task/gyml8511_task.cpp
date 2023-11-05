#include "include/gyml8511_task.hpp"

#include <cstddef>

#include "../../devices/utils/include/utils.hpp"
#include "../task_api/include/task_api.hpp"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/portmacro.h"
#include "freertos/projdefs.h"
#include "freertos/queue.h"
#include "gpio_cxx.hpp"

/* Task handle*/
TaskHandle_t xTaskGymlHandle;

/* Equivalent to static globals*/
namespace {
/* Tag used for logging*/
const char *TAG = "GYML Task";
/* Gyml sensor instance*/
sensor::Gyml8511 gyml_sensor{};
}  // namespace

/* Gyml8511 task*/
void vTaskGyml(void *params) {
  /* Used for logging*/
  ESP_LOGI(TAG, "Creating Gyml Task");
  /* Used for managing time requirements*/
  TickType_t lastWakeTime = xTaskGetTickCount();
  /* Getting global measurement variable*/
  auto measurement = static_cast<sensor::MeasureP>(params);
  /* Creating Dht11 Sensor API */
  sensor::SensorAPI sensor{&gyml_sensor, measurement};

  /* Main loop*/
  while (true) {
    /* Used for logging */
    ESP_LOGI(TAG, "Reading UV Irradiance");
    try {
      /* Mutex lock */
      if (auto pass = xSemaphoreTake(mutex, pdMS_TO_TICKS(10000));
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
      ESP_LOGI(TAG, "Exception Ocurred in Gyml measurement");
      /* Mutex unlock to prevent deadlock*/
      xSemaphoreGive(mutex);
    }
    /* Event Group */
    xEventGroupSetBits(
        event_group,
        GYML8511_READ_EVENT |
            GYML8511_READ_EVENT_HTTP); /* Setting event bits related to sensor
                                          reading*/
    /* Yields back to scheduler */
    vTaskDelayUntil(&lastWakeTime, pdMS_TO_TICKS(TASK_GYML_PERIOD_MS));
  }
}
