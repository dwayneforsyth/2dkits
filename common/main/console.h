#pragma once

#include "esp_task.h"
#include "version.h"

#define CONSOLE_TASK_PRIORITY   (ESP_TASK_PRIO_MIN + 2)
#define CONSOLE_TASK_STACK_SIZE (8 * 1024)

void consoleInit(void);
void process_command( char * line );

void commands_dfu( void );
void commands_pattern( void );
void commands_wifi( void );
void commands_download( void );
void commands_picoc( void );

#ifdef TIXCLOCK
void commands_tixclock( void );
#endif
