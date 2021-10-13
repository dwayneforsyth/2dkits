
#include <stdio.h>
#include "esp_console.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "argtable3/argtable3.h"
#include "esp_log.h"
static const char *TAG = "cmd_picoc";

#include "LICENSE.h"
#include "picoc.h"


static void register_run(void);

/*******************************************************************************

    PURPOSE: register the wifi commands

    INPUTS: none

    RETURN CODE: none

*******************************************************************************/

void commands_picoc(void)
{
    register_run();
}

/*******************************************************************************

    PURPOSE: tell wifi task to do a dfu

    NOTE: This is a call back used by the console

    INPUTS: argc / argv (arg_parse library used, see "register_dfu")

    RETURN CODE: none

*******************************************************************************/
static struct {
    struct arg_file *files;
    struct arg_lit *script;
    struct arg_lit *interactive;
    struct arg_lit *copyright;
    struct arg_end *end;
} run_args;

/* Override via STACKSIZE environment variable */
#define PICOC_STACK_SIZE (1024*4)


static int command_run(int argc, char **argv) {
    bool DontRunMain = false;
    int StackSize = PICOC_STACK_SIZE;
    Picoc pc;

    int nerrors = arg_parse(argc, argv, (void **) &run_args);
    if (nerrors != 0) {
        arg_print_errors(stderr, run_args.end, argv[0]);
        return 1;
    }

    if (run_args.copyright->count) {
        printf("This product includes picoc \na \'c\' Interputer\n");
        printf("%s\n", (char*)&__LICENSE);
	return(0);
    }

    PicocInitialize(&pc, StackSize);

    if (run_args.script->count) {
        DontRunMain = true;
    }

    if (run_args.interactive->count) {
        PicocIncludeAllSystemHeaders(&pc);
        PicocParseInteractive(&pc);
    } else { 

        if (run_args.files->count) {
            if (PicocPlatformSetExitPoint(&pc)) {
                PicocCleanup(&pc);
                return pc.PicocExitValue;
            }
            printf("scan file=>%s<\n",run_args.files->basename[0]);
            char *file;
	    asprintf( &file, "/spiffs/%s", run_args.files->basename[0]);
            PicocPlatformScanFile(&pc, file);
	    free(file);
        }
    }

    if (!DontRunMain) {
       printf("call main\n");
       PicocCallMain(&pc, 0, NULL);
    }

    PicocCleanup(&pc);
  
    printf("picoc done\n");
    return 0;
}

/*******************************************************************************

    PURPOSE: register the 'dfu' command - firmware download

    INPUTS: none

    RETURN CODE: none

*******************************************************************************/

static void register_run(void) {

    const esp_console_cmd_t run_cmd = {
        .command = "run",
        .help = "\'c\' interpurture",
        .hint = NULL,
        .func = &command_run,
        .argtable = &run_args,
    };
    
    run_args.files = arg_filen( NULL, NULL, "files", 0, 5, "run a program, calls main() as the entry point");
    run_args.script = arg_lit0( "s", NULL, "run a script, runs the program without calling main()");
    run_args.interactive = arg_lit0("i", NULL, "interactive mode");
    run_args.copyright = arg_lit0("c", NULL, "copyright info");
    run_args.end    = arg_end(2);

    ESP_ERROR_CHECK( esp_console_cmd_register(&run_cmd) );
}
