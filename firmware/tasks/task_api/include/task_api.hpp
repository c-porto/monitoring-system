#pragma once
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"

extern TaskHandle_t xTaskDhtHandle;

void create_tasks();
void create_event_groups();
void create_mutex();
void create_semphr();
