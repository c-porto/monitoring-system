#include <freertos/FreeRTOS.h>
#include <freertos/timers.h>
#include <rom/ets_sys.h>
#include <sys/_stdint.h>

#include <cmath>
#include <dht11.hpp>
#include <gpio_cxx.hpp>
#include <memory>
#include <sstream>

#include "../utils/include/utils.hpp"
#include "dht11_driver.hpp"
#include "driver/gpio.h"
#include "esp_log.h"
#include "freertos/portmacro.h"
#include "freertos/projdefs.h"
#include "hal/gpio_types.h"

namespace {
const char *TAG = "DHT11 Driver";
}

namespace sensor {
void Dht11::init() { vTaskDelay(pdMS_TO_TICKS(1000)); }

Dht11::Dht11(uint32_t dht_pin) : pin_(static_cast<gpio_num_t>(dht_pin)) {
  this->id = sensor::DHT11_ID;
  type_ = __dht11::external_lib::DHT11;
  this->init();
}

void Dht11::read(sensor::MeasureP ms) {

  __dht11::external_lib::dht_result_t res =
      __dht11::external_lib::read_dht_sensor_data(pin_, type_, &data_);

  if (res < 0) {
    ms->err = true;
    ms->last_id = this->id;
    return;
  }

  ms->err = false;
  ms->temp = data_.temperature;
  ms->hm = data_.humidity;
  ms->last_id = this->id;
  ms->date->ClockNow();
  ms->date->CalendarNow();

  ESP_LOGI(TAG, "Temperature reading is %f Celsius", ms->temp);
  ESP_LOGI(TAG, "Humidity reading is %f", ms->hm);
}
} // namespace sensor
