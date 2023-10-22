#include "include/utils.hpp"

std::ostream &operator<<(std::ostream &os, sensor::Measure const &ms) {
  os << "Temperature Reading: " << ms.temp << std::endl;
  os << "Humidity Reading: " << ms.hm << std::endl;
  os << "Uv Radiance Reading: " << ms.uv << std::endl;
  os << "Air Quality Reading: " << ms.air << std::endl;
  return os;
}

sensor::SensorAPI::SensorAPI(sensor::SensorP sensor, sensor::MeasureP data)
    : sensor_{sensor}, data_{data} {};

void sensor::SensorAPI::update_data() { sensor_->read(data_); }

std::ostream &operator<<(std::ostream &os, logs::LogData const &log) {
  os << "Sensor ID: " << log.id << std::endl;
//os << "Event Time" << log.date << std::endl;
  os << log.measure;
  return os;
}
