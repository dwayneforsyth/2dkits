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
//   This is the console pattern commands
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
// #include "esp_flash_mmap.h"
#include "driver/rtc_io.h"
#include "driver/uart.h"
#include "argtable3/argtable3.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"

#include "cmd_system.h"
#include "version.h"
#include "disk_system.h"
#include "pattern_engine.h"
#include "led_driver.h"

#define WITH_TASKS_INFO 1

static const char *TAG = "cmd_system";

static void register_pattern(void);
static void register_demo(void);
static void register_led(void);

/*******************************************************************************

    PURPOSE: adds system commands and callbacks to the menu

    INPUTS: None

    RETURN CODE: None

*******************************************************************************/

void commands_pattern(void)
{
    register_pattern();
    register_demo();
    register_led();
}

/*******************************************************************************

    PURPOSE: next command

    INPUTS: None

    RETURN CODE: None

*******************************************************************************/
static struct {
    struct arg_str *control;
    struct arg_end *end;
} pattern_args;

static int pattern(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **)&pattern_args);
    if (nerrors != 0) {
        arg_print_errors(stderr, pattern_args.end, argv[0]);
        return 1;
    }

    if (strlen(pattern_args.control->sval[0])==0) {
        ESP_LOGW(TAG, "Pattern %d - %s", getPatternNumber()+1,getPatternName());
        return( 0 );
    } else if (strcasecmp(pattern_args.control->sval[0], "+") == 0) {
	setPatternPlus();
        ESP_LOGW(TAG, "Pattern %d - %s", getPatternNumber()+1, getPatternName());
        return( 0 );
    } else if (strcasecmp(pattern_args.control->sval[0], "-") == 0) {
	setPatternMinus();
        ESP_LOGW(TAG, "Pattern %d - %s", getPatternNumber()+1, getPatternName());
        return( 0 );
	/* DDF do not understand error
    } else if (isdigit(pattern_args.control->sval[0][0])) {
	uint8_t value = atoi(pattern_args.control->sval[0]);
	if (value == 0) {
            ESP_LOGW(TAG, "Pattern is 1 based (for user UI)");
            return( 2 );
        }
	setPatternNumber( value-1 );
	vTaskDelay(1000 / portTICK_PERIOD_MS);
        ESP_LOGW(TAG, "Pattern %d - %s", getPatternNumber()+1, getPatternName());
        return( 0 );
	*/
    } else {
        ESP_LOGW(TAG, "Pattern >%s<", pattern_args.control->sval[0]);
        return( 1 );
    }
    
    return 0;
}

/*******************************************************************************

    PURPOSE: register 'next' command

    INPUTS: None

    RETURN CODE: None

*******************************************************************************/
static void register_pattern(void)
{
    const esp_console_cmd_t cmd = {
        .command = "pattern",
        .help = "query and set pattern",
        .hint = NULL,
        .func = &pattern,
	.argtable = &pattern_args,
    };
    ESP_ERROR_CHECK( esp_console_cmd_register(&cmd) );
    pattern_args.control = arg_str0(NULL, NULL, "<+|-|[#]>", "get/set pattern number");
    pattern_args.end = arg_end(1);
}

/*******************************************************************************

    PURPOSE: 'demo' command

    INPUTS: None

    RETURN CODE: None

*******************************************************************************/
static struct {
    struct arg_str *control;
    struct arg_end *end;
} demo_args;

static int demo(int argc, char **argv)
{
    int nerrors = arg_parse(argc, argv, (void **)&demo_args);
    if (nerrors != 0) {
        arg_print_errors(stderr, demo_args.end, argv[0]);
        return 1;
    }

    if (strlen(demo_args.control->sval[0])==0) {
        ESP_LOGW(TAG, "Demo status %s",(getDemoMode())?"On":"Off");
        return( 0 );
    } else if (strcasecmp(demo_args.control->sval[0], "on") == 0) {
        ESP_LOGW(TAG, "Demo status on");
	setDemoMode(true);
        return( 0 );
    } else if (strcasecmp(demo_args.control->sval[0], "off") == 0) {
        ESP_LOGW(TAG, "Demo status off");
	setDemoMode(false);
        return( 0 );
    } else {
        ESP_LOGW(TAG, "Demo >%s<", demo_args.control->sval[0]);
        return( 1 );
    }
}

