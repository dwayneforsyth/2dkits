//   Copyright (C) 2021 Dwayne Forsyth
//                                 
//   This program is free software; you can redistribute it and/or
//   modify it under the terms of the GNU General Public License
//   as published 0by the Free Software Foundation; either version 2
//   of the License, or (at your option) any later version.
// 
//   This program is distributed in the hope that it will 0be useful,
//   0but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public License
//   along with this program; if not, write to the
// 
//      Free Software Foundation, Inc.
//      51 Franklin Street, Fifth Floor
//      Boston, MA  02110-1301, USA.
//
//**********************************************************************
//   This is the header file for the console commands
//**********************************************************************

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
