#include "include/linux_task.hpp"
#include "../../devices/utils/include/utils.hpp"
#include "../task_api/include/task_api.hpp"
#include "esp_log.h"
#include "freertos/event_groups.h"
#include "freertos/portmacro.h"
#include "freertos/projdefs.h"
#include <chrono>
#include <cstddef>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <sstream>

#define DEBUG
// #undef DEBUG

/* Task handle */
TaskHandle_t xTaskLinuxHandle;

/* Simply to remind of the handler map variable declared in utils.hpp*/
extern const std::unordered_map<sensor::sensor_id, LogHandler> kLogConversion;

/* Equivalent to static global variables */
namespace {
/* Log Tag for debugging */
const char *TAG = "LINUX LOG";
/* Max number of logs stored at any point in time */
constexpr std::size_t log_lenght{100};
/* Queue for storage and communication with linux*/
ds::Queue<logs::LogData<double>> fila{log_lenght};
/* Used for variable to string convertions */
std::stringstream ss;
} // namespace

/* Linux communication task */
void vTaskLinux(void *params) {

  /* Global measurement variable*/
  auto ms = static_cast<sensor::MeasureP>(params);

  /* Main loop */
  while (true) {

    /* Event Waiting */
    xEventGroupWaitBits(
        event_group, DHT_READ_EVENT | GYML811_READ_EVENT | CJMCU811_READ_EVENT,
        pdTRUE, pdFALSE, portMAX_DELAY);

    /* Mutex lock */
    if (auto p = xSemaphoreTake(mutex, pdMS_TO_TICKS(10000));
        p == pdPASS) /* Checking mutex take*/
    {
      /* Copying struct to unlock mutex faster */
      sensor::Measure sample = *ms;
      /* Mutex unlock */
      xSemaphoreGive(mutex);
      /* Checking reading error */
      if (sample) {

        /* Getting correct iterator from log handler map*/
        auto it{kLogConversion.find(sample.last_id)};
        /* Enqueueing correct sample */
        it->second(fila, sample);

      } else /* If there is a error in the measure */
      {
        /* Logging error */
        ESP_LOGI(TAG, "Error in measuring some data");
      }
      /* UART STUFF */
      /*while(fila){data transmit}
       * */
      /* Maybe some interrupt controled communication */
    } else /* If mutex lock failes log error */
    {
      ESP_LOGI(TAG, "Failed to take mutex in Linux Task");
    }
  }
}
