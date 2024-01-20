#pragma once
#include "../../../components/ssd1306/ssd1306.h"
#include "utils.hpp"
#include <cstddef>

namespace sensor
{
namespace __ssd1306
{
using display = SSD1306_t;
}
class Measure;
using MeasureP = Measure *;
class Display
{
  public:
    Display() = default;
    void init();
    void display_str(char *, int, int);
    static void initilize()
    {
        initialized_ = true;
    }
    Display &operator<<(sensor::MeasureP ms);
    [[nodiscard]] static bool is_initialized() noexcept
    {
        return initialized_;
    }

  private:
    inline static bool initialized_{false};
    __ssd1306::display ssd_;
};
} // namespace sensor
