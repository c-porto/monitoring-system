#include "include/linux_task.hpp"
#include "../../devices/utils/include/utils.hpp"
#include "esp_log.h"
#include "freertos/portmacro.h"
#include "freertos/projdefs.h"
#include <chrono>
#include <sstream>

TaskHandle_t xTaskLinuxHandle;

namespace {
ds::Queue<logs::LogData> fila{};
const char *TAG = "LINUX LOG";
std::stringstream ss;
} // namespace

void vTaskLinux(void *params) {

  auto ms = static_cast<sensor::MeasureP>(params);

  /* Still need to implement time control*/
  std::chrono::year_month_day d;

  while (true) {
    /* Events */
    if (auto p = xSemaphoreTake(mutex, pdMS_TO_TICKS(10000)); p == pdPASS) {
      static auto sample = *ms;
      xSemaphoreGive(mutex);
      logs::LogData item(sample.last_id, d, &sample);
      fila.enqueue(item);

      /* UART STUFF */
      /* Maybe some interrupt controled communication */

    } else {
      ESP_LOGI(TAG, "Failed to take mutex in Linux Task");
    }
  }
}
