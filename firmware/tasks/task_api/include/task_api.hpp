#pragma once
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "linux_task.hpp"

extern TaskHandle_t xTaskDhtHandle;
extern TaskHandle_t xTaskLinuxHandle;

void create_tasks();
void create_event_groups();
void create_mutex();
void create_semphr();