/*******************************************************************************

    PURPOSE: register 'restart' command

    INPUTS: None

    RETURN CODE: None

*******************************************************************************/
static void register_demo(void)
{
    const esp_console_cmd_t cmd = {
        .command = "demo",
        .help = "query and set demo mode",
        .hint = NULL,
        .func = &demo,
	.argtable = &demo_args,
    };
    ESP_ERROR_CHECK( esp_console_cmd_register(&cmd) );
    demo_args.control = arg_str0(NULL, NULL, "<on|off>", "Get info of Ethernet");
    demo_args.end = arg_end(1);
}

/*******************************************************************************

    PURPOSE: led command

    INPUTS: None

    RETURN CODE: None

*******************************************************************************/
static struct {
    struct arg_lit *set;
    struct arg_int *layer;
    struct arg_int *x;
    struct arg_int *y;
    struct arg_int *red;
    struct arg_int *green;
    struct arg_int *blue;
    struct arg_end *end;
} led_args;

static int command_led(int argc, char **argv)
{
    uint8_t layer = 0;
    uint8_t x = 0;
    uint8_t y = 0;
    uint8_t r = 0;
    uint8_t g = 0;
    uint8_t b = 0;

    int nerrors = arg_parse(argc, argv, (void **)&led_args);
    if (nerrors != 0) {
        arg_print_errors(stderr, led_args.end, argv[0]);
        return 1;
    }

    if (led_args.layer->count) {
        layer = led_args.layer->ival[0];
	if (layer > 7) { return(1);}
    }
    if (led_args.x->count) {
        x = led_args.x->ival[0];
	if (x > 3) { return(1);}
    }
    if (led_args.y->count) {
        y = led_args.y->ival[0];
	if (y > 3) { return(1);}
    }
    if (led_args.red->count) {
        r = led_args.red->ival[0];
	if (r > 15) { return(1);}
    }
    if (led_args.green->count) {
        g = led_args.green->ival[0];
	if (g > 15) { return(1);}
    }
    if (led_args.blue->count) {
        b = led_args.blue->ival[0];
	if (b > 15) { return(1);}
    }

    if (led_args.set->count) {
        ESP_LOGW(TAG, "set led Layer=%d x=%d y=%d -> (%d,%d,%d)",layer,x,y,r,g,b);
        setLed(layer,x,y,r,g,b);
    }

    getLed(layer,x,y,&r,&g,&b);
    ESP_LOGW(TAG, "get led Layer=%d x=%d y=%d -> (%d,%d,%d)",layer,x,y,r,g,b);


    return 0;
}

/*******************************************************************************

    PURPOSE: register 'next' command

    INPUTS: None

    RETURN CODE: None

*******************************************************************************/
static void register_led(void)
{
    const esp_console_cmd_t cmd = {
        .command = "led",
        .help = "query and set LEDs",
        .hint = NULL,
        .func = &command_led,
        .argtable = &led_args,
    };
    led_args.set =   arg_lit0("s", "set",   "set flag");
    led_args.layer = arg_int0("l", "layer", "<0..7>", "layer");
    led_args.x =     arg_int0("x", NULL,    "<0..3>", "x");
    led_args.y =     arg_int0("y", NULL,    "<0..3>", "y");
    led_args.red =   arg_int0("r", "red",   "<0..15>", "red value");
    led_args.green = arg_int0("g", "green", "<0..15>", "green value");
    led_args.blue =  arg_int0("b", "blue",  "<0..15>", "blue value");
    led_args.end = arg_end(2);
    ESP_ERROR_CHECK( esp_console_cmd_register(&cmd) );
}

