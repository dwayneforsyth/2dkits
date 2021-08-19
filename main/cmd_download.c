
#include <stdio.h>
#include "esp_console.h"
#include "argtable3/argtable3.h"
#include "esp_log.h"
static const char *TAG = "cmd_download";

#include "download_file.h"

static void register_download(void);

/*******************************************************************************

    PURPOSE: register the wifi commands

    INPUTS: none

    RETURN CODE: none

*******************************************************************************/

void commands_download(void)
{
    register_download();
}

/*******************************************************************************

    PURPOSE: tell wifi task to do a dfu

    NOTE: This is a call back used by the console

    INPUTS: argc / argv (arg_parse library used, see "register_dfu")

    RETURN CODE: none

*******************************************************************************/
static struct {
    struct arg_file *url;
    struct arg_file *file;
    struct arg_end *end;
} download_args;

#define CONFIG_TEST_DOWNLOAD_URL "http://www.2dkits.com/kits/kit55/img/index.html"

static int command_download(int argc, char **argv) {
    char * url;
    char * file;

    int nerrors = arg_parse(argc, argv, (void **) &download_args);
    if (nerrors != 0) {
        arg_print_errors(stderr, download_args.end, argv[0]);
        return 1;
    }

    asprintf( &url, "http://www.2dkits.com/kits/kit25/patterns/%s", 
                     download_args.url->basename[0]);
    asprintf( &file, "/spiffs/%s", 
                      download_args.file->basename[0]);
    printf( "%s %s\n", url, file );

    download_file( file, url );

    free( url);
    free( file);
    return 0;
}

/*******************************************************************************

    PURPOSE: register the 'download' command - firmware download

    INPUTS: none

    RETURN CODE: none

*******************************************************************************/

static void register_download(void) {

    const esp_console_cmd_t download_cmd = {
        .command = "download",
        .help = "download file to flash drive",
        .hint = NULL,
        .func = &command_download,
        .argtable = &download_args,
    };
    
    download_args.file = arg_file1("f", "file", "file name", "filename to store in on flash drive"),
    download_args.url = arg_file1("u", "url", "web url", "url to download"),
    download_args.end    = arg_end(2);

    ESP_ERROR_CHECK( esp_console_cmd_register(&download_cmd) );
}
