#include "http_task.hpp"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/portmacro.h"
#include "freertos/projdefs.h"
#include "freertos/task.h"
#include "http_rq.hpp"
#include "utils.hpp"

static int create_json_obj(sensor::Measure *ms, char *obj_string);

static const logs::Logger log{ "Http Task" };

void http_client_task(void *sample_ptr)
{
	sensor::MeasureP sample = static_cast<sensor::Measure *>(sample_ptr);

	while (true) {
		xEventGroupWaitBits(event_group, SENSOR_READ_EVENT, pdTRUE,
				    pdTRUE, portMAX_DELAY);

		mutex_lock(mutex, 500U);

		sensor::Measure s_cpy{ sample->uv, sample->temp, sample->air,
				       sample->hm };

		mutex_unlock(mutex);

		Result res = http_send_sample(&s_cpy);

		log << res;
	}
}
