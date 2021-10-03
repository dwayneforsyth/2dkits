

#pragma once

#include "esp_task.h"

#define DFU_TASK_PRIORITY   (ESP_TASK_PRIO_MIN + 2)
#define DFU_TASK_STACK_SIZE (8 * 1024)

void perform_dfu(uint8_t flags);
bool checkForUpdate( uint8_t flags);
