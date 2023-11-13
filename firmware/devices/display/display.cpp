#include "include/display.hpp"

namespace sensor {
void init() {}
void display_str(const char *) {}
Display &operator<<(Display &ds, sensor::MeasureP ms) { return ds; }
} // namespace sensor
