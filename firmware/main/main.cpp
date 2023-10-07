/* Blink C++ Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include "esp_log.h"
#include "gpio_cxx.hpp"
#include <../devices/dht11/include/dht11.hpp>
#include <../tasks/dht11_task/include/dht11_task.hpp>
#include <../tasks/task_api/include/task_api.hpp>
#include <chrono>
#include <cstdlib>
#include <sstream>
#include <stdio.h>
#include <string>
#include <thread>

#define DHT_PIN 25

extern "C" void app_main(void) {
  Dht11 dht_sensor{DHT_PIN};
  double result[2]{0};
  std::stringstream ss1;
  std::stringstream ss2;

  for (;;) {
    auto sample = dht_sensor.read();

    if (sample) {

      result[0] = sample->temp;
      result[1] = sample->humidity;

      ss1 << "Temperature: " << result[0];
      ss2 << "Humidity: " << result[0];

      char *temp = (char *)ss1.str().c_str();
      char *hm = (char *)ss2.str().c_str();

      printf("%s", temp);
      printf("%s", hm);

      std::this_thread::sleep_for(std::chrono::milliseconds(1200));

    } else {
      ESP_LOGI("NULL ERROR", "Something happened during communication");
    }
  }
}
