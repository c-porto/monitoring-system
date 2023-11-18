#include "include/gyml8511.hpp"

#include <cmath>
#include <cstddef>
#include <stdexcept>

#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_err.h"
#include "esp_log.h"
#include "hal/adc_types.h"
#include "utils.hpp"

#define DEBUG_SENSOR

namespace {
const char *TAG = "Gyml8511";
}

namespace sensor {
adc_oneshot_unit_handle_t __gyml8511::init_gyml8511_adc() {
  adc_oneshot_unit_handle_t adc1_handle;
  adc_oneshot_unit_init_cfg_t init_config1 = {
      .unit_id = ADC_UNIT_1,
      .ulp_mode = ADC_ULP_MODE_DISABLE,
  };
  if (adc_oneshot_new_unit(&init_config1, &adc1_handle) != ESP_OK) {
    ESP_LOGI(TAG, "Error in creating ADC");
    throw std::runtime_error("Error in creating ADC");
  }
  adc_oneshot_chan_cfg_t config = {
      .atten = ADC_ATTEN_DB_11,
      .bitwidth = ADC_BITWIDTH_DEFAULT,
  };
  if (adc_oneshot_config_channel(adc1_handle, ADC_CHANNEL_5, &config) !=
      ESP_OK) {
    ESP_LOGI(TAG, "Error in configuring ADC");
    throw std::runtime_error("Error in configuring ADC");
  }
  return adc1_handle;
}
adc_cali_handle_t __gyml8511::init_gyml8511_adc_cali() {
  adc_cali_handle_t cali_handle;
  adc_cali_line_fitting_config_t cali_config = {
      .unit_id = ADC_UNIT_1,
      .atten = ADC_ATTEN_DB_11,
      .bitwidth = ADC_BITWIDTH_DEFAULT,
  };
  if (adc_cali_create_scheme_line_fitting(&cali_config, &cali_handle) !=
      ESP_OK) {
    ESP_LOGI(TAG, "Error Creating cali scheme for adc");
    throw std::runtime_error("Error Creating cali scheme for adc");
  }
  return cali_handle;
}
inline void Gyml8511::convert_uvintensity_from_raw() {
  uv_intensity_ = ((uv_raw_ / 1000) - cv_.min_input_voltage) *
                      (cv_.max_output_uv - cv_.min_output_uv) /
                      (cv_.max_input_voltage - cv_.min_input_voltage) +
                  cv_.min_output_uv;
}

void Gyml8511::init() {
  try {
    adc_handle_ = __gyml8511::init_gyml8511_adc();
    adc_cali_ = __gyml8511::init_gyml8511_adc_cali();
  } catch (std::exception &ex) {
    ESP_LOGI(::TAG, "Exception ocurred in gyml init: %s", ex.what());
    throw std::runtime_error("Could not initialize gyml");
  }
}

void Gyml8511::read(MeasureP ms) {
  int sample_raw{0};
  int v{0};

  if (adc_oneshot_read(adc_handle_, ADC_CHANNEL_5, &sample_raw) != ESP_OK) {
    ESP_LOGI(TAG, "Failed to Read ADC on reading");
    ms->err = true;
    return;
  }

  if (adc_cali_raw_to_voltage(adc_cali_, sample_raw, &v) != ESP_OK) {
    ESP_LOGI(TAG, "Error on adc raw to voltage conversion");
    ms->err = true;
    return;
  }

  uv_raw_ = v;

  convert_uvintensity_from_raw();

  ms->uv = uv_intensity_;
  ms->err = false;
  ms->last_id = this->id;
  ms->date->ClockNow();
  ms->date->CalendarNow();

#ifdef DEBUG_SENSOR

  ESP_LOGI(TAG, "Sensor reading is %f mW/cm^2", ms->uv);

#endif
}

} // namespace sensor
