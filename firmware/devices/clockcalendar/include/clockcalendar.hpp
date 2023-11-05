#pragma once

#include <chrono>
#include <string>

#include "calendar.hpp"
#include "clock.hpp"

namespace logs {
class ClockCalendar : public Calendar, public Clock {
 public:
  std::string GenerateTimestamp() const;
  uint64_t TotalTimeOn() const;
  int day() const { return day_; }
  int month() const { return month_; }
  int year() const { return year_; }
  int hour() const { return hour_; }
  int minute() const { return minute_; }
  int second() const { return second_; }

 private:
  time_t initial_time_{
      std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())};
};
}  // namespace logs
