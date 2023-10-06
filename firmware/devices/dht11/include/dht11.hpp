#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/portmacro.h"
#include "freertos/task.h"

#include "esp_log.h"
#include <memory>
#include <sys/_stdint.h>
#include <gpio_cxx.hpp>

#define MAX_TEMP 50
#define MIN_TEMP 0
#define MAX_RH 90
#define MIN_RH 20
#define START_TIME_US 22000
#define PULL_UP_TIME_US 30 
#define RESPONSE_TIME_US 80
#define DATA_BEGIN_TIME_US 50
#define MAX_LOW_BIT_TIME_US 28
#define MAX_HIGH_BIT_TIME_US 70

typedef enum {
    DHT11_OK,
    DHT11_TIMEOUT_ERR,
    DHT11_CHECKSUM_ERR,
    DHT11_BUS_ERR,
} dht_err;


void dht_init(idf::GPIONum pin);
dht_err check_bus_status(idf::GPIONum pin_number,uint8_t time_espected, uint8_t min_time, idf::GPIOLevel level_expected);

struct dht_data {
  uint8_t int_rh_data{0};
  uint8_t dec_rh_data{0};
  uint8_t int_t_data{0};
  uint8_t dec_t_data{0};
  uint8_t check_sum{0};
};

struct dht_reading {
    double temp{};
    double humidity{};
    dht_reading(double T, double Hm);
};

using TH_SampleP = std::unique_ptr<dht_reading>;
using TH_Sample = dht_reading;

class Dht11 {
    dht_data raw_data_;
    idf::GPIONum pin;
    dht_err is_valid_data() const;
    void init_comm();
    
public:
    TH_SampleP read();
    Dht11(uint32_t dht_pin);
};
