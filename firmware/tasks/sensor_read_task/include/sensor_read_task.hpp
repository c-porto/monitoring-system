#pragma once
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"

void sensor_read_task(void *sample_ptr);
