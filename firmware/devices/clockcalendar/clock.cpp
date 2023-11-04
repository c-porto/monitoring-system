#include "include/clock.hpp"
#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

namespace logs {
void Clock::ClockNow(){  

  auto now = std::chrono::system_clock::now();
  auto timer = std::chrono::system_clock::to_time_t(now);
  std::tm bt = *std::localtime(&timer);
  std::stringstream hour;
  std::stringstream minute;
  std::stringstream second;

  hour << std::put_time(&bt, "%H");
  minute << std::put_time(&bt, "%M");
  second << std::put_time(&bt, "%S");

  hour_ = std::stoi(hour.str());
  minute_ = std::stoi(minute.str());
  second_ = std::stoi(second.str());
}

}  // namespace logs
