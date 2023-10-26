#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/portmacro.h"
#include "freertos/task.h"

#include "../../utils/include/utils.hpp"
#include "esp_log.h"
#include <gpio_cxx.hpp>
#include <memory>
#include <sys/_stdint.h>

#define NUMBER_OF_BITS 40U
#define MAX_TEMP 50U
#define MIN_TEMP 0U
#define MAX_RH 90U
#define MIN_RH 20U
#define START_TIME_MS 22UL
#define PULL_UP_TIME_US 35U
#define RESPONSE_TIME_US 80U
#define DATA_BEGIN_TIME_US 50U
#define MAX_LOW_BIT_TIME_US 28U
#define MAX_HIGH_BIT_TIME_US 70U

typedef enum
{
    DHT11_RX_LOW,
    DHT11_RX_HIGH,
    DHT11_RX_ERR,
} dht_rx_level;

typedef enum
{
    DHT11_OK,
    DHT11_TIMEOUT_ERR,
    DHT11_CHECKSUM_ERR,
    DHT11_BUS_ERR,
} dht_err;

void dht_init(idf::GPIONum pin);
dht_err check_bus_status(idf::GPIONum pin_number, std::size_t time_espected, idf::GPIOLevel level_expected);
dht_rx_level rx_bit(idf::GPIONum pin_number, std::size_t time_espected, std::size_t min_time, idf::GPIOLevel level_expected);

struct dht_data
{
    uint8_t int_rh_data{0};
    uint8_t dec_rh_data{0};
    uint8_t int_t_data{0};
    uint8_t dec_t_data{0};
    uint8_t check_sum{0};
};

struct dht_reading
{
    double temp{};
    double humidity{};
    dht_reading(double T, double Hm);
};

class Dht11 : public sensor::Sensor
{
  public:
    Dht11(uint32_t dht_pin);
    void read(sensor::MeasureP ms) override;
    void init() override;

  private:
    dht_data raw_data_;
    idf::GPIONum pin_;
    dht_err is_valid_data() const;
    void init_comm();
};
