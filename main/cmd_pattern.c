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

#include "cmd_system.h"
#include "version.h"
#include "disk_system.h"
#include "pattern_engine.h"

#define WITH_TASKS_INFO 1

static const char *TAG = "cmd_system";

static void register_pattern(void);
static void register_demo(void);

/*******************************************************************************

    PURPOSE: adds system commands and callbacks to the menu

    INPUTS: None

    RETURN CODE: None

*******************************************************************************/

void commands_pattern(void)
{
    register_pattern();
    register_demo();
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
        ESP_LOGW(TAG, "Pattern %d - %s", getPatternNumber(),getPatternName());
        return( 0 );
    } else if (strcasecmp(pattern_args.control->sval[0], "+") == 0) {
	setPatternPlus();
        ESP_LOGW(TAG, "Pattern %d - %s", getPatternNumber(), getPatternName());
        return( 0 );
    } else if (strcasecmp(pattern_args.control->sval[0], "-") == 0) {
	setPatternMinus();
        ESP_LOGW(TAG, "Pattern %d - %s", getPatternNumber(), getPatternName());
        return( 0 );
    } else if (isdigit(pattern_args.control->sval[0][0])) {
	uint8_t value = atoi(pattern_args.control->sval[0]);
	setPatternNumber( value );
	vTaskDelay(1000 / portTICK_PERIOD_MS);
        ESP_LOGW(TAG, "Pattern %d - %s", getPatternNumber(), getPatternName());
        return( 0 );
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

