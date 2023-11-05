#include "include/calendar.hpp"

#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

namespace logs {
void Calendar::CalendarNow() {
  auto now = std::chrono::system_clock::now();
  auto timer = std::chrono::system_clock::to_time_t(now);
  std::tm bt = *std::localtime(&timer);
  std::stringstream month;
  std::stringstream day;
  std::stringstream year;

  month << std::put_time(&bt, "%m");
  day << std::put_time(&bt, "%d");
  year << std::put_time(&bt, "%Y");

  month_ = std::stoi(month.str());
  day_ = std::stoi(day.str());
  year_ = std::stoi(year.str());
}
}  // namespace logs
