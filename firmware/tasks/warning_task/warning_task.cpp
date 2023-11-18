#include "include/warning_task.hpp"

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <chrono>
#include <cstddef>
#include <sstream>

#include "../../devices/utils/include/utils.hpp"
#include "../linux_task/include/uart_gpio.hpp"
#include "../task_api/include/task_api.hpp"
#include "esp_log.h"
#include "freertos/event_groups.h"
#include "freertos/portmacro.h"
#include "freertos/projdefs.h"

/* Task handle */
TaskHandle_t xTaskWarningHandle;

/* Equivalent to static global variables */
namespace {
/* Warning Task Tag for debugging */
const char *TAG = "WARNING TASK";
/* Bool Variable to Represent Inadequate Enviroment */
bool is_env_ok{true};
} // namespace

/* Warning Task Responsible for Activating the Buzzer and LEDS */
void vTaskWarning(void *params) {
  /* Buzzer object */
  Buzzer bz{};

  /* Getting global measurement variable*/
  sensor::MeasureP measurement = &ms;

  /* Init Gpio's */
  if (gpio_led_init()) {
    ESP_LOGI(::TAG, "Error Configuring GPIO");
  }
  /* Init Buzzer PWM */
  if (bz.buzzer_init()) {
    ESP_LOGI(::TAG, "Error Configuring Buzzer");
  }
  vTaskDelay(pdMS_TO_TICKS(10000));

  /* Main loop */
  while (true) {

    /* Checking Enviroment for Buzzer Activation*/
    if (!is_env_ok) {
      /* Turn Buzzer On*/
      if (bz.buzzer_toggle(BuzzerState::ON))
        ESP_LOGI(::TAG, "Error Chaging Buzzer State to ON");
    } else {
      /* Turn Buzzer Off*/
      if (bz.buzzer_toggle(BuzzerState::OFF))
        ESP_LOGI(::TAG, "Error Chaging Buzzer State to OFF");
    }

    /* Event Waiting */
    ESP_LOGI(::TAG, "Waiting for sensor events");
    xEventGroupWaitBits(
        event_group, DHT_READ_EVENT | GYML8511_READ_EVENT | CJMCU811_READ_EVENT,
        pdFALSE, pdFALSE, portMAX_DELAY);

    /* Mutex lock */
    if (auto p = xSemaphoreTake(mutex, pdMS_TO_TICKS(8000));
        p == pdPASS) /* Checking mutex take*/
    {
      ESP_LOGI(::TAG, "Taking Mutex");
      /* Checking Threshold for Temperature */
      if (measurement->temp > static_cast<double>(Thresholds::MAX_TEMP_ALLOWED)) {
        /* Temp Led Gpio Turn On */
        gpio_set_level(static_cast<gpio_num_t>(LedsGpio::TEMP_LED_PIN), 1);
        is_env_ok = false;
      } else {
        /* Temp Led Gpio Turn Off */
        gpio_set_level(static_cast<gpio_num_t>(LedsGpio::TEMP_LED_PIN), 0);
        is_env_ok = true;
      }
      /* Checking Threshold for Humidity */
      if (measurement->hm < static_cast<double>(Thresholds::MIN_HUMIDITY_ALLOWED)) {
        /* Humidity Led Gpio Turn On */
        gpio_set_level(static_cast<gpio_num_t>(LedsGpio::HUMIDITY_LED_PIN), 1);
        is_env_ok = false;
      } else {
        /* Humidity Led Gpio Turn Off */
        gpio_set_level(static_cast<gpio_num_t>(LedsGpio::HUMIDITY_LED_PIN), 0);
        is_env_ok = true;
      }
      /* Checking Threshold for Light Irradiance */
      if (measurement->uv * 1000 > static_cast<double>(Thresholds::MAX_UV_ALLOWED)) {
        /* Uv Led Gpio Turn On */
        gpio_set_level(static_cast<gpio_num_t>(LedsGpio::UV_LED_PIN), 1);
        is_env_ok = false;
      } else {
        /* Uv Led Gpio Turn Off */
        gpio_set_level(static_cast<gpio_num_t>(LedsGpio::UV_LED_PIN), 0);
        is_env_ok = true;
      }
      /* Checking Threshold for CO2 ppm */
      if (measurement->air > static_cast<double>(Thresholds::MAX_CO2_ALLOWED)) {
        /* Air Led Gpio Turn On */
        gpio_set_level(static_cast<gpio_num_t>(LedsGpio::AIR_LED_PIN), 1);
        is_env_ok = false;
      } else {
        /* Air Led Gpio Turn Off */
        gpio_set_level(static_cast<gpio_num_t>(LedsGpio::AIR_LED_PIN), 0);
        is_env_ok = true;
      }
      /* Mutex unlock */
      xSemaphoreGive(mutex);
    } else /* If mutex lock failes log error */
    {
      ESP_LOGI(TAG, "Failed to take mutex in Linux Task");
    }
  }
}
