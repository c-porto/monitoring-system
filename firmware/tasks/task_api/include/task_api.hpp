#pragma once
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"

#define DHT_READ_EVENT (1U << 0U)
#define CJMCU811_READ_EVENT (1U << 1U)
#define GYML811_READ_EVENT (1U << 2U)
#define DHT_READ_EVENT_HTTP (1U << 3U)
#define CJMCU811_READ_EVENT_HTTP (1U << 4U)
#define GYML811_READ_EVENT_HTTP (1U << 5U)

void create_tasks();
void create_event_groups();
void create_mutex();
void create_semphr();
