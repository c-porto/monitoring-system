#include "include/clockcalendar.hpp"

namespace logs {
std::string ClockCalendar::GenerateTimestamp() {
  return "[" + std::to_string(hour_) + ":" + std::to_string(minute_) + ":" +
         std::to_string(second_) + " " + std::to_string(day_) + "/" +
         std::to_string(month_) + "/" + std::to_string(year_) + "]";
}
} // namespace logs
