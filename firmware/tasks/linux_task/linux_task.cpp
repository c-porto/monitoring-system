#include "include/linux_task.hpp"
#include "../../devices/utils/include/utils.hpp"
#include "esp_log.h"
#include "freertos/portmacro.h"
#include "freertos/projdefs.h"
#include <chrono>
#include <sstream>

/* Task handle */
TaskHandle_t xTaskLinuxHandle;

/* Simply to remind of the handler map variable declared in utils.hpp*/
extern const std::unordered_map<sensor::sensor_id, LogHandler> log_conversion;

/* Equivalent to static global variables */
namespace {
/* Queue for storage and communication with linux*/
ds::Queue<logs::LogData<double>> fila{};
/* Log Tag for debugging */
const char *TAG = "LINUX LOG";
/* Used for variable to string convertions */
std::stringstream ss;
} // namespace

/* Linux communication task */
void vTaskLinux(void *params) {

  /* Global measurement variable*/
  auto ms = static_cast<sensor::MeasureP>(params);

  /* Still need to implement time control*/
  std::chrono::year_month_day d;

  /* Main loop */
  while (true) {

    /* Event Waiting */

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
        auto sensor_log{log_conversion.find(sample.last_id)};
        /* Enqueueing correct sample */
        sensor_log->second(fila, sample);
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
