#include "include/cjmcu811_task.hpp"

#include <../../devices/cjmcu-811/include/cjmcu-811.hpp>
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
TaskHandle_t xTaskCjmcu811Handle;

/* Equivalent to static globals*/
namespace {
/* Tag used for logging*/
const char *TAG = "Cjmcu-811 Task";
/* Cjmcu811 sensor instance*/
sensor::Cjmcu811 cjmcu_sensor{};
}  // namespace

/* Cjmcu811 task*/
void vTaskCjmcu(void *params) {
  /* Used for logging*/
  ESP_LOGI(::TAG, "Creating Cjmcu Task");
  /* Used for managing time requirements*/
  TickType_t lastWakeTime = xTaskGetTickCount();
  /* Getting global measurement variable*/
  auto measurement = static_cast<sensor::MeasureP>(params);
  /* Creating Dht11 Sensor API */
  sensor::SensorAPI sensor{&cjmcu_sensor, measurement};
  /* Used for logging*/
  ESP_LOGI(::TAG, "Initializing Sensor");
  /* Initializing cjmcu sensor*/
  cjmcu_sensor.init(); /* This initialization can take as much as 20 minutes for
                          accurate readings*/

  /* Main loop*/
  while (true) {
    /* Used for logging */
    ESP_LOGI(::TAG, "Reading Air quality");
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
        ESP_LOGI(::TAG, "Failed to obtain mutex in time");
      }
    }
    /* Treats gpio exceptions*/
    catch (idf::GPIOException &err) {
      /* Logs error */
      ESP_LOGI(::TAG, "Exception Ocurred in Cjmcu measurement");
      /* Mutex unlock to prevent deadlock*/
      xSemaphoreGive(mutex);
    }
    /* Event Group */
    xEventGroupSetBits(
        event_group,
        CJMCU811_READ_EVENT |
            CJMCU811_READ_EVENT_HTTP); /* Setting event bits related to sensor
                                          reading*/
    /* Yields back to scheduler */
    vTaskDelayUntil(&lastWakeTime, pdMS_TO_TICKS(TASK_CJMCU811_PERIOD_MS));
  }
}
