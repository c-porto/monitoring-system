#include "include/linux_task.hpp"
#include "../../devices/utils/include/utils.hpp"
#include "esp_log.h"
#include "freertos/portmacro.h"
#include "freertos/projdefs.h"
#include <chrono>
#include <sstream>

TaskHandle_t xTaskLinuxHandle;
extern const std::unordered_map<sensor::sensor_id, log_handler> log_conversion;

namespace
{
ds::Queue<logs::LogData<double>> fila{};
const char *TAG = "LINUX LOG";
std::stringstream ss;
} // namespace

void vTaskLinux(void *params)
{

    auto ms = static_cast<sensor::MeasureP>(params);

    /* Still need to implement time control*/
    std::chrono::year_month_day d;

    while (true)
    {

        /* Event Waiting */

        if (auto p = xSemaphoreTake(mutex, pdMS_TO_TICKS(10000)); p == pdPASS)
        {
            static sensor::Measure sample = *ms;
            xSemaphoreGive(mutex);
            if (!sample.err)
            {
                auto sensor_log{log_conversion.find(sample.last_id)};
                sensor_log->second(fila, sample);
            }
            else
            {
                ESP_LOGI(TAG, "Error in measuring some data");
            }
            /* UART STUFF */
            /* Maybe some interrupt controled communication */
        }
        else
        {
            ESP_LOGI(TAG, "Failed to take mutex in Linux Task");
        }
    }
}
