#include "include/uart_gpio.hpp"

#include "driver/gpio.h"
#include "driver/ledc.h"
#include "driver/uart.h"
#include "esp_log.h"
#include "hal/gpio_types.h"
#include "hal/ledc_types.h"
#include "hal/uart_types.h"
#include "soc/clk_tree_defs.h"

bool gpio_led_init() {
  bool err{false};
  gpio_config_t io_conf = {};

  io_conf.intr_type = GPIO_INTR_DISABLE;
  io_conf.mode = GPIO_MODE_OUTPUT;
  io_conf.pin_bit_mask =
      (GPIO_OUTPUT_SEL((uint64_t)LedsGpio::TEMP_LED_PIN) |
       GPIO_OUTPUT_SEL((uint64_t)LedsGpio::HUMIDITY_LED_PIN) |
       GPIO_OUTPUT_SEL((uint64_t)LedsGpio::UV_LED_PIN) |
       GPIO_OUTPUT_SEL((uint64_t)LedsGpio::AIR_LED_PIN));
  io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
  io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
  if (gpio_config(&io_conf) != ESP_OK) {
    ESP_LOGI("GPIO", "Error in Leds Gpio Config");
    err = true;
  }
  return err;
}

bool Buzzer::buzzer_init() {
  bool err{false};
  ledc_timer_config_t ledc_timer_conf{.speed_mode = LEDC_LOW_SPEED_MODE,
                                      .duty_resolution = LEDC_TIMER_15_BIT,
                                      .timer_num = this->get_timer(),
                                      .freq_hz = this->get_freq(),
                                      .clk_cfg = LEDC_AUTO_CLK};
  if (ledc_timer_config(&ledc_timer_conf) != ESP_OK) {
    ESP_LOGI("BUZZER", "Failed to Config LEDC timer");
    err = true;
  }
  ledc_channel_config_t ledc_channel_conf{
      .gpio_num = this->get_gpio_num(),
      .speed_mode = this->get_speed_mode(),
      .channel = this->get_channel(),
      .intr_type = LEDC_INTR_DISABLE,
      .timer_sel = this->get_timer(),
      .duty = (1U << 14U),
      .hpoint = 0,
  };
  if (ledc_channel_config(&ledc_channel_conf) != ESP_OK) {
    ESP_LOGI("BUZZER", "Failed to Config LEDC Channel");
    err = true;
  }
  if (ledc_timer_pause(this->get_speed_mode(), this->get_timer()) != ESP_OK) {
    ESP_LOGI("BUZZER", "Failed to Pause Buzzer");
    err = true;
    this->set_buzzer_state(BuzzerState::ON);
  } else {
    this->set_buzzer_state(BuzzerState::OFF);
  }
  return err;
}

bool Buzzer::buzzer_toggle(BuzzerState bzs) {
  bool err{false};
  if (this->is_buzzer_on() && bzs == BuzzerState::ON) {
    return err;
  } else if (this->is_buzzer_on() && bzs == BuzzerState::OFF) {
    if (ledc_timer_pause(this->get_speed_mode(), this->get_timer()) != ESP_OK) {
      ESP_LOGI("BUZZER", "Failed to Change State on Pausing");
      err = true;
    } else {
      this->set_buzzer_state(BuzzerState::OFF);
    }
  } else if (!this->is_buzzer_on() && bzs == BuzzerState::ON) {
    if (ledc_timer_resume(this->get_speed_mode(), this->get_timer()) !=
        ESP_OK) {
      ESP_LOGI("BUZZER", "Failed to Change State on Resuming");
      err = true;
    } else {
      this->set_buzzer_state(BuzzerState::ON);
    }
  }
  return err;
}

QueueHandle_t host_uart_init() {
  uart_port_t __port = UART_NUM_2;
  uart_config_t __uart{
      .baud_rate = 115200,
      .data_bits = UART_DATA_8_BITS,
      .parity = UART_PARITY_DISABLE,
      .stop_bits = UART_STOP_BITS_1,
      .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
      .source_clk = UART_SCLK_DEFAULT,
  };
  if (uart_param_config(__port, &__uart) != ESP_OK)
    ESP_LOGI("UART", "Failed to Configure uart");
  if (uart_set_pin(__port, GPIO_NUM_1, GPIO_NUM_3, UART_PIN_NO_CHANGE,
                   UART_PIN_NO_CHANGE) != ESP_OK)
    ESP_LOGI("UART", "Failed to Configure uart pins");

  QueueHandle_t uart_queue;

  if (uart_driver_install(__port, UART_BUFFER_SIZE * 2, UART_BUFFER_SIZE * 2, 20,
                          &uart_queue, 0) != ESP_OK)
    ESP_LOGI("UART", "Failed to Install uart driver");

  return uart_queue;
}
