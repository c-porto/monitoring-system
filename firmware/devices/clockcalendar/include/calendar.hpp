#pragma once
namespace logs {
class Calendar {
public:
  Calendar() : month_{0}, day_{0}, year_{0} {}
  void CalendarNow();

protected:
  int month_;
  int day_;
  int year_;
};
} // namespace logs
