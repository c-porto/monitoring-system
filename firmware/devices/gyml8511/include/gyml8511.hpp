#pragma once
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"
#include "esp_adc/adc_continuous.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_err.h"
#include <cstddef>
#include <utils.hpp>

namespace sensor {
namespace __gyml8511 {
struct ConversionValues {
  double min_input_voltage{0.99};
  double max_input_voltage{2.8};
  double min_output_uv{0.0};
  double max_output_uv{15.0};
};
adc_oneshot_unit_handle_t init_gyml8511_adc();
adc_cali_handle_t init_gyml8511_adc_cali();
} // namespace __gyml8511

class Gyml8511 final : public Sensor {
public:
  Gyml8511() : uv_intensity_{0.0}, uv_raw_{0.0} {
    this->id = GYML8511_ID;
    this->init();
  }
  void init() override;
  void read(MeasureP ms) override;

private:
  inline void convert_uvintensity_from_raw();
  const std::size_t gyml8511_pin_{33};
  double uv_intensity_;
  double uv_raw_;
  adc_oneshot_unit_handle_t adc_handle_;
  adc_cali_handle_t adc_cali_;
  __gyml8511::ConversionValues cv_{};
};

} // namespace sensor
