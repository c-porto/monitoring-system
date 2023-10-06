/* Blink C++ Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include "esp_log.h"
#include "gpio_cxx.hpp"
#include <../devices/dht11/include/dht11.hpp>
#include <../tasks/task_api/include/task_api.hpp>
#include <cstdlib>
#include <thread>

using namespace idf;
using namespace std;

extern "C" void app_main(void) {
    create_tasks(); 
}
