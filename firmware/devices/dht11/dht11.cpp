#include "../utils/include/utils.hpp"
#include "esp_log.h"
#include "freertos/portmacro.h"
#include "freertos/projdefs.h"
#include <cmath>
#include <dht11.hpp>
#include <freertos/FreeRTOS.h>
#include <freertos/timers.h>
#include <gpio_cxx.hpp>
#include <memory>
#include <rom/ets_sys.h>
#include <sstream>
#include <sys/_stdint.h>

#define DEBUG_SENSOR

namespace {
const char *TAG = "DHT11 Driver";
}

namespace sensor {
__dht11::dht_err check_bus_status(idf::GPIONum pin, std::size_t time_espected,
                                  idf::GPIOLevel level_expected) {
  const idf::GPIOInput dht_pin(pin);
  int8_t time_count = 0;
  while (dht_pin.get_level() == level_expected) {
    if (time_count < time_espected) {
      ets_delay_us(1);
      time_count++;
    } else {
      return __dht11::DHT11_TIMEOUT_ERR;
    }
  }
  __dht11::dht_err status =
      (time_count != 0) ? __dht11::DHT11_OK : __dht11::DHT11_BUS_ERR;
  return status;
}

__dht11::dht_rx_level rx_bit(idf::GPIONum pin_number, std::size_t time_espected,
                             std::size_t min_time,
                             idf::GPIOLevel level_expected) {
  const idf::GPIOInput dht_pin(pin_number);
  std::size_t time_count = 0;
  __dht11::dht_rx_level bit;
  while (dht_pin.get_level() == level_expected) {
    if (time_count < time_espected) {
      ets_delay_us(1);
      time_count++;
    } else {
      return __dht11::DHT11_RX_ERR;
    }
  }
  bit =
      (time_count > min_time) ? __dht11::DHT11_RX_HIGH : __dht11::DHT11_RX_LOW;
  return bit;
}

__dht11::dht_reading::dht_reading(double T, double Hm)
    : temp{T}, humidity{Hm} {}

void Dht11::init() {
  const idf::GPIO_Output dht_pin(pin_);
  dht_pin.set_high();
}

Dht11::Dht11(uint32_t dht_pin) : pin_(dht_pin) {
  id = sensor::DHT11_ID;
  this->init();
}

__dht11::dht_err Dht11::is_valid_data() const {
  uint8_t tmp = raw_data_.dec_t_data + raw_data_.int_t_data +
                raw_data_.int_rh_data + raw_data_.dec_rh_data;
  __dht11::dht_err frame_check = (tmp == raw_data_.check_sum)
                                     ? __dht11::DHT11_OK
                                     : __dht11::DHT11_CHECKSUM_ERR;
  return frame_check;
}

void Dht11::init_comm() {
  try {
    const idf::GPIO_Output dht_pin_out(pin_);
    dht_pin_out.set_low();
    vTaskDelay(pdMS_TO_TICKS(START_TIME_MS));
    dht_pin_out.set_high();
    ets_delay_us(PULL_UP_TIME_US);
    const idf::GPIOInput dht_pin_in(pin_);
  } catch (idf::GPIOException &exc) {
    ESP_LOGI(TAG, "GPIO ERROR");
  }
}

void Dht11::read(sensor::MeasureP ms) {
  __dht11::dht_err timing_err;
  __dht11::dht_rx_level bit;
  uint8_t buffer[5]{0};
  this->init_comm();

  try {
    timing_err =
        check_bus_status(this->pin_, RESPONSE_TIME_US, idf::GPIOLevel::LOW);
    if (timing_err != __dht11::DHT11_OK) {
      ms->err = true;
      return;
    }
    timing_err =
        check_bus_status(this->pin_, RESPONSE_TIME_US, idf::GPIOLevel::HIGH);
    if (timing_err != __dht11::DHT11_OK) {
      ms->err = true;
      return;
    }
    std::size_t idx{0};
    for (std::size_t i{0}; i < NUMBER_OF_BITS; ++i) {
      idx = std::floor(i / 8U);
      timing_err = check_bus_status(this->pin_, DATA_BEGIN_TIME_US,
                                    idf::GPIOLevel::HIGH);
      if (timing_err != __dht11::DHT11_OK) {
        ms->err = true;
        return;
      }
      bit = rx_bit(this->pin_, MAX_HIGH_BIT_TIME_US, MAX_LOW_BIT_TIME_US,
                   idf::GPIOLevel::HIGH);
      buffer[idx] |= (bit << ((NUMBER_OF_BITS - i - 1U) % 8U));
    }
  } catch (const idf::GPIOException &exp) {
    ESP_LOGI(TAG, "GPIO ERROR IN READ");
    ms->err = true;
  }

  this->raw_data_.int_rh_data = buffer[0];
  this->raw_data_.dec_rh_data = buffer[1];
  this->raw_data_.int_t_data = buffer[2];
  this->raw_data_.dec_t_data = buffer[3];
  this->raw_data_.check_sum = buffer[4];

  if (this->is_valid_data() != __dht11::DHT11_OK) {
    ms->err = true;
    ESP_LOGI(TAG, "Failed in check sum");
    return;
  }

  auto temperature = static_cast<double>(this->raw_data_.int_t_data);
  auto humidity = static_cast<double>(this->raw_data_.int_rh_data);

  this->init();
  ms->err = false;
  ms->temp = temperature;
  ms->hm = humidity;
  ms->last_id = this->id;

#ifdef DEBUG_SENSOR

  ESP_LOGI(TAG, "Temperature reading is %f Celsius", temperature);
  ESP_LOGI(TAG, "Humidity reading is %f", humidity);

#endif
}
} // namespace sensor
