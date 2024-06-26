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
//   This is the console system commands
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
// #include "esp_spi_flash.h"
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
#include "global.h"
#include "sha_file.h"

#define WITH_TASKS_INFO 1

static const char *TAG = "cmd_system";

static void register_heap(void);
static void register_version(void);
static void register_restart(void);
static void register_factoryrestart(void);
static void register_tasks(void);
static void register_time(void);
static void register_dir(void);

/*******************************************************************************

    PURPOSE: adds system commands and callbacks to the menu

    INPUTS: None

    RETURN CODE: None

*******************************************************************************/

void register_system(void)
{
    register_heap();
    register_version();
    register_restart();
    register_factoryrestart();
    register_tasks();
    register_time();
    register_dir();
}

/*******************************************************************************

    PURPOSE: give hardware and software version info

    INPUTS: None

    RETURN CODE: None

*******************************************************************************/
static int get_version(int argc, char **argv)
{
//    esp_chip_info_t info;
//    esp_chip_info(&info);
    ESP_LOGW(TAG, "IDF Version:%s", esp_get_idf_version());
    ESP_LOGW(TAG, "Chip info:");
//    ESP_LOGW(TAG, "\tmodel:%s", info.model == CHIP_ESP32 ? "ESP32" : "Unknow");
//    ESP_LOGW(TAG, "\tcores:%d", info.cores);
//    ESP_LOGW(TAG, "\tfeature:%s%s%s%s%d%s",
//           info.features & CHIP_FEATURE_WIFI_BGN ? "/802.11bgn" : "",
//           info.features & CHIP_FEATURE_BLE ? "/BLE" : "",
//           info.features & CHIP_FEATURE_BT ? "/BT" : "",
//           info.features & CHIP_FEATURE_EMB_FLASH ? "/Embedded-Flash:" : "/External-Flash:",
//           spi_flash_get_chip_size() / (1024 * 1024), " MB");
//    ESP_LOGW(TAG, "\trevision number:%d", info.revision);
    ESP_LOGW(TAG, "\tsoftware version: %d.%d.%d",MAJOR, MINOR, BUILD);
    ESP_LOGW(TAG, "\tkit number: %d", KIT_NUMBER);
    ESP_LOGW(TAG, "\tkit name: %s", KIT_NAME);
    return 0;
}

/*******************************************************************************

    PURPOSE: register 'version' command

    INPUTS: None

    RETURN CODE: None

*******************************************************************************/
static void register_version(void)
{
    const esp_console_cmd_t cmd = {
        .command = "version",
        .help = "Get version of chip and SDK",
        .hint = NULL,
        .func = &get_version,
    };
    ESP_ERROR_CHECK( esp_console_cmd_register(&cmd) );
}

/*******************************************************************************

    PURPOSE: time command

    INPUTS: None

    RETURN CODE: None

*******************************************************************************/
static int get_time(int argc, char **argv)
{
    time_t      now;
    char        timeBuf[64];
    struct tm   timeinfo;

    time(&now);

    localtime_r(&now, &timeinfo);
    strftime(timeBuf, sizeof(timeBuf), "%c", &timeinfo);
    ESP_LOGW(TAG, "%s\n", timeBuf);

    return 0;
}

/*******************************************************************************

    PURPOSE: register 'time' command

    INPUTS: None

    RETURN CODE: None

*******************************************************************************/
static void register_time(void)
{
    const esp_console_cmd_t cmd = {
        .command = "time",
        .help = "Get system time",
        .hint = NULL,
        .func = &get_time,
    };
    ESP_ERROR_CHECK( esp_console_cmd_register(&cmd) );
}

/*******************************************************************************

    PURPOSE: 'restart' command

    INPUTS: None

    RETURN CODE: None

*******************************************************************************/

static int factoryrestart(int argc, char **argv)
{
    ESP_LOGW(TAG, "factory Restarting");
    initSettings();
    storeSettings();
    return(0);
}

/*******************************************************************************

    PURPOSE: register 'restart' command

    INPUTS: None

    RETURN CODE: None

*******************************************************************************/
static void register_factoryrestart(void)
{
    const esp_console_cmd_t cmd = {
        .command = "factoryreset",
        .help = "factory reset of blinkie, deletes all settings",
        .hint = NULL,
        .func = &factoryrestart,
    };
    ESP_ERROR_CHECK( esp_console_cmd_register(&cmd) );
}
/*******************************************************************************

    PURPOSE: 'restart' command

    INPUTS: None

    RETURN CODE: None

*******************************************************************************/

static int restart(int argc, char **argv)
{
    ESP_LOGW(TAG, "Restarting");
    esp_restart();
}

/*******************************************************************************

    PURPOSE: register 'restart' command

    INPUTS: None

    RETURN CODE: None

*******************************************************************************/
static void register_restart(void)
{
    const esp_console_cmd_t cmd = {
        .command = "reset",
        .help = "Software reset of the chip",
        .hint = NULL,
        .func = &restart,
    };
    ESP_ERROR_CHECK( esp_console_cmd_register(&cmd) );
}

