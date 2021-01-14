
#include <stdio.h>
#include "esp_console.h"
#include "argtable3/argtable3.h"
#include "esp_log.h"
static const char *TAG = "cmd_dfu";

#include "dfu.h"
#include "upgrade_disk.h"

static void register_dfu(void);
static void register_checkdisk(void);

/*******************************************************************************

    PURPOSE: register the wifi commands

    INPUTS: none

    RETURN CODE: none

*******************************************************************************/

void commands_dfu(void)
{
    register_dfu();
    register_checkdisk();
}

/*******************************************************************************

    PURPOSE: tell wifi task to do a dfu

    NOTE: This is a call back used by the console

    INPUTS: argc / argv (arg_parse library used, see "register_dfu")

    RETURN CODE: none

*******************************************************************************/
static struct {
    struct arg_lit *forced;
    struct arg_lit *test;
    struct arg_end *end;
} dfu_args;

static int command_dfu(int argc, char **argv) {
    bool forced = false;
    bool test = false;

    int nerrors = arg_parse(argc, argv, (void **) &dfu_args);
    if (nerrors != 0) {
        arg_print_errors(stderr, dfu_args.end, argv[0]);
        return 1;
    }
    if (dfu_args.forced->count) {
        forced = true;
    }
    if (dfu_args.test->count) {
        test = true;
    }
  
    perform_dfu(forced+test*2);
    return 0;
}

/*******************************************************************************

    PURPOSE: register the 'dfu' command - firmware download

    INPUTS: none

    RETURN CODE: none

*******************************************************************************/

static void register_dfu(void) {

    const esp_console_cmd_t dfu_cmd = {
        .command = "dfu",
        .help = "check for \'Downloadable Firmware Upgrade\' and perform if needed",
        .hint = NULL,
        .func = &command_dfu,
        .argtable = &dfu_args,
    };
    
    dfu_args.forced = arg_litn("f", "forced", 0, 1, "forces dfu"),
    dfu_args.test = arg_litn("t", "testbuild", 0, 1, "load development test build"),
    dfu_args.end    = arg_end(2);

    ESP_ERROR_CHECK( esp_console_cmd_register(&dfu_cmd) );
}

/*******************************************************************************

    PURPOSE: check disk hook

    INPUTS: none

    RETURN CODE: none

*******************************************************************************/
static int command_checkdisk(int argc, char **argv) {
    auditDiskFiles();

    return 0;
}

/*******************************************************************************

    PURPOSE: register the 'dfu' command - firmware download

    INPUTS: none

    RETURN CODE: none

*******************************************************************************/

static void register_checkdisk(void) {

    const esp_console_cmd_t checkdisk_cmd = {
        .command = "chkdsk",
        .help = "check to see if we have the files needed on disk",
        .hint = NULL,
        .func = &command_checkdisk,
    };

    ESP_ERROR_CHECK( esp_console_cmd_register(&checkdisk_cmd) );
}
