#include "esp_log.h"
#include "freertos/portmacro.h"
#include <dht11.hpp>
#include <freertos/FreeRTOS.h>
#include <freertos/timers.h>
#include <gpio_cxx.hpp>
#include <memory>
#include <rom/ets_sys.h>
#include <sstream>
#include <sys/_stdint.h>

void dht_init(idf::GPIONum pin) {
  const idf::GPIO_Output dht_pin(pin);
  dht_pin.set_high();
}

dht_err check_bus_status(idf::GPIONum pin, uint8_t time_espected,
                         uint8_t min_time, idf::GPIOLevel level_expected) {
  const idf::GPIOInput dht_pin(pin);
  int8_t time_count = 0;
  while (dht_pin.get_level() == level_expected) {
    if (time_count < time_espected) {
      ets_delay_us(1);
      time_count++;
    } else {
      return DHT11_TIMEOUT_ERR;
    }
  }
  dht_err status = (time_count >= min_time) ? DHT11_OK : DHT11_BUS_ERR;
  return status;
}

dht_reading::dht_reading(double T, double Hm) : temp{T}, humidity{Hm} {}

Dht11::Dht11(uint32_t dht_pin) : pin(dht_pin) {}

dht_err Dht11::is_valid_data() const {
  uint8_t tmp = raw_data_.dec_t_data + raw_data_.int_t_data +
                raw_data_.int_rh_data + raw_data_.dec_rh_data;
  dht_err frame_check =
      (tmp == raw_data_.check_sum) ? DHT11_OK : DHT11_CHECKSUM_ERR;
  return frame_check;
}

void Dht11::init_comm() {
  try {

    const idf::GPIO_Output dht_pin_out(pin);
    dht_pin_out.set_low();
    ets_delay_us(START_TIME_US);
    dht_pin_out.set_high();
    ets_delay_us(PULL_UP_TIME_US);
    const idf::GPIOInput dht_pin_in(pin);

  } catch (idf::GPIOException &exc) {
    ESP_LOGI("DHT11", "GPIO ERROR");
  }
}

TH_SampleP Dht11::read() { return std::make_unique<TH_Sample>(0.0, 0.0); }
