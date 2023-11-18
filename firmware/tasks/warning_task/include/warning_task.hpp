#pragma once
#include "utils.hpp"
#include <freertos/FreeRTOS.h>
#include <freertos/event_groups.h>
#include <freertos/semphr.h>
#include <freertos/task.h>


#define TASK_WARNING_NAME "TaskWarning"
#define TASK_WARNING_STACK_SIZE 2048
#define TASK_WARNING_PRIORITY 4
#define TASK_WARNING_CORE 1

enum class Thresholds : uint32_t {
  MAX_TEMP_ALLOWED = 25UL,     /* Value choosed for test convenience*/
  MIN_HUMIDITY_ALLOWED = 20UL, /* Value choosed for test convenience*/
  MAX_UV_ALLOWED = 520UL,        /* Value choosed for test convenience*/
  MAX_CO2_ALLOWED = 2500UL,    /* Actual warmeful value for C02 ppm*/
};

extern SemaphoreHandle_t mutex;
extern EventGroupHandle_t event_group;
extern TaskHandle_t xTaskWarningHandle;
extern sensor::Measure ms;

void vTaskWarning(void *params);
