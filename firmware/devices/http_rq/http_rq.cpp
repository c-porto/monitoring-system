#include "include/http_rq.hpp"
#include "cJSON.h"
#include "cJSON_Utils.h"
#include "esp_err.h"
#include "utils.hpp"
#include "wifi.hpp"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "freertos/ringbuf.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_http_client.h"
#include <cstdint>
#include <cstring>

#define WIFI_CONNECTED_BIT (1U << 0U)
#define WIFI_FAIL_BIT (1U << 1U)

namespace
{
const logs::Logger log{ "Http RQ" };
const char *const TAG = "HTTP RQ";
EventGroupHandle_t s_wifi_event_group;
int s_retry_num = 0;
}

esp_err_t _http_event_handler(esp_http_client_event_t *evt)
{
	switch (evt->event_id) {
	case HTTP_EVENT_ERROR:
		ESP_LOGD(TAG, "HTTP_EVENT_ERROR");
		break;
	case HTTP_EVENT_ON_CONNECTED:
		ESP_LOGD(TAG, "HTTP_EVENT_ON_CONNECTED");
		break;
	case HTTP_EVENT_HEADER_SENT:
		ESP_LOGD(TAG, "HTTP_EVENT_HEADER_SENT");
		break;
	case HTTP_EVENT_ON_HEADER:
		ESP_LOGD(TAG, "HTTP_EVENT_ON_HEADER, key=%s, value=%s",
			 evt->header_key, evt->header_value);
		break;
	case HTTP_EVENT_ON_DATA:
		ESP_LOGD(TAG, "HTTP_EVENT_ON_DATA, evt->data_len=%d",
			 evt->data_len);
		if (!esp_http_client_is_chunked_response(evt->client)) {
			ESP_LOG_BUFFER_HEXDUMP(TAG, evt->data, evt->data_len,
					       ESP_LOG_DEBUG);
		}
		break;
	case HTTP_EVENT_ON_FINISH:
		ESP_LOGD(TAG, "HTTP_EVENT_ON_FINISH");
		break;
	case HTTP_EVENT_DISCONNECTED:
		ESP_LOGD(TAG, "HTTP_EVENT_DISCONNECTED");
		break;
	case HTTP_EVENT_REDIRECT:
		ESP_LOGD(TAG, "HTTP_EVENT_REDIRECT");
		break;
	}
	return ESP_OK;
}

static void event_handler(void *arg, esp_event_base_t event_base,
			  int32_t event_id, void *event_data)
{
	if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
		esp_wifi_connect();
	} else if (event_base == WIFI_EVENT &&
		   event_id == WIFI_EVENT_STA_DISCONNECTED) {
		if (s_retry_num < 5U) {
			esp_wifi_connect();
			s_retry_num++;
			ESP_LOGI(TAG, "retry to connect to the AP");
		} else {
			xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
		}
		ESP_LOGI(TAG, "connect to the AP fail");
	} else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
		ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
		ESP_LOGI(TAG, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
		s_retry_num = 0;
		xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
	}
}

