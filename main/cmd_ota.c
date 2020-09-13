
#include <stdio.h>
#include "esp_console.h"
#include "argtable3/argtable3.h"
#include "esp_log.h"
static const char *TAG = "cmd_ota";

#include "ota.h"

static void register_ota(void);

/*******************************************************************************

    PURPOSE: register the wifi commands

    INPUTS: none

    RETURN CODE: none

*******************************************************************************/

void commands_ota(void)
{
    register_ota();
}

/*******************************************************************************

    PURPOSE: tell wifi task to do a ota

    NOTE: This is a call back used by the console

    INPUTS: argc / argv (arg_parse library used, see "register_ota")

    RETURN CODE: none

*******************************************************************************/
static struct {
    struct arg_lit *forced;
    struct arg_lit *test;
    struct arg_end *end;
} ota_args;

static int command_ota(int argc, char **argv) {
    bool forced = false;
    bool test = false;

    int nerrors = arg_parse(argc, argv, (void **) &ota_args);
    if (nerrors != 0) {
        arg_print_errors(stderr, ota_args.end, argv[0]);
        return 1;
    }
    if (ota_args.forced->count) {
        forced = true;
    }
    if (ota_args.test->count) {
        test = true;
    }
  
    perform_ota(forced+test*2);
    return 0;
}

/*******************************************************************************

    PURPOSE: register the 'ota' command - firmware download

    INPUTS: none

    RETURN CODE: none

*******************************************************************************/

static void register_ota(void) {

    const esp_console_cmd_t ota_cmd = {
        .command = "ota",
        .help = "check for ota and perform if needed",
        .hint = NULL,
        .func = &command_ota,
        .argtable = &ota_args,
    };
    
    ota_args.forced = arg_litn("f", "forced", 0, 1, "force ota flag"),
    ota_args.test = arg_litn("t", "testbuild", 0, 1, "test build ota flag"),
    ota_args.end    = arg_end(2);

    ESP_ERROR_CHECK( esp_console_cmd_register(&ota_cmd) );
}
