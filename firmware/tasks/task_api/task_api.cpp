#include "include/task_api.hpp"

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/projdefs.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include "http_task.hpp"
#include "sensor_read_task.hpp"
#include "utils.hpp"
#include <cstdlib>

namespace
{
const char *TAG = "TASK API";
}

/* Global mutex to protect measurament global variable and i2c bus */
mutex_t mutex;

/* Global event group handle to provide event syncronization */
EventGroupHandle_t event_group;

/* Creates all tasks*/
void create_tasks(void *sample_ptr)
{
	if (xTaskCreatePinnedToCore(sensor_read_task, "Sensor Read Task",
				    4096UL, sample_ptr, 5U, NULL,
				    0) == pdFAIL) {
		ESP_LOGI(::TAG, "Failed to create Sensor Read Task");
		std::exit(1);
	}
	if (xTaskCreatePinnedToCore(http_client_task, "Http Task", 10000U,
				    sample_ptr, 5U, NULL, 1) == pdFAIL) {
		ESP_LOGI(::TAG, "Failed to create Http Client Task");
		std::exit(1);
	}
}

/* Creates event_group */
void create_event_groups()
{
	event_group = xEventGroupCreate();
}

/* Creates mutex */
void create_mutex()
{
	mutex = mutex_create();
}
