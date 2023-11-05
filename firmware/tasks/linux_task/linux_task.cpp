#include "include/linux_task.hpp"

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <chrono>
#include <cstddef>
#include <sstream>

#include "../../devices/utils/include/utils.hpp"
#include "../task_api/include/task_api.hpp"
#include "esp_log.h"
#include "freertos/event_groups.h"
#include "freertos/portmacro.h"
#include "freertos/projdefs.h"

#define DEBUG
// #undef DEBUG

/* Task handles */
TaskHandle_t xTaskLinuxHandle;
TaskHandle_t xTaskQueueHandle;

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
/* Global variable to store command from host */
HostCmd host_cmd;
}  // namespace

/* Queue Manipulation task */
void vTaskQueue(void *params) {
  /* Global measurement variable*/
  auto ms = static_cast<sensor::MeasureP>(params);

  /* Main loop */
  while (true) {
    /* Event Waiting */
    xEventGroupWaitBits(
        event_group, DHT_READ_EVENT | GYML8511_READ_EVENT | CJMCU811_READ_EVENT,
        pdTRUE, pdFALSE, portMAX_DELAY);

    /* Mutex lock */
    if (auto p = xSemaphoreTake(mutex, pdMS_TO_TICKS(8000));
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
    } else /* If mutex lock failes log error */
    {
      ESP_LOGI(TAG, "Failed to take mutex in Linux Task");
    }
  }
}

void vTaskLinux(void *params) {
  /* Global measurement variable*/
  auto ms = static_cast<sensor::MeasureP>(params);

  /* Simply using the clockcalendar shared pointer since its the only thing used
   * in this task */
  auto time = ms->date;

  /* Main loop */
  while (true) {
    /* Event Waiting */
    xEventGroupWaitBits(event_group, HOST_CMD_EVENT, pdTRUE, pdTRUE,
                        portMAX_DELAY);

    /* Host Communication Section */
    /* Switch to Handle All Possible Commands from Host*/
    switch (host_cmd) {
        /* Send Total Time to Host Command */
      case HostCmd::TOTAL_TIME:
        ESP_LOGI(::TAG, "Sending Total Time to Host");
        /* Sending Total Time */
        for (std::size_t i{0}; i < 8; ++i) {
          uint8_t time_tmp = ((time->TotalTimeOn() >> (8 * i)) & 0xffUL);
          /*uart_send(time_tmp)*/
        }
        host_cmd = HostCmd::WAIT;
        break;
        /* Send All Events to Host Command */
      case HostCmd::EVENTS:
        ESP_LOGI(::TAG, "Sending All Events to Host");
        while (fila) {
          std::string log_msg = fila.dequeue().log_to_string();
          for (char const &byte : log_msg) {
            /* uart_send(byte)*/
          }
        }
        host_cmd = HostCmd::WAIT;
        break;
        /* Waiting for new Commands */
      case HostCmd::WAIT:
        ESP_LOGI(::TAG, "Waiting Commands from Host");
        break;
    }
  }
}

static void some_uart_interrupt() {
  BaseType_t xHigherPriorityTaskWoken = pdFALSE;
  /* uart_receive handle */
  if (true /* According to cmd given*/) {
    host_cmd = HostCmd::EVENTS;
  } else {
    host_cmd = HostCmd::TOTAL_TIME;
  }
  xEventGroupSetBitsFromISR(event_group, HOST_CMD_EVENT,
                            &xHigherPriorityTaskWoken);
  portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}
