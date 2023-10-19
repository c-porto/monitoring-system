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

void dht_init(idf::GPIONum pin)
{
    const idf::GPIO_Output dht_pin(pin);
    dht_pin.set_high();
}

dht_err check_bus_status(idf::GPIONum pin, size_t time_espected, idf::GPIOLevel level_expected)
{
    const idf::GPIOInput dht_pin(pin);
    int8_t time_count = 0;
    while (dht_pin.get_level() == level_expected)
    {
        if (time_count < time_espected)
        {
            ets_delay_us(1);
            time_count++;
        }
        else
        {
            return DHT11_TIMEOUT_ERR;
        }
    }
    dht_err status = (time_count != 0) ? DHT11_OK : DHT11_BUS_ERR;
    return status;
}

dht_rx_level rx_bit(idf::GPIONum pin_number, size_t time_espected, size_t min_time, idf::GPIOLevel level_expected)
{
    const idf::GPIOInput dht_pin(pin_number);
    size_t time_count = 0;
    dht_rx_level bit;
    while (dht_pin.get_level() == level_expected)
    {
        if (time_count < time_espected)
        {
            ets_delay_us(1);
            time_count++;
        }
        else
        {
            return DHT11_RX_ERR;
        }
    }
    bit = (time_count > min_time) ? DHT11_RX_HIGH : DHT11_RX_LOW;
    return bit;
}

dht_reading::dht_reading(double T, double Hm) : temp{T}, humidity{Hm}
{
}

Dht11::Dht11(uint32_t dht_pin) : pin(dht_pin)
{
    id{sensor::DHT11_ID};
    dht_init(this->pin);
}

dht_err Dht11::is_valid_data() const
{
    uint8_t tmp = raw_data_.dec_t_data + raw_data_.int_t_data + raw_data_.int_rh_data + raw_data_.dec_rh_data;
    dht_err frame_check = (tmp == raw_data_.check_sum) ? DHT11_OK : DHT11_CHECKSUM_ERR;
    return frame_check;
}

void Dht11::init_comm()
{
    try
    {

        const idf::GPIO_Output dht_pin_out(pin);
        dht_pin_out.set_low();
        vTaskDelay(pdMS_TO_TICKS(START_TIME_MS));
        dht_pin_out.set_high();
        ets_delay_us(PULL_UP_TIME_US);
        const idf::GPIOInput dht_pin_in(pin);
    }
    catch (idf::GPIOException &exc)
    {
        ESP_LOGI("DHT11", "GPIO ERROR");
    }
}

TH_SampleP Dht11::read()
{
    std::unique_ptr<TH_Sample> err_case{};
    dht_err timing_err;
    dht_rx_level bit;
    uint8_t buffer[5]{0};
    this->init_comm();

    try
    {
        timing_err = check_bus_status(this->pin, RESPONSE_TIME_US, idf::GPIOLevel::LOW);
        if (timing_err != DHT11_OK)
        {
            return err_case;
        }
        timing_err = check_bus_status(this->pin, RESPONSE_TIME_US, idf::GPIOLevel::HIGH);
        if (timing_err != DHT11_OK)
        {
            return err_case;
        }

        for (size_t i = 0; i < NUMBER_OF_BITS; ++i)
        {
            size_t idx = std::floor(i / 8U);
            timing_err = check_bus_status(this->pin, DATA_BEGIN_TIME_US, idf::GPIOLevel::HIGH);
            if (timing_err != DHT11_OK)
            {
                return err_case;
            }
            bit = rx_bit(this->pin, MAX_HIGH_BIT_TIME_US, MAX_LOW_BIT_TIME_US, idf::GPIOLevel::HIGH);
            buffer[idx] |= (bit << ((NUMBER_OF_BITS - i - 1U) % 8U));
        }
    }
    catch (const idf::GPIOException &exp)
    {
        ESP_LOGI("DHT11", "GPIO ERROR IN READ");
    }

    this->raw_data_.int_rh_data = buffer[0];
    this->raw_data_.dec_rh_data = buffer[1];
    this->raw_data_.int_t_data = buffer[2];
    this->raw_data_.dec_t_data = buffer[3];
    this->raw_data_.check_sum = buffer[4];

    if (this->is_valid_data() != DHT11_OK)
    {
        return err_case;
    }

    double temperature = static_cast<double>(this->raw_data_.int_t_data);
    double humidity = static_cast<double>(this->raw_data_.int_rh_data);

    dht_init(this->pin);

    return std::make_unique<TH_Sample>(temperature, humidity);
}
