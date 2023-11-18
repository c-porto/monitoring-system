#pragma once

#include <chrono>
#include <ctime>
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
  void StartTimeTracking(std::time_t now) { initial_time_ = now; }

private:
  std::time_t initial_time_;
};
} // namespace logs
