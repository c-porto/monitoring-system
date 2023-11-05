#pragma once
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"

#define DHT_READ_EVENT (1UL << 0UL)
#define CJMCU811_READ_EVENT (1UL << 1UL)
#define GYML8511_READ_EVENT (1UL << 2UL)
#define DHT_READ_EVENT_HTTP (1UL << 3UL)
#define CJMCU811_READ_EVENT_HTTP (1UL << 4UL)
#define GYML8511_READ_EVENT_HTTP (1UL << 5UL)
#define HOST_CMD_EVENT (1UL << 6UL)

void create_tasks();
void create_event_groups();
void create_mutex();
void create_semphr();
