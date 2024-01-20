#include <cstdlib>
#include <stdlib.h>

#include "display.hpp"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "task_api.hpp"
#include "utils.hpp"

/* Defining the app_main function*/
extern "C" {
void app_main();
}

namespace {
/* Global measurament variable*/
sensor::Measure ms;
}

/* Used for adjusting system time*/
/* Main function */
extern "C" void app_main(void) {
  /* Changing Main task priority to initialize all other tasks first */
  vTaskPrioritySet(NULL, 10);
  /* Creates mutexes*/
  create_mutex();
  /* Creates event groups*/
  create_event_groups();
  /* Creates tasks */
  create_tasks(static_cast<void *>(&::ms));
  /* Espressif-idf Framework starts the FreeRTOS scheduler automatically*/
}
