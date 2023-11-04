#pragma once

#include "calendar.hpp"
#include "clock.hpp"
#include <string>

namespace logs {
class ClockCalendar : public Calendar, public Clock {
public:
  std::string GenerateTimestamp();
  int day() const { return day_; }
  int month() const { return month_; }
  int year() const { return year_; }
  int hour() const { return hour_; }
  int minute() const { return minute_; }
  int second() const { return second_; }
};
} // namespace logs