/*******************************************************************************

    PURPOSE: 'heap' command

    INPUTS: None

    RETURN CODE: None

*******************************************************************************/

static int heap_size(int argc, char **argv)
{
    uint32_t heap_size = heap_caps_get_minimum_free_size(MALLOC_CAP_DEFAULT);
    ESP_LOGW(TAG, "min heap size: %lu\n", heap_size);
    ESP_LOGW(TAG, "current size:  %lu\n", esp_get_free_heap_size());
    return 0;
}

/*******************************************************************************

    PURPOSE: register 'heap' command

    INPUTS: None

    RETURN CODE: None

*******************************************************************************/
static void register_heap(void)
{
    const esp_console_cmd_t heap_cmd = {
        .command = "heap",
        .help = "Get minimum size of free heap memory that was available during program execution",
        .hint = NULL,
        .func = &heap_size,
    };
    ESP_ERROR_CHECK( esp_console_cmd_register(&heap_cmd) );

}

/*******************************************************************************

    PURPOSE: tasks command - give freeRTOS task info

    INPUTS: None

    RETURN CODE: None

*******************************************************************************/
static int tasks_info(int argc, char **argv)
{
#if CONFIG_IDF_TARGET_ESP32
    const size_t bytes_per_task = 40; /* see vTaskList description */
    char *task_list_buffer = malloc(uxTaskGetNumberOfTasks() * bytes_per_task);
    if (task_list_buffer == NULL) {
        ESP_LOGE(TAG, "failed to allocate buffer for vTaskList output");
        return 1;
    }
    fputs("Task Name\tStatus\tPrio\tHWM\tTask#\tAffinity", stdout);
    fputs("\n", stdout);
    vTaskList(task_list_buffer);
    fputs(task_list_buffer, stdout);
    free(task_list_buffer);
#endif
    return 0;
}

/*******************************************************************************

    PURPOSE: register 'tasks' command

    INPUTS: None

    RETURN CODE: None

*******************************************************************************/
static void register_tasks(void)
{
    const esp_console_cmd_t cmd = {
        .command = "tasks",
        .help = "Get information about running tasks",
        .hint = NULL,
        .func = &tasks_info,
    };
    ESP_ERROR_CHECK( esp_console_cmd_register(&cmd) );
}

/*******************************************************************************
    PURPOSE: dir ascii header callback

    INPUTS: path - dir path
            req - httpd connection

    RETURN CODE: NONE

*******************************************************************************/
void asciiHeader_cb( char *path, void *data) {
    printf("\nList of Directory [%s]\n", path);
    printf("--------------------------------------------------------------------------\n");
    printf("T  Name                     Size  Sha1 Hash\n");
    printf("--------------------------------------------------------------------------\n");
}

/*******************************************************************************
    PURPOSE: dir ascii line callback. called for each file in dir.

    INPUTS: type - dir entry type (always 'f'?)
            size - size in bytes
	    tbuffer - time file was created (not used?)
	    name - file name
            data - not used

    RETURN CODE: NONE

*******************************************************************************/
void asciiLine_cb( char type, char * size, char * tbuffer, char *name, void *data) {
    printf( "%c  %-20s %s  %s\r\n", type, name, size, tbuffer);
}

/*******************************************************************************
    PURPOSE: dir ascii footer callback

    INPUTS: total - bytes in files (without blocking loss)
            nfile - number of files
	    tot - total disk space
	    used - total used disk space
            data - not used

    RETURN CODE: NONE

*******************************************************************************/
void asciiFooter_cb(uint64_t total, int nfiles, uint32_t tot, uint32_t used, void *data) {
    if (total) {
        printf("-----------------------------------\n");
        if (total < (1024*1024)) printf("   %8d", (int)total);
        else if ((total/1024) < (1024*1024)) printf("   %6dKB", (int)(total / 1024));
        else printf("   %6dMB", (int)(total / (1024 * 1024)));
        printf(" in %d file(s)\n", nfiles);
    }
    printf("-----------------------------------\n");
    printf("SPIFFS: free %ld KB of %ld KB\n", (tot-used) / 1024, tot / 1024);
    printf("-----------------------------------\n\n");
}


/*******************************************************************************

    PURPOSE: 'dir' command

    INPUTS: None

    RETURN CODE: None

*******************************************************************************/

static int dir(int argc, char **argv)
{
    diskDirCfg_t req = {
	    .path = "/spiffs",
	    .header_cb = asciiHeader_cb,
	    .line_cb = asciiLine_cb,
	    .footer_cb = asciiFooter_cb };

    disk_dir(req);

    return 0;
}

/*******************************************************************************

    PURPOSE: register 'heap' command

    INPUTS: None

    RETURN CODE: None

*******************************************************************************/
static void register_dir(void)
{
    const esp_console_cmd_t dir_cmd = {
        .command = "dir",
        .help = "list contents of flash drive",
        .hint = NULL,
        .func = &dir,
    };
    ESP_ERROR_CHECK( esp_console_cmd_register(&dir_cmd) );

}
