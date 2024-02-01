#include "utils.hpp"
#include "esp_log.h"

sensor::Measure::Measure(light_t l, temperature_t t, air_quality_t a,
			 humidity_t h)
	: temp{ t }
	, hm{ h }
	, uv{ l }
	, air{ a }
{
}

sensor::Measure::Measure()
	: temp{ 0.0 }
	, hm{ 0.0 }
	, uv{ 0.0 }
	, air{ 0.0 }
{
}

namespace logs
{

Logger::Logger(const char *tag)
{
	tag_ = tag;
}

Logger const &Logger::operator<<(Result r) const
{
	switch (r) {
	default:
	case Result::Ok:
		ESP_LOGI(tag_.c_str(), "Http GET returned Ok");
		break;
	case Result::Err:
		ESP_LOGI(tag_.c_str(), "Http GET returned Err");
		break;
	}

	return *this;
}

Logger const &Logger::operator<<(sensor::MeasureP sample) const
{
	ESP_LOGI(tag_.c_str(), "Temperature: %f", sample->temp);
	ESP_LOGI(tag_.c_str(), "Humidity: %f", sample->hm);
	ESP_LOGI(tag_.c_str(), "Air quality: %f", sample->air);
	ESP_LOGI(tag_.c_str(), "Uv Irradiance: %f", sample->uv);
	return *this;
}

Logger const &Logger::operator<<(const char *str) const
{
	ESP_LOGI(tag_.c_str(), "%s", str);
	return *this;
}

} // namespace logs
