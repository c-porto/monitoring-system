#include "aht10.hpp"
#include "cjmcu-811.hpp"
#include "display.hpp"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/portmacro.h"
#include "freertos/projdefs.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include "gyml8511.hpp"
#include "utils.hpp"
#include <vector>

namespace
{
const logs::Logger log{ "Sensor Read Task" };
sensor::Display display;
sensor::Cjmcu811 cjmcu;
sensor::Gyml8511 gyml;
std::vector<sensor::Sensor *> sensors = { &gyml, &cjmcu };
} // namespace

void sensor_read_task(void *sample_ptr)
{
	sensor::Measure *sample = static_cast<sensor::Measure *>(sample_ptr);

	sensor::Aht10 aht{ 12, 14 };

	display.init();

	sensors.push_back(&aht);

	for (auto &sensor : sensors) {
		sensor->init();
	}

    vTaskDelay(pdMS_TO_TICKS(5000U));

	TickType_t last_cycle = xTaskGetTickCount();

	while (true) {
		mutex_lock(mutex, 500U);

		for (auto &sensor : sensors) {
			sensor->read(sample);
		}

		mutex_unlock(mutex);

		display << sample;
		log << sample;

		xEventGroupSetBits(event_group, SENSOR_READ_EVENT);

		vTaskDelayUntil(&last_cycle, pdMS_TO_TICKS(5000U));
	}
}