void wifi_init(void)
{
	s_wifi_event_group = xEventGroupCreate();

	ESP_ERROR_CHECK(esp_netif_init());

	ESP_ERROR_CHECK(esp_event_loop_create_default());
	esp_netif_create_default_wifi_sta();

	wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
	ESP_ERROR_CHECK(esp_wifi_init(&cfg));

	esp_event_handler_instance_t instance_any_id;
	esp_event_handler_instance_t instance_got_ip;
	ESP_ERROR_CHECK(esp_event_handler_instance_register(
		WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL,
		&instance_any_id));
	ESP_ERROR_CHECK(esp_event_handler_instance_register(
		IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, NULL,
		&instance_got_ip));

	wifi_config_t wifi_config = {
		.sta = {
			.ssid = __WIFI_SSID,
			.password = __WIFI_PASSWORD,
			/* Setting a password implies station will connect to all security modes including WEP/WPA.
			 * However these modes are deprecated and not advisable to be used. Incase your Access point
			 * doesn't support WPA2, these mode can be enabled by commenting below line */
			//.threshold.authmode = WIFI_AUTH_WPA2_PSK,
			.pmf_cfg = {
				.capable = true,
				.required = false
			},
		},
	};
	ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
	ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
	ESP_ERROR_CHECK(esp_wifi_start());

	ESP_LOGI(TAG, "wifi_init_sta finished.");

	/* Waiting until either the connection is established (WIFI_CONNECTED_BIT) or connection failed for the maximum
	 * number of re-tries (WIFI_FAIL_BIT). The bits are set by event_handler() (see above) */
	EventBits_t bits = xEventGroupWaitBits(
		s_wifi_event_group, WIFI_CONNECTED_BIT | WIFI_FAIL_BIT, pdFALSE,
		pdFALSE, portMAX_DELAY);

	/* xEventGroupWaitBits() returns the bits before the call returned, hence we can test which event actually
	 * happened. */
	if (bits & WIFI_CONNECTED_BIT) {
		ESP_LOGI(TAG, "connected to ap SSID:%s password:%s",
			 __WIFI_SSID, __WIFI_PASSWORD);
	} else if (bits & WIFI_FAIL_BIT) {
		ESP_LOGI(TAG, "Failed to connect to SSID:%s, password:%s",
			 __WIFI_SSID, __WIFI_PASSWORD);
	} else {
		ESP_LOGE(TAG, "UNEXPECTED EVENT");
	}

	/* The event will not be processed after unregister */
	ESP_ERROR_CHECK(esp_event_handler_instance_unregister(
		IP_EVENT, IP_EVENT_STA_GOT_IP, instance_got_ip));
	ESP_ERROR_CHECK(esp_event_handler_instance_unregister(
		WIFI_EVENT, ESP_EVENT_ANY_ID, instance_any_id));
	vEventGroupDelete(s_wifi_event_group);
}

static int create_json_obj(sensor::Measure *ms, char *obj_string)
{
	int err = 0;
	cJSON *sample = cJSON_CreateObject();

	if (cJSON_AddNumberToObject(sample, "temp", ms->temp) == NULL) {
		err = -1;
		goto exit;
	}

	if (cJSON_AddNumberToObject(sample, "hm", ms->hm) == NULL) {
		err = -1;
		goto exit;
	}

	if (cJSON_AddNumberToObject(sample, "uv", ms->uv) == NULL) {
		err = -1;
		goto exit;
	}

	if (cJSON_AddNumberToObject(sample, "air", ms->air) == NULL) {
		err = -1;
		goto exit;
	}

	obj_string = cJSON_Print(sample);

	if (obj_string == NULL) {
		err = -1;
		log << "Couldn't create the json object";
	}

exit:
	cJSON_Delete(sample);
	return err;
}

Result http_send_sample(sensor::MeasureP sample)
{
	char *obj_string = nullptr;
	uint8_t buffer[513] = { 0 };
	esp_err_t err;
	Result res = Result::Ok;

	if (create_json_obj(sample, obj_string) < 0) {
		return Result::Err;
	}

	esp_http_client_config_t cfg = {
		.host = "http://0.0.0.0:42068",
		.path = "/api/post_ms",
		.event_handler = _http_event_handler,
		.transport_type = HTTP_TRANSPORT_OVER_TCP,
		.user_data = buffer,
	};

	esp_http_client_handle_t client = esp_http_client_init(&cfg);

	/* This if statement is absolutely not necessary, its only here 
     * to suppress the -Wnonnull warning/errors. */
	if (obj_string) {
		esp_http_client_set_url(client, "0.0.0.0:42068/api/post_ms");
		esp_http_client_set_method(client, HTTP_METHOD_POST);
		esp_http_client_set_header(client, "Content-Type",
					   "application/json");
		esp_http_client_set_post_field(client, obj_string,
					       strlen(obj_string));
		err = esp_http_client_perform(client);

		if (err == ESP_OK) {
			ESP_LOGI(
				TAG,
				"HTTP PATCH Status = %d, content_length = %" PRId64,
				esp_http_client_get_status_code(client),
				esp_http_client_get_content_length(client));
		} else {
			ESP_LOGE(TAG, "HTTP PATCH request failed: %s",
				 esp_err_to_name(err));
			res = Result::Err;
		}
	}

	esp_http_client_cleanup(client);
	cJSON_free(obj_string);
	return res;
}
