#pragma once

namespace logs {
class Clock {
 public:
  Clock() : hour_{0}, minute_{0}, second_{0} {}
  void ClockNow();

 protected:
  int hour_, minute_, second_;
};
}  // namespace logs
