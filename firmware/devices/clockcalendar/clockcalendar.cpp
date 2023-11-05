#include "include/clockcalendar.hpp"

#include <chrono>

namespace logs {
std::string ClockCalendar::GenerateTimestamp() const {
  return "[" + std::to_string(hour_) + ":" + std::to_string(minute_) + ":" +
         std::to_string(second_) + " " + std::to_string(day_) + "/" +
         std::to_string(month_) + "/" + std::to_string(year_) + "]";
}
uint64_t ClockCalendar::TotalTimeOn() const {
  auto tmp =
      std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
  auto total_time = tmp - initial_time_;
  return (uint32_t)total_time;
}
}  // namespace logs
