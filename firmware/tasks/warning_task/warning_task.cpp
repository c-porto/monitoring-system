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
const char *TAG = "WARNING TAG";
/* Bool Variable to Represent Inadequate Enviroment */
bool is_env_ok{true};
}  // namespace

/* Warning Task Responsible for Activating the Buzzer and LEDS */
void vTaskWarning(void *params) {
  /* Buzzer object */
  Buzzer bz{};

  /* Getting global measurement variable*/
  auto ms = static_cast<sensor::MeasureP>(params);

  /* Init Gpio's */
  if (gpio_led_init()) {
    ESP_LOGI(::TAG, "Error Configuring GPIO");
  }
  /* Init Buzzer PWM */
  if (bz.buzzer_init()) {
    ESP_LOGI(::TAG, "Error Configuring Buzzer");
  }

  /* Main loop */
  while (true) {
    /* Event Waiting */
    xEventGroupWaitBits(
        event_group, DHT_READ_EVENT | GYML8511_READ_EVENT | CJMCU811_READ_EVENT,
        pdTRUE, pdFALSE, portMAX_DELAY);

    if (!is_env_ok) {
      /* Turn Buzzer On*/
      if (bz.buzzer_toggle(BuzzerState::ON))
        ESP_LOGI(::TAG, "Error Chaging Buzzer State to ON");
    } else {
      /* Turn Buzzer Off*/
      if (bz.buzzer_toggle(BuzzerState::OFF))
        ESP_LOGI(::TAG, "Error Chaging Buzzer State to OFF");
    }
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
        /* Checking Threshold for Temperature */
        if (sample.temp > (double)Thresholds::MAX_TEMP_ALLOWED) {
          /* Temp Led Gpio Turn On */
          gpio_set_level((gpio_num_t)LedsGpio::TEMP_LED_PIN, 1);
          is_env_ok = false;
        } else {
          /* Temp Led Gpio Turn Off */
          gpio_set_level((gpio_num_t)LedsGpio::TEMP_LED_PIN, 0);
          is_env_ok = true;
        }
        /* Checking Threshold for Humidity */
        if (sample.hm < (double)Thresholds::MIN_HUMIDITY_ALLOWED) {
          /* Humidity Led Gpio Turn On */
          gpio_set_level((gpio_num_t)LedsGpio::HUMIDITY_LED_PIN, 1);
          is_env_ok = false;
        } else {
          /* Humidity Led Gpio Turn Off */
          gpio_set_level((gpio_num_t)LedsGpio::HUMIDITY_LED_PIN, 0);
          is_env_ok = true;
        }
        /* Checking Threshold for Light Irradiance */
        if (sample.uv > (double)Thresholds::MAX_UV_ALLOWED) {
          /* Uv Led Gpio Turn On */
          gpio_set_level((gpio_num_t)LedsGpio::UV_LED_PIN, 1);
          is_env_ok = false;
        } else {
          /* Uv Led Gpio Turn Off */
          gpio_set_level((gpio_num_t)LedsGpio::UV_LED_PIN, 0);
          is_env_ok = true;
        }
        /* Checking Threshold for CO2 ppm */
        if (sample.air > (double)Thresholds::MAX_CO2_ALLOWED) {
          /* Air Led Gpio Turn On */
          gpio_set_level((gpio_num_t)LedsGpio::AIR_LED_PIN, 1);
          is_env_ok = false;
        } else {
          /* Air Led Gpio Turn Off */
          gpio_set_level((gpio_num_t)LedsGpio::AIR_LED_PIN, 0);
          is_env_ok = true;
        }
      } else /* If there is a error in the measure */
      {
        /* Logging error */
        ESP_LOGI(TAG, "Error in measuring some data");
        /* Checking the other valid measurements */
        switch (sample.last_id) {
          case sensor::DHT11_ID:
            /* Checking Threshold for Light Irradiance */
            if (sample.uv > (double)Thresholds::MAX_UV_ALLOWED) {
              /* Uv Led Gpio Turn On */
              gpio_set_level((gpio_num_t)LedsGpio::UV_LED_PIN, 1);
              is_env_ok = false;
            } else {
              /* Uv Led Gpio Turn Off */
              gpio_set_level((gpio_num_t)LedsGpio::UV_LED_PIN, 0);
              is_env_ok = true;
            }
            /* Checking Threshold for CO2 ppm */
            if (sample.air > (double)Thresholds::MAX_CO2_ALLOWED) {
              /* Air Led Gpio Turn On */
              gpio_set_level((gpio_num_t)LedsGpio::AIR_LED_PIN, 1);
              is_env_ok = false;
            } else {
              /* Air Led Gpio Turn Off */
              gpio_set_level((gpio_num_t)LedsGpio::AIR_LED_PIN, 0);
              is_env_ok = true;
            }
            break;
          case sensor::CJMCU811_ID:
            /* Checking Threshold for Temperature */
            if (sample.temp > (double)Thresholds::MAX_TEMP_ALLOWED) {
              /* Temp Led Gpio Turn On */
              gpio_set_level((gpio_num_t)LedsGpio::TEMP_LED_PIN, 1);
              is_env_ok = false;
            } else {
              /* Temp Led Gpio Turn Off */
              gpio_set_level((gpio_num_t)LedsGpio::TEMP_LED_PIN, 0);
              is_env_ok = true;
            }
            /* Checking Threshold for Humidity */
            if (sample.hm < (double)Thresholds::MIN_HUMIDITY_ALLOWED) {
              /* Humidity Led Gpio Turn On */
              gpio_set_level((gpio_num_t)LedsGpio::HUMIDITY_LED_PIN, 1);
              is_env_ok = false;
            } else {
              /* Humidity Led Gpio Turn Off */
              gpio_set_level((gpio_num_t)LedsGpio::HUMIDITY_LED_PIN, 0);
              is_env_ok = true;
            }
            /* Checking Threshold for Light Irradiance */
            if (sample.uv > (double)Thresholds::MAX_UV_ALLOWED) {
              /* Uv Led Gpio Turn On */
              gpio_set_level((gpio_num_t)LedsGpio::UV_LED_PIN, 1);
              is_env_ok = false;
            } else {
              /* Uv Led Gpio Turn Off */
              gpio_set_level((gpio_num_t)LedsGpio::UV_LED_PIN, 0);
              is_env_ok = true;
            }
            break;
          case sensor::GYML8511_ID:
            /* Checking Threshold for Temperature */
            if (sample.temp > (double)Thresholds::MAX_TEMP_ALLOWED) {
              /* Temp Led Gpio Turn On */
              gpio_set_level((gpio_num_t)LedsGpio::TEMP_LED_PIN, 1);
              is_env_ok = false;
            } else {
              /* Temp Led Gpio Turn Off */
              gpio_set_level((gpio_num_t)LedsGpio::TEMP_LED_PIN, 0);
              is_env_ok = true;
            }
            /* Checking Threshold for Humidity */
            if (sample.hm < (double)Thresholds::MIN_HUMIDITY_ALLOWED) {
              /* Humidity Led Gpio Turn On */
              gpio_set_level((gpio_num_t)LedsGpio::HUMIDITY_LED_PIN, 1);
              is_env_ok = false;
            } else {
              /* Humidity Led Gpio Turn Off */
              gpio_set_level((gpio_num_t)LedsGpio::HUMIDITY_LED_PIN, 0);
              is_env_ok = true;
            }
            /* Checking Threshold for CO2 ppm */
            if (sample.air > (double)Thresholds::MAX_CO2_ALLOWED) {
              /* Air Led Gpio Turn On */
              gpio_set_level((gpio_num_t)LedsGpio::AIR_LED_PIN, 1);
              is_env_ok = false;
            } else {
              /* Air Led Gpio Turn Off */
              gpio_set_level((gpio_num_t)LedsGpio::AIR_LED_PIN, 0);
              is_env_ok = true;
            }
            break;
        }
      }
    } else /* If mutex lock failes log error */
    {
      ESP_LOGI(TAG, "Failed to take mutex in Linux Task");
    }
  }
}
