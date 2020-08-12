

#pragma once

#include "esp_task.h"

#define OTA_TASK_PRIORITY   (ESP_TASK_PRIO_MIN + 2)
#define OTA_TASK_STACK_SIZE (8 * 1024)

void perform_ota(uint8_t flags);

