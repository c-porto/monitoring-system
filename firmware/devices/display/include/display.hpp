#pragma once

namespace sensor {
class Measure;
using MeasureP = Measure *;
class Display {
public:
  Display() = default;
  void init();
  void display_str(const char *) const;
  friend Display &operator<<(Display &ds, sensor::MeasureP ms);
  [[nodiscard]] static bool is_initialized() noexcept { return initialized_; }

private:
  inline static bool initialized_{false};
};
} // namespace sensor
