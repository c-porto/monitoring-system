#include <utils.hpp>

class Display {
public:
  Display() = default;
  void init() const;
  void display_str(const char *) const;
  friend Display &operator<<(Display &ds, sensor::MeasureP ms);
};
