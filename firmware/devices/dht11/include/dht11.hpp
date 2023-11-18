#pragma once

#include <sys/_stdint.h>

#include <gpio_cxx.hpp>
#include <memory>

#include "../../utils/include/utils.hpp"
#include "dht11_driver.hpp"
#include "driver/gpio.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/portmacro.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include "hal/gpio_types.h"

namespace sensor {

using reading = __dht11::external_lib::dht_reading;
using dht_type = __dht11::external_lib::dht_type_t;

class Dht11 final : public Sensor {
public:
  Dht11(uint32_t dht_pin);
  void read(sensor::MeasureP ms) override;
  void init() override;

private:
  gpio_num_t pin_;
  reading data_;
  dht_type type_;
};

} // namespace sensor
