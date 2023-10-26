#include "esp_log.h"
#include "gpio_cxx.hpp"
#include <../devices/dht11/include/dht11.hpp>
#include <../tasks/dht11_task/include/dht11_task.hpp>
#include <../tasks/task_api/include/task_api.hpp>
#include <cstdlib>
#include <thread>

/* Main function */
extern "C" void app_main(void) {
  /* Creates mutexes*/
  create_mutex();
  /* Creates semaphores*/
  create_semphr();
  /* Creates event groups*/
  create_event_groups();
  /* Creates tasks */
  create_tasks();
  /* Espressif-idf Framework starts the FreeRTOS scheduler automatically*/
}
