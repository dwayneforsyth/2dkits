#pragma once

#include "esp_task.h"

#define CONSOLE_TASK_PRIORITY   (ESP_TASK_PRIO_MIN + 2)
#define CONSOLE_TASK_STACK_SIZE (8 * 1024)

void consoleInit(void);
void process_command( char * line );

void commands_ota( void );
void commands_pattern( void );
