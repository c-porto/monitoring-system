#include "include/linux_task.hpp"

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <chrono>
#include <cstddef>
#include <cstring>
#include <memory>
#include <sstream>

#include "../../devices/utils/include/utils.hpp"
#include "../task_api/include/task_api.hpp"
#include "driver/uart.h"
#include "esp_log.h"
#include "freertos/event_groups.h"
#include "freertos/portmacro.h"
#include "freertos/projdefs.h"
#include "include/uart_gpio.hpp"

#define DEBUG
// #undef DEBUG

/* Task handles */
TaskHandle_t xTaskLinuxHandle;
TaskHandle_t xTaskQueueHandle;
TaskHandle_t xTaskUartHandle;

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
logs::HostCmd host_cmd;
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
  /* Task Should Never Reach Here */
  ESP_LOGI(::TAG, "Deleting Queue Task");
  vTaskDelete(NULL);
}

/* Linux Host Communication task */
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
      case logs::HostCmd::TOTAL_TIME:
        ESP_LOGI(::TAG, "Sending Total Time to Host");
        /* Sending Total Time */
        for (std::size_t i{0}; i < 8; ++i) {
          uint8_t time_tmp = ((time->TotalTimeOn() >> (8 * i)) & 0xffUL);
          ESP_LOGI(::TAG, "Sending Total Time, iter %u", (unsigned int)i);
          uart_write_bytes(UART_PORT, (const char *)&time_tmp, 1);
        }
        host_cmd = logs::HostCmd::WAIT;
        break;
        /* Send All Events to Host Command */
      case logs::HostCmd::EVENTS:
        ESP_LOGI(::TAG, "Sending All Events to Host");
        while (fila) {
          /* Getting LogData in String Format */
          std::string log_msg = fila.dequeue().log_to_string();
          ESP_LOGI(::TAG, "Sending Event Registered in Logs");
          uart_write_bytes(UART_PORT, log_msg.c_str(),
                           std::strlen(log_msg.c_str()));
        }
        host_cmd = logs::HostCmd::WAIT;
        break;
        /* Waiting for new Commands */
      case logs::HostCmd::WAIT:
        ESP_LOGI(::TAG, "Waiting Commands from Host");
        break;
    }
  }
  /* Task Should Never Reach Here */
  ESP_LOGI(::TAG, "Deleting Linux Task");
  vTaskDelete(NULL);
}

/* Uart Handle task*/
void vTaskUart(void *params) {
  /* Queue Handle with uart events */
  auto event_queue = host_uart_init();
  /* Uart Event buffer*/
  uart_event_t event;
  /* Buffer for received messages */
  uint8_t *buffer = new uint8_t[UART_BUFFER_SIZE];

  /* Main Loop */
  while (true) {
    /* Waiting for Uart Event */
    if (xQueueReceive(event_queue, (void *)&event, portMAX_DELAY)) {
      ESP_LOGI(::TAG, "Uart Event");
      /* Checking Event Type */
      switch (event.type) {
        /* Data Event*/
        case UART_DATA:
          /* Reading from Rx Fifo */
          uart_read_bytes(UART_PORT, buffer, event.size, portMAX_DELAY);
          /* Checking Event Log Command from Host */
          if ((unsigned char)buffer[0] == 'L') {
            host_cmd = logs::HostCmd::EVENTS;
            xEventGroupSetBits(
                event_group,
                HOST_CMD_EVENT); /* Unblocking Linux Communication Task*/
            /* Checking Total Time Command from Host */
          } else if ((unsigned char)buffer[0] == 'T') {
            host_cmd = logs::HostCmd::TOTAL_TIME;
            xEventGroupSetBits(
                event_group,
                HOST_CMD_EVENT); /* Unblocking Linux Communication Task*/
            /* Invalid Command Check */
          } else {
            ESP_LOGI(::TAG, "Invalid Host Command");
            uart_write_bytes(UART_PORT, "Invalid Command",
                             std::strlen("Invalid Command"));
          }
          /* Setting Buffer bytes to Zero */
          std::memset(buffer, 0, UART_BUFFER_SIZE);
          break;
          /* All Other Uart Events are Irrelevant to Application */
        default:
          ESP_LOGI(::TAG, "uart event type: %d", event.type);
          host_cmd = logs::HostCmd::WAIT;
      }
    }
  }
  /* Task Should Never Reach Here */
  delete[] buffer;
  ESP_LOGI(::TAG, "Deleting Uart Task");
  vTaskDelete(NULL);
}
