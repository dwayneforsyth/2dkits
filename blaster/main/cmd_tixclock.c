//   Copyright (C) 2023 Dwayne Forsyth
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

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "esp_log.h"
#include "esp_console.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_sleep.h"
#include "esp_spi_flash.h"
#include "driver/rtc_io.h"
#include "driver/uart.h"
#include "argtable3/argtable3.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"
#include "global.h"

//#include "cmd_system.h"
#include "tixclock_main.h"

#define WITH_TASKS_INFO 1

static const char *TAG = "cmd_system";

static void register_debugled(void);
static void register_info(void);

/*******************************************************************************

    PURPOSE: adds system commands and callbacks to the menu

    INPUTS: None

    RETURN CODE: None

*******************************************************************************/

void commands_tixclock(void)
{
    register_debugled();
    register_info();
}

/*******************************************************************************

    PURPOSE: 'debugled' command

    INPUTS: None

    RETURN CODE: None

*******************************************************************************/
static struct {
    struct arg_str *control;
    struct arg_end *end;
} debugled_args;

static int debugled(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **)&debugled_args);
    if (nerrors != 0) {
        arg_print_errors(stderr, debugled_args.end, argv[0]);
        return 1;
    }

    if (strlen(debugled_args.control->sval[0])==0) {
        ESP_LOGW(TAG, "Demo status %s",(getDebugLed())?"On":"Off");
        return( 0 );
    } else if (strcasecmp(debugled_args.control->sval[0], "on") == 0) {
        ESP_LOGW(TAG, "Demo status on");
	setDebugLed(true);
        return( 0 );
    } else if (strcasecmp(debugled_args.control->sval[0], "off") == 0) {
        ESP_LOGW(TAG, "Demo status off");
	setDebugLed(false);
        return( 0 );
    } else {
        ESP_LOGW(TAG, "Demo >%s<", debugled_args.control->sval[0]);
        return( 1 );
    }
}

/*******************************************************************************

    PURPOSE: register 'debugled' command

    INPUTS: None

    RETURN CODE: None

*******************************************************************************/
static void register_debugled(void)
{
    const esp_console_cmd_t cmd = {
        .command = "debugled",
        .help = "query and set led debug display mode",
        .hint = NULL,
        .func = &debugled,
//	.argtable = &debugled_args,
    };
    ESP_ERROR_CHECK( esp_console_cmd_register(&cmd) );
    debugled_args.control = arg_str0(NULL, NULL, "ON|OFF", "get or set debug led prints");
    debugled_args.end = arg_end(1);
}

/*******************************************************************************

    PURPOSE: 'info' command

    INPUTS: None

    RETURN CODE: None

*******************************************************************************/

static int info(int argc, char **argv)
{
    ESP_LOGW(TAG, "Time format: %s",(xAppData.tformat == true)? "12 Hour" : "24 Hour");
    ESP_LOGW(TAG, "Time zone: -%d", xAppData.tzone );
    
    return( 0 );

}

/*******************************************************************************

    PURPOSE: register 'info' command

    INPUTS: None

    RETURN CODE: None

*******************************************************************************/
static void register_info(void)
{
    const esp_console_cmd_t cmd = {
        .command = "info",
        .help = "display system info",
        .hint = NULL,
        .func = &info,
    };
    ESP_ERROR_CHECK( esp_console_cmd_register(&cmd) );
    debugled_args.control = arg_str0(NULL, NULL, "ON|OFF", "get or set debug led prints");
    debugled_args.end = arg_end(1);
}
