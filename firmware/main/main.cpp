#include <../devices/clockcalendar/include/clockcalendar.hpp>
#include <../devices/dht11/include/dht11.hpp>
#include <../tasks/dht11_task/include/dht11_task.hpp>
#include <../tasks/task_api/include/task_api.hpp>
#include <chrono>
#include <cstdlib>
#include <memory>
#include <stdlib.h>
#include <thread>

#include "cjmcu-811.hpp"
#include "display.hpp"
#include "driver/gpio.h"
#include "esp_log.h"
#include "freertos/portmacro.h"
#include "freertos/projdefs.h"
#include "gpio_cxx.hpp"
#include "gyml8511.hpp"
#include "hal/gpio_types.h"
#include "uart_gpio.hpp"
#include "utils.hpp"

/* Defining the app_main function*/
extern "C" {
void app_main();
}

/* Global measurament variable*/
sensor::Measure ms;
/* Global display variable */
sensor::Display display;

/* Used for adjusting system time*/
void set_system_time(int year, int month, int day, int hour, int min, int sec) {
  std::tm timeinfo = {0};
  timeinfo.tm_year = year - 1900;
  timeinfo.tm_mon = month - 1; // 0-based
  timeinfo.tm_mday = day;
  timeinfo.tm_hour = hour;
  timeinfo.tm_min = min;
  timeinfo.tm_sec = sec;

  // Set the timezone offset to UTC-3 (Brazilian Standard Time)
  timeinfo.tm_isdst = 0; // Daylight Saving Time is not observed in Brazil
  std::time_t local_time =
      std::mktime(&timeinfo) - 3 * 3600; // Adjust for UTC-3

  std::chrono::system_clock::time_point tp =
      std::chrono::system_clock::from_time_t(local_time);
  std::chrono::system_clock::duration d = tp.time_since_epoch();
  auto seconds = std::chrono::duration_cast<std::chrono::seconds>(d);

  struct timeval tv = {0};
  tv.tv_sec = seconds.count();
  settimeofday(&tv, nullptr);
}

/* Main function */
extern "C" void app_main(void) {
  /* Changing Main task priority to initialize all other tasks first */
  vTaskPrioritySet(NULL, 10);
  /* Setting system time to Embedded System software deadline */
  set_system_time(2023, 11, 19, 3, 59, 59);
  /* Updating initial time in clockcalendar */
  ms.date->StartTimeTracking(
      std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()));
  /* Initialing Display */
  display.init();
  /* Creates mutexes*/
  create_mutex();
  /* Creates event groups*/
  create_event_groups();
  /* Creates tasks */
  create_tasks();
  /* Espressif-idf Framework starts the FreeRTOS scheduler automatically*/
}
