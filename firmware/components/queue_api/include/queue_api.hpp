#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <freertos/task.h>
#include <dht11.hpp>

extern QueueHandle_t xQueueTH_Handle;

void create_queues();
