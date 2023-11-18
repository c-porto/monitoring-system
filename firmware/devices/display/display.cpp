#include "include/display.hpp"
#include "sdkconfig.h"
#include "ssd1306.h"
#include <cstddef>
#include <cstring>
#include <sstream>

namespace sensor {
void Display::init() {
  if (!initialized_) {
    i2c_master_init(&ssd_, CONFIG_SDA_GPIO, CONFIG_SCL_GPIO, CONFIG_RESET_GPIO);
    ssd1306_init(&ssd_, 128, 64);
    ssd1306_clear_screen(&ssd_, false);
    Display::initilize();
  }
}
void Display::display_str(char *text, int line, int strlen) {
  ssd1306_display_text(&ssd_, line, text, strlen, false);
}
Display &operator<<(Display &ds, sensor::MeasureP ms) {
  if (!Display::is_initialized()) {
    return ds;
  }
  std::stringstream temp;
  std::stringstream humidity;
  std::stringstream co2;
  std::stringstream uv;

  temp << "Temp: " << ms->temp << " C";
  humidity << "Hm: " << ms->hm << " %";
  co2 << "CO2: " << ms->air << " ppm";
  uv << "Uv: " << ms->uv;

  ds.display_str(const_cast<char *>(temp.str().c_str()), 0,
                 temp.str().length());
  ds.display_str(const_cast<char *>(humidity.str().c_str()), 1,
                 humidity.str().length());
  ds.display_str(const_cast<char *>(co2.str().c_str()), 2, co2.str().length());
  ds.display_str(const_cast<char *>(uv.str().c_str()), 3, uv.str().length());
  ds.display_str("                ", 4, 16);
  ds.display_str("                ", 5, 16);
  ds.display_str("                ", 6, 16);
  ds.display_str("                ", 7, 16);

  return ds;
}
} // namespace sensor
