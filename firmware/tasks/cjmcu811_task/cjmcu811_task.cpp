#include "../../devices/utils/include/utils.hpp"
#include "freertos/FreeRTOS.h"
#include "freertos/portmacro.h"
#include "freertos/projdefs.h"
#include "freertos/queue.h"
#include "gpio_cxx.hpp"
#include <cstddef>
#include "esp_log.h"
#include "include/cjmcu811_task.hpp"
#include <../../devices/cjmcu-811/include/cjmcu-811.hpp>

/* Task handle*/
TaskHandle_t xTaskCjmcu811Handle;

/* Equivalent to static globals*/
namespace {
/* Tag used for logging*/
const char *TAG = "Cjmcu-811 Task";
/* Cjmcu811 sensor instance*/
sensor::Cjmcu811 cjmcu_sensor{};
} // namespace

/* Cjmcu811 task*/
void vTaskCjmcu(void *params) {
  /* Used for logging*/
  ESP_LOGI(::TAG, "Creating Cjmcu Task");
  /* Used for logging*/
  ESP_LOGI(::TAG, "Initializing Sensor");
  cjmcu_sensor.init();
  /* Used for managing time requirements*/
  TickType_t lastWakeTime = xTaskGetTickCount();
  /* Getting global measurement variable*/
  auto measurement = static_cast<sensor::MeasureP>(params);
  /* Creating Dht11 Sensor API */
  sensor::SensorAPI sensor{&cjmcu_sensor, measurement};

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
    /* Semaphore Give */
    xSemaphoreGive(sensor_read_semphr);
    /* Event Group */
    /* Yields back to scheduler */
    vTaskDelayUntil(&lastWakeTime, pdMS_TO_TICKS(TASK_CJMCU811_PERIOD_MS));
  }
}
