#include <sstream>
#include <utils.hpp>

class Display {
public:
  void init() const;
  void display_str(const char *) const;
  Display &operator<<(sensor::MeasureP ms) {
    /* Print on line 1*/
    ss_ << "Temperatura:" << ms->temp << "C";
    /* Print on line 1*/
    ss_ << "Humidade:" << ms->temp << "UNIT";
    /* Print on line 2*/
    ss_ << "IrUV:" << ms->uv << "UNIT";
    /* Print on line 3*/
    ss_ << "AirQuality:" << ms->air << "UNIT";
    return *this;
  }

private:
  std::stringstream ss_;
};
