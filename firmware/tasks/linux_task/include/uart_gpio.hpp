#include <sys/_stdint.h>

#include "driver/gpio.h"
#include "driver/ledc.h"
#include "driver/uart.h"
#include "hal/gpio_types.h"
#include "hal/ledc_types.h"

enum class BuzzerState {
  ON,
  OFF,
};

enum class LedsGpio {
  TEMP_LED_PIN = GPIO_NUM_16,
  HUMIDITY_LED_PIN = GPIO_NUM_17,
  AIR_LED_PIN = GPIO_NUM_12,
  UV_LED_PIN = GPIO_NUM_14,
};

class Buzzer final {
 public:
  auto get_channel() const { return channel_; }
  auto get_timer() const { return timer_; }
  auto get_freq() const { return freq_; }
  auto get_gpio_num() const { return gpio_; }
  auto get_speed_mode() const { return speed_mode_; }
  bool buzzer_toggle(BuzzerState);
  bool buzzer_init();
  [[nodiscard]] auto is_buzzer_on() const { return is_on_; }
  Buzzer() {
    channel_ = LEDC_CHANNEL_0;
    timer_ = LEDC_TIMER_0;
    freq_ = 4186;
  }
  void set_buzzer_state(BuzzerState on_or_off) {
    switch (on_or_off) {
      case BuzzerState::ON:
        is_on_ = true;
        break;
      case BuzzerState::OFF:
        is_on_ = false;
        break;
    }
  }

 private:
  ledc_channel_t channel_;
  ledc_timer_t timer_;
  uint32_t freq_;
  gpio_num_t gpio_{GPIO_NUM_27};
  ledc_mode_t speed_mode_{LEDC_LOW_SPEED_MODE};
  bool is_on_{false};
};

#define GPIO_OUTPUT_SEL(PIN) (1ULL << (PIN))

bool gpio_led_init();
bool host_uart_init();
