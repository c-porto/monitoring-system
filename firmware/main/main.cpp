#include <cstdlib>
#include <stdlib.h>

#include "display.hpp"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "task_api.hpp"
#include "utils.hpp"
#include "nvs_flash.h"

/* Defining the app_main function*/
extern "C" {
void app_main();
}

namespace
{
/* Global measurament variable*/
sensor::Measure ms;
} // namespace

/* Used for adjusting system time*/
/* Main function */
extern "C" void app_main(void)
{
	/* Changing Main task priority to initialize all other tasks first */
	vTaskPrioritySet(NULL, 10);
	// Initialize NVS
	esp_err_t ret = nvs_flash_init();
	if (ret == ESP_ERR_NVS_NO_FREE_PAGES ||
	    ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
		ESP_ERROR_CHECK(nvs_flash_erase());
		ret = nvs_flash_init();
	}
	ESP_ERROR_CHECK(ret);
	/* Creates mutexes*/
	create_mutex();
	/* Creates event groups*/
	create_event_groups();
	/* Creates tasks */
	create_tasks(static_cast<void *>(&ms));
	/* Espressif-idf Framework starts the FreeRTOS scheduler automatically*/
}
